#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Version 3.0
 *
 * Title	: com.c
 *
 * Description	: Asynchronous Adaptor I/O functions.
 *
 * Notes	: Refer to the PC-XT Tech Ref Manual Section 1-185
 *		  For a detailed description of the Asynchronous Adaptor Card.
 *
 */

#ifdef SCCSID
#line 16
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif
#line 18

#ifdef SEGMENTATION
#line 20
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 27

/*
 *    O/S include files.
 */
#include <stdio.h>
#include <ctype.h>
#line 33
#if defined(NTVDM) && defined(MONITOR)
#line 34
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif
#line 36
#include TypesH
#include StringH

/*
 * SoftPC include files
 */
#include "xt.h"
#include CpuH
#include "sas.h"
#include "bios.h"
#include "ios.h"
#include "rs232.h"
#include "trace.h"
#include "error.h"
#include "config.h"
#include "host_com.h"
#include "ica.h"
#include "debug.h"
#include "timer.h"
#include "quick_ev.h"
#include "idetect.h"
#include "ckmalloc.h"
#line 58
#ifdef GISP_CPU
#line 59
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_CPU platform branch is not selected; see mirror register.
#endif /* GISP_CPU */
#line 61

#ifndef NEC_98
LOCAL UTINY selectBits[4] = { 0x1f, 0x3f, 0x7f, 0xff } ;
#endif // NEC_98
#line 65

#if defined(NEC_98)
#line 67
#error Unsupported NEC PC-98 configuration
#endif

/*
 * =====================================================================
 * The rs232 adaptor state
 * =====================================================================
 */

/*
 * batch_size, current_count 
 *	The IRET_HOOKS parameters batch_size and curr_count are used to prevent
 *	the number of interrupts that are emulated in one batch getting too
 *	large. When we reach the batch size we'll unhook the interrupt, and
 *	wait a while.
 * batch_running, qev_running
 *	These variables are used to prevent multiple quick events or batches
 *	running at once on a single adapter.
 */
#if defined(NEC_98)
#line 89
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 126

static struct ADAPTER_STATE
{
	BUFFER_REG tx_buffer;
	BUFFER_REG rx_buffer;
	DIVISOR_LATCH divisor_latch;
	INT_ENABLE_REG int_enable_reg;
	INT_ID_REG int_id_reg;
	LINE_CONTROL_REG line_control_reg;
	MODEM_CONTROL_REG modem_control_reg;
	LINE_STATUS_REG line_status_reg;
	MODEM_STATUS_REG modem_status_reg;
#if defined(NTVDM) && defined(FIFO_ON)
#line 139
#error Excluded unsupported FIFO_ON configuration





#endif
#line 146
	half_word scratch;      /* scratch register */

	int break_state;        /* either OFF or ON */
	int loopback_state;     /* either OFF or ON */
	int dtr_state;          /* either OFF or ON */
	int rts_state;          /* either OFF or ON */
	int out1_state;         /* either OFF or ON */
	int out2_state;         /* either OFF or ON */

	int receiver_line_status_interrupt_state;
	int data_available_interrupt_state;
	int tx_holding_register_empty_interrupt_state;
	int modem_status_interrupt_state;
	int hw_interrupt_priority;
	int com_baud_ind;
	int had_first_read;
#ifdef IRET_HOOKS
#line 163
#error DIVERGENCE MVDM-HOST-DIV-525: IRET_HOOKS platform branch is not selected; see mirror register.
#endif /* IRET_HOOKS */
#line 168
#ifdef NTVDM
    MODEM_STATUS_REG last_modem_status_value;
    int modem_status_changed;
#endif
} adapter_state[NUM_SERIAL_PORTS];


#ifdef NTVDM
#define MODEM_STATE_CHANGE()	asp->modem_status_changed = TRUE;
#else
#line 178
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#endif // NEC_98
#line 181


#ifdef IRET_HOOKS
#line 184
#error DIVERGENCE MVDM-HOST-DIV-525: IRET_HOOKS platform branch is not selected; see mirror register.
#endif /* IRET_HOOKS */
#line 191

/*
 * For synchronisation of adapter input.
 * Note this code is essential for the VMS equivalent of the async
 * event manager.  Removing it causes characters to be lost on reception.
 */
static int com_critical[NUM_SERIAL_PORTS];
#define is_com_critical(adapter)	(com_critical[adapter] != 0)
#define com_critical_start(adapter)	(++com_critical[adapter])
#define com_critical_end(adapter)	(--com_critical[adapter])
#define com_critical_reset(adapter)	(com_critical[adapter] = 0)


/*
 * Used to determine whether a flush input is needed for a LCR change
 */
#ifndef NEC_98
static LINE_CONTROL_REG LCRFlushMask;
#endif // NEC_98
#line 210

/*
 *	Please note that the following arrays have been made global in order
 *	that they can be accessed from some SUN_VA code. Please do not make
 *	them static.
 */

#if defined(NTVDM) && defined(FIFO_ON)
#line 218
#error Excluded unsupported FIFO_ON configuration
#endif
#line 220

/*
 * The delay needed in microseconds between receiving 2 characters
 * note this time is about 10% less than the time for actual reception.
 *
 * These delays have been heavily fudged and are now based on the idea that
 * most of the comms interrupt handlers can handle 9600 baud.  So as a
 * result the delays between 2 characters are now always set for 9600 baud.
 * Also note the delays of the faster baud rates have been decreased to
 * 1/2 of original delays, again this is to try to empty the host buffers
 * quickly enough to avoid buffer overflows.
 * NB these figures are heuristic.
 *
 * Finally it may be possible that the transmit delays will have to
 * be similarly fudged.
 */
unsigned long RX_delay[] =
{
	34, /* 115200 baud */
	67, /* 57600 baud */
	103, /* 38400 baud */
	900, /* 19200 baud */
	900, /* 9600 baud */
	900, /* 7200 baud */
	900, /* 4800 baud */
	900, /* 3600 baud */
	900, /* 2400 baud */
	900, /* 2000 baud */
	900, /* 1800 baud */
	900, /* 1200 baud */
	900, /* 600 baud */
	900, /* 300 baud */
	900, /* 150 baud */
	900, /* 134 baud */
	900, /* 110 baud */
	900, /* 75 baud */
	900  /* 50 baud */
};

/*
 * the delay needed in microseconds between transmitting 2 characters
 * note this time is about 10% more than the time for actual transmission.
 */
unsigned long TX_delay[] =
{
	83, /* 115200 baud */
	165, /* 57600 baud */
	253, /* 38400 baud */
	495, /* 19200 baud */
	1100, /* 9600 baud */
	1375, /* 7200 baud */
	2063, /* 4800 baud */
	2750, /* 3600 baud */
	4125, /* 2400 baud */
	5042, /* 2000 baud */
	5500, /* 1800 baud */
	8250, /* 1200 baud */
	16500, /* 600 baud */
	33000, /* 300 baud */
	66000, /* 150 baud */
	73920, /* 134 baud */
	89980, /* 110 baud */
	132000, /* 75 baud */
	198000  /* 50 baud */
};

#ifndef PROD
#line 287
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* !PROD */
#line 290
/*
 * =====================================================================
 * Other variables
 * =====================================================================
 */

#if !defined(PROD) || defined(SHORT_TRACE)
#line 297
#error Unsupported excluded original configuration
#endif /* !PROD || SHORT_TRACE */
#line 299

#ifdef PS_FLUSHING
#line 301
#error DIVERGENCE MVDM-HOST-DIV-530: PS_FLUSHING platform branch is not selected; see mirror register.
#endif	/* PS_FLUSHING */
#line 304

/* Control TX pacing */
IBOOL tx_pacing_enabled = FALSE;

/*
 * =====================================================================
 * Static forward declarations
 * =====================================================================
 */
