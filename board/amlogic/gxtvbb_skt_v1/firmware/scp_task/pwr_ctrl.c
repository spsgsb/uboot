// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <config.h>

static unsigned int pwm_voltage_table[][2] = {
	{0x180004, 900},
	{0x170005, 910},
	{0x160006, 920},
	{0x150007, 930},
	{0x140008, 940},
	{0x130009, 950},
	{0x12000a, 960},
	{0x11000b, 970},
	{0x10000c, 980},
	{0x0f000d, 990},
	{0x0e000e, 1000},
	{0x0d000f, 1010},
	{0x0c0010, 1020},
	{0x0b0011, 1030},
	{0x0a0012, 1040},
	{0x090013, 1050},
	{0x080014, 1060},
	{0x070015, 1070},
	{0x060016, 1080},
	{0x050017, 1090},
	{0x040018, 1100},
	{0x030019, 1110},
	{0x02001a, 1120},
	{0x01001b, 1130},
	{0x00001c, 1140},
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define P_PIN_MUX_REG3		(*((volatile unsigned *)(0xda834400 + (0x2f << 2))))
#define P_PIN_MUX_REG7		(*((volatile unsigned *)(0xda834400 + (0x33 << 2))))

#define P_PWM_MISC_REG_AB	(*((volatile unsigned *)(0xc1100000 + (0x2156 << 2))))
#define P_PWM_PWM_B		(*((volatile unsigned *)(0xc1100000 + (0x2155 << 2))))
#define P_PWM_MISC_REG_CD	(*((volatile unsigned *)(0xc1100000 + (0x2196 << 2))))
#define P_PWM_PWM_D		(*((volatile unsigned *)(0xc1100000 + (0x2195 << 2))))
#define P_AO_PWM_PWM_B1		(*((volatile unsigned *)(0xc8100400 + (0x55 << 2))))
#define P_EE_TIMER_E		(*((volatile unsigned *)(0xc1100000 + (0x2662 << 2))))

enum pwm_id {
	pwm_a = 0,
	pwm_b,
	pwm_c,
	pwm_d,
	pwm_e,
	pwm_f,
	pwm_ao_a,
	pwm_ao_b,
};

static unsigned int ao_timer_ctrl;
static unsigned int ao_timera;

static void reset_ao_timera(void)
{
	unsigned int val;
	ao_timer_ctrl = readl(AO_TIMER_REG);
	ao_timera = readl(AO_TIMERA_REG);
	/* set ao timera work mode and interrrupt time 100us resolution*/
	val = (1 << 2) | (3 << 0) | (1 << 3);
	writel(val, AO_TIMER_REG);
	/* periodic time 10ms */
	writel(100,AO_TIMERA_REG);
}

static void restore_ao_timer(void)
{
	writel(ao_timer_ctrl,AO_TIMER_REG);
	writel(ao_timera,AO_TIMERA_REG);
}

static void pwm_set_voltage(unsigned int id, unsigned int voltage)
{
	int to;
	return;
	uart_puts("set vddee to 0x");
	uart_put_hex(voltage, 16);
	uart_puts("mv\n");
	for (to = 0; to < ARRAY_SIZE(pwm_voltage_table); to++) {
		if (pwm_voltage_table[to][1] >= voltage) {
			break;
		}
	}
	if (to >= ARRAY_SIZE(pwm_voltage_table)) {
		to = ARRAY_SIZE(pwm_voltage_table) - 1;
	}
	switch (id) {
	case pwm_b:
		P_PWM_PWM_B = pwm_voltage_table[to][0];
		break;

	case pwm_ao_b:
		P_AO_PWM_PWM_B1 = pwm_voltage_table[to][0];
		break;
	case pwm_d:
		P_PWM_PWM_D = pwm_voltage_table[to][0];
		break;
	default:
		break;
	}
	_udelay(200);
}

static void power_off_3v3_5v(void)
{
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 2, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 18, 1 << 18);
}

static void power_on_3v3_5v(void)
{
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 2, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 18, 0);
}

static void power_off_usb5v(void)
{
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 5, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 21, 0);
}

static void power_on_usb5v(void)
{
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 5, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 21, 1 << 21);
}

static void power_off_at_clk81(void)
{
	pwm_set_voltage(pwm_ao_b, CONFIG_VDDEE_SLEEP_VOLTAGE);	// reduce power
	power_off_3v3_5v();
	power_off_usb5v();
}

static void power_on_at_clk81(unsigned int suspend_from)
{
	pwm_set_voltage(pwm_ao_b, CONFIG_VDDEE_INIT_VOLTAGE);
	power_on_usb5v();
	power_on_3v3_5v();
	_udelay(6000);
}

static void power_off_at_24M(void)
{
}

static void power_on_at_24M(void)
{
}

