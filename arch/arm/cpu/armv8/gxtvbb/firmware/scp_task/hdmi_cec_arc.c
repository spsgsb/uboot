// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifdef CONFIG_CEC_WAKEUP
#include "secure_apb.h"
#include "cec_tx_reg.h"
#ifndef NULL
#define NULL ((void *)0)
#endif
#define CEC_DBG_PRINT
#ifdef CEC_DBG_PRINT
static void cec_dbg_print(char *s, int v)
{
	uart_puts(s);
	uart_put_hex(v,8);
	_udelay(100);
}
static void cec_dbg_prints(char *s)
{
	uart_puts(s);
	_udelay(100);
}
#else
	#define cec_dbg_print(s,v)
	#define cec_dbg_prints(s)
#endif

static void cec_reset_addr(void);
struct cec_tx_msg_t {
	unsigned char buf[16];
	unsigned char retry;
	unsigned char len;
};

#define CEX_TX_MSG_BUF_NUM	  4
#define CEC_TX_MSG_BUF_MASK	 (CEX_TX_MSG_BUF_NUM - 1)

struct cec_tx_msg {
	struct cec_tx_msg_t msg[CEX_TX_MSG_BUF_NUM];
	unsigned char send_idx;
	unsigned char queue_idx;
};

struct cec_tx_msg cec_tx_msgs = {};


static int cec_strlen(char *p)
{
	int i=0;

	while (*p++)
		i++;
	return i;
}

static void *cec_memcpy(void *memto, const void *memfrom, unsigned int size)
{
	char *tempfrom = (char *)memfrom;
	char *tempto = (char *)memto;

	if ((memto == NULL) || (memfrom == NULL))
		return NULL;
	while (size -- > 0)
		*tempto++ = *tempfrom++;
	return memto;
}

static void waiting_aocec_free(void) {
	do {
		unsigned long cnt = 0;
		while (readl(P_AO_CEC_RW_REG) & (1<<23))
		{
			if (5000 == cnt++)
			{
				break;
			}
		}
	} while(0);
}

static unsigned long cec_rd_reg(unsigned long addr)
{
	unsigned long data32;
	waiting_aocec_free();
	data32  = 0;
	data32 |= 0    << 16;  // [16]   cec_reg_wr
	data32 |= 0    << 8;   // [15:8] cec_reg_wrdata
	data32 |= addr << 0;   // [7:0]  cec_reg_addr
	writel(data32, P_AO_CEC_RW_REG);
	waiting_aocec_free();
	data32 = ((readl(P_AO_CEC_RW_REG)) >> 24) & 0xff;
	return (data32);
} /* cec_rd_reg */

static void cec_wr_reg (unsigned long addr, unsigned long data)
{
	unsigned long data32;
	waiting_aocec_free();
	data32  = 0;
	data32 |= 1    << 16;  // [16]   cec_reg_wr
	data32 |= data << 8;   // [15:8] cec_reg_wrdata
	data32 |= addr << 0;   // [7:0]  cec_reg_addr
	writel(data32, P_AO_CEC_RW_REG);
} /* aocec_wr_only_reg */

static void cec_rx_read_pos_plus(void)
{
	(cec_msg.rx_read_pos ==  cec_msg.rx_buf_size - 1) ?
				(cec_msg.rx_read_pos = 0) :
				(cec_msg.rx_read_pos++);
}

static void cec_arbit_bit_time_set(unsigned bit_set, unsigned time_set)
{
	//11bit:bit[10:0]
	switch (bit_set) {
	case 3:
		//3 bit
		cec_wr_reg(AO_CEC_TXTIME_4BIT_BIT7_0, time_set & 0xff);
		cec_wr_reg(AO_CEC_TXTIME_4BIT_BIT10_8, (time_set >> 8) & 0x7);
		break;
		//5 bit
	case 5:
		cec_wr_reg(AO_CEC_TXTIME_2BIT_BIT7_0, time_set & 0xff);
		cec_wr_reg(AO_CEC_TXTIME_2BIT_BIT10_8, (time_set >> 8) & 0x7);
		//7 bit
	case 7:
		cec_wr_reg(AO_CEC_TXTIME_17MS_BIT7_0, time_set & 0xff);
		cec_wr_reg(AO_CEC_TXTIME_17MS_BIT10_8, (time_set >> 8) & 0x7);
		break;
	default:
		break;
	}
}