#if defined(NEC_98)
#line 314
#error Unsupported NEC PC-98 configuration
#else  // NEC_98
#line 348
static void raise_rls_interrupt IPT1(struct ADAPTER_STATE *, asp);
static void raise_rda_interrupt IPT1(struct ADAPTER_STATE *, asp);
static void raise_ms_interrupt IPT1(struct ADAPTER_STATE *,asp);
static void raise_thre_interrupt IPT1(struct ADAPTER_STATE *, asp);
static void generate_iir IPT1(struct ADAPTER_STATE *, asp);
static void raise_interrupt IPT1(struct ADAPTER_STATE *, asp);
static void clear_interrupt IPT1(struct ADAPTER_STATE *, asp);
static void com_flush_input IPT1(int, adapter);
static void com_send_not_finished IPT1(int, adapter);
#ifndef NTVDM
#line 358
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 360
void   com_inb IPT2(io_addr, port, half_word *, value);
void   com_outb IPT2(io_addr, port, half_word, value);
void   com_recv_char IPT1(int, adapter);
GLOBAL void recv_char IPT1(long, adapter);
void   com_modem_change IPT1(int, adapter);
static void modem_change IPT1(int, adapter);
static void set_recv_char_status IPT1(struct ADAPTER_STATE *, asp);
static void set_xmit_char_status IPT1(struct ADAPTER_STATE *, asp);
static void set_break IPT1(int, adapter);
static void set_baud_rate IPT1(int, adapter);
static void set_line_control IPT2(int, adapter, int, value);
static void set_dtr IPT1(int, adapter);
static void set_rts IPT1(int, adapter);
static void set_out1 IPT1(int, adapter);
static void set_out2 IPT1(int, adapter);
static void set_loopback IPT1(int, adapter);
static void super_trace IPT1(char *, string);
void   com1_flush_printer IPT0();
void   com2_flush_printer IPT0();
static void com_reset IPT1(int, adapter);
GLOBAL VOID com_init IPT1(int, adapter);
void   com_post IPT1(int, adapter);
void   com_close IPT1(int, adapter);
LOCAL void next_batch IPT1(long, l_adapter);
#ifdef NTVDM
static void lsr_change(struct ADAPTER_STATE *asp, unsigned int error);
#ifdef FIFO_ON
#line 387
#error DIVERGENCE MVDM-HOST-DIV-525: FIFO_ON platform branch is not selected; see mirror register.
#endif
#line 389
#endif
#endif // NEC_98
#line 391

/*
 * =====================================================================
 * Subsidiary functions - for interrupt emulation
 * =====================================================================
 */

#if defined(NEC_98)
#line 399
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 455

static void raise_rls_interrupt IFN1(struct ADAPTER_STATE *, asp)
{
	/*
	 * Follow somewhat dubious advice on Page 1-188 of XT Tech Ref
	 * regarding the adapter card sending interrupts to the system.
	 * Apparently confirmed by the logic diagram.
	 */
	if ( asp->modem_control_reg.bits.OUT2 == 0 )
		return;
	
	/*
	 * Check if receiver line status interrupt is enabled
	 */
	if ( asp->int_enable_reg.bits.rx_line == 0 )
		return;
	
	/*
	 * Raise interrupt
	 */
	raise_interrupt(asp);
	asp->receiver_line_status_interrupt_state = ON;
}

static void raise_rda_interrupt IFN1(struct ADAPTER_STATE *, asp)
{
	if (( asp->modem_control_reg.bits.OUT2 == 0 ) &&
		( asp->loopback_state == OFF ))
		return;
	
	/*
	 * Check if data available interrupt is enabled
	 */
	if ( asp->int_enable_reg.bits.data_available == 0 )
		return;
	
	/*
	 * Raise interrupt
	 */
	raise_interrupt(asp);
	asp->data_available_interrupt_state = ON;
}

static void raise_ms_interrupt IFN1(struct ADAPTER_STATE *, asp)
{
	if ( asp->modem_control_reg.bits.OUT2 == 0 )
		return;
	
	/*
	 * Check if modem status interrupt is enabled
	 */
	if ( asp->int_enable_reg.bits.modem_status == 0 )
		return;
	
	/*
	 * Raise interrupt
	 */
	raise_interrupt(asp);
	asp->modem_status_interrupt_state = ON;
}

static void raise_thre_interrupt IFN1(struct ADAPTER_STATE *, asp)
{
	if ( asp->modem_control_reg.bits.OUT2 == 0 )
		return;
	
	/*
	 * Check if tx holding register empty interrupt is enabled
	 */
	if ( asp->int_enable_reg.bits.tx_holding == 0 )
		return;
	
	/*
	 * Raise interrupt
	 */
	raise_interrupt(asp);
	asp->tx_holding_register_empty_interrupt_state = ON;
}

static void generate_iir IFN1(struct ADAPTER_STATE *, asp)
{
	/*
	 * Set up interrupt identification register with highest priority
	 * pending interrupt.
	 */
	
	if ( asp->receiver_line_status_interrupt_state == ON )
	{
		asp->int_id_reg.bits.interrupt_ID = RLS_INT;
		asp->int_id_reg.bits.no_int_pending = 0;
	}
	else if ( asp->data_available_interrupt_state == ON )
	{
		asp->int_id_reg.bits.interrupt_ID = RDA_INT;
		asp->int_id_reg.bits.no_int_pending = 0;
	}
#if defined(NTVDM) && defined(FIFO_ON)
#line 552
#error Excluded unsupported FIFO_ON configuration




#endif
#line 558
	else if ( asp->tx_holding_register_empty_interrupt_state == ON )
	{
		asp->int_id_reg.bits.interrupt_ID = THRE_INT;
		asp->int_id_reg.bits.no_int_pending = 0;
	}
	else if ( asp->modem_status_interrupt_state == ON )
	{
		asp->int_id_reg.bits.interrupt_ID = MS_INT;
		asp->int_id_reg.bits.no_int_pending = 0;
	}
	else
	{
		/* clear interrupt */
		asp->int_id_reg.bits.no_int_pending = 1;
		asp->int_id_reg.bits.interrupt_ID = 0;
	}
}
#endif // NEC_98
#line 576

#if defined(NEC_98)
#line 578
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 612
static void raise_interrupt IFN1(struct ADAPTER_STATE *, asp)
{
	/*
	 * Make sure that some thing else has not raised an interrupt
	 * already.
	 */
	if ( ( asp->receiver_line_status_interrupt_state      == OFF )
	&&   ( asp->data_available_interrupt_state            == OFF )
	&&   ( asp->tx_holding_register_empty_interrupt_state == OFF )
	&&   ( asp->modem_status_interrupt_state              == OFF ) 
#if defined(NTVDM) && defined(FIFO_ON)
#line 623
#error Excluded unsupported FIFO_ON configuration
#endif
#line 625
       )
	{
#ifndef DELAYED_INTS
		ica_hw_interrupt(0, asp->hw_interrupt_priority, 1);
#else
#line 630
#error Unsupported excluded original configuration
#endif
#line 633
	}
}

static void clear_interrupt IFN1(struct ADAPTER_STATE *, asp)
{
	/*
	 * Make sure that some thing else has not raised an interrupt
	 * already.  If so then we cant drop the line.
	 */
	if ( ( asp->receiver_line_status_interrupt_state      == OFF )
	&&   ( asp->data_available_interrupt_state            == OFF )
	&&   ( asp->tx_holding_register_empty_interrupt_state == OFF )
	&&   ( asp->modem_status_interrupt_state              == OFF ) 
#if defined(NTVDM) && defined(FIFO_ON)
#line 647
#error Excluded unsupported FIFO_ON configuration
#endif
#line 649
       )
	{
		ica_clear_int(0, asp->hw_interrupt_priority);
	}
}
#endif // NEC_98
#line 655

#if defined(NTVDM) && defined(FIFO_ON)
#line 657
#error Excluded unsupported FIFO_ON configuration


















#endif
#line 677



/*
 * =====================================================================
 * The Adaptor functions
 * =====================================================================
 */

static void com_flush_input IFN1(int, adapter)
{
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	int finished, error_mask;
	long input_ready = 0; 

	sure_note_trace1(RS232_VERBOSE, "flushing the input for COM%c",
		adapter+'1');
	finished=FALSE;
	while(!finished) 
	{
		host_com_ioctl(adapter, HOST_COM_INPUT_READY,
			(long)&input_ready);
		if (input_ready) 
		{
			host_com_read(adapter, (UTINY *)&asp->rx_buffer,
				&error_mask);
		}
		else
		{
			finished=TRUE;
		}
	}
	set_xmit_char_status(asp);
}

