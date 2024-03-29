// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
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
#include "../aml_lcd_common.h"
#include "../aml_lcd_reg.h"

#define LCD_EXTERN_NAME		"spi_LD070WS2"
#define LCD_EXTERN_TYPE		LCD_EXTERN_SPI

#define GPIO_SPI_CS		0 /* index */
#define GPIO_SPI_CLK		1 /* index */
#define GPIO_SPI_DATA		2 /* index */
#define SPI_CLK_FREQ		10 //KHz
#define SPI_CLK_POL		1

#define SPI_DELAY		30 //unit: us

static struct lcd_extern_config_s *ext_config;

#define LCD_EXTERN_CMD_SIZE        3
static unsigned char init_on_table[] = {
	0xc0, 0x00, 0x21, /* reset */
	0xc0, 0x00, 0xa5, /* standby */
	0xc0, 0x01, 0x30, /* enable FRC/Dither */
	0xc0, 0x02, 0x40, /* enable normally black */
	0xc0, 0x0e, 0x5f, /* enable test mode1 */
	0xc0, 0x0f, 0xa4, /* enable test mode2 */
	0xc0, 0x0d, 0x00, /* enable SDRRS, enlarge OE width */
	0xc0, 0x02, 0x43, /* adjust charge sharing time */
	0xc0, 0x0a, 0x28, /* trigger bias reduction */
	0xc0, 0x10, 0x41, /* adopt 2 line/1 dot */
	0xfd, 50,   0,  /* delay 50ms */
	0xc0, 0x00, 0xad, /* display on */
	0xff, 0x00, 0x00, /* ending */
};

static unsigned char init_off_table[] = {
	0xc0, 0x00, 0xa5, /* standby */
	0xff, 0x00, 0x00, /* ending */
};

static void lcd_extern_set_csb(unsigned v)
{
	aml_lcd_extern_gpio_set(ext_config->spi_gpio_cs, v);
	udelay(SPI_DELAY);
}

static void lcd_extern_set_scl(unsigned v)
{
	aml_lcd_extern_gpio_set(ext_config->spi_gpio_clk, v);
	udelay(SPI_DELAY);
}

static void lcd_extern_set_sda(unsigned v)
{
	aml_lcd_extern_gpio_set(ext_config->spi_gpio_data, v);
	udelay(SPI_DELAY);
}

static void spi_gpio_init(void)
{
	lcd_extern_set_csb(1);
	lcd_extern_set_scl(1);
	lcd_extern_set_sda(1);
}

static void spi_gpio_off(void)
{
	lcd_extern_set_sda(0);
	lcd_extern_set_scl(0);
	lcd_extern_set_csb(0);
}

static void spi_write_8(unsigned char addr, unsigned char data)
{
	int i;
	unsigned int sdata;

	sdata = (unsigned int)(addr & 0x3f);
	sdata <<= 10;
	sdata |= (data & 0xff);
	sdata &= ~(1<<9); //write flag

	lcd_extern_set_csb(1);
	lcd_extern_set_scl(1);
	lcd_extern_set_sda(1);

	lcd_extern_set_csb(0);
	for (i = 0; i < 16; i++) {
		lcd_extern_set_scl(0);
		if (sdata & 0x8000)
			lcd_extern_set_sda(1);
		else
			lcd_extern_set_sda(0);
		sdata <<= 1;
		lcd_extern_set_scl(1);
	}

	lcd_extern_set_csb(1);
	lcd_extern_set_scl(1);
	lcd_extern_set_sda(1);
	udelay(SPI_DELAY);
}

static int lcd_extern_spi_write(unsigned char *buf, int len)
{
	if (len != 2) {
		EXTERR("%s: len %d error\n", __func__, len);
		return -1;
	}
	spi_write_8(buf[0], buf[1]);
	return 0;
}

static int lcd_extern_power_cmd_dynamic_size(unsigned char *table, int flag)
{
	int i = 0, j = 0, max_len = 0, step = 0;
	unsigned char type, cmd_size;
	int delay_ms, ret = 0;

	if (flag)
		max_len = ext_config->table_init_on_cnt;
	else
		max_len = ext_config->table_init_off_cnt;

	while ((i + 1) < max_len) {
		type = table[i];
		if (type == LCD_EXT_CMD_TYPE_END)
			break;
		if (lcd_debug_print_flag) {
			EXTPR("%s: step %d: type=0x%02x, cmd_size=%d\n",
				__func__, step, type, table[i+1]);
		}
		cmd_size = table[i+1];
		if (cmd_size == 0)
			goto power_cmd_dynamic_next;
		if ((i + 2 + cmd_size) > max_len)
			break;

		if (type == LCD_EXT_CMD_TYPE_NONE) {
			/* do nothing */
		} else if (type == LCD_EXT_CMD_TYPE_GPIO) {
			if (cmd_size < 2) {
				EXTERR("step %d: invalid cmd_size %d for GPIO\n",
					step, cmd_size);
				goto power_cmd_dynamic_next;
			}
			aml_lcd_extern_gpio_set(table[i+2], table[i+3]);
			if (cmd_size > 2) {
				if (table[i+4] > 0)
					mdelay(table[i+4]);
			}
		} else if (type == LCD_EXT_CMD_TYPE_DELAY) {
			delay_ms = 0;
			for (j = 0; j < cmd_size; j++)
				delay_ms += table[i+2+j];
			if (delay_ms > 0)
				mdelay(delay_ms);
		} else if (type == LCD_EXT_CMD_TYPE_CMD) {
			ret = lcd_extern_spi_write(&table[i+2], cmd_size);
		} else if (type == LCD_EXT_CMD_TYPE_CMD_DELAY) {
			ret = lcd_extern_spi_write(&table[i+2], (cmd_size-1));
			if (table[i+1+cmd_size] > 0)
				mdelay(table[i+1+cmd_size]);
		} else {
			EXTERR("%s: %s(%d): type 0x%02x invalid\n",
				__func__, ext_config->name, ext_config->index, type);
		}
power_cmd_dynamic_next:
		i += (cmd_size + 2);
		step++;
	}

	return ret;
}