static void cec_hw_buf_clear(void)
{
	cec_wr_reg(CEC_RX_MSG_CMD, RX_DISABLE);
	cec_wr_reg(CEC_TX_MSG_CMD, TX_ABORT);
	cec_wr_reg(CEC_RX_CLEAR_BUF, 1);
	cec_wr_reg(CEC_TX_CLEAR_BUF, 1);
	_udelay(100);
	cec_wr_reg(CEC_RX_CLEAR_BUF, 0);
	cec_wr_reg(CEC_TX_CLEAR_BUF, 0);
	_udelay(100);
	cec_wr_reg(CEC_RX_MSG_CMD, RX_NO_OP);
	cec_wr_reg(CEC_TX_MSG_CMD, TX_NO_OP);
}

void remote_cec_hw_reset(void)
{
	unsigned int reg;
	cec_dbg_prints("cec reset\n");

	reg = readl(P_AO_CRT_CLK_CNTL1);
	/* 24MHz/ (731 + 1) = 32786.885Hz */
	reg &= ~(0x7ff << 16);
	reg |= (731 << 16);     /* divider from 24MHz */
	reg |= (0x1 << 26);
	reg &= ~(0x800 << 16);  /* select divider */
	writel(reg, P_AO_CRT_CLK_CNTL1);

	/* set up pinmux */
	writel(readl(P_AO_RTI_PIN_MUX_REG) & (~(1 << 18 | 1 << 17)), P_AO_RTI_PIN_MUX_REG);
	writel(readl(P_AO_RTI_PULL_UP_REG) & (~(1 << 9)), P_AO_RTI_PULL_UP_REG);
	writel(readl(P_AO_RTI_PIN_MUX_REG) | (1 << 16), P_AO_RTI_PIN_MUX_REG);
	// Assert SW reset AO_CEC
	writel(0x1, P_AO_CEC_GEN_CNTL);
	// Enable gated clock (Normal mode).
	writel(readl(P_AO_CEC_GEN_CNTL) | (1<<1), P_AO_CEC_GEN_CNTL);
	_udelay(100);
	// Release SW reset
	writel(readl(P_AO_CEC_GEN_CNTL) & ~(1<<0), P_AO_CEC_GEN_CNTL);
	writel(readl(P_AO_CEC_INTR_MASKN) | (0x03 << 1), P_AO_CEC_INTR_MASKN);

	cec_arbit_bit_time_set(3, 0x118);
	cec_arbit_bit_time_set(5, 0x000);
	cec_arbit_bit_time_set(7, 0x2aa);
}

static unsigned char remote_cec_ll_rx(void)
{
	int i;
	int print = 1;
	unsigned char rx_msg_length = cec_rd_reg(CEC_RX_MSG_LENGTH) + 1;

	cec_dbg_prints("cec R:");
	for (i = 0; i < rx_msg_length; i++) {
		cec_msg.buf[cec_msg.rx_write_pos].msg[i] = cec_rd_reg(CEC_RX_MSG_0_HEADER + i);
		if (print) {
			cec_dbg_print(" ", cec_msg.buf[cec_msg.rx_write_pos].msg[i]);
		}
		if (i == 1 && cec_msg.buf[cec_msg.rx_write_pos].msg[i] == CEC_OC_VENDOR_COMMAND_WITH_ID) {
			/* do not print command with ID */
			print = 0;
		}
	}
	cec_msg.buf[cec_msg.rx_write_pos].msg_len = rx_msg_length;
	cec_dbg_prints("\n");

	return 0;
}
static void cec_buf_clear(void)
{
	int i;

	for (i = 0; i < 16; i++)
		cec_msg.buf[cec_msg.rx_read_pos].msg[i] = 0;
}

