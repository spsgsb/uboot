// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include "registers.h"
#include <stdint.h>
#include "task_apis.h"

#define UART_PORT_CONS (0xff803000 + (0x000 << 2))

#define UART_STP_BIT UART_MODE_MASK_STP_1BIT
#define UART_PRTY_BIT 0
#define UART_CHAR_LEN   UART_MODE_MASK_CHAR_8BIT
#define UART_MODE_RESET_MASK	\
					(UART_MODE_MASK_RST_TX \
					| UART_MODE_MASK_RST_RX \
					| UART_MODE_MASK_CLR_ERR)

#define UART_WFIFO      (0<<2)
#define UART_RFIFO	(1<<2)
#define UART_MODE	(2<<2)
#define UART_STATUS     (3<<2)
#define UART_IRQCTL	(4<<2)
#define UART_CTRL	(5<<2)
#define UART_MODE_MASK_STP_1BIT                 (0<<16)
#define UART_MODE_MASK_CHAR_8BIT                (0<<20)
#define UART_MODE_MASK_TX_EN                    (1<<12)
#define UART_MODE_MASK_RX_EN                    (1<<13)
#define UART_MODE_MASK_RST_TX                   (1<<22)
#define UART_MODE_MASK_RST_RX                   (1<<23)
#define UART_MODE_MASK_CLR_ERR                  (1<<24)
#define UART_STAT_MASK_TFIFO_FULL               (1<<21)
#define UART_STAT_MASK_TFIFO_EMPTY              (1<<22)
#define UART_CTRL_USE_XTAL_CLK			(1<<24)
#define UART_CTRL_USE_NEW_BAUD_RATE		(1<<23)

#define P_UART(uart_base, reg)		(uart_base+reg)
#define P_UART_WFIFO(uart_base)		P_UART(uart_base, UART_WFIFO)
#define P_UART_MODE(uart_base)		P_UART(uart_base, UART_MODE)
#define P_UART_CTRL(uart_base)		P_UART(uart_base, UART_CTRL)
#define P_UART_STATUS(uart_base)	P_UART(uart_base, UART_STATUS)


static int uart_tx_isfull(void)
{
	return readl(P_UART_STATUS(UART_PORT_CONS)) &
		UART_STAT_MASK_TFIFO_FULL;
}

void wait_uart_empty(void)
{
#if 0
	while (!(readl(P_UART_STATUS(UART_PORT_CONS)) & (1 << 22)))
		;
#else
	unsigned int count=0;
	do {
		if ((readl(P_UART_STATUS(UART_PORT_CONS)) & (1 << 22)) == 0)
			_udelay(4);
		else
			break;
		count++;
	} while ( count < 20000);
#endif
}

void uart_tx_flush(void)
{
	while (!(readl(P_UART_STATUS(UART_PORT_CONS)) &
		UART_STAT_MASK_TFIFO_EMPTY))
		;
}

int uart_putc(int c)
{
	if (c == '\n')
		uart_putc('\r');

	/* Wait until TX is not full */
	while (uart_tx_isfull())
		;

	writel((char)c, P_UART_WFIFO(UART_PORT_CONS));
	/*wait_uart_empty();*/
	uart_tx_flush();
	return c;
}

int uart_puts(const char *s)
{
	while (*s)
		uart_putc(*s++);
	return 1;
}

void uart_put_hex(unsigned int data, unsigned bitlen)
{
	int i;
	unsigned char s;
	for (i = bitlen - 4; i >= 0; i -= 4) {
		if ((data >> i) == 0) {
			uart_putc(0x30);
			continue;
		}
		s = (data >> i) & 0xf;
		if (s < 10)
			uart_putc(0x30 + s);
		else
			uart_putc(0x61 + s - 10);
	}
}
