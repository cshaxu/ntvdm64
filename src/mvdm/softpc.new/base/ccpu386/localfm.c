#include <insignia.h>
#include <host_def.h>
#include <host_inc.h>
/* DIVERGENCE(MVDM-HOST-DIV-137): the selected CCPU40 SAS provider owns
 * `Sas`; retain the original remaining shared-state carrier without a second
 * SAS vector.  The C-VID profile supplies the exact CPU/Video layouts. */
#include "../cvidc/cpuint_c.h"
typedef enum CPU_INT_TYPE CPU_INT_TYPE;
#include "../cvidc/cpu4gen.h"
#include "../cvidc/evidgen.h"

IHP Gdp;
struct CpuVector Cpu;
struct VideoVector Video;
