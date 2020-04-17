/*
 * drivers/amlogic/media/vout/lcd/lcd_extern/mipi_ILI9806E.c
 *
 * Copyright (C) 2019 Spotify AB. All rights reserved.
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/gpio.h>
#ifdef CONFIG_OF_LIBFDT
#include <libfdt.h>
#endif
#include <amlogic/aml_lcd.h>
#include <amlogic/aml_lcd_extern.h>
#include "lcd_extern.h"

#define LCD_EXTERN_NAME			"mipi_ILI9806E"
#define LCD_EXTERN_TYPE			LCD_EXTERN_MIPI

/******************** mipi command ********************/
/* format:  data_type, cmd_size, data.... */
/*	data_type=0xff,
 *		0 < cmd_size < 0xff means delay ms,
 *		cmd_size=0 or 0xff means ending.
 *	data_type=0xf0, for gpio control
 *		data0=gpio_index, data1=gpio_value.
 *		data0=gpio_index, data1=gpio_value, data2=delay.
 *	data_type=0xfd, for delay ms
 *		data0=delay, data_1=delay, ..., data_n=delay.
 */
static unsigned char mipi_init_on_table[] = {
//***************************************************************//LCD SETING
   0x39, 6, 0xFF, 0xFF, 0x98, 0x06, 0x04, 0x01,        /*  Change to Page 1 CMD    RS: FFh ENEXTC P.127    RS: Page 1 */
      0x15, 2, 0x08, 0x10,    /* Output    SDA   RS: 08h Interface Mode Control P.181 */
                     /*  RS: SDO_STATUS =1: always output , but without output tri-state. */
                     /*  RS: SEPT_SDIO =0: SPI interface transfer data through SDA pin.  */
      0x15, 2, 0x20, 0x00,    /* set DE/VSYNC mode    平台注意   RS: 20h Display Function Control 1 P.183 */
                     /*  RS: DE mode */
      0x15, 2, 0x21, 0x01,    /* DE = 1 Active   RS: 21h Display Function Control 2 P.184 */
                     /*  RS: EPL: DE polarity “1”= High enable */
                     /*  RS: DPL: PCLK polarity set “0”=data fetched at the rising time */
                     /*  RS: HSPL: HS polarity 0”=Low level sync clock */
                     /*  RS: VSPL: VS polarity “0”= Low level sync clock */
      0x15, 2, 0x30, 0x02,    /* Resolution setting 480 X 800   RS: 30h Resolution Control P.192 */
                     /*  RS: 0b010 480x800 */
      0x15, 2, 0x31, 0x02,    /* Inversion setting 2-dot   RS: 31h Display Inversion Control P.193 */
                     /*  RS: 0b0010 2 dot inversion */
      0x15, 2, 0x60, 0x07,    /*  RS: 60h Source Timing Adjust P.221 */
                     /*  RS: Source SDTI timing adjustment T_OP_CLK. T_OP_CLK : 4 * T_OSC_CLK = 0.22us */
      0x15, 2, 0x61, 0x06,    /*  RS: 61h Source Timing Adjust 2 P.222 */
                     /*  RS: Source CRTI timing adjustment T_OP_CLK */
      0x15, 2, 0x62, 0x06,    /*  RS: 62h Source Timing Adjust 3 P.223 */
                     /*  RS: Source EQTI timing adjustment */
      0x15, 2, 0x63, 0x04,    /*  RS: 63h Source Timing Adjust 4 P.224 */
                     /*  RS: Source PCTI timing adjustment */
      0x15, 2, 0x40, 0x18,    /* BT  AVDD,AVDD 倍压   RS: 40h Power Control 1 P.197 */
                     /* 10,14    RS: 0b01 EXT_CPCK_SEL output x 2 waveform. 0b1000 BT VCI X 3 / VCI X -2 */
      0x15, 2, 0x41, 0x33,    /* AVDD，AVEE设置   RS: 41h Power Cont0x10ol 2 P.199 */
                     /* avdd +5.4v,avee-5.2v ;55   RS: 0b011 0x10DVDH_CLP DDVDH clamp level 5.2V; 0b011 DDVDL_CLP DDVDL clamp level -5.2V */
      0x15, 2, 0x42, 0x12,    /* VGL ,VGH  SET    RS: 42h Power Co0x10trol 3 P.201 */
                     /* 11    RS: 0b01 VGH_CP DDVDH+VCIP-DDVDL; 0b10 VGL_CP DDVDL+VCL-VCIP */
      0x15, 2, 0x43, 0x89,    /* Default      RS: 43h Power Control 4 P.202 */
                     /* 09    RS: 0b1 VGH clamp level enabled; 0b1001 VGH clamp level 15.0V */
      0x15, 2, 0x44, 0x86,    /*  RS: 44h Power Control 5 P.203 */
                     /* 0C    RS: 0b1 VGL clamp level enabled; 0b0110 VGL clamp level -10.0V */
      0x15, 2, 0x46, 0x55,    /* 增加      RS: 46h Power Control 7 P.206 */
                     /*  RS: 0b101 DDVDH_PK1 step-up cycle for step-up circuit 1: 1/2 */
                     /*  RS: 0b101 DDVDL_PK2 step-up cycle for step-up circuit 2: 1/2 */
      0x15, 2, 0x47, 0x55,    /* 增加       RS: 47h Power Control 8 P.208 */
                     /*  RS: 0b101 VCL_PK3 step-up cycle for step-up circuit 3: 1/2 */
                     /*  RS: 0b101 VGHL_PK4 step-up cycle for step-up circuit 4: 1/2 */
      0x15, 2, 0x45, 0x14,    /*  RS: 45h Power Control 4 P.204 */
                     /*  RS: 0b0001 VGH_REG operation voltage 9.0V */
                     /*  RS: 0b0100 VGL_REG operation voltage -9.0V */
      0x15, 2, 0x50, 0x50,    /* VREG1      RS: Power Control 9 P.210 */
                     /*  RS: VREG1OUT 4.0000V */
      0x15, 2, 0x51, 0x50,    /* VREG2      RS: Power Control 10 P.212 */
                     /*  RS: VREG2OUT -4.0000V */
      0x15, 2, 0x52, 0x00,    /* Flicker MSB      RS: VCOM Control 1 P.214 */
      0x15, 2, 0x53, 0x38,    /* Flicker LSB      RS: VCOM Control 2 P.214 */
                     /* VCOM     RS: 0b000111000 or 0x038 VCOM voltage -0.8875V */
      0x15, 2, 0xA0, 0x00,    /* Positive Gamma   RS: Position Gamma Control P.229 */
      0x15, 2, 0xA1, 0x09,    /*   */
      0x15, 2, 0xA2, 0x0C,    /*   */
      0x15, 2, 0xA3, 0x0F,    /*   */
      0x15, 2, 0xA4, 0x06,    /*   */
      0x15, 2, 0xA5, 0x09,    /*   */
      0x15, 2, 0xA6, 0x07,    /*   */
      0x15, 2, 0xA7, 0x16,    /*   */
      0x15, 2, 0xA8, 0x06,    /*   */
      0x15, 2, 0xA9, 0x09,    /*   */
      0x15, 2, 0xAA, 0x11,    /*   */
      0x15, 2, 0xAB, 0x06,    /*   */
      0x15, 2, 0xAC, 0x0E,    /*   */
      0x15, 2, 0xAD, 0x19,    /*   */
      0x15, 2, 0xAE, 0x0E,    /*   */
      0x15, 2, 0xAF, 0x00,    /*   */
      0x15, 2, 0xC0, 0x00,    /* Negative Gamma   RS: Negative Gamma Correction P.230 */
      0x15, 2, 0xC1, 0x09,    /*   */
      0x15, 2, 0xC2, 0x0C,    /*   */
      0x15, 2, 0xC3, 0x0F,    /*   */
      0x15, 2, 0xC4, 0x06,    /*   */
      0x15, 2, 0xC5, 0x09,    /*   */
      0x15, 2, 0xC6, 0x07,    /*   */
      0x15, 2, 0xC7, 0x16,    /*   */
      0x15, 2, 0xC8, 0x06,    /*   */
      0x15, 2, 0xC9, 0x09,    /*   */
      0x15, 2, 0xCA, 0x11,    /*   */
      0x15, 2, 0xCB, 0x06,    /*   */
      0x15, 2, 0xCC, 0x0E,    /*   */
      0x15, 2, 0xCD, 0x19,    /*   */
      0x15, 2, 0xCE, 0x0E,    /*   */
      0x15, 2, 0xCF, 0x00,    /*   */
   0x39, 6, 0xFF, 0xFF, 0x98, 0x06, 0x04, 0x06,        /*  Change to Page 6 CMD for GIP timing     RS: FFh ENEXTC P.127 */
      /*  RS: GIP Setting */
      0x15, 2, 0x00, 0xA0,    /*  RS: STV_A_Rise[10:8] 0b101; GIP_0_SET0 0 */
      0x15, 2, 0x01, 0x05,    /*  RS: STV_A_Rise[7:0] 0x05 0b00000101 so STV_A_Rise is 0b10100000101 which is 1285 */
      0x15, 2, 0x02, 0x00,    /*  RS: GIP_0_SET1 0 */
      0x15, 2, 0x03, 0x00,    /*  RS: GIP_0_SET2 0 */
      0x15, 2, 0x04, 0x01,    /*  RS: GIP_0_SET3 1 */
      0x15, 2, 0x05, 0x01,    /*  RS: GIP_0_SET4 1 */
      0x15, 2, 0x06, 0x88,    /*  RS: CLK_A_Rise[10:8] 0b100; GIP_0_SET5 0b01000 */
      0x15, 2, 0x07, 0x04,    /*  RS: CLK_A_Rise[7:0] 0b00000100 so CLK_A_Rise is 0b10000000100 which is 1028 */
      0x15, 2, 0x08, 0x01,    /*  RS: GPI_0_SET6 */
      0x15, 2, 0x09, 0x90,    /*  RS: GPI_0_SET7 */
      0x15, 2, 0x0A, 0x04,    /*  RS: GPI_0_SET8 */
      0x15, 2, 0x0B, 0x01,    /*  RS: GPI_0_SET9 */
      0x15, 2, 0x0C, 0x01,    /*  RS: GPI_0_SET10 */
      0x15, 2, 0x0D, 0x01,    /*  RS: GPI_0_SET11 */
      0x15, 2, 0x0E, 0x00,    /*  RS: GPI_0_SET12 */
      0x15, 2, 0x0F, 0x00,    /*  RS: GPI_0_SET13 */
      0x15, 2, 0x10, 0x55,    /*  RS: GPI_0_SET14 */
      0x15, 2, 0x11, 0x50,    /*  RS: GPI_0_SET15 */
      0x15, 2, 0x12, 0x01,    /*  RS: GPI_0_SET16 */
      0x15, 2, 0x13, 0x85,    /*  RS: GPI_0_SET17 */
      0x15, 2, 0x14, 0x85,    /*  RS: GPI_0_SET18 */
      0x15, 2, 0x15, 0xC0,    /*  RS: GPI_0_SET19 */
      0x15, 2, 0x16, 0x0B,    /*  RS: GPI_0_SET20 */
      0x15, 2, 0x17, 0x00,    /*  RS: GPI_0_SET21 */
      0x15, 2, 0x18, 0x00,    /*  RS: GPI_0_SET220x10 */
      0x15, 2, 0x19, 0x00,    /*  RS: GPI_0_SET230x10 */
      0x15, 2, 0x1A, 0x00,    /*  RS: GPI_0_SET24 */
      0x15, 2, 0x1B, 0x00,    /*  RS: GPI_0_SET25 */
      0x15, 2, 0x1C, 0x00,    /*  RS: GPI_0_SET26 */
      0x15, 2, 0x1D, 0x00,    /*  RS: GPI_0_SET27 */
      0x15, 2, 0x20, 0x01,    /*  RS: GPI_1_SET0 */
      0x15, 2, 0x21, 0x23,    /*  RS: GPI_1_SET1 */
      0x15, 2, 0x22, 0x45,    /*  RS: GPI_1_SET2 */
      0x15, 2, 0x23, 0x67,    /*  RS: GPI_1_SET3 */
      0x15, 2, 0x24, 0x01,    /*  RS: GPI_1_SET4 */
      0x15, 2, 0x25, 0x23,    /*  RS: GPI_1_SET5 */
      0x15, 2, 0x26, 0x45,    /*  RS: GPI_1_SET6 */
      0x15, 2, 0x27, 0x67,    /*  RS: GPI_1_SET7 */
      0x15, 2, 0x30, 0x02,    /*  RS: GPI_2_SET0 */
      0x15, 2, 0x31, 0x22,    /*  RS: GPI_2_SET1 */
      0x15, 2, 0x32, 0x11,    /*  RS: GPI_2_SET2 */
      0x15, 2, 0x33, 0xAA,    /*  RS: GPI_2_SET3 */
      0x15, 2, 0x34, 0xBB,    /*  RS: GPI_2_SET4 */
      0x15, 2, 0x35, 0x66,    /*  RS: GPI_2_SET5 */
      0x15, 2, 0x36, 0x00,    /*  RS: GPI_2_SET6 */
      0x15, 2, 0x37, 0x22,    /*  RS: GPI_2_SET7 */
      0x15, 2, 0x38, 0x22,    /*  RS: GPI_2_SET8 */
      0x15, 2, 0x39, 0x22,    /*  RS: GPI_2_SET9 */
      0x15, 2, 0x3A, 0x22,    /*  RS: GPI_2_SET10 */
      0x15, 2, 0x3B, 0x22,    /*  RS: GPI_2_SET11 */
      0x15, 2, 0x3C, 0x22,    /*  RS: GPI_2_SET12 */
      0x15, 2, 0x3D, 0x22,    /*  RS: GPI_2_SET13 */
      0x15, 2, 0x3E, 0x22,    /*  RS: GPI_2_SET14 */
      0x15, 2, 0x3F, 0x22,    /*  RS: GPI_2_SET15 */
      0x15, 2, 0x40, 0x22,    /*  RS: GPI_2_SET16 */
      0x15, 2, 0x52, 0x12,    /*  RS: GOUT_VGLO Control 1 52h P.268    RS: GOUT_VGLO_SO[3:0] 0b0010 VGL_REG */
      0x15, 2, 0x53, 0x12,    /*  RS: GOUT_VGLO Control 53h P.269    RS: GOUT_VGLO_DSP[3:0] 0b0010 VGL_REG */
   0x39, 6, 0xFF, 0xFF, 0x98, 0x06, 0x04, 0x07, /*  Change to Page 7 CMD for GIP timing       RS: Page 7 */
      0x15, 2, 0x17, 0x32,    /*  RS: VGL_REG_ENABLE 17h P.269    RS: 0b1 enable */
      0x15, 2, 0x02, 0x17,    /*  RS: Power Bias Control 02h P.271 */
                     /*  RS: DDVDL_CLP_ISC[2:0] 0b001 4uA DDVDL Clamp Bias */
                     /*  RS: DDVDH_CLP_ISC[2:0] 0b111 16uA DDVDH Clamp Bias */
      0x15, 2, 0x18, 0x1D,    /*  RS: VREG1/2OUT ENABLE 18h P.270 */
                     /*  RS: 0b1 VREGEN enable */
      0x15, 2, 0xE1, 0x79,    /*  RS: Time Control[3:0] E1h P.275 */
                     /*  RS: TIME_SHIFT_ERR[3:0] 0b0111 "Timming option"? */
   0x39, 6, 0xFF, 0xFF, 0x98, 0x06, 0x04, 0x00, /*  Change to Page 0 CMD for Normal command     RS: Page 0 */
      0x15, 2, 0x3A, 0x50,    /*  RS: Interface Pixel Format 3Ah P.164  */
                     /* 16BIT    RS: 0b101 16-bit / pixel */
                     /* 24BIT    RS: 0b111 24-bit / pixel */
      0x05, 1, 0x11, /*  RS: Sleep out 11h P.151 */
      0xfd, 1, 120, /*  RS: 120ms wait is required    delay(ms)*/
      0x05, 1, 0x29, /*  RS: Display ON 29h */
      0xfd, 1, 25, /*  RS: 25ms why???    delay(ms)*/
   0xff, 0,
};

