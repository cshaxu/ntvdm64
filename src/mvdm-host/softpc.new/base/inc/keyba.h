
/*
 * SoftPC AT Revision 2.0
 *
 * Title        : Keyboard Adpator definitions
 *
 * Description  : Definitions for users of the keyboard Adaptor
 *
 * Author       : WTG Charnell
 *
 * Notes        : None
 */



/* @(#)keyba.h	1.10 08/10/92 Copyright Insignia Solutions Ltd."; */


#define RESEND_CODE 0xfe
#define ACK_CODE 0xfa
#define BAT_COMPLETION_CODE 0xaa

extern void kbd_inb IPT2( io_addr, port, half_word *, val );
extern void kbd_outb IPT2( io_addr, port, half_word, val );
#ifndef REAL_KBD
extern void ( *host_key_down_fn_ptr ) IPT1( int, key );
extern void ( *host_key_up_fn_ptr ) IPT1( int, key );
#endif
extern void ( *do_key_repeats_fn_ptr ) IPT0();
extern void keyboard_init IPT0();
extern void keyboard_post IPT0();
extern void AT_kbd_init IPT0();
extern void AT_kbd_post IPT0();

#ifdef HUNTER
#error DIVERGENCE MVDM-HOST-DIV-519: HUNTER platform branch is not selected; see mirror register.
#endif