#if defined(NEC_98)
#line 713
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 721
static void com_send_not_finished IFN1(int, adapter)
{
	struct ADAPTER_STATE *asp = &adapter_state[adapter];

	asp->line_status_reg.bits.tx_holding_empty=0;
	asp->line_status_reg.bits.tx_shift_empty=0;
}
#endif // NEC_98
#line 729


#if defined(NEC_98)
#line 732
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 742
#ifndef NTVDM
#line 743
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#endif // NEC_98
#line 754


#if defined(NEC_98)
#line 757
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 1095
void com_inb IFN2(io_addr, port, half_word *, value)
{
	int adapter = adapter_for_port(port);
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	long input_ready = 0;
	boolean adapter_was_critical;

#ifdef NTVDM
    if((port & 0x7) != RS232_MSR) host_com_lock(adapter);
#endif /* NTVDM */

	switch(port & 0x7)
	{
	case RS232_TX_RX:
		IDLE_comlpt();
		if (asp->line_control_reg.bits.DLAB == 0)
		{
			/*
			 * Read of rx buffer
			 */
#ifndef NTVDM
#line 1116
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else /* NTVDM is defined */
#line 1122
            //Flushing on first read removes characters from
            //the communications system that are needed !!!!
            //This assumes that the first read from the comms
            //system will return one character only. This is
            //a false assumption under NT windows.
#endif /* !NTVDM */
#line 1128
			*value = asp->rx_buffer;
		
			adapter_was_critical =
				(asp->line_status_reg.bits.data_ready == 1);
		
			asp->line_status_reg.bits.data_ready = 0;
			asp->data_available_interrupt_state = OFF;
			clear_interrupt(asp);

			if ( asp->loopback_state == OFF )
			{
				/*
				 * Adapter out of critical region,
				 * check for further input.  For IRET_HOOKS
				 * we don't need to do this, as receipt
				 * of the next character is kicked off
				 * by the IRET, however we do something
				 * else instead.  If this is the first
				 * character of a batch, we kick off a quick
				 * event for what will eventually be the
				 * start of the next batch (assuming there
				 * isn't already a quick event running).
				 * In any case we increment the count of
				 * characters in this batch.
				 */
				if (adapter_was_critical)
				{
#ifdef NTVDM
#ifdef FIFO_ON
#line 1157
#error DIVERGENCE MVDM-HOST-DIV-525: FIFO_ON platform branch is not selected; see mirror register.
#else /* !FIFO_ON */
#line 1175
                    host_com_char_read(adapter,
                    asp->int_enable_reg.bits.data_available
                    );
#endif /* !FIFO_ON */
#line 1179
#endif /* NTVDM */

#ifndef NTVDM
#line 1182
#error Excluded unsupported non-NTVDM serial profile
#endif /* !NTVDM */
#line 1213
				}


			}
			else
			{
				set_xmit_char_status(asp);
			}
#ifdef IRET_HOOKS
#line 1222
#error DIVERGENCE MVDM-HOST-DIV-525: IRET_HOOKS platform branch is not selected; see mirror register.
#endif /* IRET_HOOKS */
#line 1229
		}
		else
			*value = (IU8)(asp->divisor_latch.byte.LSByte);
#ifdef SHORT_TRACE
#line 1233
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1241
#ifndef PROD
#line 1242
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1253
		break;
																		
	case RS232_IER:
		if (asp->line_control_reg.bits.DLAB == 0)
			*value = asp->int_enable_reg.all;
		else
			*value = (IU8)(asp->divisor_latch.byte.MSByte);
#ifdef SHORT_TRACE
#line 1261
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1268
#ifndef PROD
#line 1269
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1279
		break;

	case RS232_IIR:
		generate_iir(asp);
		*value = asp->int_id_reg.all;

		if ( asp->int_id_reg.bits.interrupt_ID == THRE_INT )
		{
			asp->tx_holding_register_empty_interrupt_state = OFF;
			clear_interrupt(asp);
		}

#ifdef SHORT_TRACE
#line 1292
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1299
#ifndef PROD
#line 1300
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1310
		break;
	
	case RS232_LCR:
#ifdef NTVDM
        /* Before returning the information on the current configuation
           of the serial link make sure the System comms port is open */

        {
            extern int host_com_open(int adapter);

            host_com_open(adapter);
        }
#endif


		*value = asp->line_control_reg.all;
#ifdef SHORT_TRACE
#line 1327
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1334
#ifndef PROD
#line 1335
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1345
		break;
	
	case RS232_MCR:
		*value = asp->modem_control_reg.all;
#ifdef SHORT_TRACE
#line 1350
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1357
#ifndef PROD
#line 1358
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1368
		break;
	
	case RS232_LSR:
		*value = asp->line_status_reg.all;
	
		asp->line_status_reg.bits.overrun_error = 0;
		asp->line_status_reg.bits.parity_error = 0;
		asp->line_status_reg.bits.framing_error = 0;
		asp->line_status_reg.bits.break_interrupt = 0;
		asp->receiver_line_status_interrupt_state = OFF;
		clear_interrupt(asp);
#if defined(NTVDM) && defined(FIFO_ON)
#line 1380
#error Excluded unsupported FIFO_ON configuration
#endif
#line 1382
	
#ifdef SHORT_TRACE
#line 1384
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1396
#ifndef PROD
#line 1397
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1423
		break;
	
	case RS232_MSR:

#ifndef NTVDM
#line 1428
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else
#line 1449
		if(!asp->modem_status_changed && asp->loopback_state == OFF)
		{
		    *value = asp->last_modem_status_value.all;
		}
		else
		{
		    host_com_lock(adapter);
		    asp->modem_status_changed = TRUE;

		    /* if the adapter is not opened yet, just return POST
		       value.
		     */
		    if (host_com_check_adapter(adapter)) {
			if(asp->loopback_state == OFF)
			{
			    com_modem_change(adapter);
			    asp->modem_status_changed = FALSE;

			}
			else
			{
			    asp->modem_status_reg.bits.CTS = asp->modem_control_reg.bits.RTS;
			    asp->modem_status_reg.bits.DSR = asp->modem_control_reg.bits.DTR;
			    asp->modem_status_reg.bits.RI = asp->modem_control_reg.bits.OUT1;
			    asp->modem_status_reg.bits.RLSD = asp->modem_control_reg.bits.OUT2;
			}
		    }

		    *value = asp->modem_status_reg.all;

		    asp->modem_status_reg.bits.delta_CTS = 0;
		    asp->modem_status_reg.bits.delta_DSR = 0;
		    asp->modem_status_reg.bits.delta_RLSD = 0;
		    asp->modem_status_reg.bits.TERI = 0;
		    asp->modem_status_interrupt_state = OFF;
		    host_com_msr_callback (adapter, asp->modem_status_reg.all);
		    clear_interrupt(asp);
		    asp->last_modem_status_value.all = asp->modem_status_reg.all;
		    host_com_unlock(adapter);
		}
#endif /* ifndef NTVDM */
#line 1490


#ifdef SHORT_TRACE
#line 1493
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1500
#ifndef PROD
#line 1501
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1511
		break;

/*
 * Scratch register.  Just output the value stored.
 */
	case RS232_SCRATCH:
		*value = asp->scratch;
		break;
	
	}

#ifndef PROD
#line 1523
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1535

#ifdef NTVDM
	if((port & 0x7) != RS232_MSR) host_com_unlock(adapter);
#endif NTVDM
}


