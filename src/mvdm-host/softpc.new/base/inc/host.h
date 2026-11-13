/*
 * VPC-XT Revision 1.0
 *
 * Title	: host.h
 *
 * Description	: Host specific declarations for the machine dependant
 *		  modules of SoftPC. THIS IS A BASE INCLUDE FILE
 *
 * Author	: Henry Nash
 *
 * Notes	: Everything in here must portable !!!
 */

/* SccsID[]="@(#)host.h	1.6 09/14/93 Copyright Insignia Solutions Ltd."; */

/*
 * ============================================================================
 * External declarations and macros
 * ============================================================================
 */

#line 22
#ifdef PC_CONFIG
#line 23
#error DIVERGENCE MVDM-HOST-DIV-531: PC_CONFIG branch is not selected; see mirror register.
#endif
#line 25

IMPORT	VOID	host_flip_real_floppy_ind IPT2(int, drive, BOOL, active);


extern void host_floppy_init();
extern void host_floppy_term();
extern void host_reset();

extern boolean host_rdiskette_open_drive();


/* Unix Utilities - xxxx_unix.c */
extern char           *host_get_cur_dir();
#ifndef host_memset
extern void            host_memset();
#endif /* nhost_memset */
extern int             host_get_file_size();
extern boolean         host_file_is_directory();
extern boolean         host_validate_pathname();
extern boolean         host_check_rd_wt_access();
extern boolean         host_check_read_access();
#ifdef SUSPEND
#line 47
#error Unsupported excluded original configuration
#endif
#line 49
#ifndef REAL_VGA
IMPORT VOID memset4 IPT3( IU32, data, ULONG *, laddr, ULONG, count );
#endif
IMPORT VOID fill_alternate_bytes IPT3( IS8 *, start, IS8 *, end, IS8, value);
IMPORT VOID fill_both_bytes IPT3( IU16, data, IU16 *, dest, ULONG, len );

#line 55
#ifdef	SETUID_ROOT
#line 56
#error DIVERGENCE MVDM-HOST-DIV-529: SETUID_ROOT platform branch is not selected; see mirror register.
#endif	/* SETUID_ROOT */
#line 63

