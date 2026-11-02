#include "windows.h"
#include "host_def.h"
#include "insignia.h"
/*[
Name:  hunt.c
	Derived From:	X_hunt.c ( base 2.0 )
	Author:		gvdl ( Original by Mike McCusker )
	Created On:	3 May 1991
Sccs ID: 07/29/91 @(#)hunt.c 1.7
	Purpose:	Contains all host_dependent code for hunter

	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.

]*/

#include <stdio.h>
#include <errno.h>
#include TypesH


#include "xt.h"
#include "ios.h"
#include CpuH
#include "sas.h"
#include "bios.h"
#include "gvi.h"
#include "cga.h"
#include "error.h"
#include "config.h"
#include "nt_uis.h"



#include "debug.h"

#ifdef HUNTER
#error Excluded unsupported HUNTER configuration
#endif /*HUNTER*/