static void cec_tx_buf_init(void)
{
	int i, j;
	for (j = 0; j < CEX_TX_MSG_BUF_NUM; j++) {
		for (i = 0; i < 16; i++) {
			cec_tx_msgs.msg[j].buf[i] = 0;
		}
		cec_tx_msgs.msg[j].retry = 0;
		cec_tx_msgs.msg[j].len = 0;
	}
}

static int cec_queue_tx_msg(unsigned char *msg, unsigned char len)
{
	int s_idx, q_idx;

	s_idx = cec_tx_msgs.send_idx;
	q_idx = cec_tx_msgs.queue_idx;
	if (((q_idx + 1) & CEC_TX_MSG_BUF_MASK) == s_idx) {
		cec_dbg_prints("tx buffer full, abort msg\n");
		cec_reset_addr();
		return -1;
	}
	if (len && msg) {
		cec_memcpy(cec_tx_msgs.msg[q_idx].buf, msg, len);
		cec_tx_msgs.msg[q_idx].len = len;
		cec_tx_msgs.queue_idx = (q_idx + 1) & CEC_TX_MSG_BUF_MASK;
	}
	return 0;
}

static int cec_triggle_tx(unsigned char *msg, unsigned char len)
{
	int i;

	if ((TX_IDLE == cec_rd_reg(CEC_TX_MSG_STATUS)) ||
	    (TX_DONE == cec_rd_reg(CEC_TX_MSG_STATUS))) {
		cec_dbg_prints("cec T:");
		for (i = 0; i < len; i++) {
			cec_wr_reg(CEC_TX_MSG_0_HEADER + i, msg[i]);
			cec_dbg_print(" ", msg[i]);
		}
		cec_dbg_prints("\n");
		cec_wr_reg(CEC_TX_MSG_LENGTH, len-1);
		cec_wr_reg(CEC_TX_MSG_CMD, TX_REQ_CURRENT); //TX_REQ_NEXT
		return 0;
	}
	return -1;
}

static int remote_cec_ll_tx(unsigned char *msg, unsigned char len)
{
	cec_queue_tx_msg(msg, len);
	cec_triggle_tx(msg, len);

	return 0;
}

static int ping_cec_ll_tx(unsigned char *msg, unsigned char len)
{
	int i;
	int ret = 0;
	unsigned int n = 900;
	unsigned int reg;

	ret = cec_rd_reg(CEC_RX_MSG_STATUS);
	cec_dbg_print("rx stat:", ret);
	ret = cec_rd_reg(CEC_TX_MSG_STATUS);
	cec_dbg_print(", tx stat:", ret);
	cec_dbg_prints("\n");

	while (cec_rd_reg(CEC_TX_MSG_STATUS) == TX_BUSY) {
		/*
		 * waiting tx to idle if it is busy, other device may in tx state
		 */
	}
	if (cec_rd_reg(CEC_TX_MSG_STATUS) == TX_ERROR)
		cec_wr_reg(CEC_TX_MSG_CMD, TX_NO_OP);

	for (i = 0; i < len; i++) {
		cec_wr_reg(CEC_TX_MSG_0_HEADER + i, msg[i]);
	}
	cec_wr_reg(CEC_TX_MSG_LENGTH, len-1);
	cec_wr_reg(CEC_TX_MSG_CMD, TX_REQ_CURRENT); //TX_REQ_NEXT
	ret = cec_rd_reg(CEC_RX_MSG_STATUS);
	cec_dbg_print("rx stat:", ret);
	ret = cec_rd_reg(CEC_TX_MSG_STATUS);
	cec_dbg_print(", tx stat:", ret);
	cec_dbg_prints("\n");

	while (1) {
		reg = cec_rd_reg(CEC_TX_MSG_STATUS);
		if ( reg == TX_DONE ) {
			ret = TX_DONE;
			cec_wr_reg(CEC_TX_MSG_CMD, TX_NO_OP);
			cec_dbg_prints("ping_cec_ll_tx:TX_DONE\n");
			break;
		}

		if (reg == TX_ERROR) {
			ret = TX_ERROR;
			cec_wr_reg(CEC_TX_MSG_CMD, TX_NO_OP);
			cec_dbg_prints("ping_cec_ll_tx:TX_ERROR\n");
			break;
		}
		if (!(n--)) {
			cec_dbg_prints("ping_cec_ll_tx:TX_BUSY\n");
			ret = TX_BUSY;
			cec_wr_reg(CEC_TX_MSG_CMD, TX_NO_OP);
			break;
		}
		if (reg != TX_BUSY) {
			break;
		}
		_udelay(500);
	}

	return ret;
}

