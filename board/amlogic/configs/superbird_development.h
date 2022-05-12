
/*
 * board/amlogic/configs/superbird_development.h
 *
 * Copyright (C) 2015 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __SUPERBIRD_DEVELOPMENT_H__
#define __SUPERBIRD_DEVELOPMENT_H__

#include <asm/arch/cpu.h>

#define CONFIG_SYS_GENERIC_BOARD  1
#ifndef CONFIG_AML_MESON
#warning "include warning"
#endif

#define CONFIG_VERSION_VARIABLE

/*
 * platform power init config
 */
#define CONFIG_PLATFORM_POWER_INIT
#define CONFIG_VCCK_INIT_VOLTAGE	860		// VCCK power up voltage
#define CONFIG_VDDEE_INIT_VOLTAGE	800		// VDDEE power up voltage
#define CONFIG_VDDEE_SLEEP_VOLTAGE	770		// VDDEE suspend voltage

/* configs for CEC */
#define CONFIG_CEC_OSD_NAME		"AML_TV"
#define CONFIG_CEC_WAKEUP
/*if use bt-wakeup,open it*/
#define CONFIG_BT_WAKEUP
/* SMP Definitinos */
#define CPU_RELEASE_ADDR		secondary_boot_func

/* config saradc*/
#define CONFIG_CMD_SARADC 1
#define CONFIG_SARADC_CH  2

/* Bootloader Control Block function
   That is used for recovery and the bootloader to talk to each other
  */
#define CONFIG_BOOTLOADER_CONTROL_BLOCK

#define CONFIG_CMD_BOOTCTOL_AVB

/* Serial config */
#define CONFIG_CONS_INDEX 2
#define CONFIG_BAUDRATE  115200
#define CONFIG_AML_MESON_SERIAL   1
#define CONFIG_SERIAL_MULTI		1

//Enable ir remote wake up for bl30
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL1 0xef10fe01 //amlogic tv ir --- power
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL2 0XBB44FB04 //amlogic tv ir --- ch+
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL3 0xF20DFE01 //amlogic tv ir --- ch-
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL4 0XBA45BD02 //amlogic small ir--- power
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL5 0xe51afb04
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL6 0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL7 0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL8 0xFFFFFFFF
#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL9 0xFFFFFFFF

/*config the default parameters for adc power key*/
#define CONFIG_ADC_POWER_KEY_CHAN   2  /*channel range: 0-7*/
#define CONFIG_ADC_POWER_KEY_VAL    0  /*sample value range: 0-1023*/

#undef CONFIG_SILENT_CONSOLE_UPDATE_ON_RELOC
#define CONFIG_SILENT_CONSOLE 1
#define CONFIG_SILENT_CONSOLE_UPDATE_ON_RELOC 1
#define CONFIG_SILENT_CONSOLE_UPDATE_ON_SET 1
#define CONFIG_SYS_DEVICE_NULLDEV 1

#define CONFIG_ENABLE_AVB_MODE "avb2"
#define CONFIG_ENABLE_SYSTEM_AS_ROOT_MODE "systemroot"
#define AVB_USE_SPOTIFY_DEV_KEY 1

#undef ENABLE_SILENT_KERNEL
#ifdef ENABLE_SILENT_KERNEL
#define KERNEL_CONSOLE_ARGS "quiet"
#else
#define KERNEL_CONSOLE_ARGS "console=ttyS0,115200 no_console_suspend earlycon=aml-uart,0xff803000"
#endif

#define ENABLE_BOOTCHART
#ifdef ENABLE_BOOTCHART
#define INIT_ARGS "init=/sbin/bootchartd bootchart_init=/sbin/pre-init"
#else
#define INIT_ARGS "init=/sbin/pre-init"
#endif

