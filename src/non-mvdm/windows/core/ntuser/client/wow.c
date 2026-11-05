/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/client/wow.c true subset.
 * DIVERGENCE(OPENNT-HOST-108): retain only the unchanged _GetDlgItem
 * direct-child lookup. Other WOW client services have separate owners.
 * Standalone precomp supplies the retained, same-address WND graph. */
#line 14
#include "precomp.h"

#line 15
#ifndef WOW
#line 16
#line 17
#endif
#line 18

#line 19
#ifdef WOW
#line 20
#error Excluded unsupported original 16-bit WOW source branch
#line 119
#else // WOW
#line 120
#line 137
#endif // !WOW
#line 249


PWND _GetDlgItem(
    PWND pwnd,
    int id)
{
    if (pwnd != NULL) {
        pwnd = REBASEPWND(pwnd, spwndChild);
        while (pwnd != NULL) {
            if ((int)pwnd->spmenu == id)
                break;
            pwnd = REBASEPWND(pwnd, spwndNext);
        }
    }

    return pwnd;
}

#line 313
#ifdef WOW
#line 314
#error Excluded unsupported original 16-bit WOW source branch
#line 322
#endif
#line 323

#line 1067
#ifdef WOW
#line 1068
#error Excluded unsupported original 16-bit WOW source branch
#line 1076
#endif
#line 1077


#line 304
#ifdef WOW
#line 305
#error Excluded unsupported original WOW branch
#line 307
#else
#line 308
#line 309
#endif
#line 310

#line 435
#ifdef WOW
#line 436
#error Excluded unsupported original WOW branch
#line 442
#else
#line 443
#line 460
#endif
#line 461

#line 719
#ifdef WOW
#line 720
#error Excluded unsupported original WOW branch
#line 726
#else
#line 727
#line 728
#endif
#line 729

#line 776
#ifdef WOW
#line 777
#error Excluded unsupported original WOW branch
#line 779
#else
#line 780
#line 781
#endif
#line 782

#line 795
#ifdef WOW
#line 796
#error Excluded unsupported original WOW branch
#line 802
#else
#line 803
#line 804
#endif
#line 805

#line 835
#ifdef WOW
#line 836
#error Excluded unsupported original WOW branch
#else
#line 838
#line 839
#endif
#line 840

#line 890
#ifndef WOW
#line 891
#line 893
#endif
#line 894

#line 911
#ifdef WOW
#line 912
#error Excluded unsupported original WOW branch
#else
#line 914
#line 1062
#endif
#line 1063


#line 519
#ifdef ENABLE_CLIENTSIDE_ACCESSCHECK
#line 520
#error Excluded unsupported original ENABLE_CLIENTSIDE_ACCESSCHECK branch
#line 526
#endif
#line 527

#line 752
#ifdef ENABLE_CLIENTSIDE_ACCESSCHECK
#line 753
#error Excluded unsupported original ENABLE_CLIENTSIDE_ACCESSCHECK branch
#line 758
#endif
#line 759