#define DEVICE_TV		0
#define DEVICE_RECORDER		1
#define DEVICE_RESERVED		2
#define DEVICE_TUNER		3
#define DEVICE_PLAYBACK		4
#define DEVICE_AUDIO_SYSTEM	5
#define DEVICE_PURE_CEC_SWITCH	6
#define DEVICE_VIDEO_PROCESSOR	7

static unsigned char log_addr_to_devtye(unsigned int addr)
{
	static unsigned char addr_map[] = {
		DEVICE_TV,
		DEVICE_RECORDER,
		DEVICE_RECORDER,
		DEVICE_TUNER,
		DEVICE_PLAYBACK,
		DEVICE_AUDIO_SYSTEM,
		DEVICE_TUNER,
		DEVICE_TUNER,
		DEVICE_PLAYBACK,
		DEVICE_RECORDER,
		DEVICE_TUNER,
		DEVICE_PLAYBACK,
		DEVICE_RESERVED,
		DEVICE_RESERVED,
		DEVICE_TV
	};
	return addr_map[addr & 0xf];
}

static void cec_report_physical_address(void)
{
	unsigned char msg[5];
	unsigned char phy_addr_ab = (readl(P_AO_DEBUG_REG1) >> 8) & 0xff;
	unsigned char phy_addr_cd = readl(P_AO_DEBUG_REG1) & 0xff;

	msg[0] = ((cec_msg.log_addr & 0xf) << 4)| CEC_BROADCAST_ADDR;
	msg[1] = CEC_OC_REPORT_PHYSICAL_ADDRESS;
	msg[2] = phy_addr_ab;
	msg[3] = phy_addr_cd;
	msg[4] = log_addr_to_devtye(cec_msg.log_addr);

	remote_cec_ll_tx(msg, 5);
}

static void cec_report_device_power_status(int dst)
{
	unsigned char msg[3];

	msg[0] = ((cec_msg.log_addr & 0xf) << 4)| (dst & 0xf);
	msg[1] = CEC_OC_REPORT_POWER_STATUS;
	msg[2] = cec_msg.power_status;

	remote_cec_ll_tx(msg, 3);
}

static void cec_set_stream_path(void)
{
	unsigned char phy_addr_ab = (readl(P_AO_DEBUG_REG1) >> 8) & 0xff;
	unsigned char phy_addr_cd = readl(P_AO_DEBUG_REG1) & 0xff;

	if ((hdmi_cec_func_config >> CEC_FUNC_MASK) & 0x1) {
		if ((hdmi_cec_func_config >> AUTO_POWER_ON_MASK) & 0x1) {
			if ((phy_addr_ab == cec_msg.buf[cec_msg.rx_read_pos].msg[2]) &&
			    (phy_addr_cd == cec_msg.buf[cec_msg.rx_read_pos].msg[3]))  {
				cec_msg.cec_power = 0x1;
			}
		}
	}
}

static void cec_device_vendor_id(void)
{
	unsigned char msg[5];

	msg[0] = ((cec_msg.log_addr & 0xf) << 4)| CEC_BROADCAST_ADDR;
	msg[1] = CEC_OC_DEVICE_VENDOR_ID;
	msg[2] = 0x00;
	msg[3] = 0x00;
	msg[4] = 0x00;

	remote_cec_ll_tx(msg, 5);
}

static void cec_menu_status_smp(int menu_status, int dst)
{
	unsigned char msg[3];

	msg[0] = ((cec_msg.log_addr & 0xf) << 4)| (dst & 0xf);
	msg[1] = CEC_OC_MENU_STATUS;
	msg[2] = menu_status;

	remote_cec_ll_tx(msg, 3);
}

