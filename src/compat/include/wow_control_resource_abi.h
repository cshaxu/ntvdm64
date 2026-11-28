/* Copyright (c) 1985-95, Microsoft Corporation.
 * OPENNT-HOST-105 declaration facade: bounded declarations from original
 * public/internal/windows/inc/winuserp.h for the original client bodies.
 * Resource bytes only; never a modern USER allocation layout. */
#ifndef OPENNT_CONTROL_RESOURCE_ABI_H
#define OPENNT_CONTROL_RESOURCE_ABI_H
#ifdef RC_INVOKED
/* XP SP1 a2f6d7c9: resource IDs from windows/core/ntuser/client/usercli.h,
 * windows/core/ntuser/inc/user.h and public/internal/windows/inc/winuserp.h.
 * Exact per-symbol original paths/lines: t485-s1/n022-res-identities.json.
 * RC-only declarations reuse this resource ABI without importing C layouts. */
#define IDD_MDI_ACTIVATE         9
#define IDS_EXITWINDOWS_TITLE           710
#define IDS_REASON_APPLICATION_HUNG_DESCRIPTION                 8279
#define IDS_REASON_APPLICATION_HUNG_TITLE                       8258
#define IDS_REASON_APPLICATION_MAINTENANCE_DESCRIPTION          8281
#define IDS_REASON_APPLICATION_MAINTENANCE_TITLE                8260
#define IDS_REASON_APPLICATION_PM_DESCRIPTION                   8287
#define IDS_REASON_APPLICATION_PM_TITLE                         8268
#define IDS_REASON_APPLICATION_UNSTABLE_DESCRIPTION             8280
#define IDS_REASON_APPLICATION_UNSTABLE_TITLE                   8259
#define IDS_REASON_DEFAULT_TITLE                                8269
#define IDS_REASON_HARDWARE_INSTALLATION_DESCRIPTION            8276
#define IDS_REASON_HARDWARE_MAINTENANCE_DESCRIPTION             8275
#define IDS_REASON_OPERATINGSYSTEM_RECONFIG_DESCRIPTION         8278
#define IDS_REASON_OPERATINGSYSTEM_UPGRADE_DESCRIPTION          8277
#define IDS_REASON_OTHERFAILURE_HUNG_DESCRIPTION                8286
#define IDS_REASON_OTHERFAILURE_HUNG_TITLE                      8266
#define IDS_REASON_OTHERFAILURE_TITLE                           8267
#define IDS_REASON_OTHER_DESCRIPTION                            8282
#define IDS_REASON_PLANNED_HARDWARE_INSTALLATION_TITLE          8253
#define IDS_REASON_PLANNED_HARDWARE_MAINTENANCE_TITLE           8251
#define IDS_REASON_PLANNED_OPERATINGSYSTEM_RECONFIG_TITLE       8257
#define IDS_REASON_PLANNED_OPERATINGSYSTEM_UPGRADE_TITLE        8255
#define IDS_REASON_PLANNED_OTHER_TITLE                          8262
#define IDS_REASON_POWERFAILURE_CORDUNPLUGGED_DESCRIPTION       8284
#define IDS_REASON_POWERFAILURE_CORDUNPLUGGED_TITLE             8264
#define IDS_REASON_POWERFAILURE_ENVIRONMENT_DESCRIPTION         8285
#define IDS_REASON_POWERFAILURE_ENVIRONMENT_TITLE               8265
#define IDS_REASON_SYSTEMFAILURE_BLUESCREEN_DESCRIPTION         8283
#define IDS_REASON_SYSTEMFAILURE_BLUESCREEN_TITLE               8263
#define IDS_REASON_UNPLANNED_HARDWARE_INSTALLATION_TITLE        8252
#define IDS_REASON_UNPLANNED_HARDWARE_MAINTENANCE_TITLE         8250
#define IDS_REASON_UNPLANNED_OPERATINGSYSTEM_RECONFIG_TITLE     8256
#define IDS_REASON_UNPLANNED_OPERATINGSYSTEM_UPGRADE_TITLE      8254
#define IDS_REASON_UNPLANNED_OTHER_TITLE                        8261
#define IDS_RESTART_OTHERUSERS          714
#define IDS_SHUTDOWN_OTHERUSERS         713
#define IDS_SHUTDOWN_REMOTE             711
#define IDS_SHUTDOWN_REMOTE_OTHERUSERS  712
#define ID_CLOSEMENU            0x20
#define ID_CNTX_AAFS        0x0000800FL
#define ID_CNTX_ASS         0x0000800DL
#define ID_CNTX_DISPLAYCTRL 0x00008001L
#define ID_CNTX_IAFS        0x00008010L
#define ID_CNTX_INSERTCTRL  0x00008013L
#define ID_CNTX_ISS         0x0000800EL
#define ID_CNTX_LRE         0x00008006L
#define ID_CNTX_LRM         0x00008004L
#define ID_CNTX_LRO         0x00008008L
#define ID_CNTX_NADS        0x0000800BL
#define ID_CNTX_NODS        0x0000800CL
#define ID_CNTX_PDF         0x0000800AL
#define ID_CNTX_RLE         0x00008007L
#define ID_CNTX_RLM         0x00008005L
#define ID_CNTX_RLO         0x00008009L
#define ID_CNTX_RS          0x00008011L
#define ID_CNTX_RTL         0x00008000L
#define ID_CNTX_US          0x00008012L
#define ID_CNTX_ZWJ         0x00008002L
#define ID_CNTX_ZWNJ        0x00008003L
#define ID_DIALOGSYSMENU        0x30
#define ID_EC_PROPERTY_MENU      1
#define ID_HELPMENU            4
#define ID_HSCROLLMENU          0x40
#define ID_SYSMENU              0x10
#define ID_VSCROLLMENU          0x50
#define STR_ABORT                   802
#define STR_CANCEL                  801
#define STR_CLOSE                   807
#define STR_CONTINUE                810
#define STR_HELP                    808
#define STR_IGNORE                  804
#define STR_IMECLOSE                701
#define STR_IMEOPEN                 700
#define STR_NO                      806
#define STR_OK                      800
#define STR_RECONVERTSTRING         705
#define STR_RETRY                   803
#define STR_SHUTDOWN_POWEROFF       707
#define STR_SHUTDOWN_REBOOT         708
#define STR_SHUTDOWN_SHUTDOWN       706
#define STR_SOFTKBDCLOSE            703
#define STR_SOFTKBDOPEN             702
#define STR_TRYAGAIN                809
#define STR_TT_HELP                 904
#define STR_TT_MAX                  901
#define STR_TT_MIN                  900
#define STR_TT_RESDOWN              903
#define STR_TT_RESUP                902
#define STR_TT_SCLOSE               905
#define STR_UNKNOWN                 709
#define STR_YES                     805
#else
/* OPENNT-HOST-117: original winuserp.h private button style. */
#define BS_PUSHBOX          0x0000000AL
#define BS_HORZMASK         0x00000300L
#define BS_VERTMASK         0x00000C00L
/* OPENNT-HOST-116: original winuserp.h private DrawState constants. */
#define DST_TEXTMAX             0x0002
#define DST_GLYPH               0x0005
#define DST_TYPEMASK            0x0007
#define DSS_DEFAULT             0x0040
/* OPENNT-HOST-113: original winuserp.h resource-table constants. */
#define DFC_CACHE               0xFFFF
#define DFCS_INMENU             0x0040
#define DFCS_INSMALL            0x0080
#define DFCS_CACHEICON          0x0000
#define DFCS_CACHEBUTTONS       0x0001
#include <string.h>
#include <wchar.h>
int WCSToMBEx(WORD, LPCWSTR, int, LPSTR *, int, BOOL);
#include "wow_multibyte.h"
#define WCSToMB(pUnicodeString, cbUnicodeChar, ppAnsiString, nAnsiChar,\
bAllocateMem)\
WCSToMBEx(0, pUnicodeString, cbUnicodeChar, ppAnsiString, nAnsiChar, bAllocateMem)
#include <pshpack2.h>
typedef struct {
    WORD wDlgVer;
    WORD wSignature;
    DWORD dwHelpID;
    DWORD dwExStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
} DLGTEMPLATE2;
typedef DLGTEMPLATE2 *LPDLGTEMPLATE2A;
typedef DLGTEMPLATE2 *LPDLGTEMPLATE2W;
typedef DLGTEMPLATE2 *LPDLGTEMPLATE2;
typedef struct {
    DWORD dwHelpID;
    DWORD dwExStyle;
    DWORD style;
    short x;
    short y;
    short cx;
    short cy;
    DWORD dwID;
} DLGITEMTEMPLATE2;
#include <poppack.h>

#define ES_FMTMASK          0x0003L
#define ES_COMBOBOX         0x0200L
#define DDL_NOFILES         0x1000
#define WM_LBTRACKPOINT                 0x0131
#define WM_BEGINDRAG                    0x022C
#define SW_SCROLLWINDOW     0x8000  /* Called from ScrollWindow() */
#define DS_VALID40          0x3FFFL
#define DS_COMMONDIALOG     0x4000L
#include "wow_dialog_flags.h"
#endif /* RC_INVOKED */
#endif
