#ifndef _RS232_H
#define _RS232_H

/*[
	Name:		rs232.h
	Derived From:	Base 2.0
	Author:		Paul Huckle
	Created On:	
	Sccs ID:	05/11/94 @(#)rs232.h	1.14
	Purpose:	Definitions for users of the RS232 Adapter Module

	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.

]*/

/*
 * ============================================================================
 * Structure/Data definitions
 * ============================================================================
 */

#ifndef NEC_98
/* register type definitions follow: */

typedef half_word BUFFER_REG;

#ifdef LITTLEND
typedef union {
   word all;
   struct {
      WORD_BIT_FIELD LSByte:8;
      WORD_BIT_FIELD MSByte:8;
   } byte;
} DIVISOR_LATCH;
#endif
#ifdef BIGEND
#line 37
#error DIVERGENCE MVDM-HOST-DIV-524: BIGEND platform branch is not selected; see mirror register.
#endif
#else //NEC_98
#line 46
#error Unsupported NEC PC-98 configuration
#endif //NEC_98
#line 254

#ifdef BIT_ORDER2
typedef union {
 	half_word all;
	struct {
		 HALF_WORD_BIT_FIELD data_available:1;
		 HALF_WORD_BIT_FIELD tx_holding:1;
		 HALF_WORD_BIT_FIELD rx_line:1;
		 HALF_WORD_BIT_FIELD modem_status:1;
		 HALF_WORD_BIT_FIELD pad:4;
	       } bits;
      } INT_ENABLE_REG;
#endif
#ifdef BIT_ORDER1
#line 268
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#endif
#line 279

#if defined(NTVDM) && defined(FIFO_ON)
#line 281
#error Excluded unsupported FIFO_ON configuration
#else   /* NTVDM */

#ifdef BIT_ORDER2
#line 306
typedef union {
 	half_word all;
	struct {
		 HALF_WORD_BIT_FIELD no_int_pending:1;
		 HALF_WORD_BIT_FIELD interrupt_ID:2;
		 HALF_WORD_BIT_FIELD pad:5;
	       } bits;
      } INT_ID_REG;
#endif
#ifdef BIT_ORDER1
#line 316
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#endif
#line 325

#endif  /* ifdef NTVDM */
#line 327

#ifdef BIT_ORDER2
typedef union {
 	half_word all;
	struct {
		 HALF_WORD_BIT_FIELD word_length:2;
		 HALF_WORD_BIT_FIELD no_of_stop_bits:1;
		 HALF_WORD_BIT_FIELD parity_enabled:1;
		 HALF_WORD_BIT_FIELD even_parity:1;
		 HALF_WORD_BIT_FIELD stick_parity:1;
		 HALF_WORD_BIT_FIELD set_break:1;
		 HALF_WORD_BIT_FIELD DLAB:1;
	       } bits;
      } LINE_CONTROL_REG;
#endif
#ifdef BIT_ORDER1
#line 343
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#endif
#line 356

#ifdef BIT_ORDER2
typedef union {
 	half_word all;
	struct {
		 HALF_WORD_BIT_FIELD DTR:1;
		 HALF_WORD_BIT_FIELD RTS:1;
		 HALF_WORD_BIT_FIELD OUT1:1;
		 HALF_WORD_BIT_FIELD OUT2:1;
		 HALF_WORD_BIT_FIELD loop:1;
		 HALF_WORD_BIT_FIELD pad:3;
	       } bits;
      } MODEM_CONTROL_REG;
#endif
#ifdef BIT_ORDER1
#line 371
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#endif
#line 383

#ifdef BIT_ORDER2
typedef union {
 	half_word all;
	struct {
		 HALF_WORD_BIT_FIELD data_ready:1;
		 HALF_WORD_BIT_FIELD overrun_error:1;
		 HALF_WORD_BIT_FIELD parity_error:1;
		 HALF_WORD_BIT_FIELD framing_error:1;
		 HALF_WORD_BIT_FIELD break_interrupt:1;
		 HALF_WORD_BIT_FIELD tx_holding_empty:1;
		 HALF_WORD_BIT_FIELD tx_shift_empty:1;
#if defined(NTVDM) && defined(FIFO_ON)
#line 396
#error Excluded unsupported FIFO_ON configuration
#else
#line 398
		 HALF_WORD_BIT_FIELD pad:1;
#endif
#line 400
	       } bits;
      } LINE_STATUS_REG;
#endif
#ifdef BIT_ORDER1
#line 404
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#endif
#line 422

#ifdef BIT_ORDER2
typedef union {
 	half_word all;
	struct {
		 HALF_WORD_BIT_FIELD delta_CTS:1;
		 HALF_WORD_BIT_FIELD delta_DSR:1;
		 HALF_WORD_BIT_FIELD TERI:1;
		 HALF_WORD_BIT_FIELD delta_RLSD:1;
		 HALF_WORD_BIT_FIELD CTS:1;
		 HALF_WORD_BIT_FIELD DSR:1;
		 HALF_WORD_BIT_FIELD RI:1;
		 HALF_WORD_BIT_FIELD RLSD:1;
	       } bits;
      } MODEM_STATUS_REG;
#endif
#line 438
#ifdef BIT_ORDER1
#line 439
#error DIVERGENCE MVDM-HOST-DIV-524: BIT_ORDER1 platform branch is not selected; see mirror register.
#endif
#line 453

#if defined(NEC_98)
#line 455
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 481
#if defined(NTVDM) && defined(FIFO_ON)
#line 482
#error Excluded unsupported FIFO_ON configuration
#endif
#line 514

/* register select code definitions follow: */