static unsigned char mipi_init_off_table[] = {
	0x05, 1, 0x28, /* display off */
	0xfd, 1, 10,   /* delay 10ms */
	0x05, 1, 0x10, /* sleep in */
	0xfd, 1, 150,  /* delay 150ms */
	0xff, 0,   /* ending */
};

static int lcd_extern_driver_update(struct aml_lcd_extern_driver_s *ext_drv)
{
	if (ext_drv == NULL) {
		EXTERR("%s driver is null\n", LCD_EXTERN_NAME);
		return -1;
	}

	ext_drv->config->cmd_size = LCD_EXT_CMD_SIZE_DYNAMIC;
	ext_drv->config->table_init_on  = &mipi_init_on_table[0];
	ext_drv->config->table_init_on_cnt  = sizeof(mipi_init_on_table);
	ext_drv->config->table_init_off = &mipi_init_off_table[0];
	ext_drv->config->table_init_off_cnt  = sizeof(mipi_init_off_table);

	return 0;
}

int aml_lcd_extern_mipi_ILI9806E_probe(struct aml_lcd_extern_driver_s *ext_drv)
{
	int ret = 0;

	ret = lcd_extern_driver_update(ext_drv);

	if (lcd_debug_print_flag)
		EXTPR("%s: %d\n", __func__, ret);
	return ret;
}
