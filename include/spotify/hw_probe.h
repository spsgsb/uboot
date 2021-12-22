/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2021 Spotify AB. All rights reserved.
 */

#ifndef __SPOTIFY_HW_PROBE__
#define __SPOTIFY_HW_PROBE__

enum sp_board_revision_e {
    REV_A = 0x00,
    REV_B,
    REV_UNKNOWN = 0xff
};
typedef enum sp_board_revision_e sp_board_revision;

enum sp_display_stack_e {
    STACK_BOE = 0x00,
    STACK_WILY,
    STACK_UNKNOWN = 0xff
};
typedef enum sp_display_stack_e sp_display_stack;

struct sp_display_stack_conf_s {
    uint8_t i2c_addr;
    uint16_t i2c_reg;
    uint32_t hw_id;
    sp_display_stack id;
};
typedef struct sp_display_stack_conf_s sp_display_stack_conf;

#define BOE_I2C_ADDR 0x2e
#define BOE_I2C_REG 0x9e // 0x9e00 //0xd6e0
#define BOE_HW_ID 0x17000000

#define WILY_I2C_ADDR 0x2e
#define WILY_I2C_REG 0x9e
#define WILY_HW_ID 0x00

static const sp_display_stack_conf sp_display_stack_confs[] = {
    { BOE_I2C_ADDR, BOE_I2C_REG, BOE_HW_ID, STACK_BOE },
    { WILY_I2C_ADDR, WILY_I2C_REG, WILY_HW_ID, STACK_WILY },
};

static const int sp_display_stack_confs_len = 2;

// ex: ret = plat_i2c_read(addr, reg, buff, buff_len);
typedef int (*sp_plat_i2c_read)(int, int, void*, size_t);


// NOLINTNEXTLINE(misc-definitions-in-headers)
sp_board_revision sp_probe_board_revision(void) {
    return REV_A;
}

// NOLINTNEXTLINE(misc-definitions-in-headers)
sp_display_stack sp_probe_display_stack(sp_plat_i2c_read plat_i2c_read) {
    printf("sp_hw_probe: display stack start\n");
    uint32_t r = -1;
    int i;
    sp_display_stack_conf c;
    uint32_t* buff;

    buff = malloc(sizeof(uint32_t));

    for (i = 0; i < sp_display_stack_confs_len; i++) {
        c = sp_display_stack_confs[i];
        r = plat_i2c_read(c.i2c_addr, c.i2c_reg, buff, sizeof(uint32_t));

        if (r) {
            goto error;
        }

        if (memcmp(&c.hw_id, buff, sizeof(uint32_t)) == 0) {
            printf("sp_hw_probe: display stack end\n");
            return c.id;
        }
    }

error:
    printf("sp_hw_probe: display stack end\n");
    free(buff);
    return STACK_UNKNOWN;
}

#endif
