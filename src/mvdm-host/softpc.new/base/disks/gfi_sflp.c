#include "insignia.h"
#include "host_def.h"
/*[
	Name:		gfi_sflop.c
	Derived From:	2.0 gfi_sflop.c
	Author:		Jerry Kramskoy
	Created On:	Unknown
	Sccs ID:	@(#)gfi_sflop.c	1.16 08/14/92
	Purpose:
		Interface between FLA and the IBM PC. The PC acts as a server
		for remote procedure calls from the base product to access
		the 8272A controller.
	Notes:	Invalid FDC commands should be trapped by FLA

	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.

]*/
#ifdef SLAVEPC
#error Excluded unsupported SLAVEPC configuration
#endif /* SLAVEPC */
