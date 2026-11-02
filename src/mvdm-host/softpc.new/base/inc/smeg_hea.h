/*[
************************************************************************

        Name:           smeg_head.h
        Author:         W. Plummer
        Created:        May 1992
        Sccs ID:        @(#)smeg_head.h	1.4 08/10/92
        Purpose:        Stats Gathering for the SMEG utility

        (c)Copyright Insignia Solutions Ltd., 1992. All rights reserved.

************************************************************************
]*/

#ifdef SMEG
#error DIVERGENCE MVDM-HOST-DIV-605: unsupported SMEG profile
#else /* SMEG */
#line 91

#define SMEG_INC(smeg_id)		/* SMEG_INC == NOP */
#define SMEG_SET(smeg_id)		/* SMEG_SET == NOP */
#define SMEG_CLEAR(smeg_id)		/* SMEG_CLEAR == NOP */

#endif /* SMEG */
#line 97

#define SMEG_TRUE  1
#define SMEG_FALSE 0
