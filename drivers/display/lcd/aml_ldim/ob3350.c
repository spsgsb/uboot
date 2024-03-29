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
#include <amlogic/aml_ldim.h>
#include "../aml_lcd_reg.h"
#include "../aml_lcd_common.h"
#include "ldim_drv.h"
#include "ldim_dev_drv.h"

static int ob3350_on_flag;

static int ob3350_hw_init_on(void)
{
	struct aml_ldim_driver_s *ldim_drv = aml_ldim_get_driver();

	ldim_set_duty_pwm(&(ldim_drv->ldev_conf->ldim_pwm_config));
	ldim_set_duty_pwm(&(ldim_drv->ldev_conf->analog_pwm_config));

	ldim_set_gpio(ldim_drv->ldev_conf->en_gpio, ldim_drv->ldev_conf->en_gpio_on);
	mdelay(2);
	ldim_drv->pinmux_ctrl(1);
	mdelay(20);

	return 0;
}

static int ob3350_hw_init_off(void)
{
	struct aml_ldim_driver_s *ldim_drv = aml_ldim_get_driver();

	ldim_drv->pinmux_ctrl(0);
	ldim_set_gpio(ldim_drv->ldev_conf->en_gpio, ldim_drv->ldev_conf->en_gpio_off);

	return 0;
}

static unsigned int ob3350_get_value(unsigned int level)
{
	struct aml_ldim_driver_s *ldim_drv = aml_ldim_get_driver();

	unsigned int val;
	unsigned int dim_max, dim_min;

	dim_max = ldim_drv->ldev_conf->dim_max;
	dim_min = ldim_drv->ldev_conf->dim_min;

	val = dim_min + ((level * (dim_max - dim_min)) / LD_DATA_MAX);

	return val;
}

static int ob3350_smr(unsigned short *buf, unsigned char len)
{
	struct aml_ldim_driver_s *ldim_drv = aml_ldim_get_driver();
	unsigned short val;

	val = ob3350_get_value(buf[0]);
	ldim_drv->ldev_conf->ldim_pwm_config.pwm_duty = val;

	if (ob3350_on_flag == 0) {
		if (lcd_debug_print_flag)
			LDIMPR("%s: on_flag=%d\n", __func__, ob3350_on_flag);
		return 0;
	}

	if (len != 1) {
		LDIMERR("%s: data len %d invalid\n", __func__, len);
		return -1;
	}

	ldim_set_duty_pwm(&(ldim_drv->ldev_conf->ldim_pwm_config));

	return 0;
}

static void ob3350_dim_range_update(void)
{
	struct aml_ldim_driver_s *ldim_drv = aml_ldim_get_driver();
	struct ldim_dev_config_s *ldim_dev;

	ldim_dev = ldim_drv->ldev_conf;
	ldim_dev->dim_max = ldim_dev->ldim_pwm_config.pwm_duty_max;
	ldim_dev->dim_min = ldim_dev->ldim_pwm_config.pwm_duty_min;
}

static int ob3350_power_on(void)
{
	ob3350_hw_init_on();
	ob3350_on_flag = 1;

	LDIMPR("%s: ok\n", __func__);
	return 0;
}

static int ob3350_power_off(void)
{
	ob3350_on_flag = 0;
	ob3350_hw_init_off();

	LDIMPR("%s: ok\n", __func__);
	return 0;
}

static int ob3350_ldim_driver_update(struct aml_ldim_driver_s *ldim_drv)
{
	struct ldim_dev_config_s *ldim_dev = ldim_drv->ldev_conf;

	ldim_dev->ldim_pwm_config.pwm_duty_max = ldim_dev->dim_max;
	ldim_dev->ldim_pwm_config.pwm_duty_min = ldim_dev->dim_min;
	ldim_dev->dim_range_update = ob3350_dim_range_update;

	ldim_drv->device_power_on = ob3350_power_on;
	ldim_drv->device_power_off = ob3350_power_off;
	ldim_drv->device_bri_update = ob3350_smr;

	return 0;
}

int ldim_dev_ob3350_probe(struct aml_ldim_driver_s *ldim_drv)
{
	ob3350_on_flag = 0;
	ob3350_ldim_driver_update(ldim_drv);

	return 0;
}

int ldim_dev_ob3350_remove(struct aml_ldim_driver_s *ldim_drv)
{
	return 0;
}