void com_outb IFN2(io_addr, port, half_word, value)
{
	int adapter = adapter_for_port(port);
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	int	i;

#ifdef NTVDM
	host_com_lock(adapter);
#endif NTVDM


#ifndef PROD
#line 1554
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1561
	
	switch(port & 0x7)
	{
	case RS232_TX_RX:
		IDLE_comlpt();
		if (asp->line_control_reg.bits.DLAB == 0)
		{
			/*
			 * Write char from tx buffer
			 */
			asp->tx_holding_register_empty_interrupt_state = OFF;
			clear_interrupt(asp);
			asp->tx_buffer = value;
			asp->line_status_reg.bits.tx_holding_empty = 0;
			asp->line_status_reg.bits.tx_shift_empty = 0;
			if ( asp->loopback_state == OFF )
			{
#ifdef PS_FLUSHING
#line 1579
#error DIVERGENCE MVDM-HOST-DIV-530: PS_FLUSHING platform branch is not selected; see mirror register.
#endif	/* PS_FLUSHING */
#line 1589
				host_com_write(adapter, asp->tx_buffer);
#if defined (DELAYED_INTS) || defined (NTVDM)
				set_xmit_char_status(asp);
#else
#line 1593
#error Unsupported excluded original configuration
#endif /* DELAYED_INTS || NTVDM */
#line 1600
#ifdef PS_FLUSHING
#line 1601
#error DIVERGENCE MVDM-HOST-DIV-530: PS_FLUSHING platform branch is not selected; see mirror register.
#endif	/* PS_FLUSHING */
#line 1603
			}
			else
			{	/* Loopback case requires masking off */
				/* of bits based upon word length.    */
				asp->rx_buffer = asp->tx_buffer & selectBits[asp->line_control_reg.bits.word_length] ;
				set_xmit_char_status(asp);
				set_recv_char_status(asp);
			}
		}
		else
		{
			asp->divisor_latch.byte.LSByte = value;
#ifndef NTVDM
#line 1616
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 1618
		}
#ifdef SHORT_TRACE
#line 1620
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1628
#ifndef PROD
#line 1629
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1640
		break;
																		
	case RS232_IER:
		if (asp->line_control_reg.bits.DLAB == 0)
		{
#ifdef NTVDM
            int org_da = asp->int_enable_reg.bits.data_available;
#endif
			asp->int_enable_reg.all = value & 0xf;
			/*
			 * Kill off any pending interrupts for those items
			 * which are set now as disabled
			 */
			if ( asp->int_enable_reg.bits.data_available == 0 )
				asp->data_available_interrupt_state = OFF;
			if ( asp->int_enable_reg.bits.tx_holding == 0 )
				asp->tx_holding_register_empty_interrupt_state =
					OFF;
			if ( asp->int_enable_reg.bits.rx_line == 0 )
				asp->receiver_line_status_interrupt_state = OFF;
			if ( asp->int_enable_reg.bits.modem_status == 0 )
				asp->modem_status_interrupt_state = OFF;
			
			/*
			 * Check for immediately actionable interrupts
			 * If you change these, change the code for out2 as well.
			 */
			if ( asp->line_status_reg.bits.data_ready == 1 )
				raise_rda_interrupt(asp);
			if ( asp->line_status_reg.bits.tx_holding_empty == 1 )
				raise_thre_interrupt(asp);

			/* lower int line if no outstanding interrupts */
			clear_interrupt(asp);

#ifdef NTVDM
		       // Inform the host interface if the status of the
		       // data available interrupt has changed

		       if(org_da != asp->int_enable_reg.bits.data_available)
		       {
			       host_com_da_int_change(adapter,
					   asp->int_enable_reg.bits.data_available,
					   asp->line_status_reg.bits.data_ready);
		       }
#endif /* NTVDM */
		}
		else
		{
			asp->divisor_latch.byte.MSByte = value;
#ifndef NTVDM
#line 1691
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 1693
		}
#ifdef SHORT_TRACE
#line 1695
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1702
#ifndef PROD
#line 1703
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1713
		break;
		
#if defined(NTVDM) && defined(FIFO_ON)
#line 1716
#error Excluded unsupported FIFO_ON configuration


























#else /* !(NTVDM && FIFO_ON) */
#line 1744
	case RS232_IIR:
		/* 
		 * Essentially a READ ONLY register
		 */
#ifdef SHORT_TRACE
#line 1749
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1756
#ifndef PROD
#line 1757
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1767
		break;
#endif /* NTVDM && FIFO_ON */
#line 1769

	case RS232_LCR:
#ifdef NTVDM
        /* The NT host code attempts to distinguish between applications
           that probe the UART and those that use it. Probes of the UART
           will not cause the systems comms port to be opened. The NT
           host code inherits the line settings from NT when the system
           comms port is opened. Therefore before an application reads
           or writes to the divisor bytes or the LCR the system
           comms port must be opened. This prevents the application
           reading incorrect values for the divisor bytes and writes
           to the divisor bytes getting overwritten by the system
           defaults. */

        {
            extern int host_com_open(int adapter);

            host_com_open(adapter);
        }
#endif /* NTVDM */

		if ((value & LCRFlushMask.all)
		!= (asp->line_control_reg.all & LCRFlushMask.all))
			com_flush_input(adapter);

		set_line_control(adapter, value);
		set_break(adapter);
#ifdef SHORT_TRACE
#line 1797
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1804
#ifndef PROD
#line 1805
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1815
		break;
		
	case RS232_MCR:
#ifdef SHORT_TRACE
#line 1819
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1826
		/*
		 * Optimisation - DOS keeps re-writing this register
		 */
		if ( asp->modem_control_reg.all == value )
			break;
		
		asp->modem_control_reg.all = value;
		asp->modem_control_reg.bits.pad = 0;

		/* Must be called before set_dtr */
		set_loopback(adapter);
		set_dtr(adapter);
		set_rts(adapter);
		set_out1(adapter);
		set_out2(adapter);
#ifndef PROD
#line 1842
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1852
		break;
		
	case RS232_LSR:
		i = asp->line_status_reg.bits.tx_shift_empty;   /* READ ONLY */
		asp->line_status_reg.all = value;
		asp->line_status_reg.bits.tx_shift_empty = (unsigned char)i;
#ifdef SHORT_TRACE
#line 1859
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1866
#ifndef PROD
#line 1867
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1877
		break;
		
	case RS232_MSR:
		/*
		 * Essentially a READ ONLY register.
		 */
#ifdef SHORT_TRACE
#line 1884
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 1891
#ifndef PROD
#line 1892
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 1902
		/* DrDOS writes to this reg after setting int on MSR change
		 * and expects to get an interrupt back!!! So we will oblige.
		 * Writing to this reg only seems to affect the delta bits 
		 * (bits 0-3) of the reg.
		 */
		if ((value & 0xf) != (asp->modem_status_reg.all & 0xf))
		{
			asp->modem_status_reg.all &= 0xf0;
			asp->modem_status_reg.all |= value & 0xf;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			if (asp->loopback_state == OFF)
				raise_ms_interrupt(asp);

			MODEM_STATE_CHANGE();
		}
		break;

/*
 * Scratch register.  Just store the value.
 */
	case RS232_SCRATCH:
		asp->scratch = value;
		break;
	
	}


#ifdef NTVDM
	host_com_unlock(adapter);
#endif
}
#endif // NEC_98
#line 1935


#ifdef IRET_HOOKS
#line 1938
#error Excluded unsupported IRET_HOOKS configuration















































































































#endif /* of ifdef IRET_HOOKS */
#line 2053

/*
 * =====================================================================
 * Subsidiary functions - for transmitting characters
 * =====================================================================
 */

#if defined(NEC_98)
#line 2061
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2116

#ifdef  NTVDM
// This code has been added for the MS project!!!!!!!


void com_recv_char(int adapter)
{
    struct ADAPTER_STATE *asp = &adapter_state[adapter];
    int error;

#ifdef FIFO_ON
#line 2127
#error DIVERGENCE MVDM-HOST-DIV-525: FIFO_ON platform branch is not selected; see mirror register.
#endif
#line 2154

    {
    error = 0;
    host_com_read(adapter, (char *)&asp->rx_buffer, &error);
    if (error != 0)
    {
        lsr_change(asp, error);
                raise_rls_interrupt(asp);
        }
    set_recv_char_status(asp);
    }
}
#ifdef FIFO_ON
#line 2167
#error DIVERGENCE MVDM-HOST-DIV-525: FIFO_ON platform branch is not selected; see mirror register.
#endif
#line 2180