static void power_off_ddr(void)
{
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 3, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 19, 0);
}

static void power_on_ddr(void)
{
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 3, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 19, 1 << 19);
}

static void power_off_ee(void)
{
	pwm_set_voltage(pwm_ao_b, CONFIG_VDDEE_INIT_VOLTAGE);
	writel(readl(AO_RTI_PWR_CNTL_REG0)
	       | ((0x3 << 3) | (0x1 << 1)), AO_RTI_PWR_CNTL_REG0);
	_udelay(100);
	writel(readl(AO_RTI_PWR_CNTL_REG0) & (~(0x1 << 9)),
	       AO_RTI_PWR_CNTL_REG0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 8, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 24, 1 << 24);
}

static void power_on_ee(void)
{
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 8, 0);
	aml_update_bits(AO_GPIO_O_EN_N, 1 << 24, 0);
	_udelay(10000);
	pwm_set_voltage(pwm_ao_b, CONFIG_VDDEE_INIT_VOLTAGE);
	writel(readl(AO_RTI_PWR_CNTL_REG0) | (0x1 << 9), AO_RTI_PWR_CNTL_REG0);
	_udelay(500);
	writel(readl(AO_RTI_PWR_CNTL_REG0)
	       & (~((0x3 << 3) | (0x1 << 1))), AO_RTI_PWR_CNTL_REG0);
}

static void power_off_at_32k(unsigned int suspend_from)
{
	if (suspend_from == SYS_POWEROFF) {
		power_off_ddr();
		power_off_ee();
	}
}

static void power_on_at_32k(unsigned int suspend_from)
{
	if (suspend_from == SYS_POWEROFF) {
		power_on_ee();
		power_on_ddr();
	}
}

static void wakeup_timer_setup(void)
{
	/* 1ms resolution*/
	unsigned value;
	value = readl(P_ISA_TIMER_MUX);
	value |= ((0x3<<0) | (0x1<<12) | (0x1<<16));
	writel(value, P_ISA_TIMER_MUX);
	writel(9, P_ISA_TIMERA);
}
static void wakeup_timer_clear(void)
{
	unsigned value;
	value = readl(P_ISA_TIMER_MUX);
	value &= ~((0x1<<12) | (0x1<<16));
	writel(value, P_ISA_TIMER_MUX);
}