/* args/envs */
#define CONFIG_SYS_MAXARGS  64
#define CONFIG_EXTRA_ENV_SETTINGS \
        "silent=off\0"\
        "firstboot=1\0"\
        "upgrade_step=0\0"\
        "jtag=disable\0"\
        "loadaddr=1080000\0"\
        "panel_type=lcd_8\0" \
        "outputmode=panel\0" \
        "lcd_debug_test=0\0" \
        "display_stack=unknown\0"\
        "display_width=480\0" \
        "display_height=800\0" \
        "display_bpp=16\0" \
        "display_color_index=16\0" \
        "display_layer=osd0\0" \
        "display_color_fg=0xffff\0" \
        "display_color_bg=0\0" \
        "display_init=1\0"\
        "skip_charger_check=0\0"\
        "dtb_mem_addr=0x1000000\0" \
        "fb_addr=0x1f800000\0" \
        "fb_width=480\0" \
        "fb_height=800\0" \
        "frac_rate_policy=1\0" \
        "usb_burning=update 1000\0" \
        "fdt_high=0x20000000\0"\
        "try_auto_burn=update 700 750;\0"\
        "sdcburncfg=aml_sdc_burn.ini\0"\
        "sdc_burning=sdc_burn ${sdcburncfg}\0"\
        "wipe_data=successful\0"\
        "wipe_cache=successful\0"\
        "EnableSelinux=enforcing\0" \
        "recovery_part=recovery\0"\
        "recovery_offset=0\0"\
        "osd_reverse=0\0"\
        "video_reverse=0\0"\
        "lock=10001000\0"\
        "active_slot=_a\0"\
        "boot_part=boot\0"\
        "reboot_mode_android=""normal""\0"\
        "Irq_check_en=0\0"\
        "fs_type=""ro rootwait skip_initramfs""\0"\
        "system_mode=1\0"\
        "avb2=1\0"\
        "initargs="\
            INIT_ARGS " " KERNEL_CONSOLE_ARGS " ramoops.pstore_en=1 ramoops.record_size=0x8000 ramoops.console_size=0x4000 rootfstype=ext4"\
            "\0"\
        "storeargs="\
            "setenv bootargs ${initargs} ${fs_type} reboot_mode_android=${reboot_mode_android} logo=${display_layer},loaded,${fb_addr} fb_width=${fb_width} fb_height=${fb_height} vout=${outputmode},enable panel_type=${panel_type} frac_rate_policy=${frac_rate_policy} osd_reverse=${osd_reverse} video_reverse=${video_reverse} irq_check_en=${Irq_check_en}  androidboot.selinux=${EnableSelinux} androidboot.firstboot=${firstboot} jtag=${jtag} uboot_version=${gitver};"\
	"setenv bootargs ${bootargs} androidboot.hardware=amlogic;"\
            "\0"\
        "storeboot="\
            "boot_cooling;"\
            "run storeargs;"\
            "get_valid_slot;"\
            "setenv bootargs ${bootargs} androidboot.slot_suffix=${active_slot};" \
            "consume_boot_try;"\
            "if imgread kernel ${boot_part} ${loadaddr}; then bootm ${loadaddr}; fi;"\
            "run update;"\
            "\0"\
        "splash_boot="\
            "imgread pic logo bootup_spotify $loadaddr;bmp display $bootup_spotify_offset;bmp scale;"\
            "run storeboot;"\
            "\0"\
        "update="\
            /*first usb burning, second sdc_burn, third ext-sd autoscr/recovery, last udisk autoscr/recovery*/\
            "run usb_burning; "\
            "\0"\
        "init_display="\
            "if test ${display_init} = 1; then "\
                "osd open;"\
		"osd clear;"\
		"imgread pic logo bootup_spotify $loadaddr;"\
		"bmp display $bootup_spotify_offset;"\
		"bmp scale;"\
		"vout output ${outputmode};"\
            "fi;"\
            "\0"\
        "bcb_cmd="\
            "get_avb_mode;"\
            "get_valid_slot;"\
            "\0"\
	"check_charger="\
	    "mw 0xFF6346DC 0x33000000;"             \
	    "mw.b 0x1337DEAD 0x00 1;"\
	    "mw.b 0x1330DEAD 0x12 1;"\
	    "mw.b 0x1331DEAD 0x13 1;"\
	    "mw.b 0x1332DEAD 0x15 1;"\
	    "mw.b 0x1333DEAD 0x16 1;"\
	    "i2c dev 2;" \
	    "i2c read 0x35 0x3 1 0x1337DEAD;"\
	    "if cmp.b 0x1337DEAD 0x1330DEAD 1; then "\
	        "run storeboot;"\
	    "elif cmp.b 0x1337DEAD 0x1331DEAD 1; then "\
		"run storeboot;"\
	    "elif cmp.b 0x1337DEAD 0x1332DEAD 1; then "\
		"run storeboot;"\
	    "elif cmp.b 0x1337DEAD 0x1333DEAD 1; then "\
		"run storeboot;"\
	    "else "\
		"osd open;"\
		"osd clear;"\
		"imgread pic logo bad_charger $loadaddr;"\
		"bmp display $bad_charger_offset;"\
		"bmp scale;"\
		"vout output ${outputmode};"\
		"while true; do sleep 1; "\
		    "if gpio input GPIOAO_3; then "\
			"run splash_boot; "\
		    "fi; "\
		    "i2c read 0x35 0x3 1 0x1337DEAD;"\
		    "if test ${skip_charger_check} = 1; then "   \
			"run storeboot;"\
		    "elif cmp.b 0x1337DEAD 0x1330DEAD 1; then "\
			"run splash_boot;"\
		    "elif cmp.b 0x1337DEAD 0x1331DEAD 1; then "\
			"run splash_boot;"\
		    "elif cmp.b 0x1337DEAD 0x1332DEAD 1; then "\
			"run splash_boot;"\
		    "elif cmp.b 0x1337DEAD 0x1333DEAD 1; then "\
			"run splash_boot;"\
		    "fi;"\
		    "i2c mw 0x35 0x09 0x8F 1;" \
		"done;"\
	    "fi;"\
            "\0"\