#else /* NTVDM */
#line 2182
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 2198

/*
 * BCN 2151 - recv_char must use long param to match add_event function prototype
 */
GLOBAL void 
recv_char IFN1(long, adapt_long)
{
	int adapter = adapt_long;

	/*
	 * Character available on input device, read char, format char
	 * checking for parity and overrun errors, raise the appropriate
	 * interrupt.
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	int error_mask = 0;
	
	host_com_read(adapter, (UTINY *)&asp->rx_buffer, &error_mask);
	
	if (error_mask)
	{
		/*
		 * Set line status register and raise line status interrupt
		 */
		if (error_mask & HOST_COM_OVERRUN_ERROR)
			asp->line_status_reg.bits.overrun_error = 1;
		
		if (error_mask & HOST_COM_FRAMING_ERROR)
			asp->line_status_reg.bits.framing_error = 1;
		
		if (error_mask & HOST_COM_PARITY_ERROR)
			asp->line_status_reg.bits.parity_error = 1;
		
		if (error_mask & HOST_COM_BREAK_RECEIVED)
			asp->line_status_reg.bits.break_interrupt = 1;
		
		raise_rls_interrupt(asp);
	}
	
	set_recv_char_status(asp);
	
#line 2239
#ifdef DOCUMENTATION
#line 2240
#error DIVERGENCE MVDM-HOST-DIV-529: DOCUMENTATION platform branch is not selected; see mirror register.
#endif
#line 2261
}
#endif // NEC_98
#line 2263

#ifdef NTVDM
#ifndef NEC_98
static void lsr_change(struct ADAPTER_STATE *asp, unsigned int new_lsr)
{
    if (new_lsr & HOST_COM_OVERRUN_ERROR)
    asp->line_status_reg.bits.overrun_error = 1;
    if (new_lsr & HOST_COM_FRAMING_ERROR)
    asp->line_status_reg.bits.framing_error = 1;
    if (new_lsr & HOST_COM_PARITY_ERROR)
    asp->line_status_reg.bits.parity_error = 1;
    if (new_lsr & HOST_COM_BREAK_RECEIVED)
    asp->line_status_reg.bits.break_interrupt = 1;
/* we have no control of serial driver fifo enable/disabled states
   we may receive a fifo error even the application doesn't enable it.
   fake either framing or parity error
*/
    if (new_lsr & HOST_COM_FIFO_ERROR)
#ifdef FIFO_ON
#line 2282
#error DIVERGENCE MVDM-HOST-DIV-525: FIFO_ON platform branch is not selected; see mirror register.
#else
#line 2289
    if (asp->line_control_reg.bits.parity_enabled == PARITYENABLE_OFF)
        asp->line_status_reg.bits.framing_error = 1;
    else
        asp->line_status_reg.bits.parity_error = 1;
#endif
#line 2294

}
#endif // !NEC_98
#line 2297

void com_lsr_change(int adapter)
{
#ifndef NEC_98
    int new_lsr;
    struct ADAPTER_STATE *asp = &adapter_state[adapter];

    new_lsr = -1;
    host_com_ioctl(adapter, HOST_COM_LSR, (long)&new_lsr);
    if (new_lsr !=  -1)
    lsr_change(asp, new_lsr);
#endif  // !NEC_98
#line 2309
}

#endif /* NTVDM */

/*
 * One of the modem control input lines has changed state
 */
void com_modem_change IFN1(int, adapter)
{
	modem_change(adapter);
}

#if defined(NEC_98)
#line 2322
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2340
static void modem_change IFN1(int, adapter)
{
	/*
	 * Update the modem status register after a change to one of the
	 * modem control input lines
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	long modem_status = 0;
	int cts_state, dsr_state, rlsd_state, ri_state;
	
	if (asp->loopback_state == OFF)
	{
		/* get current modem input state */
		host_com_ioctl(adapter, HOST_COM_MODEM, (long)&modem_status);
		cts_state  = (modem_status & HOST_COM_MODEM_CTS)  ? ON : OFF;
		dsr_state  = (modem_status & HOST_COM_MODEM_DSR)  ? ON : OFF;
		rlsd_state = (modem_status & HOST_COM_MODEM_RLSD) ? ON : OFF;
		ri_state   = (modem_status & HOST_COM_MODEM_RI)   ? ON : OFF;
		
		/*
		 * Establish CTS state
		 */
		switch(change_state(cts_state, asp->modem_status_reg.bits.CTS))
		{
		case ON:
			asp->modem_status_reg.bits.CTS = ON;
			asp->modem_status_reg.bits.delta_CTS = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
			MODEM_STATE_CHANGE();
			break;
		
		case OFF:
			asp->modem_status_reg.bits.CTS = OFF;
			asp->modem_status_reg.bits.delta_CTS = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
			MODEM_STATE_CHANGE();
			break;
		
		case LEAVE_ALONE:
			break;
		}
		
		/*
		 * Establish DSR state
		 */
		switch(change_state(dsr_state, asp->modem_status_reg.bits.DSR))
		{
		case ON:
			asp->modem_status_reg.bits.DSR = ON;
			asp->modem_status_reg.bits.delta_DSR = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
			MODEM_STATE_CHANGE();
			break;
		
		case OFF:
			asp->modem_status_reg.bits.DSR = OFF;
			asp->modem_status_reg.bits.delta_DSR = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
			MODEM_STATE_CHANGE();
			break;
		
		case LEAVE_ALONE:
			break;
		}
		
		/*
		 * Establish RLSD state
		 */
		switch(change_state(rlsd_state,
			asp->modem_status_reg.bits.RLSD))
		{
		case ON:
			asp->modem_status_reg.bits.RLSD = ON;
			asp->modem_status_reg.bits.delta_RLSD = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
			MODEM_STATE_CHANGE();
			break;
		
		case OFF:
			asp->modem_status_reg.bits.RLSD = OFF;
			asp->modem_status_reg.bits.delta_RLSD = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
			MODEM_STATE_CHANGE();
			break;
		
		case LEAVE_ALONE:
			break;
		}
		
		/*
		 * Establish RI state
		 */
		switch(change_state(ri_state, asp->modem_status_reg.bits.RI))
		{
		case ON:
			asp->modem_status_reg.bits.RI = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);
			MODEM_STATE_CHANGE();
			break;
		
		case OFF:
			asp->modem_status_reg.bits.RI = OFF;
			asp->modem_status_reg.bits.TERI = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
			MODEM_STATE_CHANGE();
			break;
		
		case LEAVE_ALONE:
			break;
		}
	}
}
#endif // NEC_98
#line 2467

#if defined(NEC_98)
#line 2469
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2485
static void set_recv_char_status IFN1(struct ADAPTER_STATE *, asp)
{
	/*
	 * Check for data overrun and set up correct interrupt
	 */
	if ( asp->line_status_reg.bits.data_ready == 1 )
	{
		sure_note_trace0(RS232_VERBOSE, "overrun error in set_recv_char_status");
		asp->line_status_reg.bits.overrun_error = 1;
		raise_rls_interrupt(asp);
	}
	else
	{
		asp->line_status_reg.bits.data_ready = 1;
		raise_rda_interrupt(asp);
	}
}
#endif // NEC_98
#line 2503

static void set_xmit_char_status IFN1(struct ADAPTER_STATE *, asp)
{
	/*
	 * Set line status register and raise interrupt
	 */
#if defined(NEC_98)
#line 2510
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2514
	asp->line_status_reg.bits.tx_holding_empty = 1;
	asp->line_status_reg.bits.tx_shift_empty = 1;
	raise_thre_interrupt(asp);
#endif // NEC_98
#line 2518
}

#ifdef NTVDM
GLOBAL void tx_shift_register_empty(int adapter)
{
    struct ADAPTER_STATE *asp = &adapter_state[adapter];
#if defined(NEC_98)
#line 2525
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2527
    asp->line_status_reg.bits.tx_shift_empty = 1;
#endif // NEC_98
#line 2529
}
GLOBAL void tx_holding_register_empty(int adapter)
{
    struct ADAPTER_STATE *asp = &adapter_state[adapter];
#if defined(NEC_98)
#line 2534
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2537
    asp->line_status_reg.bits.tx_holding_empty = 1;
    raise_thre_interrupt(asp);
#endif // NEC_98
#line 2540
}
#endif

