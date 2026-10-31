#ifndef WOW_CONTROL_MESSAGE_ABI_H
#define WOW_CONTROL_MESSAGE_ABI_H

/* Original OpenNT winuserp.h control messages, evaluated with the selected
 * NT4 WINVER>=0x0400 winuser.h CB_MSGMAX (0x0162), not the host SDK's
 * growing public message range. These numbers connect original WOW thunks
 * and recovered USER controls; they are not modern private control APIs. */
#define CBEC_SETCOMBOFOCUS 0x0163
#define CBEC_KILLCOMBOFOCUS 0x0164
#ifndef LBCB_CARETON
#define LBCB_CARETON 0x01A3
#define LBCB_CARETOFF 0x01A4
#endif
#define LB_INSERTSTRINGUPPER 0x01AA
#define LB_INSERTSTRINGLOWER 0x01AB
#define LB_ADDSTRINGUPPER 0x01AC
#define LB_ADDSTRINGLOWER 0x01AD
#define LBCB_STARTTRACK 0x01AE
#define LBCB_ENDTRACK 0x01AF
#ifndef WM_DRAGLOOP
#define WM_DRAGLOOP 0x022D
#define WM_DRAGSELECT 0x022E
#define WM_DRAGMOVE 0x022F
#endif
#endif
