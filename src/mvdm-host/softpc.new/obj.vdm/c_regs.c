#ifndef MONITOR

//
// temp file for Assembler CPU which provides a simple C cpu -> A cpu register
// mapping for MS libraries (DEM/WOW32/XMS) which are linked for ccpu register
// gets & sets. STF.

#include "host_def.h"
#include "insignia.h"
#include "xt.h"
#include CpuH

#ifdef A3CPU
#error Excluded unsupported A3CPU configuration
#endif	// A3CPU
#endif  // !MONITOR
