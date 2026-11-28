/* Copyright (c) 1985-95, Microsoft Corporation.
 * OpenNT windows/core/ntuser/kernel/inctlpan.c:79-94.
 * Revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * Include after the button resource layout declares its button-index range.
 */
#ifndef WOW_BUTTON_STYLE_TABLE_H
#define WOW_BUTTON_STYLE_TABLE_H

#include "wow_control_resource_abi.h"

/* Row zero corresponds to OBI_RADIOMASK. */
static CONST WORD sysBmpStyles[OBI_3STATE_CDI - OBI_RADIOMASK + 1][2] = {
    DFC_BUTTON,    DFCS_BUTTONRADIOMASK,                            // OBI_RADIOMASK
    DFC_BUTTON,    DFCS_BUTTONCHECK,                                // OBI_CHECK
    DFC_BUTTON,    DFCS_BUTTONCHECK | DFCS_CHECKED,                 // OBI_CHECK_C
    DFC_BUTTON,    DFCS_BUTTONCHECK | DFCS_PUSHED,                  // OBI_CHECK_D
    DFC_BUTTON,    DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_PUSHED,   // OBI_CHECK_CD
    DFC_BUTTON,    DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_INACTIVE, // OBI_CHECK_CDI
    DFC_BUTTON,    DFCS_BUTTONRADIOIMAGE,                                // OBI_RADIO
    DFC_BUTTON,    DFCS_BUTTONRADIOIMAGE | DFCS_CHECKED,                 // OBI_RADIO_C
    DFC_BUTTON,    DFCS_BUTTONRADIOIMAGE | DFCS_PUSHED,                  // OBI_RADIO_D
    DFC_BUTTON,    DFCS_BUTTONRADIOIMAGE | DFCS_CHECKED | DFCS_PUSHED,   // OBI_RADIO_CD
    DFC_BUTTON,    DFCS_BUTTONRADIOIMAGE | DFCS_CHECKED | DFCS_INACTIVE,  // OBI_RADIO_CDI
    DFC_BUTTON,    DFCS_BUTTON3STATE,                               // OBI_3STATE
    DFC_BUTTON,    DFCS_BUTTON3STATE | DFCS_CHECKED,                // OBI_3STATE_C
    DFC_BUTTON,    DFCS_BUTTON3STATE | DFCS_PUSHED,                 // OBI_3STATE_D
    DFC_BUTTON,    DFCS_BUTTON3STATE | DFCS_CHECKED | DFCS_PUSHED,   // OBI_3STATE_CD
    DFC_BUTTON,    DFCS_BUTTON3STATE | DFCS_CHECKED | DFCS_INACTIVE,  // OBI_3STATE_CDI
};

#endif