/*
 * =====================================================================
 * Subsidiary functions - for setting comms parameters
 * =====================================================================
 */

static void set_break IFN1(int, adapter)
{
	/*
	 * Process the set break control bit. Bit 6 of the Line Control
	 * Register.
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	
#if defined(NEC_98)
#line 2558
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2561
	switch ( change_state((int)asp->line_control_reg.bits.set_break,
		asp->break_state) )
#endif // NEC_98
#line 2564
	{
	case ON:
		asp->break_state = ON;
		host_com_ioctl(adapter, HOST_COM_SBRK, 0);
		break;
	
	case OFF:
		asp->break_state = OFF;
		host_com_ioctl(adapter, HOST_COM_CBRK, 0);
		break;
	
	case LEAVE_ALONE:
		break;
	}
}

/*
 * The following table is derived from page 1-200 of the XT Tech Ref 1st Ed
 * (except rates above 9600 which are not OFFICIALLY supported on the XT and
 * AT, but are theoretically possible) */

#if defined(NEC_98)
#line 2586
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2633
static word valid_latches[] =
{ 
	1, 	2, 	3, 	6, 	12, 	16, 	24, 	32, 
	48, 	58, 	64, 	96, 	192,	384, 	768, 	857, 
	1047, 	1536, 	2304 
};
#endif // NEC_98
#line 2640

#if defined(NEC_98)
#line 2642
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2665
#if !defined(PROD) || defined(IRET_HOOKS)
#line 2666
#error Unsupported excluded original configuration
#endif /* !PROD or IRET_HOOKS*/
#endif // NEC_98
#line 2690

static word speeds[] =
{ 
	HOST_COM_B115200, 
	HOST_COM_B57600, 
	HOST_COM_B38400, 
	HOST_COM_B19200, 
	HOST_COM_B9600, 
	HOST_COM_B7200, 
	HOST_COM_B4800, 
	HOST_COM_B3600,
	HOST_COM_B2400, 
	HOST_COM_B2000, 
	HOST_COM_B1800, 
	HOST_COM_B1200, 
	HOST_COM_B600, 
	HOST_COM_B300, 
	HOST_COM_B150, 
	HOST_COM_B134, 
	HOST_COM_B110,
	HOST_COM_B75, 
	HOST_COM_B50 
};

#if defined(NEC_98)
#line 2715
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2718
static int no_valid_latches = 
	(int)(sizeof(valid_latches)/sizeof(valid_latches[0]));
#endif // NEC_98
#line 2721

#if defined(NEC_98)
#line 2723
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 2761


#if defined(NEC_98)
#line 2764
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 2773
static void set_baud_rate IFN1(int, adapter)
{
	/*
	 * Map divisor latch into a valid line speed and set our Unix
	 * device accordingly. Note as the sixteen bit divisor latch is
	 * likely to be written in two eight bit bytes, we ignore illegal
	 * values of the sixteen bit divisor latch - hoping a second
	 * byte will be written to produce a legal value. In addition
	 * the reset value (0) is illegal!
	 *
	 * For IRET hooks, we need to determine the batch size from
	 * the line speed, and an idea of how many quick events
	 * we can get per second. We add one to alow us to catch-up!
	 * Hence 
	 * batch size = line_speed (in bits per second)
	 *	/ number of bits in a byte
	 *	* number of quick events ticks per second (normally 1000000)
	 *	/ the length in quick event ticks of a batch
	 *	+ 1
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	int i;

	com_flush_input(adapter);

#ifndef NTVDM
#line 2799
#error Excluded unsupported non-NTVDM serial profile
#else /* NTVDM */
#line 2827
    //The host is not limited in the baud rates that it supports

    if(asp->divisor_latch.all)
        /* baudrate = clock frequency / (diviso * 16) by taking
           frequency as 1.8432 MHZ
        */
        host_com_ioctl(adapter,HOST_COM_BAUD,115200/asp->divisor_latch.all);
#endif /* NTVDM */
#line 2835
}
#endif // NEC_98
#line 2837

#if defined(NEC_98)
#line 2839
#error Unsupported NEC PC-98 configuration
#endif // NEC_98
#line 2965

#ifndef NEC_98
static void set_line_control IFN2(int, adapter, int, value)
{
	/*
	 * Set Number of data bits
	 *     Parity bits
	 *     Number of stop bits
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	LINE_CONTROL_REG newLCR;
	int newParity, parity;
	
	newLCR.all = (unsigned char)value;

	/*
	 * Set up the number of data bits
	 */
	if (asp->line_control_reg.bits.word_length != newLCR.bits.word_length)
		host_com_ioctl(adapter, HOST_COM_DATABITS,
			newLCR.bits.word_length + 5);
	
	/*
	 * Set up the number of stop bits
	 */
	if (asp->line_control_reg.bits.no_of_stop_bits
	!= newLCR.bits.no_of_stop_bits)
		host_com_ioctl(adapter, HOST_COM_STOPBITS,
			newLCR.bits.no_of_stop_bits + 1);

	/* What are new settings to check for a difference */
#ifdef NTVDM
	if (newLCR.bits.parity_enabled == PARITYENABLE_OFF)
#else
#line 2999
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 3001
	{
		newParity = HOST_COM_PARITY_NONE;
	}
	else if (newLCR.bits.stick_parity == PARITY_STICK)
	{
#ifdef NTVDM
		newParity = newLCR.bits.even_parity == EVENPARITY_ODD ?
#else
#line 3009
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 3011
			HOST_COM_PARITY_MARK : HOST_COM_PARITY_SPACE;
	}
	else /* regular parity */
	{
#ifdef NTVDM
		newParity = newLCR.bits.even_parity == EVENPARITY_ODD ?
#else
#line 3018
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 3020
			HOST_COM_PARITY_ODD : HOST_COM_PARITY_EVEN;
	}

	/*
	 * Try to make sense of the current parity setting
	 */
#ifdef NTVDM
	if (asp->line_control_reg.bits.parity_enabled == PARITYENABLE_OFF)
#else
#line 3029
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 3031
	{
		parity = HOST_COM_PARITY_NONE;
	}
	else if (asp->line_control_reg.bits.stick_parity == PARITY_STICK)
	{
#ifdef NTVDM
		parity = asp->line_control_reg.bits.even_parity == EVENPARITY_ODD ?
#else
#line 3039
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 3041
			HOST_COM_PARITY_MARK : HOST_COM_PARITY_SPACE;
	}
	else /* regular parity */
	{
#ifdef NTVDM
		parity = asp->line_control_reg.bits.even_parity == EVENPARITY_ODD ?
#else
#line 3048
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 3050
			HOST_COM_PARITY_ODD : HOST_COM_PARITY_EVEN;
	}

	if (newParity != parity)
		host_com_ioctl(adapter, HOST_COM_PARITY, newParity);

#ifdef NTVDM
    //Change in the status of the DLAB selection bit, now is the time
    //to change the baud rate.

    if(!newLCR.bits.DLAB && asp->line_control_reg.bits.DLAB)
        set_baud_rate(adapter);
#endif

	/* finally update the current line control settings */
	asp->line_control_reg.all = (unsigned char)value;
}
#endif // NEC_98
#line 3068

#if defined(NEC_98)
#line 3070
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 3098
static void set_dtr IFN1(int, adapter)
{
	/*
	 * Process the DTR control bit, Bit 0 of the Modem Control
	 * Register.
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	
	switch ( change_state((int)asp->modem_control_reg.bits.DTR,
				asp->dtr_state) )
	{
	case ON:
		asp->dtr_state = ON;
		if (asp->loopback_state == OFF)
		{
			/* set the real DTR modem output */
			host_com_ioctl(adapter, HOST_COM_SDTR, 0);
		}
		else
		{
			/*
			 * loopback the DTR modem output into the
			 * DSR modem input
			 */
			asp->modem_status_reg.bits.DSR = ON;
			asp->modem_status_reg.bits.delta_DSR = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case OFF:
		asp->dtr_state = OFF;
		if (asp->loopback_state == OFF)
		{
			/* clear the real DTR modem output */
			host_com_ioctl(adapter, HOST_COM_CDTR, 0);
		}
		else
		{
			/*
			 * loopback the DTR modem output into the
			 * DSR modem input
			 */
			asp->modem_status_reg.bits.DSR = OFF;
			asp->modem_status_reg.bits.delta_DSR = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case LEAVE_ALONE:
		break;
	}
}
#endif // NEC_98
#line 3158

#if defined(NEC_98)
#line 3160
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 3188
static void set_rts IFN1(int, adapter)
{
	/*
	 * Process the RTS control bit, Bit 1 of the Modem Control
	 * Register.
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	
	switch ( change_state((int)asp->modem_control_reg.bits.RTS,
				asp->rts_state) )
	{
	case ON:
		asp->rts_state = ON;
		if (asp->loopback_state == OFF)
		{
			/* set the real RTS modem output */
			host_com_ioctl(adapter, HOST_COM_SRTS, 0);
		}
		else
		{
			/* loopback the RTS modem out into the CTS modem in */
			asp->modem_status_reg.bits.CTS = ON;
			asp->modem_status_reg.bits.delta_CTS = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case OFF:
		asp->rts_state = OFF;
		if (asp->loopback_state == OFF)
		{
			/* clear the real RTS modem output */
			host_com_ioctl(adapter, HOST_COM_CRTS, 0);
		}
		else
		{
			/* loopback the RTS modem out into the CTS modem in */
			asp->modem_status_reg.bits.CTS = OFF;
			asp->modem_status_reg.bits.delta_CTS = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case LEAVE_ALONE:
		break;
	}
}
#endif // NEC_98
#line 3242

#ifndef NEC_98
static void set_out1 IFN1(int, adapter)
{
	/*
	 * Process the OUT1 control bit, Bit 2 of the Modem Control
	 * Register.
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	
	switch ( change_state((int)asp->modem_control_reg.bits.OUT1,
				asp->out1_state) )
	{
	case ON:
		asp->out1_state = ON;
		if (asp->loopback_state == OFF)
		{
			/* 
			 * In the real adapter, this modem control output
			 * signal is not connected; so no real modem
			 * control change is required
			 */
		}
		else
		{
			/* loopback the OUT1 modem out into the RI modem in */
			asp->modem_status_reg.bits.RI = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case OFF:
		asp->out1_state = OFF;
		if (asp->loopback_state == OFF)
		{
			/* 
			 * In the real adapter, this modem control output
			 * signal is not connected; so no real modem control
			 * change is required
			 */
		}
		else
		{
			/* loopback the OUT1 modem out into the RI modem in */
			asp->modem_status_reg.bits.RI = OFF;
			asp->modem_status_reg.bits.TERI = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case LEAVE_ALONE:
		break;
	}
}

static void set_out2 IFN1(int, adapter)
{
	/*
	 * Process the OUT2 control bit, Bit 3 of the Modem Control
	 * Register.
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	
	switch ( change_state((int)asp->modem_control_reg.bits.OUT2,
				asp->out2_state) )
	{
	case ON:
		asp->out2_state = ON;
		if (asp->loopback_state == OFF)
		{
			/* 
			 * In the real adapter, this modem control output
			 * signal is used to determine whether the
			 * communications card should send interrupts; so
			 * check for immediately actionable interrupts.
			 * If you change this code, change the equivalent code
			 * for the interrupt enable register.
			 */
			if ( asp->line_status_reg.bits.data_ready == 1 )
				raise_rda_interrupt(asp);
			if ( asp->line_status_reg.bits.tx_holding_empty == 1 )
				raise_thre_interrupt(asp);
		}
		else
		{
			/* loopback the OUT2 modem output into the RLSD modem input */
			asp->modem_status_reg.bits.RLSD = ON;
			asp->modem_status_reg.bits.delta_RLSD = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case OFF:
		asp->out2_state = OFF;
		if (asp->loopback_state == OFF)
		{
			/* 
			 * In the real adapter, this modem control output signal
			 * is used to determine whether the communications
			 * card should send interrupts; so no real modem
			 * control change is required
			 */
		}
		else
		{
			/* loopback the OUT2 modem out into the RLSD modem in */
			asp->modem_status_reg.bits.RLSD = OFF;
			asp->modem_status_reg.bits.delta_RLSD = ON;
			host_com_msr_callback (adapter, asp->modem_status_reg.all);

			raise_ms_interrupt(asp);
		}
		MODEM_STATE_CHANGE();
		break;
	
	case LEAVE_ALONE:
		break;
	}
}