#define CONFIG_PREBOOT  \
            "run bcb_cmd; "\
            "run init_display;"\
            "run storeargs;"\
            "bcb uboot-command;"

#define CONFIG_BOOTCOMMAND "run check_charger"

#define CONFIG_SPOTIFY_PROBE_HW 1
#define CONFIG_SPOTIFY_AVB 1

//#define CONFIG_ENV_IS_NOWHERE  1
#define CONFIG_ENV_SIZE   (64*1024)
#define CONFIG_FIT 1
#define CONFIG_OF_LIBFDT 1
#define CONFIG_ANDROID_BOOT_IMAGE 1
#define CONFIG_ANDROID_IMG 1
#define CONFIG_SYS_BOOTM_LEN (64<<20) /* Increase max gunzip size*/

/* cpu */
#define CONFIG_CPU_CLK					1704 //MHz. Range: 360-2000, should be multiple of 24

/* ATTENTION */
/* DDR configs move to board/amlogic/[board]/firmware/timing.c */

#define CONFIG_NR_DRAM_BANKS			1
/* ddr functions */
#define CONFIG_DDR_FULL_TEST			0 //0:disable, 1:enable. ddr full test
#define CONFIG_CMD_DDR_D2PLL			0 //0:disable, 1:enable. d2pll cmd
#define CONFIG_CMD_DDR_TEST				0 //0:disable, 1:enable. ddrtest cmd
#define CONFIG_DDR_LOW_POWER			0 //0:disable, 1:enable. ddr clk gate for lp
#define CONFIG_DDR_ZQ_PD				0 //0:disable, 1:enable. ddr zq power down
#define CONFIG_DDR_USE_EXT_VREF			0 //0:disable, 1:enable. ddr use external vref
#define CONFIG_DDR4_TIMING_TEST			0 //0:disable, 1:enable. ddr4 timing test function
#define CONFIG_DDR_PLL_BYPASS			0 //0:disable, 1:enable. ddr pll bypass function
#define CONFIG_DDR_NONSEC_SCRAMBLE		0 //0:disable, 1:enable. non-sec region scramble function

