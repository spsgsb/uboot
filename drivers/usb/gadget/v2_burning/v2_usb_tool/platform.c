// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

 #include "platform.h"
//#include "power_gate.h"
#include <asm/arch/secure_apb.h>
#include <asm/cpu_id.h>

/*CONFIG_AML_MESON_8 include m8, m8baby, m8m2, etc... defined in cpu.h*/
#if !(defined(CONFIG_USB_XHCI) || defined(CONFIG_USB_DWC_OTG_294))
#error "platform is not GX !!"
#endif//#if


#if (defined CONFIG_USB_XHCI)
#if (defined CONFIG_TXLX_USB)
#define PREI_USB_PHY_2_REG_BASE 0xffe09020
#define PREI_USB_PHY_3_REG_BASE 0xffe09080
#else
#define PREI_USB_PHY_2_REG_BASE 0xd0078020
#define PREI_USB_PHY_3_REG_BASE 0xd0078080
#endif

#if (defined CONFIG_USB_DEVICE_V2)
typedef struct u2p_aml_regs {
    volatile uint32_t u2p_r0;
    volatile uint32_t u2p_r1;
} u2p_aml_regs_t;

typedef union u2p_r0 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned host_device:1;
		unsigned power_ok:1;
		unsigned hast_mode:1;
		unsigned POR:1;
		unsigned IDPULLUP0:1;
		unsigned DRVVBUS0:1;
		unsigned reserved:26;
    } b;
} u2p_r0_t;

typedef union u2p_r1 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned phy_rdy:1;
		unsigned IDDIG0:1;
		unsigned OTGSESSVLD0:1;
		unsigned VBUSVALID0:1;
		unsigned reserved:28;
    } b;
} u2p_r1_t;

typedef struct usb_aml_regs {
    volatile uint32_t usb_r0;
    volatile uint32_t usb_r1;
    volatile uint32_t usb_r2;
    volatile uint32_t usb_r3;
    volatile uint32_t usb_r4;
    volatile uint32_t usb_r5;
} usb_aml_regs_t;

typedef union usb_r0 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned reserved:17;
		unsigned p30_lane0_tx2rx_loopback:1;
		unsigned p30_lane0_ext_pclk_reg:1;
		unsigned p30_pcs_rx_los_mask_val:10;
		unsigned u2d_ss_scaledown_mode:2;
		unsigned u2d_act:1;
    } b;
} usb_r0_t;

typedef union usb_r1 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned u3h_bigendian_gs:1;
		unsigned u3h_pme_en:1;
		unsigned u3h_hub_port_overcurrent:3;
		unsigned reserved_1:2;
		unsigned u3h_hub_port_perm_attach:3;
		unsigned reserved_2:2;
		unsigned u3h_host_u2_port_disable:2;
		unsigned reserved_3:2;
		unsigned u3h_host_u3_port_disable:1;
		unsigned u3h_host_port_power_control_present:1;
		unsigned u3h_host_msi_enable:1;
		unsigned u3h_fladj_30mhz_reg:6;
		unsigned p30_pcs_tx_swing_full:7;
    } b;
} usb_r1_t;

typedef union usb_r2 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned reserved:20;
		unsigned p30_pcs_tx_deemph_3p5db:6;
		unsigned p30_pcs_tx_deemph_6db:6;
    } b;
} usb_r2_t;

typedef union usb_r3 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned p30_ssc_en:1;
		unsigned p30_ssc_range:3;
		unsigned p30_ssc_ref_clk_sel:9;
		unsigned p30_ref_ssp_en:1;
		unsigned reserved:18;
    } b;
} usb_r3_t;

typedef union usb_r4 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned p21_PORTRESET0:1;
		unsigned p21_SLEEPM0:1;
        unsigned mem_pd:2;
		unsigned p21_only:1;
		unsigned reserved:27;
    } b;
} usb_r4_t;