static void cec_give_deck_status(int dst)
{
	unsigned char msg[3];

	msg[0] = ((cec_msg.log_addr & 0xf) << 4) | (dst & 0xf);
	msg[1] = CEC_OC_DECK_STATUS;
	msg[2] = 0x1a;

	remote_cec_ll_tx(msg, 3);
}

/*static void cec_standby(void)
{
	unsigned char msg[2];

	msg[0] = ((cec_msg.log_addr & 0xf) << 4) | CEC_BROADCAST_ADDR;
	msg[1] = CEC_OC_STANDBY;

	remote_cec_ll_tx(msg, 2);
}*/

static void cec_set_osd_name(int dst)
{
	unsigned char msg[16];
	unsigned char osd_len = cec_strlen(CONFIG_CEC_OSD_NAME);

	msg[0] = ((cec_msg.log_addr & 0xf) << 4) | (dst & 0xf);
	msg[1] = CEC_OC_SET_OSD_NAME;
	cec_memcpy(&msg[2], CONFIG_CEC_OSD_NAME, osd_len);

	remote_cec_ll_tx(msg, osd_len + 2);
}

static void cec_get_version(int dst)
{
	unsigned char dest_log_addr = cec_msg.log_addr & 0xf;
	unsigned char msg[3];

	if (0xf != dest_log_addr) {
		msg[0] = ((cec_msg.log_addr & 0xf) << 4) | (dst & 0xf);
		msg[1] = CEC_OC_CEC_VERSION;
		msg[2] = CEC_VERSION_14A;
		remote_cec_ll_tx(msg, 3);
	}
}

static int check_addr(int phy_addr)
{
	unsigned int local_addr = (readl(P_AO_DEBUG_REG1)) & 0xffff;
	unsigned int i, mask = 0xf000, a, b;

	for (i = 0; i < 4; i++) {
		if (!(local_addr & mask)) {
			break;
		}
		a = local_addr & mask;
		b = phy_addr & mask;
		if (a != b)	{// node is not same
			cec_dbg_prints("addr fail 1\n");
			return 0;
		}
		mask >>= 4;
	}
	cec_dbg_prints("addr ok\n");
	return 1;
}

static int is_playback_dev(int addr)
{
	if (addr != CEC_PLAYBACK_DEVICE_1_ADDR &&
	    addr != CEC_PLAYBACK_DEVICE_2_ADDR &&
	    addr != CEC_PLAYBACK_DEVICE_3_ADDR) {
		return 0;
	}
	return 1;
}