#define CONFIG_SKIP_NAND_INIT 1

/* storage: emmc/nand/sd */
#define		CONFIG_STORE_COMPATIBLE 1
#define 	CONFIG_ENV_OVERWRITE
#define 	CONFIG_CMD_SAVEENV
/* fixme, need fix*/

#if (defined(CONFIG_ENV_IS_IN_AMLNAND) || defined(CONFIG_ENV_IS_IN_MMC)) && defined(CONFIG_STORE_COMPATIBLE)
#error env in amlnand/mmc already be compatible;
#endif

/*
*				storage
*		|---------|---------|
*		|					|
*		emmc<--Compatible-->nand
*					|-------|-------|
*					|				|
*					MTD<-Exclusive->NFTL
*/
/* axg only support slc nand */
/* swither for mtd nand which is for slc only. */
/* support for mtd */
#define CONFIG_AML_MTD 1
/* support for nftl */
//#define CONFIG_AML_NAND	1

#if defined(CONFIG_AML_NAND) && defined(CONFIG_AML_MTD)
#error CONFIG_AML_NAND/CONFIG_AML_MTD can not support at the sametime;
#endif

#ifdef CONFIG_AML_MTD

/* bootlaoder is construct by bl2 and fip
 * when DISCRETE_BOOTLOADER is enabled, bl2 & fip
 * will not be stored continuously, and nand layout
 * would be bl2|rsv|fip|normal, but not
 * bl2|fip|rsv|noraml anymore
 */
#define CONFIG_DISCRETE_BOOTLOADER

#ifdef  CONFIG_DISCRETE_BOOTLOADER
#define CONFIG_TPL_SIZE_PER_COPY          0x200000
#define CONFIG_TPL_COPY_NUM               4
#define CONFIG_TPL_PART_NAME              "tpl"
/* for bl2, restricted by romboot */
//SKT 1024 pages only support 4 block, so 4 copies
#define CONFIG_BL2_COPY_NUM               4
#endif /* CONFIG_DISCRETE_BOOTLOADER */

#define CONFIG_CMD_NAND 1
#define CONFIG_MTD_DEVICE y
/* mtd parts of ourown.*/
#define CONFIFG_AML_MTDPART	1
/* mtd parts by env default way.*/
/*
#define MTDIDS_NAME_STR		"aml_nand.0"
#define MTDIDS_DEFAULT		"nand1=" MTDIDS_NAME_STR
#define MTDPARTS_DEFAULT	"mtdparts=" MTDIDS_NAME_STR ":" \
					"3M@8192K(logo),"	\
					"10M(recovery),"	\
					"8M(kernel),"	\
					"40M(rootfs),"	\
					"-(data)"
*/
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_CMD_NAND_TORTURE 1
#define CONFIG_CMD_MTDPARTS   1
#define CONFIG_MTD_PARTITIONS 1
#define CONFIG_SYS_MAX_NAND_DEVICE  2
#define CONFIG_SYS_NAND_BASE_LIST   {0}
#endif
/* endof CONFIG_AML_MTD */
#define		CONFIG_AML_SD_EMMC 1
#ifdef		CONFIG_AML_SD_EMMC
	#define 	CONFIG_GENERIC_MMC 1
	#define 	CONFIG_CMD_MMC 1
	#define CONFIG_CMD_GPT 1
	#define	CONFIG_SYS_MMC_ENV_DEV 1
	#define CONFIG_EMMC_DDR52_EN 0
	#define CONFIG_EMMC_DDR52_CLK 35000000