static int lcd_extern_power_cmd_fixed_size(unsigned char *table, int flag)
{
	int i = 0, j, max_len, step = 0;
	unsigned char type, cmd_size;
	int delay_ms, ret = 0;

	if (table == NULL) {
		EXTERR("%s: init_table %d is NULL\n", __func__, flag);
		return -1;
	}

	cmd_size = ext_config->cmd_size;
	if (cmd_size < 2) {
		EXTERR("%s: cmd_size %d is invalid\n", __func__, cmd_size);
		return -1;
	}

	if (flag)
		max_len = ext_config->table_init_on_cnt;
	else
		max_len = ext_config->table_init_off_cnt;

	while ((i + cmd_size) <= max_len) {
		type = table[i];
		if (type == LCD_EXT_CMD_TYPE_END)
			break;
		if (lcd_debug_print_flag) {
			EXTPR("%s: step %d: type=0x%02x, cmd_size=%d\n",
				__func__, step, type, cmd_size);
		}
		if (type == LCD_EXT_CMD_TYPE_NONE) {
			//do nothing
		} else if (type == LCD_EXT_CMD_TYPE_GPIO) {
			aml_lcd_extern_gpio_set(table[i+1], table[i+2]);
			if (cmd_size > 3) {
				if (table[i+3] > 0)
					mdelay(table[i+3]);
			}
		} else if (type == LCD_EXT_CMD_TYPE_DELAY) {
			delay_ms = 0;
			for (j = 0; j < (cmd_size - 1); j++)
				delay_ms += table[i+1+j];
			if (delay_ms > 0)
				mdelay(delay_ms);
		} else if (type == LCD_EXT_CMD_TYPE_CMD) {
			ret = lcd_extern_spi_write(&table[i+1], (cmd_size-1));
		} else if (type == LCD_EXT_CMD_TYPE_CMD_DELAY) {
			ret = lcd_extern_spi_write(&table[i+1], (cmd_size-2));
			if (table[i+cmd_size-1] > 0)
				mdelay(table[i+cmd_size-1]);
		} else {
			EXTERR("%s(%d: %s): type 0x%02x is invalid\n",
				__func__, ext_config->index, ext_config->name, type);
		}
		i += cmd_size;
		step++;
	}

	return ret;
}

static int lcd_extern_power_ctrl(int flag)
{
	unsigned char *table;
	unsigned char cmd_size;
	int ret = 0;

	spi_gpio_init();

	cmd_size = ext_config->cmd_size;
	if (flag)
		table = ext_config->table_init_on;
	else
		table = ext_config->table_init_off;
	if (cmd_size < 1) {
		EXTERR("%s: cmd_size %d is invalid\n", __func__, cmd_size);
		ret = -1;
		goto power_ctrl_next;
	}
	if (table == NULL) {
		EXTERR("%s: init_table %d is NULL\n", __func__, flag);
		ret = -1;
		goto power_ctrl_next;
	}
	if (cmd_size == LCD_EXT_CMD_SIZE_DYNAMIC)
		ret = lcd_extern_power_cmd_dynamic_size(table, flag);
	else
		ret = lcd_extern_power_cmd_fixed_size(table, flag);

power_ctrl_next:
	mdelay(10);
	spi_gpio_off();

	if (ret) {
		EXTERR("%s: %s(%d): %d\n", __func__, ext_config->name,
			ext_config->index, flag);
	} else {
		EXTPR("%s: %s(%d): %d\n", __func__, ext_config->name,
			ext_config->index, flag);
	}
	return ret;
}

static int lcd_extern_power_on(void)
{
	int ret;

	ret = lcd_extern_power_ctrl(1);
	return ret;
}

static int lcd_extern_power_off(void)
{
	int ret;

	ret = lcd_extern_power_ctrl(0);
	return ret;
}

static int lcd_extern_driver_update(struct aml_lcd_extern_driver_s *ext_drv)
{
	if (ext_drv == NULL) {
		EXTERR("%s driver is null\n", LCD_EXTERN_NAME);
		return -1;
	}

	if (ext_drv->config->table_init_loaded == 0) {
		ext_drv->config->cmd_size = LCD_EXTERN_CMD_SIZE;
		ext_drv->config->table_init_on  = init_on_table;
		ext_drv->config->table_init_on_cnt  = sizeof(init_on_table);
		ext_drv->config->table_init_off = init_off_table;
		ext_drv->config->table_init_off_cnt  = sizeof(init_off_table);
	}
	ext_drv->config->spi_delay_us = 1000 / ext_drv->config->spi_clk_freq;

	ext_drv->power_on  = lcd_extern_power_on;
	ext_drv->power_off = lcd_extern_power_off;

	return 0;
}

int aml_lcd_extern_spi_LD070WS2_probe(struct aml_lcd_extern_driver_s *ext_drv)
{
	int ret = 0;

	ext_config = ext_drv->config;
	ret = lcd_extern_driver_update(ext_drv);

	if (lcd_debug_print_flag)
		EXTPR("%s: %d\n", __func__, ret);
	return ret;
}
