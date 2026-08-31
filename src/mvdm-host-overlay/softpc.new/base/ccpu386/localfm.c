/*
 * DIVERGENCE(MVDM-HOST-DIV-137): CCPU's selected `ccpusas4.c` already owns
 * `Sas`, whereas the historical localfm carrier declares it beside Gdp, Cpu,
 * and Video.  Retain the same original state-carrier names for the selected
 * CCPU40 profile without creating a second SAS state object.
 */
#include <insignia.h>
#include <host_def.h>
#include <host_inc.h>
/* The CCPU carrier owns the complete CPU vector, while the paired C-VID
 * generator owns the complete VideoVector declaration.  Do not let generic
 * include ordering silently select CCPU's five-slot generated evidgen.h. */
#include "../../../../mvdm-host/softpc.new/base/cvidc/cpuint_c.h"
typedef enum CPU_INT_TYPE CPU_INT_TYPE;
#include "../../../../mvdm-host/softpc.new/base/cvidc/cpu4gen.h"
#include "../../../../mvdm-host/softpc.new/base/cvidc/evidgen.h"

IHP Gdp;
struct CpuVector Cpu;
struct VideoVector Video;