#endif
#define		CONFIG_PARTITIONS 1
#define 	CONFIG_SYS_NO_FLASH  1
//#define     CONFIG_AML_GPT

/* meson SPI */
#define CONFIG_AML_SPIFC
//#define CONFIG_AML_SPICC
#if defined CONFIG_AML_SPIFC || defined CONFIG_AML_SPICC
	#define CONFIG_OF_SPI
	#define CONFIG_DM_SPI
	#define CONFIG_CMD_SPI
#endif
/* SPI flash config */
#ifdef CONFIG_AML_SPIFC
	#define CONFIG_SPI_FLASH
	#define CONFIG_DM_SPI_FLASH
	#define CONFIG_CMD_SF
	/* SPI flash surpport list */
	#define CONFIG_SPI_FLASH_ATMEL
	#define CONFIG_SPI_FLASH_EON
	#define CONFIG_SPI_FLASH_GIGADEVICE
	#define CONFIG_SPI_FLASH_MACRONIX
	#define CONFIG_SPI_FLASH_SPANSION
	#define CONFIG_SPI_FLASH_STMICRO
	#define CONFIG_SPI_FLASH_SST
	#define CONFIG_SPI_FLASH_WINBOND
	#define CONFIG_SPI_FRAM_RAMTRON
	#define CONFIG_SPI_M95XXX
	#define CONFIG_SPI_FLASH_ESMT
	/* SPI nand flash support */
	#define CONFIG_SPI_NAND
	#define CONFIG_BL2_SIZE (64 * 1024)
#endif

#if defined CONFIG_AML_MTD || defined CONFIG_SPI_NAND
	#define CONFIG_CMD_NAND 1
	#define CONFIG_MTD_DEVICE y
	#define CONFIG_RBTREE
	#define CONFIG_CMD_NAND_TORTURE 1
	#define CONFIG_CMD_MTDPARTS   1
	#define CONFIG_MTD_PARTITIONS 1
	#define CONFIG_SYS_MAX_NAND_DEVICE  2
	#define CONFIG_SYS_NAND_BASE_LIST   {0}
#endif

/* vpu */
#define CONFIG_AML_VPU 1
//#define CONFIG_VPU_CLK_LEVEL_DFT 7

/* DISPLAY & HDMITX */
#undef CONFIG_AML_HDMITX20
#define CONFIG_AML_CANVAS 1
#define CONFIG_AML_VOUT 1
#define CONFIG_AML_OSD 1
#define CONFIG_OSD_SCALE_ENABLE 1
#define CONFIG_CMD_BMP 1
#define CONFIG_AML_MINUI 1

#if defined(CONFIG_AML_VOUT)
#undef CONFIG_AML_CVBS
#endif

#define CONFIG_AML_LCD    1
#define CONFIG_AML_LCD_TABLET 1
#define CONFIG_AML_LCD_EXTERN 1
#define CONFIG_AML_LCD_EXTERN_MIPI_ST7701 1

/* USB
 * Enable CONFIG_MUSB_HCD for Host functionalities MSC, keyboard
 * Enable CONFIG_MUSB_UDD for Device functionalities.
 */
/* #define CONFIG_MUSB_UDC		1 */
#define CONFIG_CMD_USB 1
#if defined(CONFIG_CMD_USB)
	#define CONFIG_GXL_XHCI_BASE            0xff500000
	#define CONFIG_GXL_USB_PHY2_BASE        0xffe09000
	#define CONFIG_GXL_USB_PHY3_BASE        0xffe09080
	#define CONFIG_USB_PHY_20				0xff636000
	#define CONFIG_USB_PHY_21				0xff63A000
	#define CONFIG_USB_STORAGE      1
	#define CONFIG_USB_XHCI		1
	#define CONFIG_USB_XHCI_AMLOGIC_V2 1
	#define CONFIG_USB_GPIO_PWR  			GPIOEE(GPIOH_6)
	#define CONFIG_USB_GPIO_PWR_NAME		"GPIOH_6"
	//#define CONFIG_USB_XHCI_AMLOGIC_USB3_V2		1
