// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <malloc.h>
#include <asm/byteorder.h>
#include <config.h>
#include <asm/arch/io.h>
#include <partition_table.h>
#include <libavb.h>
#include <version.h>
#include <spotify/avb.h>

#ifdef CONFIG_BOOTLOADER_CONTROL_BLOCK

static bool slot_is_bootable(AvbABSlotData* slot) {
  return slot->priority > 0 &&
         (slot->successful_boot || (slot->tries_remaining > 0));
}

static void consume_boot_try(AvbABSlotData* slot) {
    if (slot->tries_remaining > 0) {
        --(slot->tries_remaining);
    }
    slot->successful_boot = 0;
}

static bool should_failover(AvbABSlotData* slot) {
    return slot->tries_remaining <= 0 || slot->tries_remaining > AVB_AB_MAX_TRIES_REMAINING;
}

static int do_GetValidSlot(
    cmd_tbl_t * cmdtp,
    int flag,
    int argc,
    char * const argv[])
{
    char miscbuf[MISCBUF_SIZE] = {0};
    AvbABData info;
    int slot;
    bool bootable_a, bootable_b;

    if (argc != 1) {
        return cmd_usage(cmdtp);
    }

    boot_info_open_partition(miscbuf);
    boot_info_load(&info, miscbuf);

    if (!boot_info_validate(&info)) {
        printf("boot-info is invalid. Resetting.\n");
        boot_info_reset(&info);
        boot_info_save(&info, miscbuf);
    }

    slot = get_active_slot(&info);
    printf("active slot = %d\n", slot);

    if (should_failover(&(info.slots[slot]))) {
        printf("No tries remaining on slot %d!\n", slot);

        boot_info_failover(&info);
        boot_info_save(&info, miscbuf);

        slot = get_active_slot(&info);
        printf("active slot = %d\n", slot);
    }

    bootable_a = slot_is_bootable(&(info.slots[0]));
    bootable_b = slot_is_bootable(&(info.slots[1]));

    if ((slot == 0) && (bootable_a)) {
        if (has_boot_slot == 1) {
            setenv("active_slot","_a");
            setenv("boot_part","boot_a");
            setenv("slot-suffixes","0");
        }
        else {
            setenv("active_slot","normal");
            setenv("boot_part","boot");
        }
        return 0;
    }

    if ((slot == 1) && (bootable_b)) {
        if (has_boot_slot == 1) {
            setenv("active_slot","_b");
            setenv("boot_part","boot_b");
            setenv("slot-suffixes","1");
        }
        else {
            setenv("active_slot","normal");
            setenv("boot_part","boot");
        }
        return 0;
    }

    return 0;
}

static int do_SetActiveSlot(
    cmd_tbl_t * cmdtp,
    int flag,
    int argc,
    char * const argv[])
{
    char miscbuf[MISCBUF_SIZE] = {0};
    AvbABData info;

    if (argc != 2) {
        return cmd_usage(cmdtp);
    }

    if (has_boot_slot == 0) {
        printf("device is not ab mode\n");
        return -1;
    }

    boot_info_open_partition(miscbuf);
    boot_info_load(&info, miscbuf);

    if (!boot_info_validate(&info)) {
        printf("boot-info is invalid. Resetting.\n");
        boot_info_reset(&info);
        boot_info_save(&info, miscbuf);
    }

    if (strcmp(argv[1], "a") == 0) {
        setenv("active_slot","_a");
        setenv("slot-suffixes","0");
        setenv("boot_part","boot_a");
        printf("set active slot a \n");
        boot_info_set_active_slot(&info, 0);
    } else if (strcmp(argv[1], "b") == 0) {
        setenv("active_slot","_b");
        setenv("slot-suffixes","1");
        setenv("boot_part","boot_b");
        printf("set active slot b \n");
        boot_info_set_active_slot(&info, 1);
    } else {
        printf("error input slot\n");
        return -1;
    }

    boot_info_save(&info, miscbuf);

    return 0;
}

int do_GetSystemMode (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char* system;
#ifdef CONFIG_ENABLE_SYSTEM_AS_ROOT_MODE
    system = CONFIG_ENABLE_SYSTEM_AS_ROOT_MODE;
    strcpy(system, CONFIG_ENABLE_SYSTEM_AS_ROOT_MODE);
    printf("CONFIG_ENABLE_SYSTEM_AS_ROOT_MODE: %s \n", CONFIG_ENABLE_SYSTEM_AS_ROOT_MODE);
    if (strcmp(system, "systemroot") == 0) {
        setenv("system_mode","1");
    }
    else {
        setenv("system_mode","0");
    }
#else
    setenv("system_mode","0");
#endif

    return 0;
}

int do_GetAvbMode (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_ENABLE_AVB_MODE
    char* avbmode;
    avbmode = CONFIG_ENABLE_AVB_MODE;
    strcpy(avbmode, CONFIG_ENABLE_AVB_MODE);
    printf("CONFIG_ENABLE_AVB_MODE: %s \n", CONFIG_ENABLE_AVB_MODE);
    if (strcmp(avbmode, "avb2") == 0) {
        setenv("avb2","1");
    }
    else {
        setenv("avb2","0");
    }
#else
    setenv("avb2","0");
#endif

    return 0;
}

int do_ConsumeBootTry (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
    char miscbuf[MISCBUF_SIZE] = {0};
    AvbABData info;
    int slot;

    if (has_boot_slot == 0) {
        printf("device is not ab mode\n");
        return -1;
    }

    boot_info_open_partition(miscbuf);
    boot_info_load(&info, miscbuf);

    if (!boot_info_validate(&info)) {
        printf("boot-info is invalid. Resetting.\n");
        boot_info_reset(&info);
        boot_info_save(&info, miscbuf);
    }

    slot = get_active_slot(&info);

    consume_boot_try(&(info.slots[slot]));

    boot_info_save(&info, miscbuf);

    return 0;

}

#endif /* CONFIG_BOOTLOADER_CONTROL_BLOCK */

U_BOOT_CMD(
    get_valid_slot, 2, 0, do_GetValidSlot,
    "get_valid_slot",
    "\nThis command will choose valid slot to boot up which saved in misc\n"
    "partition by mark to decide whether execute command!\n"
    "So you can execute command: get_valid_slot"
);

U_BOOT_CMD(
    set_active_slot, 2, 1, do_SetActiveSlot,
    "set_active_slot",
    "\nThis command will set active slot\n"
    "So you can execute command: set_active_slot a"
);

U_BOOT_CMD(
    get_system_as_root_mode, 1,	0, do_GetSystemMode,
    "get_system_as_root_mode",
    "\nThis command will get system_as_root_mode\n"
    "So you can execute command: get_system_as_root_mode"
);

U_BOOT_CMD(
    get_avb_mode, 1,	0, do_GetAvbMode,
    "get_avb_mode",
    "\nThis command will get avb mode\n"
    "So you can execute command: get_avb_mode"
);

U_BOOT_CMD(
    consume_boot_try, 1,	0, do_ConsumeBootTry,
    "consume_boot_try",
    "\nThis command consumes 1 boot try\n"
    "So you can execute command: consume_boot_try"
);