static unsigned int cec_handle_message(void)
{
	unsigned char opcode;
	unsigned char source;
	unsigned int  phy_addr, wake;

	source = (cec_msg.buf[cec_msg.rx_read_pos].msg[0] >> 4) & 0xf;
	if (((hdmi_cec_func_config>>CEC_FUNC_MASK) & 0x1) &&
		(cec_msg.buf[cec_msg.rx_read_pos].msg_len > 1)) {
		opcode = cec_msg.buf[cec_msg.rx_read_pos].msg[1];
		switch (opcode) {
		case CEC_OC_GET_CEC_VERSION:
			cec_get_version(source);
			break;
		case CEC_OC_GIVE_DECK_STATUS:
			cec_give_deck_status(source);
			break;
		case CEC_OC_GIVE_PHYSICAL_ADDRESS:
			cec_report_physical_address();
			break;
		case CEC_OC_GIVE_DEVICE_VENDOR_ID:
			cec_device_vendor_id();
			break;
		case CEC_OC_GIVE_OSD_NAME:
			cec_set_osd_name(source);
			break;
		case CEC_OC_SET_STREAM_PATH:
			cec_set_stream_path();
			break;
		case CEC_OC_GIVE_DEVICE_POWER_STATUS:
			cec_report_device_power_status(source);
			break;
		case CEC_OC_USER_CONTROL_PRESSED:
			if (((hdmi_cec_func_config >> CEC_FUNC_MASK) & 0x1) &&
			    ((hdmi_cec_func_config >> AUTO_POWER_ON_MASK) & 0x1) &&
			     (cec_msg.buf[cec_msg.rx_read_pos].msg_len == 3) &&
			    ((0x40 == cec_msg.buf[cec_msg.rx_read_pos].msg[2]) ||
			     (0x6d == cec_msg.buf[cec_msg.rx_read_pos].msg[2]) ||
			     (0x09 == cec_msg.buf[cec_msg.rx_read_pos].msg[2]) )) {
				cec_msg.cec_power = 0x1;
			}
			break;
		case CEC_OC_MENU_REQUEST:
			cec_menu_status_smp(DEVICE_MENU_INACTIVE, source);
			break;

		/* TV Wake up by image/text view on */
		case CEC_OC_IMAGE_VIEW_ON:
		case CEC_OC_TEXT_VIEW_ON:
			if (((hdmi_cec_func_config >> CEC_FUNC_MASK) & 0x1) &&
			    ((hdmi_cec_func_config >> AUTO_POWER_ON_MASK) & 0x1) &&
			    (!is_playback_dev(cec_msg.log_addr))) {
				/* request active source needed */
				phy_addr = 0xffff;
				cec_msg.cec_power = 0x1;
				wake =  (phy_addr << 0) |
					(source << 16);
				writel(wake, P_AO_RTI_STATUS_REG1);
			}
			break;

		/* TV Wake up by active source*/
		case CEC_OC_ACTIVE_SOURCE:
			phy_addr = (cec_msg.buf[cec_msg.rx_read_pos].msg[2] << 8) |
				   (cec_msg.buf[cec_msg.rx_read_pos].msg[3] << 0);
			if (((hdmi_cec_func_config >> CEC_FUNC_MASK) & 0x1) &&
			    ((hdmi_cec_func_config >> AUTO_POWER_ON_MASK) & 0x1) &&
			    (!is_playback_dev(cec_msg.log_addr) && check_addr(phy_addr))) {
				cec_msg.cec_power = 0x1;
				wake =  (phy_addr << 0) |
					(source << 16);
				writel(wake, P_AO_RTI_STATUS_REG1);
			}
			break;

		default:
			break;
		}
	}
	cec_rx_read_pos_plus();
	return 0;
}

static void cec_reset_addr(void)
{
	remote_cec_hw_reset();
	cec_wr_reg(CEC_LOGICAL_ADDR0, 0);
	cec_hw_buf_clear();
	cec_wr_reg(CEC_LOGICAL_ADDR0, cec_msg.log_addr & 0x0f);
	_udelay(100);
	cec_wr_reg(CEC_LOGICAL_ADDR0, (0x1 << 4) | (cec_msg.log_addr & 0x0f));
}

