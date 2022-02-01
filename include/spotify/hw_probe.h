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
    STACK_HOLITECH,
    STACK_UNKNOWN = 0xff
};
typedef enum sp_display_stack_e sp_display_stack;

struct sp_display_stack_probe_s {
    uint8_t vendor_id;
    uint16_t hw_id;
    uint8_t conf_ver;
};
typedef struct sp_display_stack_probe_s sp_display_stack_probe;

struct sp_display_stack_conf_s {
    uint8_t vendor_id;
    uint16_t hw_id;
    sp_display_stack id;
};
typedef struct sp_display_stack_conf_s sp_display_stack_conf;

#define TOUCH_ADDR 0x2e
#define TOUCH_DMA_REG 0x42bd
#define TOUCH_MODE_REG 0x01
#define TOUCH_MCCODE_REG 0x8000
#define TOUCH_MCCODE_ALT_REG 0x09
#define TOUCH_CONFIG_0_REG 0xd6e0
#define TOUCH_CONFIG_1_REG 0x9e00

// tlsc6x: cfg_ver: 0x180e221a, chip_code: 0x65c, ver: 6, vendor_id: 0x11
#define BOE_VENDOR_ID 0x11
#define BOE_HW_ID 0x65c

// tlsc6x: cfg_ver: 0xc0ee003, chip_code: 0x2d5c, ver: 3, vendor_id: 0x70
#define WILY_VENDOR_ID 0x70
#define WILY_HW_ID 0x2d5c

// tlsc6x: cfg_ver: 0x40e223d, chip_code: 0xd5c, ver: 1, vendor_id: 0x11
#define HOLITECH_VENDOR_ID 0x11
#define HOLITECH_HW_ID 0xd5c

static const sp_display_stack_conf sp_display_stack_confs[] = {
    { BOE_VENDOR_ID, BOE_HW_ID, STACK_BOE },
    { WILY_VENDOR_ID, WILY_HW_ID, STACK_WILY },
    { HOLITECH_VENDOR_ID, HOLITECH_HW_ID, STACK_HOLITECH },
};

static const size_t sp_display_stack_confs_len = 3;

typedef int32_t (*sp_plat_i2c_read)(uint32_t addr, uint32_t reg, uint8_t* buffer, size_t buffer_len);
typedef int32_t (*sp_plat_i2c_write)(uint32_t addr, uint32_t reg, uint8_t* buffer, size_t buffer_len);

struct sp_plat_i2c_ops_s {
    sp_plat_i2c_read read;
    sp_plat_i2c_write write;
};
typedef struct sp_plat_i2c_ops_s sp_plat_i2c_ops;

// NOLINTNEXTLINE(misc-definitions-in-headers)
sp_board_revision sp_probe_board_revision(void) {
    return REV_A;
}

// NOLINTNEXTLINE(misc-definitions-in-headers)
int32_t sp_display_stack_detect(sp_plat_i2c_ops* ops, sp_display_stack_probe* result) {
    uint16_t conf_reg;
    unsigned char cmd_2dma_42bd[6] = { 0x28, 0x35, 0xc1, 0x00, 0x35, 0xae };
    uint8_t buff[4];

    printf("sp_hw_probe: i2c start\n");
    int ret, i, mccode;

    i = 5;
    do {
        if(i <= 0) {
            printf("sp_hw_probe: failed to enable dma!\n");
            goto error;
        }

        ret = ops->write(TOUCH_ADDR, TOUCH_DMA_REG, cmd_2dma_42bd, 6);
        printf("sp_hw_probe: trying to enable dma...\n");
        if (ret != 0)
            goto error;

        udelay(50000);

        ret = ops->read(TOUCH_ADDR, TOUCH_MODE_REG, buff, 3);
        if (ret != 0)
            goto error;

        --i;
    } while (!((0x2e == (buff[0] >> 1 )) && buff[2] == 0x01));
    printf("sp_hw_probe: enabled dma\n");


    uint32_t tmp[3];
    mccode = 0xff;
    ret = ops->read(TOUCH_ADDR, TOUCH_MCCODE_REG, (uint8_t*)&tmp, 12);
    if (ret != 0)
        goto error;

    if (tmp[2] == 0x544c4e4b) {
        switch(tmp[0]) {
            case 0x35368008:
                mccode = 1;
                break;
            case 0x35358008:
            default:
                mccode = 0;
                break;
        }
    } else {
        ret = ops->read(TOUCH_ADDR, TOUCH_MCCODE_ALT_REG, (uint8_t*)&tmp, 3);
        if (ret != 0)
            goto error;

        switch (tmp[0]) {
            case 0x444240:
            case 0x5c5c5c:
                mccode = 1;
            default:
                mccode = 0;
        }
    }

    if (mccode == 0xff) {
        printf("sp_hw_probe: failed to get mccode!");
        goto error;
    }

    printf("sp_hw_probe: mccode = %d\n", mccode);

    switch (mccode) {
        case 0:
            conf_reg = TOUCH_CONFIG_0_REG;
        case 1:
            conf_reg = TOUCH_CONFIG_1_REG;
    }

    uint16_t conf_buff[102];
    uint8_t conf_buff_len = 204;
    ret = ops->read(TOUCH_ADDR, conf_reg, (uint8_t*)conf_buff, conf_buff_len);
    if (ret != 0)
        goto error;

    printf("sp_hw_probe: read conf\n");

    uint32_t ver = (uint32_t)conf_buff[1];
    ver = (ver << 16) + (uint32_t)conf_buff[0];
    uint16_t chip_code = conf_buff[53];

    uint32_t conf_ver = ver >> 26;
    uint32_t vendor_id = (ver >> 9) & 0x7f;
    printf("sp_hw_probe: chip_code = 0x%x\n", chip_code);
    printf("sp_hw_probe: conf_ver = 0x%x\n", conf_ver);
    printf("sp_hw_probe: vendor_id = 0x%x\n", vendor_id);

    result->vendor_id = vendor_id;
    result->hw_id = chip_code;
    result->conf_ver = conf_ver;

    printf("sp_hw_probe: i2c end\n");
    return 0;
error:
    printf("sp_hw_probe: i2c error!\n");
    return -1;
}

// NOLINTNEXTLINE(misc-definitions-in-headers)
sp_display_stack sp_probe_display_stack(sp_plat_i2c_ops* ops) {
    printf("sp_hw_probe: display stack start\n");
    uint32_t r = -1;
    int i;
    sp_display_stack_conf c;
    sp_display_stack_probe p;

    r = sp_display_stack_detect(ops, &p);
    if (r != 0)
        goto error;

    for (i = 0; i < sp_display_stack_confs_len; i++) {
        c = sp_display_stack_confs[i];

        if (c.vendor_id == p.vendor_id && c.hw_id == p.hw_id) {
            printf("sp_hw_probe: display stack end\n");
            return c.id;
        }
    }

error:
    printf("sp_hw_probe: display stack error\n");
    return STACK_UNKNOWN;
}

#endif
