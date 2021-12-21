/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2021 Spotify AB. All rights reserved.
 */

#ifndef __SPOTIFY_HW_PROBE__
#define __SPOTIFY_HW_PROBE__

enum sp_board_revision_e {
    REV_A = 0,
    REV_B,
    REV_UNKNOWN
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
#define BOE_I2C_REG 0x00
#define BOE_HW_ID 0x00

#define WILY_I2C_ADDR 0x00
#define WILY_I2C_REG 0x00
#define WILY_HW_ID 0x00

static const sp_display_stack_conf sp_display_stack_confs[] = {
    { BOE_I2C_ADDR, BOE_I2C_REG, BOE_HW_ID, STACK_BOE },
    { WILY_I2C_ADDR, WILY_I2C_REG, WILY_HW_ID, STACK_WILY },
};

static const int sp_display_stack_confs_len = 2;

// ex: ret = plat_i2c_read(addr, reg);
typedef int (*sp_plat_i2c_read)(int, int);


// NOLINTNEXTLINE(misc-definitions-in-headers)
sp_board_revision sp_probe_board_revision(void) {
    return REV_A;
}

// NOLINTNEXTLINE(misc-definitions-in-headers)
sp_display_stack sp_probe_display_stack(sp_plat_i2c_read plat_i2c_read) {
    uint32_t r = -1;
    int i;
    sp_display_stack_conf c;

    for (i = 0; i < sp_display_stack_confs_len; i++) {
        c = sp_display_stack_confs[i];
        r = plat_i2c_read(c.i2c_addr, c.i2c_reg);

        if (r == c.hw_id) {
            return c.id;
        }
    }

    return STACK_UNKNOWN;
}

#endif