typedef union usb_r5 {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
		unsigned iddig_sync:1;
		unsigned iddig_reg:1;
		unsigned iddig_cfg:2;
		unsigned iddig_en0:1;
		unsigned iddig_en1:1;
		unsigned iddig_curr:1;
		unsigned usb_iddig_irq:1;
		unsigned iddig_th:8;
		unsigned iddig_cnt:8;
		unsigned reserved:8;
    } b;
} usb_r5_t;

#define PLL_REG32_16    (0xFF63A000 + 0x40)
#define PLL_REG32_17    (0xFF63A000 + 0x44)
#define PLL_REG32_18    (0xFF63A000 + 0x48)
#define USB_PHY2_ENABLE			0x10000000
#define USB_PHY2_RESET			0x20000000

static void set_usb_phy21_pll(void)
{
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_16))
		= (USB_PHY2_PLL_PARAMETER_1 | USB_PHY2_RESET | USB_PHY2_ENABLE);
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_17)) =
		USB_PHY2_PLL_PARAMETER_2;
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_18)) =
		USB_PHY2_PLL_PARAMETER_3;
	udelay(100);
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_16))
		= (((USB_PHY2_PLL_PARAMETER_1) | (USB_PHY2_ENABLE))
		& (~(USB_PHY2_RESET)));
}

static int b_platform_usb_check_sm1 (void)
{
	int rev_flag = 0;

	cpu_id_t cpu_id = get_cpu_id();

	if (cpu_id.family_id == MESON_CPU_MAJOR_ID_SM1)
		rev_flag = 1;
	else
		rev_flag = 0;

	return rev_flag;
}

#if !defined(CONFIG_USB_AMLOGIC_PHY_V2) && !defined(USE_FULL_SPEED)
static int b_platform_usb_check_g12b_revb (void)
{
	int rev_flag = 0;

	cpu_id_t cpu_id = get_cpu_id();

	if (cpu_id.family_id == MESON_CPU_MAJOR_ID_G12B) {
		if (cpu_id.chip_rev == 0xb)
			rev_flag = 1;
		else
			rev_flag = 0;
	} else {
		rev_flag = 0;
	}

	return rev_flag;
}

static void set_usb_phy21_sm1_pll(void)
{
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_16))
		= (0x09400414 | USB_PHY2_RESET | USB_PHY2_ENABLE);
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_17)) =
		0x927e0000;
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_18)) =
		0xAC5F69E5;
	udelay(100);
	(*(volatile uint32_t *)(unsigned long)(PLL_REG32_16))
		= (((0x09400414) | (USB_PHY2_ENABLE))
		& (~(USB_PHY2_RESET)));
}
#endif

#ifdef CONFIG_USB_DEVICE_V2
#define USB_REG_B 0xFF63A000

void set_usb_phy21_tuning_update(void)
{
#if !defined(CONFIG_USB_AMLOGIC_PHY_V2) && !defined(USE_FULL_SPEED)
	unsigned long phy_reg_base = USB_REG_B;

	if (b_platform_usb_check_g12b_revb() || b_platform_usb_check_sm1()) {
		(*(volatile uint32_t *)(phy_reg_base + 0x54)) = 0x2a;
		(*(volatile uint32_t *)(phy_reg_base + 0x50)) = USB_G12x_PHY_PLL_SETTING_1;
		(*(volatile uint32_t *)(phy_reg_base + 0x54)) = 0x2a;
		(*(volatile uint32_t *)(phy_reg_base + 0x34)) = USB_G12x_PHY_PLL_SETTING_3 & (0x1f << 16);
		return;
	}

	(*(volatile uint32_t *)(phy_reg_base + 0x10)) = USB_G12x_PHY_PLL_SETTING_2;
	(*(volatile uint32_t *)(phy_reg_base + 0x50)) = USB_G12x_PHY_PLL_SETTING_1;
	(*(volatile uint32_t *)(phy_reg_base + 0x38)) = USB_G12x_PHY_PLL_SETTING_5;
	(*(volatile uint32_t *)(phy_reg_base + 0x34)) = USB_G12x_PHY_PLL_SETTING_3;
#endif
}