static void get_wakeup_source(void *response, unsigned int suspend_from)
{
	struct wakeup_info *p = (struct wakeup_info *)response;
	unsigned val;
	unsigned i = 0;

	p->gpio_info_count = i;
	p->status = RESPONSE_OK;
	val = (AUTO_WAKEUP_SRC | REMOTE_WAKEUP_SRC);
#ifdef CONFIG_BT_WAKEUP
	val |= BT_WAKEUP_SRC;
#endif

#ifdef CONFIG_CEC_WAKEUP
	if (suspend_from != SYS_POWEROFF)
		val |= CEC_WAKEUP_SRC;
#endif
#ifdef CONFIG_WIFI_WAKEUP
	if (suspend_from != SYS_POWEROFF)
		val |= WIFI_WAKEUP_SRC;
#endif

	p->sources = val;

#ifdef CONFIG_BT_WAKEUP
	{
		struct wakeup_gpio_info *gpio;
		/* BT Wakeup: IN: GPIOX[21], OUT: GPIOX[20] */
		gpio = &(p->gpio_info[i]);
		gpio->wakeup_id = BT_WAKEUP;
		gpio->gpio_in_idx = GPIOX_21;
		gpio->gpio_in_ao = 0;
		gpio->gpio_out_idx = GPIOX_20;
		gpio->gpio_out_ao = 0;
		gpio->irq = IRQ_GPIO0_NUM;
		gpio->trig_type = GPIO_IRQ_FALLING_EDGE;
		p->gpio_info_count = ++i;
	}
#endif

#ifdef CONFIG_WIFI_WAKEUP
	if (suspend_from != SYS_POWEROFF) {
		struct wakeup_gpio_info *gpio;
		/*WIFI Wakeup: IN: GPIOX[7], OUT: GPIOX[6] */
		gpio = &(p->gpio_info[i]);
		gpio->wakeup_id = WIFI_WAKEUP;
		gpio->gpio_in_idx = GPIOX_7;
		gpio->gpio_in_ao = 0;
		gpio->gpio_out_idx = GPIOX_6;
		gpio->gpio_out_ao = 0;
		gpio->irq = IRQ_GPIO1_NUM;
		gpio->trig_type = GPIO_IRQ_FALLING_EDGE;
		p->gpio_info_count = ++i;
	}
#endif

}
static unsigned int detect_key(unsigned int suspend_from)
{
	int exit_reason = 0;
	unsigned int time_out = readl(AO_DEBUG_REG2);
	unsigned time_out_ms = time_out*100;
	unsigned char adc_key_cnt = 0;
	unsigned *irq = (unsigned *)WAKEUP_SRC_IRQ_ADDR_BASE;
	/* unsigned *wakeup_en = (unsigned *)SECURE_TASK_RESPONSE_WAKEUP_EN; */

	/* setup wakeup resources */
	/*auto suspend: timerA 10ms resolution */
	if (time_out_ms != 0) {
		wakeup_timer_setup();
		if (suspend_from == SYS_POWEROFF) {
			time_out_ms = time_out * 1000;
			while (1) {
				_udelay(1000);
				time_out_ms--;
				if (time_out_ms == 0)
					return AUTO_WAKEUP;

			}
		}
	}
	saradc_enable();
	reset_ao_timera();
	init_remote();
#ifdef CONFIG_CEC_WAKEUP
	if (hdmi_cec_func_config & 0x1) {
		remote_cec_hw_reset();
		cec_node_init();
	}
#endif

	/* *wakeup_en = 1;*/
	do {
#ifdef CONFIG_CEC_WAKEUP
		if (irq[IRQ_AO_CEC] == IRQ_AO_CEC_NUM) {
			irq[IRQ_AO_CEC] = 0xFFFFFFFF;
			if (suspend_from == SYS_POWEROFF)
				continue;
			if (cec_msg.log_addr) {
				if (hdmi_cec_func_config & 0x1) {
					cec_handler();
					if (cec_msg.cec_power == 0x1) {
						/*cec power key*/
						exit_reason = CEC_WAKEUP;
						break;
					}
				}
			} else if (hdmi_cec_func_config & 0x1)
				cec_node_init();
		}
#endif
		if (irq[IRQ_TIMERA] == IRQ_TIMERA_NUM) {
			irq[IRQ_TIMERA] = 0xFFFFFFFF;
			if (time_out_ms != 0)
				time_out_ms--;
			if (time_out_ms == 0) {
				wakeup_timer_clear();
				exit_reason = AUTO_WAKEUP;
			}
		}
		if (irq[IRQ_AO_TIMERA] == IRQ_AO_TIMERA_NUM) {
			irq[IRQ_AO_TIMERA] = 0xFFFFFFFF;
			if (check_adc_key_resume()) {
				adc_key_cnt++;
				/*using variable 'adc_key_cnt' to eliminate the dithering of the key*/
				if (2 == adc_key_cnt)
					exit_reason = POWER_KEY_WAKEUP;
			} else {
				adc_key_cnt = 0;
			}
		}
		if (irq[IRQ_AO_IR_DEC] == IRQ_AO_IR_DEC_NUM) {
			irq[IRQ_AO_IR_DEC] = 0xFFFFFFFF;
			if (remote_detect_key())
				exit_reason = REMOTE_WAKEUP;
		}

#ifdef CONFIG_BT_WAKEUP
		if (irq[IRQ_GPIO0] == IRQ_GPIO0_NUM) {
			irq[IRQ_GPIO0] = 0xFFFFFFFF;
			if (!(readl(PREG_PAD_GPIO4_EN_N)
			      & (0x01 << 20)) && (readl(PREG_PAD_GPIO4_O)
						  & (0x01 << 20)) &&
			    !(readl(PREG_PAD_GPIO4_I)
			      & (0x01 << 21)))
				exit_reason = BT_WAKEUP;
		}
#endif
#ifdef CONFIG_WIFI_WAKEUP
		if (irq[IRQ_GPIO1] == IRQ_GPIO1_NUM) {
			irq[IRQ_GPIO1] = 0xFFFFFFFF;
			if (suspend_from) {
				if (!(readl(PREG_PAD_GPIO4_EN_N)
				      & (0x01 << 6)) && (readl(PREG_PAD_GPIO4_O)
							 & (0x01 << 6)) &&
				    !(readl(PREG_PAD_GPIO4_I)
				      & (0x01 << 7)))
					exit_reason = WIFI_WAKEUP;
			}
		}
#endif
		if (exit_reason)
			break;
		else
			asm volatile("wfi");
	} while (1);
	wakeup_timer_clear();
	restore_ao_timer();
	saradc_disable();
	return exit_reason;
}

static void pwr_op_init(struct pwr_op *pwr_op)
{
	pwr_op->power_off_at_clk81 = power_off_at_clk81;
	pwr_op->power_on_at_clk81 = power_on_at_clk81;
	pwr_op->power_off_at_24M = power_off_at_24M;
	pwr_op->power_on_at_24M = power_on_at_24M;
	pwr_op->power_off_at_32k = power_off_at_32k;
	pwr_op->power_on_at_32k = power_on_at_32k;

	pwr_op->detect_key = detect_key;
	pwr_op->get_wakeup_source = get_wakeup_source;
}