#endif //#if defined(CONFIG_CMD_USB)

#define CONFIG_TXLX_USB        1
#define CONFIG_USB_DEVICE_V2    1
#define USB_PHY2_PLL_PARAMETER_1	0x09400414
#define USB_PHY2_PLL_PARAMETER_2	0x927e0000
#define USB_PHY2_PLL_PARAMETER_3	0xAC5F49E5
#define USB_G12x_PHY_PLL_SETTING_1	(0xfe18)
#define USB_G12x_PHY_PLL_SETTING_2	(0xfff)
#define USB_G12x_PHY_PLL_SETTING_3	(0x78000)
#define USB_G12x_PHY_PLL_SETTING_4	(0xe0004)
#define USB_G12x_PHY_PLL_SETTING_5	(0xe000c)

//UBOOT fastboot config
#define CONFIG_CMD_FASTBOOT 1
#define CONFIG_FASTBOOT_FLASH_MMC_DEV 1
#define CONFIG_FASTBOOT_FLASH 1
#define CONFIG_USB_GADGET 1
#define CONFIG_USBDOWNLOAD_GADGET 1
#define CONFIG_SYS_CACHELINE_SIZE 64
#define CONFIG_FASTBOOT_MAX_DOWN_SIZE	0x8000000
#define CONFIG_DEVICE_PRODUCT	"u200"

//UBOOT Facotry usb/sdcard burning config
#define CONFIG_AML_V2_FACTORY_BURN              1       //support facotry usb burning
#define CONFIG_AML_FACTORY_BURN_LOCAL_UPGRADE   1       //support factory sdcard burning
#define CONFIG_POWER_KEY_NOT_SUPPORTED_FOR_BURN 1       //There isn't power-key for factory sdcard burning
#define CONFIG_SD_BURNING_SUPPORT_UI            1       //Displaying upgrading progress bar when sdcard/udisk burning

#define CONFIG_AML_SECURITY_KEY                 1
#define CONFIG_UNIFY_KEY_MANAGE                 1

/* net */
//#define CONFIG_CMD_NET   1
#if defined(CONFIG_CMD_NET)
	#define CONFIG_DESIGNWARE_ETH 1
	#define CONFIG_PHYLIB	1
	#define CONFIG_NET_MULTI 1
	#define CONFIG_CMD_PING 1
	#define CONFIG_CMD_DHCP 1
	#define CONFIG_CMD_RARP 1
	#define CONFIG_HOSTNAME        arm_gxbb
//	#define CONFIG_RANDOM_ETHADDR  1				   /* use random eth addr, or default */
	#define CONFIG_ETHADDR         00:15:18:01:81:31   /* Ethernet address */
	#define CONFIG_IPADDR          10.18.9.97          /* Our ip address */
	#define CONFIG_GATEWAYIP       10.18.9.1           /* Our getway ip address */
	#define CONFIG_SERVERIP        10.18.9.113         /* Tftp server ip address */
	#define CONFIG_NETMASK         255.255.255.0
#endif /* (CONFIG_CMD_NET) */

#define CONFIG_DM_GPIO 1
#define CONFIG_AML_GPIO 1

/* other devices */
/* I2C DM driver*/
#define CONFIG_DM_I2C 1

#if defined(CONFIG_DM_I2C)
#define CONFIG_SYS_I2C_MESON		1
#else
#define CONFIG_SYS_I2C_AML			1
#define CONFIG_SYS_I2C_SPEED		400000
#endif

/* PWM DM driver*/
#define CONFIG_DM_PWM
#define CONFIG_PWM_MESON

#define CONFIG_EFUSE 1

