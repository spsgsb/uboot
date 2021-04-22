/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef __AML_LCD_TV_H__
#define __AML_LCD_TV_H__
#include <amlogic/aml_lcd.h>

//**********************************
//lcd driver version
//**********************************
#define LCD_DRV_TYPE      "tv"

//**********************************

extern void lcd_tv_config_update(struct lcd_config_s *pconf);
extern void lcd_tv_driver_init_pre(void);
extern int lcd_tv_driver_init(void);
extern void lcd_tv_driver_disable(void);

#endif
