#ifndef OPENNT_WOW_USER_DECLARATIONS_H
#define OPENNT_WOW_USER_DECLARATIONS_H

/* DIVERGENCE(ADAPTER-WIN32-042): the current SDK claims `_WINUSERP_`
 * before the byte-identical OpenNT declaration carrier can expose
 * `PFNW32ET`. Supply only that original function-pointer typedef, with its
 * original name and calling convention. This header selects no USER/GDI
 * implementation or GUI route. */
typedef VOID (APIENTRY *PFNW32ET)(VOID);

/* The selected original `wcall16.c` consumes this public WOW callback flag
 * after the modern WOW declaration carrier has been deliberately excluded. */
#ifndef WCB16_CDECL
#define WCB16_CDECL (0x1)
#endif

#endif