/* commands */
#define CONFIG_CMD_CACHE 1
#define CONFIG_CMD_BOOTI 1
#define CONFIG_CMD_EFUSE 1
#define CONFIG_CMD_I2C 1
#define CONFIG_CMD_MEMORY 1
#define CONFIG_CMD_FAT 1
#define CONFIG_CMD_GPIO 1
#define CONFIG_CMD_RUN
#define CONFIG_CMD_REBOOT 1
#define CONFIG_CMD_ECHO 1
#define CONFIG_CMD_JTAG	1
#define CONFIG_CMD_AUTOSCRIPT 1
#define CONFIG_CMD_MISC 1

/*file system*/
#define CONFIG_DOS_PARTITION 1
#define CONFIG_EFI_PARTITION 1
#define CONFIG_AML_PARTITION 1
#define CONFIG_MMC 1
#define CONFIG_FS_FAT 1
#define CONFIG_FS_EXT4 1
#define CONFIG_LZO 1

#define CONFIG_MDUMP_COMPRESS 1

/* Cache Definitions */
//#define CONFIG_SYS_DCACHE_OFF
//#define CONFIG_SYS_ICACHE_OFF

/* other functions */
#define CONFIG_NEED_BL301	1
#define CONFIG_NEED_BL32	1
#define CONFIG_CMD_RSVMEM	1
#define CONFIG_FIP_IMG_SUPPORT	1
#define CONFIG_BOOTDELAY	1 //delay 1s
#define CONFIG_SYS_LONGHELP 1
#define CONFIG_CMD_MISC     1
#define CONFIG_CMD_ITEST    1
#define CONFIG_CMD_CPU_TEMP 1
#define CONFIG_SYS_MEM_TOP_HIDE 0x08000000 //hide 128MB for kernel reserve
#define CONFIG_CMD_LOADB    1

#define CONFIG_MULTI_DTB    1

/* debug mode defines */
//#define CONFIG_DEBUG_MODE           1
#ifdef CONFIG_DEBUG_MODE
#define CONFIG_DDR_CLK_DEBUG        636
#define CONFIG_CPU_CLK_DEBUG        600
#endif

//support secure boot
#define CONFIG_AML_SECURE_UBOOT   1

#if defined(CONFIG_AML_SECURE_UBOOT)

//for SRAM size limitation just disable NAND
//as the socket board default has no NAND
//#undef CONFIG_AML_NAND

//unify build for generate encrypted bootloader "u-boot.bin.encrypt"
#define CONFIG_AML_CRYPTO_UBOOT   1

//unify build for generate encrypted kernel image
//SRC : "board/amlogic/(board)/boot.img"
//DST : "fip/boot.img.encrypt"
//#define CONFIG_AML_CRYPTO_IMG       1

#endif //CONFIG_AML_SECURE_UBOOT

#define CONFIG_SECURE_STORAGE 1

//build with uboot auto test
//#define CONFIG_AML_UBOOT_AUTO_TEST 1

//board customer ID
//#define CONFIG_CUSTOMER_ID  (0x6472616F624C4D41)

#if defined(CONFIG_CUSTOMER_ID)
  #undef CONFIG_AML_CUSTOMER_ID
  #define CONFIG_AML_CUSTOMER_ID  CONFIG_CUSTOMER_ID
#endif

/* Choose One of Ethernet Type */
#define CONFIG_ETHERNET_NONE
#undef ETHERNET_INTERNAL_PHY
#undef ETHERNET_EXTERNAL_PHY

#define CONFIG_CMD_AML_MTEST 1
#if defined(CONFIG_CMD_AML_MTEST)
#if !defined(CONFIG_SYS_MEM_TOP_HIDE)
#error CONFIG_CMD_AML_MTEST depends on CONFIG_SYS_MEM_TOP_HIDE;
#endif
#if !(CONFIG_SYS_MEM_TOP_HIDE)
#error CONFIG_SYS_MEM_TOP_HIDE should not be zero;
#endif
#endif

#define CONFIG_HIGH_TEMP_COOL 90
#endif