void set_usb_phy21_tuning_update_reset(void)
{
#if !defined(CONFIG_USB_AMLOGIC_PHY_V2) && !defined(USE_FULL_SPEED)
	unsigned long phy_reg_base = USB_REG_B;

	if (b_platform_usb_check_g12b_revb() || b_platform_usb_check_sm1())
		return;
	(*(volatile uint32_t *)(phy_reg_base + 0x38)) = 0x0;
	(*(volatile uint32_t *)(phy_reg_base + 0x34)) = USB_G12x_PHY_PLL_SETTING_3;
#endif
}

#endif

#if 0
void usb_phy21_pll_disable(void)
{
	(*(volatile uint32_t *)(unsigned long)PLL_REG32_16)
		= ((USB_PHY2_PLL_PARAMETER_1 | USB_PHY2_RESET)
		& (~(USB_PHY2_ENABLE)));
}
#endif

#if (defined CONFIG_TXLX_USB)
#define USB_RESET1       (volatile unsigned long *)0xffd01008
#else
#define USB_RESET1       (volatile unsigned long *)0xc1104408
#endif

void set_usb_phy_config(int cfg)
{
	u2p_r0_t dev_u2p_r0;
	u2p_r1_t dev_u2p_r1;

	usb_r0_t dev_usb_r0;
	usb_r4_t dev_usb_r4;

	u2p_aml_regs_t * u2p_aml_regs = (u2p_aml_regs_t * )PREI_USB_PHY_2_REG_BASE;
	usb_aml_regs_t *usb_aml_regs = (usb_aml_regs_t * )PREI_USB_PHY_3_REG_BASE;
	int cnt;
	u32 val;

	if (b_platform_usb_check_sm1() == 1) {
		val = *(volatile uint32_t *)P_AO_RTI_GEN_PWR_SLEEP0;
		*P_AO_RTI_GEN_PWR_SLEEP0 = val & (~(0x1<<17));
		val = *(volatile uint32_t *)P_AO_RTI_GEN_PWR_ISO0;
		*P_AO_RTI_GEN_PWR_ISO0 = val & (~(0x1<<17));
		val = *(volatile uint32_t *)HHI_MEM_PD_REG0;
		*P_HHI_MEM_PD_REG0 = val & (~(0x3<<30));
	}

#ifdef CONFIG_USB_DEVICE_V2
	if ((*(volatile uint32_t *)(USB_REG_B + 0x38)) != 0) {
		set_usb_phy21_tuning_update_reset();
		mdelay(150);
	}
#endif
	//step 1: usb controller reset
	*USB_RESET1 |= (1<<2);

	// step 3: enable usb INT internal USB
	dev_usb_r0.d32	 = usb_aml_regs->usb_r0;
	dev_usb_r0.b.u2d_ss_scaledown_mode = 0;
	dev_usb_r0.b.u2d_act			   = 1;
	usb_aml_regs->usb_r0 = dev_usb_r0.d32;

	// step 4: disable usb phy sleep
	dev_usb_r4.d32	 = usb_aml_regs->usb_r4;
	dev_usb_r4.b.p21_SLEEPM0   = 1;
	usb_aml_regs->usb_r4   = dev_usb_r4.d32;

	// step 5: config phy21 device mode
	dev_u2p_r0.d32	 = u2p_aml_regs->u2p_r0;
	dev_u2p_r0.b.host_device= 0;
	dev_u2p_r0.b.POR= 0;
	u2p_aml_regs->u2p_r0  = dev_u2p_r0.d32;

	udelay(10);
	//step 6: phy21 reset
	*USB_RESET1 |= (1<<17);

	udelay(50);
	// step 6: wait for phy ready
	dev_u2p_r1.d32	= u2p_aml_regs->u2p_r1;
	cnt = 0;
	while ((dev_u2p_r1.d32 & 0x00000001) != 1) {
		dev_u2p_r1.d32 = u2p_aml_regs->u2p_r1;
		if (cnt > 200)
			break;
		else {
			cnt++;
			udelay(5);
		}
	}

#ifndef CONFIG_USB_AMLOGIC_PHY_V2
	if (b_platform_usb_check_sm1())
		set_usb_phy21_sm1_pll();
	else
		set_usb_phy21_pll();
#else
	set_usb_phy21_pll();
#endif

	//--------------------------------------------------

	// ------------- usb phy21 initinal end ----------

	//--------------------------------------------------

}

