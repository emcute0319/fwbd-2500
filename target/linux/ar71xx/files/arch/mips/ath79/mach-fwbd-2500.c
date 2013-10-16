/*
 *  FWBD-2500 board support
 *
 *  Copyright (C) 2013 Darius Augulis <darius@wilibox.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define FWBD_2500_MAC0_OFFSET		0x0000
#define FWBD_2500_CALDATA_OFFSET	0x1000
#define FWBD_2500_WMAC_MAC_OFFSET	0x1002

#define FWBD_2500_GPIO_BTN_RESET	17

#define FWBD_2500_GPIO_LED_1		13
#define FWBD_2500_GPIO_LED_2		4
#define FWBD_2500_GPIO_LED_3		3
#define FWBD_2500_GPIO_LED_4		2
#define FWBD_2500_GPIO_LED_5		1
#define FWBD_2500_GPIO_LED_6		0

#define FWBD_2500_KEYS_POLL_INTERVAL		20	/* msecs */
#define FWBD_2500_KEYS_DEBOUNCE_INTERVAL	(3 * FWBD_2500_KEYS_POLL_INTERVAL)

static struct gpio_led fwbd_2500_leds_gpio[] __initdata = {
	{
		.name		= "fwbd-2500:orange:led1",
		.gpio		= FWBD_2500_GPIO_LED_1,
		.active_low	= 1,
	}, {
		.name		= "fwbd-2500:orange:led2",
		.gpio		= FWBD_2500_GPIO_LED_2,
		.active_low	= 1,
	}, {
		.name		= "fwbd-2500:green:led3",
		.gpio		= FWBD_2500_GPIO_LED_3,
		.active_low	= 1,
	}, {
		.name		= "fwbd-2500:green:led4",
		.gpio		= FWBD_2500_GPIO_LED_4,
		.active_low	= 1,
	}, {
		.name		= "fwbd-2500:green:led5",
		.gpio		= FWBD_2500_GPIO_LED_5,
		.active_low	= 1,
	}, {
		.name		= "fwbd-2500:green:led6",
		.gpio		= FWBD_2500_GPIO_LED_6,
		.active_low	= 1,
	},
};

static struct gpio_keys_button fwbd_2500_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = FWBD_2500_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= FWBD_2500_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static struct mdio_board_info fwbd_2500_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
	},
};

static void __init fwbd_2500_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);

	ath79_register_wmac(art + FWBD_2500_CALDATA_OFFSET,
			    art + FWBD_2500_WMAC_MAC_OFFSET);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_MII_GMAC0 |
				   AR934X_ETH_CFG_MII_GMAC0_SLAVE);

	ath79_register_mdio(1, 0);
	ath79_register_mdio(0, 0);
	ath79_init_mac(ath79_eth0_data.mac_addr, art + FWBD_2500_MAC0_OFFSET, 0);

	mdiobus_register_board_info(fwbd_2500_mdio0_info,
		ARRAY_SIZE(fwbd_2500_mdio0_info));

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_register_eth(0);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(fwbd_2500_leds_gpio),
		fwbd_2500_leds_gpio);
	ath79_register_gpio_keys_polled(-1, FWBD_2500_KEYS_POLL_INTERVAL,
		ARRAY_SIZE(fwbd_2500_gpio_keys),
		fwbd_2500_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_FWBD2500, "FWBD-2500", "FWBD-2500", fwbd_2500_setup);