static void set_loopback IFN1(int, adapter)
{
	/*
	 * Process the loopback control bit, Bit 4 of the Modem Control
	 * Register.
	 */
	struct ADAPTER_STATE *asp = &adapter_state[adapter];
	
	switch ( change_state((int)asp->modem_control_reg.bits.loop,
				asp->loopback_state) )
	{
		case ON:
		asp->loopback_state = ON;
		/*
		 * Subsequent calls to set_dtr(), set_rts(), set_out1() and
		 * set_out2() will cause the modem control inputs to be set
		 * according to the the modem control outputs
		 */
		break;
	
	case OFF:
		asp->loopback_state = OFF;
		/*
		 * Set the modem control inputs according to the real
		 * modem state
		 */
		modem_change(adapter);
		break;
	
	case LEAVE_ALONE:
		break;
	}
}
#endif // NEC_98
#line 3403

#ifdef SHORT_TRACE
#line 3405
#error DIVERGENCE MVDM-HOST-DIV-527: SHORT_TRACE branch is not selected; see mirror register.
#endif
#line 3425


void com1_flush_printer IFN0()
{
#ifdef NTVDM
	host_com_lock(COM1);
#endif

	host_com_ioctl(COM1, HOST_COM_FLUSH, 0);

#ifdef NTVDM
       host_com_unlock(COM1);
#endif
}

void com2_flush_printer IFN0()
{
#ifdef NTVDM
	host_com_lock(COM2);
#endif

       host_com_ioctl(COM2, HOST_COM_FLUSH, 0);

#ifdef NTVDM
       host_com_unlock(COM2);
#endif
}


#ifdef SEGMENTATION
#line 3455
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 3462