unsigned int cec_handler(void)
{
	unsigned char s_idx;
	static int busy_count = 0;

	if (0xf == cec_rd_reg(CEC_RX_NUM_MSG)) {
		cec_wr_reg(CEC_RX_CLEAR_BUF, 0x1);
		cec_wr_reg(CEC_RX_CLEAR_BUF, 0x0);
		cec_wr_reg(CEC_RX_MSG_CMD,  RX_ACK_CURRENT);
		cec_wr_reg(CEC_RX_MSG_CMD, RX_NO_OP);
		cec_dbg_prints("error:hw_buf overflow\n");
	}

	switch (cec_rd_reg(CEC_RX_MSG_STATUS)) {
	case RX_DONE:
		if (1 == cec_rd_reg(CEC_RX_NUM_MSG)) {
			remote_cec_ll_rx();
			(cec_msg.rx_write_pos == cec_msg.rx_buf_size - 1) ? (cec_msg.rx_write_pos = 0) : (cec_msg.rx_write_pos++);
		}
		cec_wr_reg(CEC_RX_MSG_CMD, RX_ACK_CURRENT);
		cec_wr_reg(CEC_RX_MSG_CMD, RX_NO_OP);
		cec_dbg_prints("RX_OK\n");
		break;
	case RX_ERROR:
		cec_dbg_prints("RX_ERROR\n");
		if (TX_ERROR == cec_rd_reg(CEC_TX_MSG_STATUS)) {
			cec_dbg_prints("TX_ERROR\n");
			cec_reset_addr();
		} else {
			cec_dbg_prints("TX_other\n");
			cec_wr_reg(CEC_RX_MSG_CMD,  RX_ACK_CURRENT);
			cec_wr_reg(CEC_RX_MSG_CMD, RX_NO_OP);
		}
		break;
	default:
		break;
	}

	switch (cec_rd_reg(CEC_TX_MSG_STATUS)) {
	case TX_DONE:
		cec_wr_reg(CEC_TX_MSG_CMD, TX_NO_OP);
		cec_tx_msgs.send_idx = (cec_tx_msgs.send_idx + 1) & CEC_TX_MSG_BUF_MASK;
		s_idx = cec_tx_msgs.send_idx;
		if (cec_tx_msgs.send_idx != cec_tx_msgs.queue_idx) {
			cec_dbg_prints("TX_OK\n");
			cec_triggle_tx(cec_tx_msgs.msg[s_idx].buf,
				       cec_tx_msgs.msg[s_idx].len);
		} else {
			cec_dbg_prints("TX_END\n");
		}
		busy_count = 0;
		break;

	case TX_ERROR:
		cec_dbg_prints("@TX_ERROR\n");
		if (RX_ERROR == cec_rd_reg(CEC_RX_MSG_STATUS)) {
			cec_dbg_prints("@RX_ERROR\n");
			cec_reset_addr();
		} else {
			cec_wr_reg(CEC_TX_MSG_CMD, TX_NO_OP);
			s_idx = cec_tx_msgs.send_idx;
			if (cec_tx_msgs.msg[s_idx].retry < 5) {
				cec_tx_msgs.msg[s_idx].retry++;
				cec_triggle_tx(cec_tx_msgs.msg[s_idx].buf,
					       cec_tx_msgs.msg[s_idx].len);
			} else {
				cec_dbg_prints("TX retry too much, abort msg\n");
				cec_tx_msgs.send_idx = (cec_tx_msgs.send_idx + 1) & CEC_TX_MSG_BUF_MASK;
			}
		}
		busy_count = 0;
		break;

	 case TX_IDLE:
		s_idx = cec_tx_msgs.send_idx;
		if (cec_tx_msgs.send_idx != cec_tx_msgs.queue_idx) { // triggle tx if idle
			cec_triggle_tx(cec_tx_msgs.msg[s_idx].buf,
				       cec_tx_msgs.msg[s_idx].len);
		}
		busy_count = 0;
		break;

	case TX_BUSY:
		busy_count++;
		if (busy_count >= 2000) {
			uart_puts("busy too long, reset hw\n");
			cec_reset_addr();
			busy_count = 0;
		}
		break;

	 default:
		break;
	}
	if (cec_msg.rx_read_pos != cec_msg.rx_write_pos) {
		cec_handle_message();
	}

	return 0;
}

/*static void check_standby(void)
{
	if (((cec_msg.log_addr & 0xf) == 0) &&
	    ((hdmi_cec_func_config >> CEC_FUNC_MASK) & 0x1) &&
	    ((hdmi_cec_func_config >> ONE_TOUCH_STANDBY_MASK) & 0x1)) {
		cec_standby();
	}
}*/