#else
typedef struct u2p_aml_regs {
	volatile uint32_t u2p_r0;
	volatile uint32_t u2p_r1;
	volatile uint32_t u2p_r2;
} u2p_aml_regs_t;

typedef union u2p_r0 {
	/** raw register data */
	uint32_t d32;
	/** register bits */
	struct {
		unsigned bypass_sel:1;   // 0
		unsigned bypass_dm_en:1; // 1
		unsigned bypass_dp_en:1; // 2
		unsigned txbitstuffenh:1;// 3
		unsigned txbitstuffen:1; // 4
		unsigned dmpulldown:1;   // 5
		unsigned dppulldown:1;   // 6
		unsigned vbusvldextsel:1;// 7
		unsigned vbusvldext:1;   // 8
		unsigned adp_prb_en:1;   // 9
		unsigned adp_dischrg:1;  // 10
		unsigned adp_chrg:1;     // 11
		unsigned drvvbus:1;      // 12
		unsigned idpullup:1;     // 13
		unsigned loopbackenb:1;  // 14
		unsigned otgdisable:1;   // 15
		unsigned commononn:1;    // 16
		unsigned fsel:3;         // 17
		unsigned refclksel:2;    // 20
		unsigned por:1;          // 22
		unsigned vatestenb:2;    // 23
		unsigned set_iddq:1;     // 25
		unsigned ate_reset:1;    // 26
		unsigned fsv_minus:1;    // 27
		unsigned fsv_plus:1;     // 28
		unsigned bypass_dm_data:1; // 29
		unsigned bypass_dp_data:1; // 30
		unsigned not_used:1;
	} b;
} u2p_r0_t;

typedef struct usb_aml_regs {
	volatile uint32_t usb_r0;
	volatile uint32_t usb_r1;
	volatile uint32_t usb_r2;
	volatile uint32_t usb_r3;
	volatile uint32_t usb_r4;
	volatile uint32_t usb_r5;
	volatile uint32_t usb_r6;
} usb_aml_regs_t;

typedef union usb_r0 {
	/** raw register data */
	uint32_t d32;
	/** register bits */
	struct {
		unsigned p30_fsel:6; // 0
		unsigned p30_phy_reset:1; // 6
		unsigned p30_test_powerdown_hsp:1; // 7
		unsigned p30_test_powerdown_ssp:1; // 8
		unsigned p30_acjt_level:5;         // 9
		unsigned p30_tx_vboost_lvl:3;      // 14
		unsigned p30_lane0_tx2rx_loopbk:1; // 17
		unsigned p30_lane0_ext_pclk_req:1; // 18
		unsigned p30_pcs_rx_los_mask_val:10; // 19
		unsigned u2d_ss_scaledown_mode:2;  // 29
		unsigned u2d_act:1; // 31
	} b;
} usb_r0_t;

typedef union usb_r4 {
	/** raw register data */
	uint32_t d32;
	/** register bits */
	struct {
		unsigned p21_PORTRESET0:1; // 0
		unsigned p21_SLEEPM0:1; // 1
		unsigned mem_pd:2;
		unsigned reserved4:28; // 31
	} b;
} usb_r4_t;
//#define   P_AO_RTC_ALT_CLK_CNTL0                               (volatile uint32_t *)(0xc8100000 + (0x25 << 2))
//#define   P_AO_RTI_PWR_CNTL_REG0                               (volatile uint32_t *)(0xc8100000 + (0x04 << 2))

