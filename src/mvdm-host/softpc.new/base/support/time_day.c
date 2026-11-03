#include "windows.h"   /* included for Sleep() */



#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 2.0
 *
 * File		: time_day.c
 *
 * Title	: Time of day
 *
 * Sccs ID	: @(#)time_day.c	1.27 4/20/94
 *
 * Description	: Get/Set time of day
 *
 * Author	: Henry Nash
 *
 * Notes	: The PC-XT version has an interrupt 18.203 times a second
 *		  to keep the counter up to date.  We interrupt at a similar
 *		  rate, but because of occasional heavy graphics or disk
 *		  operations we lose ticks. In an attempt to still keep
 *		  good time, we correct the stored time whenever the host
 *		  detects a timer event, using the host time facilities.
 *
 *                Upon reset time_of_day_init() grabs the host system time &
 *                puts it into the BIOS data area variables. Subsequent
 *                time of day accesses are maintained using the host system
 *		  time. This enables well behaved programs to keep good time
 *		  even if ticks are missed.
 *
 * Mods: (r3.4) : Make use of the host time structures host_timeval,
 *                host_timezone, and host_tm, which are equivalent
 *		  to the Unix BSD4.2 structures.
 *
 *		  Removed calls to cpu_sw_interrupt and replaced with
 *		  host_simulate
 */

#ifdef SCCSID
#line 42
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif
#line 44

#ifdef SEGMENTATION
#line 46
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 53


/*
 *    O/S include files.
 */
#include <stdlib.h>
#include <stdio.h>
#include TimeH
#include TypesH

/*
 * SoftPC include files
 */
#include "xt.h"
#include "sas.h"
#include "ios.h"
#include CpuH
#include "bios.h"
#include "fla.h"
#include "host.h"
#include "timeval.h"
#include "timer.h"
#include "error.h"

#include "cmos.h"
#include "cmosbios.h"
#include "ica.h"


/*
 * ===========================================================================
 * Local static data and defines
 * ===========================================================================
 */


#line 89
#ifdef XTSFD
#line 90
#error Unsupported excluded original configuration
#endif
#line 92


#ifdef NTVDM

BOOL UpDateInProgress(void);
#define UPDATE_IN_PROGRESS      UpDateInProgress()
IMPORT VOID host_init_bda_timer(void);

#else
#line 101
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 107

#ifdef ANSI
LOCAL void get_host_timestamp(word *, word *, half_word *);
LOCAL void write_host_timestamp(int, int);
LOCAL void TimeToTicks(int, int, int, word *, word *);
LOCAL void get_host_time(int *, int *, int *);
#else
#line 114
#error Unsupported excluded original configuration
#endif /* ANSI */
#line 119

#define TICKS_PER_HOUR      65543L
#define TICKS_PER_MIN       1092L
#define TICKS_PER_SEC       18L

/*
 * ============================================================================
 * External functions
 * ============================================================================
 */

void time_of_day()
{
    /*
     * BIOS function to return the number of PC interrupts since boot.
     */

    half_word mask;
    word low, high;
    half_word overflow, alarm;


    /*
     * Block the Alarm signal whilst we are looking at the clock timer
     */

#ifdef BSD4_2
#line 146
#error DIVERGENCE MVDM-HOST-DIV-528: BSD4_2 platform branch is not selected; see mirror register.
#endif
#line 148

    switch (getAH()) {
	case 0x00:			/* Get time	*/

#ifdef NTVDM
            sas_loadw(TIMER_LOW, &low);
	    setDX(low);

            sas_loadw(TIMER_HIGH, &high);
	    setCX(high);

            sas_load(TIMER_OVFL, &overflow);
 	    setAL(overflow);
            sas_store(TIMER_OVFL, 0);   /* Always write zero after read */

#else   /* ! NTVDM */
#line 164
#error Excluded unsupported non-NTVDM alternative
#endif /* NTVDM */
#line 206

	    break;

	case 0x01:			/* Set time	*/
	    /*
	     * Load the BIOS variables
	     */

	    sas_storew(TIMER_LOW, getDX());
	    sas_storew(TIMER_HIGH, getCX());
	    sas_store(TIMER_OVFL, 0);

#ifndef NTVDM
#line 219
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 225
	    break;
	case 2:	/* read the real time clock	*/

#ifndef NTVDM
#line 229
#error Excluded unsupported non-NTVDM time service profile
#endif
#line 233
		if( UPDATE_IN_PROGRESS )
			setCF(1);
		else
		{
			setDH( cmos_read( CMOS_SECONDS ) );
			setDL( cmos_read( CMOS_REG_B ) & 1 );	/* DSE bit	*/
			setCL( cmos_read( CMOS_MINUTES ) );
			setCH( cmos_read( CMOS_HOURS ) );
			setCF(0);
		}
#ifndef NTVDM
#line 244
#error Excluded unsupported non-NTVDM time service profile
#endif
#line 261
		break;

	case 3:	/* Set the real time clock	*/

		if( UPDATE_IN_PROGRESS )
		{
			/* initialise real time clock	*/
			cmos_write( CMOS_REG_A, 0x26 );
			cmos_write( CMOS_REG_B, 0x82 );
			cmos_read( CMOS_REG_C );
			cmos_read( CMOS_REG_D );
		}
		cmos_write( CMOS_SECONDS, getDH() );
		cmos_write( CMOS_MINUTES, getCL() );
		cmos_write( CMOS_HOURS, getCH() );
		alarm = ( cmos_read( CMOS_REG_B ) & 0x62 ) | 2;
		alarm |= (getDL() & 1);			/* only use the DSE bit	*/
		cmos_write( CMOS_REG_B, alarm );
		setCF(0);
		break;


	case 4:	/* read the date from the real time clock	*/

#ifndef NTVDM
#line 286
#error Excluded unsupported non-NTVDM time service profile
#endif
#line 290
		if( UPDATE_IN_PROGRESS )
			setCF(1);
		else
		{
			setDL( cmos_read( CMOS_DAY_MONTH ) );
			setDH( cmos_read( CMOS_MONTH ) );
			setCL( cmos_read( CMOS_YEAR ) );
			setCH( cmos_read( CMOS_CENTURY ) );
			setCF(0);
		}
#ifndef NTVDM
#line 301
#error Excluded unsupported non-NTVDM time service profile
#endif
#line 318
		break;

	case 5:	/* Set the date into the real time clock	*/

		if( UPDATE_IN_PROGRESS )
		{
			/* initialise real time clock	*/
			cmos_write( CMOS_REG_A, 0x26 );
			cmos_write( CMOS_REG_B, 0x82 );
			cmos_read( CMOS_REG_C );
			cmos_read( CMOS_REG_D );
		}
		cmos_write( CMOS_DAY_WEEK, 0 );
		cmos_write( CMOS_DAY_MONTH, getDL() );
		cmos_write( CMOS_MONTH, getDH() );
		cmos_write( CMOS_YEAR, getCL() );
		cmos_write( CMOS_CENTURY, getCH() );
		alarm = cmos_read( CMOS_REG_B ) & 0x7f;		/* clear 'set bit'	*/
		cmos_write( CMOS_REG_B, alarm);
		setCF(0);
		break;

	case 6:	/* set the alarm	*/

		if( cmos_read(CMOS_REG_B) & 0x20 )		/* alarm already enabled?	*/
		{
			setCF(1);
#ifdef BSD4_2
#line 346
#error DIVERGENCE MVDM-HOST-DIV-528: BSD4_2 platform branch is not selected; see mirror register.
#endif
#line 348
			return;
		}
		if( UPDATE_IN_PROGRESS )
		{
			/* initialise real time clock	*/
			cmos_write( CMOS_REG_A, 0x26 );
			cmos_write( CMOS_REG_B, 0x82 );
			cmos_read( CMOS_REG_C );
			cmos_read( CMOS_REG_D );
		}
		cmos_write( CMOS_SEC_ALARM, getDH() );
		cmos_write( CMOS_MIN_ALARM, getCL() );
		cmos_write( CMOS_HR_ALARM, getCH() );
		inb( ICA1_PORT_1, &mask );
		mask &= 0xfe;					/* enable alarm timer int.	*/
		outb( ICA1_PORT_1, mask );
		alarm = cmos_read( CMOS_REG_B ) & 0x7f;		/* ensure set bit turned off	*/
		alarm |= 0x20;					/* turn on alarm enable		*/
		cmos_write( CMOS_REG_B, alarm );
		break;

	case 7:

		alarm = cmos_read( CMOS_REG_B );
		alarm &= 0x57;					/* turn off alarm enable	*/	
		cmos_write( CMOS_REG_B, alarm );
		break;


#ifdef XTSFD
#line 378
#error Unsupported excluded original configuration
#else
#line 392
        default:
            ; /* Do nothing */
#endif
#line 395
    }
    setAH( 0 );

#ifdef BSD4_2
#line 399
#error DIVERGENCE MVDM-HOST-DIV-528: BSD4_2 platform branch is not selected; see mirror register.
#endif
#line 401
}


void time_int()
{
    /*
     * NT port does everything in 16 bit int08 handler
     */
#ifndef NTVDM
#line 410
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif	/* NTVDM */
#line 493
}

/*
 * ============================================================================
 * Internal Functions
 * ============================================================================
 */

/*
 *  NT's sense of time in the bios data area is always
 *  kept in sync with the real systems tic count
 *  Most of the compensation to readjust tics according
 *  to the time of day stuff is not needed
 */
#ifndef NTVDM
#line 508
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif  /* ifndef NTVDM */
#line 766


void time_of_day_init()
{
#ifndef NTVDM
#line 771
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif  /* NTVDM */
#line 802
}



#ifdef NTVDM

/*
 *  NTVDM: the rtc is setup so that the UIP bit is set on a cmos
 *  port read if the cmos ports haven't been touched for at least
 *  1 second. The IBM pc bios routine for accessing the clock
 *  polls RegA for UIP bit in a tight loop 600h times before
 *  failing the call. This means that MOST of the time the int1ah
 *  rtc fns almost never fail! To mimic this behaviour we poll
 *  the port until success, since we know that our rtc will clear
 *  UIP bit very quickly.
 */
BOOL UpDateInProgress(void)
{

   while (cmos_read(CMOS_REG_A) & 0x80) {
       Sleep(0);  // give other threads a chance to work
       }

   return FALSE;

}
#endif
