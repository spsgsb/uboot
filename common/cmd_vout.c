// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <vpp.h>
#include <amlogic/vout.h>
#ifdef CONFIG_AML_HDMITX20
#include <amlogic/hdmi.h>
#endif

#ifdef CONFIG_AML_CVBS
#include <amlogic/cvbs.h>
#endif
#ifdef CONFIG_AML_LCD
#include <amlogic/aml_lcd.h>
#endif

static int do_vout_list(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_AML_LCD
	struct aml_lcd_drv_s *lcd_drv = aml_lcd_get_driver();
#endif

#ifdef CONFIG_AML_HDMITX20
	printf("\nvalid hdmi mode:\n");
	hdmitx_device.HWOp.list_support_modes();
#endif

#ifdef CONFIG_AML_CVBS
	printf("\nvalid cvbs mode:\n");
	cvbs_show_valid_vmode();
#endif

#ifdef CONFIG_AML_LCD
	printf("\nvalid lcd mode:\n");
	if (lcd_drv) {
		if (lcd_drv->list_support_mode)
			lcd_drv->list_support_mode();
		else
			printf("no lcd list_support_mode\n");
	} else {
		printf("no lcd driver\n");
	}
#endif

	return CMD_RET_SUCCESS;
}

static int do_vout_output(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#ifdef CONFIG_AML_HDMITX20
	char mode[64];
#endif
#ifdef CONFIG_AML_LCD
	struct aml_lcd_drv_s *lcd_drv = NULL;
#endif

	if (argc != 2)
		return CMD_RET_FAILURE;

#ifdef CONFIG_AML_CVBS
	if (cvbs_outputmode_check(argv[1]) == 0) {
		vpp_matrix_update(VPP_CM_YUV);
		if (cvbs_set_vmode(argv[1]) == 0)
			return CMD_RET_SUCCESS;
	}
#endif

#ifdef CONFIG_AML_HDMITX20
	if (hdmi_outputmode_check(argv[1]) == 0) {
		vpp_matrix_update(VPP_CM_YUV);
		memset(mode, 0, sizeof(mode));
		sprintf(mode, "hdmitx output %s", argv[1]);
		run_command(mode, 0);
		return CMD_RET_SUCCESS;
	}
#endif

#ifdef CONFIG_AML_LCD
	lcd_drv = aml_lcd_get_driver();
	if (lcd_drv) {
		if (lcd_drv->lcd_outputmode_check) {
			if (lcd_drv->lcd_outputmode_check(argv[1]) == 0) {
				vpp_matrix_update(VPP_CM_RGB);
				if (lcd_drv->lcd_enable) {
					lcd_drv->lcd_enable(argv[1]);
					return CMD_RET_SUCCESS;
				} else
					printf("no lcd enable\n");
			}
		}
	} else {
		printf("no lcd driver\n");
	}
#endif

	return CMD_RET_FAILURE;
}

static int do_vout_info(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	vout_vinfo_dump();

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_vout_sub[] = {
	U_BOOT_CMD_MKENT(list, 1, 1, do_vout_list, "", ""),
	U_BOOT_CMD_MKENT(output, 3, 1, do_vout_output, "", ""),
	U_BOOT_CMD_MKENT(info, 1, 1, do_vout_info, "", ""),
};

static int do_vout(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return cmd_usage(cmdtp);

	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_vout_sub[0], ARRAY_SIZE(cmd_vout_sub));

	if (c)
		return  c->cmd(cmdtp, flag, argc, argv);
	else
		return cmd_usage(cmdtp);
}

U_BOOT_CMD(vout, CONFIG_SYS_MAXARGS, 1, do_vout,
	"VOUT sub-system",
	"vout [list | output format | info]\n"
	"    list : list for valid video mode names.\n"
	"    format : perfered output video mode\n"
	"    info : dump vinfo\n"
);