void set_usb_phy_config(int cfg)
{
    const int time_dly = 500;
    u2p_aml_regs_t * u2p_aml_regs = (u2p_aml_regs_t * )PREI_USB_PHY_2_REG_BASE;
    usb_aml_regs_t * usb_aml_regs = (usb_aml_regs_t * )PREI_USB_PHY_3_REG_BASE;

    u2p_r0_t u2p_r0;
    usb_r0_t usb_r0;
    usb_r4_t usb_r4;
    //if (!IS_CLK_GATE_ON(USB0)) {
    //        SET_CBUS_REG_MASK(GCLK_REG_USB0, GCLK_MASK_USB0);
    //}
    /*printf("%s %d\n", __func__, __LINE__);*/
    cfg = cfg;//avoid compiler warning
    *P_RESET1_REGISTER = (1<<2);//usb reset
	//writel((1 << 2),P_RESET1_REGISTER);	//usb reset
    //udelay(time_dly);//by Sam: delay after reset

	*P_AO_RTC_ALT_CLK_CNTL0 |= (1<<31)|(1<<30);
	*P_AO_RTI_PWR_CNTL_REG0 |= (4<<10);

	u2p_r0.d32 = u2p_aml_regs->u2p_r0;
#if (defined  CONFIG_AML_MESON_GXTVBB)
	u2p_r0.b.fsel = 5;

#elif  (defined CONFIG_AML_MESON_GXL)
	u2p_r0.b.fsel = 2;
#endif
    u2p_r0.b.por = 1;
	u2p_r0.b.dppulldown = 0;
	u2p_r0.b.dmpulldown = 0;
	u2p_aml_regs->u2p_r0 = u2p_r0.d32;

	u2p_r0.d32 = u2p_aml_regs->u2p_r0;
	u2p_r0.b.por = 0;
	u2p_aml_regs->u2p_r0 = u2p_r0.d32;

	usb_r0.d32 = usb_aml_regs->usb_r0;
	usb_r0.b.u2d_act = 1;
	usb_aml_regs->usb_r0 = usb_r0.d32;

    usb_r4.d32 = usb_aml_regs->usb_r4;
    usb_r4.b.p21_SLEEPM0 = 1;
    usb_aml_regs->usb_r4 = usb_r4.d32;

    udelay(time_dly);
    return;
}
#endif

//sleep sometime before and after disconnect,
//      to let usb_burning_tool.exe detect both fast plug-out and plug-in
void close_usb_phy_clock(int cfg)
{
    cfg = cfg;//avoid compiler warning

    run_command("sleep 1", 0);//improve pc compatibility!!
    dwc_otg_pullup(0);//disconnect
    __udelay(20);
    /*dwc_otg_power_off_phy();*///Don't call this as it may cause pull-down failed!!!!
    run_command("sleep 1", 0);

    return;
}
#endif

#if (defined CONFIG_USB_DWC_OTG_294)

/*
   cfg = 0 : EXT clock
   cfg = 1 : INT clock
  */

#if 1
#define PREI_USB_PHY_A_REG_BASE       0xC0000000  //0x2100
#define PREI_USB_PHY_B_REG_BASE       0xC1108420	//0X2108
#else
#define PREI_USB_PHY_A_REG_BASE       P_USB_ADDR0
#define PREI_USB_PHY_B_REG_BASE       P_USB_ADDR1
#endif//#if 0

#ifdef __USE_PORT_B
#define PREI_USB_PHY_REG_BASE   PREI_USB_PHY_B_REG_BASE
#else
#define PREI_USB_PHY_REG_BASE   PREI_USB_PHY_A_REG_BASE
#endif
//#define P_RESET1_REGISTER						   (volatile unsigned long *)0xc1104408
#define P_RESET1_REGISTER_USB                           (volatile unsigned long *)0xc1104408

