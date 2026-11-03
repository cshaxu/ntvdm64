/* SccsID = @(#)egaread.h	1.5 08/10/92 Copyright Insignia Solutions */

#ifdef ANSI
extern void ega_read_init(void);
extern void ega_read_term(void);
extern void ega_read_routines_update(void);
extern boolean ega_need_read_op(int);
#else /* ANSI */
#line 9
#error Unsupported excluded original configuration
#endif /* ANSI */
#line 14

typedef struct
{
	ULONG	mode;
	UTINY	colour_compare;
	UTINY	colour_dont_care;
} READ_STATE;

IMPORT READ_STATE read_state;
