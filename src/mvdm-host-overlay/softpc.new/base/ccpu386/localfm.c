/*
 * DIVERGENCE(MVDM-HOST-DIV-137): CCPU's selected `ccpusas4.c` already owns
 * `Sas`, whereas the historical localfm carrier declares it beside Gdp, Cpu,
 * and Video.  Retain the same original state-carrier names for the selected
 * CCPU40 profile without creating a second SAS state object.
 */
#include <insignia.h>
#include <host_def.h>
#include <host_inc.h>
#include CpuH
#include "evidgen.h"

IHP Gdp;
struct CpuVector Cpu;
struct VideoVector Video;