#define RS232_TX_RX	0
#define RS232_IER	1
#define RS232_IIR	2
#if defined(NTVDM) && defined(FIFO_ON)
#line 521
#error Excluded unsupported FIFO_ON configuration
#endif
#line 523
#define RS232_LCR	3
#define RS232_MCR	4
#define RS232_LSR	5
#define RS232_MSR	6
#define RS232_SCRATCH	7
#endif // !NEC_98
#line 529

#define RS232_COM1_TIMEOUT (BIOS_VAR_START + 0x7c)
#define RS232_COM2_TIMEOUT (BIOS_VAR_START + 0x7d)
#define RS232_COM3_TIMEOUT (BIOS_VAR_START + 0x7e)
#define RS232_COM4_TIMEOUT (BIOS_VAR_START + 0x7f)
#define RS232_PRI_TIMEOUT (BIOS_VAR_START + 0x7c)
#define RS232_SEC_TIMEOUT (BIOS_VAR_START + 0x7d)

#define GO 0           /* We can emulate requested configuration */
#define NO_GO_SPEED 1  /* We can't emulate requested line speed */
#define NO_GO_LINE  2  /* We can't emulate requested line setup */

#if defined(NTVDM) && defined(FIFO_ON)
#line 542
#error Excluded unsupported FIFO_ON configuration
#endif
#line 554

#define OFF 0
#define ON 1
#define LEAVE_ALONE 2
#define	change_state(external_state, internal_state) \
	((external_state == internal_state) ? LEAVE_ALONE : external_state)

#if defined(NTVDM) && defined(FIFO_ON)
#line 562
#error Excluded unsupported FIFO_ON configuration
#endif
#line 564

#define RLS_INT 3     /* receiver line status interrupt ID */
#define RDA_INT 2     /* data available interrupt ID */
#define THRE_INT 1    /* tx holding register empty interrupt ID */
#define MS_INT 0      /* modem status interrupt ID */

#define DATA5 0       /* line control setting for five data bits */
#define DATA6 1       /* line control setting for six data bits */
#define DATA7 2       /* line control setting for seven data bits */
#define DATA8 3       /* line control setting for eight data bits */

#define STOP1 0       /* line control setting for one stop bit */
#define STOP2 1       /* line control setting for one and a half or two
                         stop bits */

#ifdef NTVDM
// collision with winbase.h PARITY_ON define
#define PARITYENABLE_ON 1   /* line control setting for parity enabled */
#define PARITYENABLE_OFF 0  /* line control setting for parity disabled */
#else
#line 584
#error Unsupported excluded original configuration
#endif
#line 587

#ifdef NTVDM
// collision with winbase.h PARITY_ODD define
#define EVENPARITY_ODD 0  /* line control setting for odd parity */
#define EVENPARITY_EVEN 1 /* line control setting for even parity */
#else
#line 593
#error Unsupported excluded original configuration
#endif
#line 596

#define PARITY_STICK 1  /* line control setting for stick(y) parity */

#define PARITY_FIXED 2  /* Internal state setting for fixed parity */

#if defined(NEC_98)
#line 602
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 606
#define COM1 0
#define COM2 1
#if (NUM_SERIAL_PORTS > 2)
#define COM3 2
#define COM4 3
#endif
#endif // !NEC_98
#line 613

#if defined(NTVDM) && defined(FIFO_ON)
#line 615
#error Excluded unsupported FIFO_ON configuration
#endif
#line 620

/*
 * ============================================================================
 * External declarations and macros
 * ============================================================================
 */

extern void com_init IPT1(int, adapter);
extern void com_post IPT1(int, adapter);

extern void com_flush_printer IPT1(int, adapter);

extern void com_inb IPT2(io_addr, port, half_word *, value);
extern void com_outb IPT2(io_addr, port, half_word, value);

extern void com_recv_char IPT1(int, adapter);
extern void recv_char IPT1(long, adapter);
extern void com_modem_change IPT1(int, adapter);
extern void com_save_rxbytes IPT2(int,n, CHAR *,buf);
extern void com_save_txbyte IPT1(CHAR,value);

#ifdef PS_FLUSHING
#line 642
#error DIVERGENCE MVDM-HOST-DIV-530: PS_FLUSHING platform branch is not selected; see mirror register.
#endif	/* PS_FLUSHING */
#line 644

#ifdef NTVDM
extern void com_lsr_change(int adapter);
#endif
#line 648

#if defined(NEC_98)
#line 650
#error Unsupported NEC PC-98 configuration
#else  // !NEC_98
#line 659
#if (NUM_SERIAL_PORTS > 2)
#line 660
#define	adapter_for_port(port) \
	(((port & 0x300) == 0x300) ? \
		(((port & 0xf8) == 0xf8) ? COM1 : COM3) \
		        : \
		(((port & 0xf8) == 0xf8) ? COM2 : COM4))

#ifdef SHORT_TRACE
#line 667
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 669

#else
#line 671
#error Unsupported excluded original configuration
#endif /* more than 2 serial ports */
#endif // !NEC_98
#line 680

#ifdef IRET_HOOKS
#line 682
#error DIVERGENCE MVDM-HOST-DIV-525: IRET_HOOKS platform branch is not selected; see mirror register.
#endif /* IRET_HOOKS */
#line 689

/* BCN 2730 define generic macros which can be SVID3 or old style
 * in either case the structure used should be a termios
 */

#ifdef SVID3_TCGET
#line 695
#error DIVERGENCE MVDM-HOST-DIV-536: SVID3_TCGET branch is not selected; see mirror register.
#else
#line 590
#line 699
#define	TCGET TCGETA
#define	TCSET TCSETA
#define	TCSETF TCSETAF
#endif	/* SVID3_TCGET */
#line 703

#endif /* _RS232_H */
#line 596