void cec_node_init(void)
{
	static int i = 0;
	static unsigned int retry = 0;
	static int regist_devs = 0;
	static int *probe = NULL;

	int tx_stat;
	unsigned char msg[1];
	unsigned int kern_log_addr = (readl(P_AO_DEBUG_REG1) >> 16) & 0xf;
	int player_dev[3][3] =
		{{CEC_PLAYBACK_DEVICE_1_ADDR, CEC_PLAYBACK_DEVICE_2_ADDR, CEC_PLAYBACK_DEVICE_3_ADDR},
		 {CEC_PLAYBACK_DEVICE_2_ADDR, CEC_PLAYBACK_DEVICE_3_ADDR, CEC_PLAYBACK_DEVICE_1_ADDR},
		 {CEC_PLAYBACK_DEVICE_3_ADDR, CEC_PLAYBACK_DEVICE_1_ADDR, CEC_PLAYBACK_DEVICE_2_ADDR}};

	if (retry >= 12) {  // retry all device addr
		cec_msg.log_addr = 0x0f;
		uart_puts("failed on retried all possible address\n");
		return ;
	}
	writel(0, P_AO_RTI_STATUS_REG1);
	if (probe == NULL) {
		cec_msg.rx_read_pos = 0;
		cec_msg.rx_write_pos = 0;
		cec_msg.rx_buf_size = 2;

		cec_msg.power_status = 1;
		cec_msg.cec_power = 0;
		cec_tx_msgs.send_idx = 0;
		cec_tx_msgs.queue_idx = 0;
		cec_tx_buf_init();
		cec_buf_clear();
		cec_wr_reg(CEC_LOGICAL_ADDR0, 0);
		cec_hw_buf_clear();
		cec_wr_reg(CEC_LOGICAL_ADDR0, 0xf);
		_udelay(100);
		cec_wr_reg(CEC_LOGICAL_ADDR0, (0x1 << 4) | 0xf);
		/*
		 * use kernel cec logic address to detect which logic address is the
		 * started one to allocate.
		 */
		cec_dbg_print("kern log_addr:0x", kern_log_addr);
		uart_puts("\n");
		/* we don't need probe TV address */
		if (!is_playback_dev(kern_log_addr)) {
			msg[0] = (kern_log_addr << 4) | kern_log_addr;
			ping_cec_ll_tx(msg, 1);
			cec_msg.log_addr = 0x10 | kern_log_addr;
			cec_wr_reg(CEC_LOGICAL_ADDR0, 0);
			cec_hw_buf_clear();
			cec_wr_reg(CEC_LOGICAL_ADDR0, kern_log_addr);
			_udelay(100);
			cec_wr_reg(CEC_LOGICAL_ADDR0, (0x1 << 4) | kern_log_addr);
			cec_dbg_print("Set cec log_addr:0x", cec_msg.log_addr);
			cec_dbg_print(",ADDR0:", cec_rd_reg(CEC_LOGICAL_ADDR0));
			uart_puts("\n");
			probe = NULL;
			regist_devs = 0;
			i = 0;
			retry = 0;
			/*check_standby();*/
			return ;
		}
		for (i = 0; i < 3; i++) {
			if (kern_log_addr == player_dev[i][0]) {
				probe = player_dev[i];
				break;
			}
		}
		if (probe == NULL) {
			probe = player_dev[0];
		}
		i = 0;
	}

	msg[0] = (probe[i]<<4) | probe[i];
	tx_stat = ping_cec_ll_tx(msg, 1);
	if (tx_stat == TX_BUSY) {   // can't get cec bus
		retry++;
		remote_cec_hw_reset();
		if (!(retry & 0x03)) {
			cec_dbg_print("retry too much, log_addr:0x", probe[i]);
			uart_puts("\n");
		} else {
			i -= 1;
		}
	} else if (tx_stat == TX_ERROR) {
		cec_wr_reg(CEC_LOGICAL_ADDR0, 0);
		cec_hw_buf_clear();
		cec_wr_reg(CEC_LOGICAL_ADDR0, probe[i]);
		_udelay(100);
		cec_wr_reg(CEC_LOGICAL_ADDR0, (0x1 << 4) | probe[i]);
		cec_msg.log_addr = probe[i];
		cec_dbg_print("Set cec log_addr:0x", cec_msg.log_addr);
		cec_dbg_print(", ADDR0:", cec_rd_reg(CEC_LOGICAL_ADDR0));
		uart_puts("\n");
		probe = NULL;
		regist_devs = 0;
		i = 0;
		retry = 0;
		return ;
	} else if (tx_stat == TX_DONE) {
		cec_dbg_print("sombody takes cec log_addr:0x", probe[i]);
		uart_puts("\n");
		regist_devs |= (1 << i);
		retry += (4 - (retry & 0x03));
		if (regist_devs == 0x07) {
			// No avilable logical address
			cec_msg.log_addr = 0x0f;
			cec_wr_reg(CEC_LOGICAL_ADDR0, (0x1 << 4) | 0xf);
			uart_puts("CEC allocate logic address failed\n");
		}
	}
	i++;
	if (i == 3) {
		i = 0;
	}
}

#endif
