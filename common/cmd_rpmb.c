// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <config.h>
#include <common.h>
#include <asm/arch/io.h>
#include <command.h>
#include <malloc.h>
#include <asm/arch/secure_apb.h>

static int do_rpmb_state(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int nReturn = __LINE__;

#define AML_RPMB_STATE (((readl(AO_SEC_GP_CFG7))>>22) & 0x01)

	char *pARG = getenv("bootargs");

	if (pARG)
	{
		//printf("1 bootargs=%s\n",pARG);

		char *szBuffer=malloc(strlen(pARG)+64);
		nReturn = __LINE__;

		if (szBuffer)
		{
			memset(szBuffer+strlen(pARG),0,64);
			strcpy(szBuffer,pARG);
			char *pFind = strstr(szBuffer,"androidboot.rpmb_state");
			if (!pFind)
				sprintf(szBuffer,"%s androidboot.rpmb_state=%d",pARG,AML_RPMB_STATE);
			else
				pFind[23] = AML_RPMB_STATE ? '1':'0';

			//printf("2 bootargs=%s\n",szBuffer);

			setenv("rpmb_state",AML_RPMB_STATE?"1":"0"); //need this?
			setenv("bootargs",szBuffer);
			free(szBuffer);
			szBuffer = 0;

			nReturn = 0;
		}
		else
			printf("aml log : internal sys error!\n");

	}

	return nReturn;
}

U_BOOT_CMD(rpmb_state, CONFIG_SYS_MAXARGS, 0, do_rpmb_state,
		"RPMB sub-system",
		"RPMB state\n");


/****************************************************/