#if defined(NEC_98)
#line 3464
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 3526
static void com_reset IFN1(int, adapter)
{
	struct ADAPTER_STATE *asp = &adapter_state[adapter];

	/* setup the LCRFlushMask if it has not already been setup */
	if (!LCRFlushMask.all)
	{
		LCRFlushMask.all = ~0;	 /* turn all bits on */

		/*
		 * Now turn off the bits that should NOT cause the input
		 * to be flushed.  Note set_break is handled seperately by
		 * the set_break() routine.
		 */
		LCRFlushMask.bits.DLAB = 0;
		LCRFlushMask.bits.no_of_stop_bits = 0;
		LCRFlushMask.bits.set_break = 0;
	}
		
	/*
	 * Set default state of all adapter registers
	 */
	asp->int_enable_reg.all = 0;

#ifdef NTVDM
        // Tell host side the state of the data available interrupt
    host_com_da_int_change(adapter,asp->int_enable_reg.bits.data_available,0);
#endif /* NTVDM */

	
	asp->int_id_reg.all = 0;
	asp->int_id_reg.bits.no_int_pending = 1;
	
	/* make sure a change occurs to 0 */
	asp->line_control_reg.all = ~0;
	
	/*
	 * set up modem control reg so next set_dtr etc.
	 * Will produce required status
	 */
	asp->modem_control_reg.all = 0;
	asp->modem_control_reg.bits.DTR = ON;
	asp->modem_control_reg.bits.RTS = ON;
	asp->modem_control_reg.bits.OUT1 = ON;
	asp->modem_control_reg.bits.OUT2 = ON;
	host_com_ioctl(adapter, HOST_COM_SDTR, 0);
	host_com_ioctl(adapter, HOST_COM_SRTS, 0);

	asp->line_status_reg.all = 0;
	asp->line_status_reg.bits.tx_holding_empty = 1;
	asp->line_status_reg.bits.tx_shift_empty = 1;
	
	asp->modem_status_reg.all = 0;
	MODEM_STATE_CHANGE();
	host_com_msr_callback (adapter, asp->modem_status_reg.all);
	
	/*
	 * Set up default state of our state variables
	 */
	asp->receiver_line_status_interrupt_state = OFF;
	asp->data_available_interrupt_state = OFF;
	asp->tx_holding_register_empty_interrupt_state = OFF;
	asp->modem_status_interrupt_state = OFF;
	asp->break_state = OFF;
	asp->loopback_state = OFF;
	asp->dtr_state = ON;
	asp->rts_state = ON;
	asp->out1_state = ON;
	asp->out2_state = ON;
#if defined(NTVDM) && defined(FIFO_ON)
#line 3596
#error Excluded unsupported FIFO_ON configuration






#endif
#line 3604
		
	/*
	 * Reset adapter synchronisation
	 */
	com_critical_reset(adapter);

	/*
	 * Set Unix devices to default state
	 */
	set_baud_rate(adapter);
	set_line_control(adapter, 0);
	set_break(adapter);

	/* Must be called before set_dtr */
	set_loopback(adapter);
	set_dtr(adapter);
	set_rts(adapter);
	set_out1(adapter);
	set_out2(adapter);

#ifdef IRET_HOOKS
#line 3625
#error DIVERGENCE MVDM-HOST-DIV-525: IRET_HOOKS platform branch is not selected; see mirror register.
#endif /* IRET_HOOKS */
#line 3640
}
#endif // NEC_98
#line 3642

#line 3643
#ifndef COM3_ADAPTOR
#line 3644
#error Unsupported excluded original configuration
#endif
#line 3646
#ifndef COM4_ADAPTOR
#line 3647
#error Unsupported excluded original configuration
#endif
#line 3649

#if defined(NEC_98)
#line 3651
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 3657
static IU8 com_adaptor[4] = {COM1_ADAPTOR,COM2_ADAPTOR,
                             COM3_ADAPTOR,COM4_ADAPTOR};
static io_addr port_start[4] = {RS232_COM1_PORT_START,
				RS232_COM2_PORT_START,
				RS232_COM3_PORT_START,
				RS232_COM4_PORT_START};
static io_addr port_end[4] = {RS232_COM1_PORT_END,
                          RS232_COM2_PORT_END,
                          RS232_COM3_PORT_END,
                          RS232_COM4_PORT_END};
static int int_pri[4] = {CPU_RS232_PRI_INT,
                         CPU_RS232_SEC_INT,
                         CPU_RS232_PRI_INT,
                         CPU_RS232_SEC_INT};
static int timeout[4] = {RS232_COM1_TIMEOUT,
                         RS232_COM2_TIMEOUT,
                         RS232_COM3_TIMEOUT,
                         RS232_COM4_TIMEOUT};
#endif // NEC_98
#line 3676


#if defined(NEC_98)
#line 3679
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 3777
GLOBAL VOID com_init IFN1(int, adapter)
{
	io_addr i;

#ifdef NTVDM
	host_com_lock(adapter);
	host_com_disable_open(adapter,TRUE);
#endif

	adapter_state[adapter].had_first_read = FALSE;
	
	/* Set up the IO chip select logic for this adaptor */
#ifdef NTVDM
    {
        extern BOOL VDMForWOW;
        extern void wow_com_outb(io_addr port, half_word value);
        extern void wow_com_inb(io_addr port, half_word *value);

            io_define_inb(com_adaptor[adapter],VDMForWOW ? wow_com_inb: com_inb);
            io_define_outb(com_adaptor[adapter],VDMForWOW ? wow_com_outb: com_outb);
        }
#else
#line 3799
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 3802
 
	for(i = port_start[adapter]; i <= port_end[adapter]; i++)
		io_connect_port(i, com_adaptor[adapter], IO_READ_WRITE);
         

	adapter_state[adapter].hw_interrupt_priority = int_pri[adapter];
 
	/* reset adapter state */
	host_com_reset(adapter);

	/* reset adapter state */
	com_reset(adapter);

#ifndef NTVDM
#line 3816
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else /* not NTVDM */
#line 3819
	host_com_disable_open(adapter,FALSE);
	host_com_unlock(adapter);
#endif
#line 3822

	return;
}
#endif // NEC_98
#line 3826

void com_post IFN1(int, adapter)
{
        /* Set up BIOS data area. */
	sas_storew( BIOS_VAR_START + (2*adapter), port_start[adapter]);
	sas_store(timeout[adapter] , (half_word)1 );
}
 
void com_close IFN1(int, adapter)
{
#ifdef NTVDM
	host_com_lock(adapter);
#endif

#ifndef PROD
#line 3841
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 3845
	/* reset host specific communications channel */
	config_activate((UTINY)(C_COM1_NAME + adapter), FALSE);

#ifdef NTVDM
	host_com_unlock(adapter);
#endif
}

#ifdef NTVDM

/*********************************************************/
/* Com extentions - DAB (MS-project) */

#if defined(NEC_98)
#line 3859
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 3892
GLOBAL void SyncBaseLineSettings(int adapter,DIVISOR_LATCH *divisor_latch,
                 LINE_CONTROL_REG *LCR_reg)
{
    register struct ADAPTER_STATE *asp = &adapter_state[adapter];

    //Setup baud rate control register
    asp->divisor_latch.all = (*divisor_latch).all;

    //Setup line control settings
    asp->line_control_reg.bits.word_length = (*LCR_reg).bits.word_length;
    asp->line_control_reg.bits.no_of_stop_bits = (*LCR_reg).bits.no_of_stop_bits
;
    asp->line_control_reg.bits.parity_enabled = (*LCR_reg).bits.parity_enabled;
    asp->line_control_reg.bits.stick_parity = (*LCR_reg).bits.stick_parity;
    asp->line_control_reg.bits.even_parity = (*LCR_reg).bits.even_parity;
}
#endif // NEC_98
#line 3909

GLOBAL void setup_RTSDTR(int adapter)
{
    struct ADAPTER_STATE *asp = &adapter_state[adapter];

    host_com_ioctl(adapter,asp->dtr_state == ON ? HOST_COM_SDTR : HOST_COM_CDTR,
0);
    host_com_ioctl(adapter,asp->rts_state == ON ? HOST_COM_SRTS : HOST_COM_CRTS,
0);
}

GLOBAL int AdapterReadyForCharacter(int adapter)
{
    BOOL AdapterReady = FALSE;

    /*......................................... Are RX interrupts enabled */

#if defined(NEC_98)
#line 3927
#error Unsupported NEC PC-98 configuration
#else // NEC_98
#line 3930
    if(adapter_state[adapter].line_status_reg.bits.data_ready == 0 &&
       adapter_state[adapter].data_available_interrupt_state == OFF)
#endif // NEC_98
#line 3933
    {
        AdapterReady = TRUE;
    }

    return(AdapterReady);
}

// This function returns the ICA controller and line used to generate
// interrupts on a adapter. This information is used to register a EOI
// hook.


GLOBAL void com_int_data(int adapter,int *controller, int *line)
{
    struct ADAPTER_STATE *asp = &adapter_state[adapter];

    *controller = 0;                            // Controller ints raised on
    *line = (int) asp->hw_interrupt_priority;   // Line ints raised on
}

#endif /* NTVDM */

#ifdef PS_FLUSHING
#line 3956
#error DIVERGENCE MVDM-HOST-DIV-530: PS_FLUSHING platform branch is not selected; see mirror register.
#endif	/* PS_FLUSHING */
#line 3991



/********************************************************/
/* Com debugging shell - Ade Brownlow / Ian Wellock
 * NB: This stuff only works for COM1. It is called from yoda using 'cd'
 * - comdebug - from the yoda command line....
 */
#ifndef PROD
#line 4000
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* !PROD */
#line 4526
/********************************************************/