#define USB_CLK_SEL_XTAL				0
#define USB_CLK_SEL_XTAL_DIV_2	1
#define USB_CLK_SEL_DDR_PLL			2
#define USB_CLK_SEL_MPLL_OUT0		3
#define USB_CLK_SEL_MPLL_OUT1		4
#define USB_CLK_SEL_MPLL_OUT2		5
#define USB_CLK_SEL_FCLK_DIV2		6
#define USB_CLK_SEL_FCLK_DIV3		7

typedef struct usb_aml_regs {
    volatile uint32_t config;
    volatile uint32_t ctrl;
    volatile uint32_t endp_intr;
    volatile uint32_t adp_bc;
    volatile uint32_t dbg_uart;
    volatile uint32_t test;
    volatile uint32_t tune;
} usb_aml_regs_t;

typedef union usb_config_data {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
        unsigned clk_en     :1;
        unsigned clk_sel    :3;
        unsigned clk_div    :7;
        unsigned reserved0  :1;
        unsigned clk_32k_alt_sel:1;
        unsigned reserved1  :15;
        unsigned test_trig  :1;
    } b;
} usb_config_data_t;

typedef union usb_ctrl_data {
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct {
        unsigned soft_prst:1;
        unsigned soft_hreset:1;
        unsigned ss_scaledown_mode:2;
        unsigned clk_det_rst:1;
        unsigned intr_sel:1;
        unsigned reserved:2;
        unsigned clk_detected:1;
        unsigned sof_sent_rcvd_tgl:1;
        unsigned sof_toggle_out:1;
        unsigned not_used:4;
        unsigned por:1;
        unsigned sleepm:1;
        unsigned txbitstuffennh:1;
        unsigned txbitstuffenn:1;
        unsigned commononn:1;
        unsigned refclksel:2;
        unsigned fsel:3;
        unsigned portreset:1;
        unsigned thread_id:6;
    } b;
} usb_ctrl_data_t;

void set_usb_phy_config(int cfg)
{

	const int time_dly = 5000;
	usb_aml_regs_t * usb_aml_regs = (usb_aml_regs_t * )PREI_USB_PHY_REG_BASE;
	usb_config_data_t config;
	usb_ctrl_data_t control;

	/*CLK_GATE_ON(USB0);*/
	//if(!IS_CLK_GATE_ON(USB0)){
	//	SET_CBUS_REG_MASK(GCLK_REG_USB0, GCLK_MASK_USB0);
	//}
	/*printf("%s %d\n", __func__, __LINE__);*/
	cfg = cfg;//avoid compiler warning
	/**P_RESET1_REGISTER = (1<<2);//usb reset*/
	*P_RESET1_REGISTER_USB = (1<<2);//usb reset
	udelay(time_dly);//by Sam: delay after reset

	config.d32 = usb_aml_regs->config;

//    config.b.clk_sel    = 0;
//    config.b.clk_div    = 1;
//    config.b.clk_32k_alt_sel = 1;
	usb_aml_regs->config = config.d32;

	control.d32 = usb_aml_regs->ctrl;
	control.b.fsel = 5;
	control.b.por = 1;
	usb_aml_regs->ctrl = control.d32;
	udelay(time_dly);

	control.b.por = 0;
	usb_aml_regs->ctrl = control.d32;
	udelay(time_dly);//by Sam: delay 0.5s to wait usb clam down

	control.d32 = usb_aml_regs->ctrl;
	if (!control.b.clk_detected) {
		printf("Error, usb phy clock not detected!\n");
	}

    return;
}

#if 0
int chip_watchdog(void)
{
	watchdog_clear();
	return 0;
};
#endif

//sleep sometime before and after disconnect,
//      to let usb_burning_tool.exe detect both fast plug-out and plug-in
void close_usb_phy_clock(int cfg)
{
        cfg = cfg;//avoid compiler warning

        run_command("sleep 1", 0);//sleep sometime to improve pc compatibility!!
        dwc_otg_pullup(0);//disconnect
        __udelay(20);
        /*dwc_otg_power_off_phy();*///Don't call this as it may cause pull-down failed!!!!
        run_command("sleep 1", 0);//sleep sometime to improve pc compatibility!!

        return;
}
#endif

