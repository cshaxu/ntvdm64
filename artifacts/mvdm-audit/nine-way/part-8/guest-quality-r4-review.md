# Guest classification review r4

The frozen current combined ledger yielded 33 previously unsampled excluded guest inputs and no remaining guest partial rows under these exact filters. Frozen inputs, generator and sample are `guest-quality-r4-baseline.json`, `guest-quality-r4-freeze.py` and `guest-quality-r4-frozen.csv`. The observer `guest-quality-r4-observer.py` reproduces per-file current and two original hashes and literal source references in `guest-quality-r4-observations.json`. All 33 live current hashes match the freeze and neither original differs. Actual changed mirror sources must remain partial; none were found here.

The classification question is whether *overall exclusion* is justified, not whether these immutable build inputs are linked into the current host. Below are exact references and conservative recommendations. Includes and filenames are source evidence, not proof of resolved include path, produced guest bytes, actual locale or working behavior. Missing matches do not prove the input has no use. These are pending candidates until coordinator confirms capability applicability or a replacement provider; historical build-tool source can remain excluded only with the scope/authority established for that independent tool. No product, guest, shared ledger, build or process changed.

| Path | Example original/current historical reference | Review conclusion |
|---|---|---|
| `dos/v86/doskrnl/dos/divmes.inc` | `src/mvdm/dos/v86/doskrnl/dos/dosmes.inc:72:include divmes.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/doskrnl/dos/MSDOS.CL1` | `src/mvdm/dos/v86/doskrnl/dos/divmes.inc:6:include msdos.cl1` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/doskrnl/dos/MSDOS.CL2` | `src/mvdm/dos/v86/doskrnl/dos/makefile:28:msdos.cl2: msdos.skl /` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/doskrnl/dos/MSDOS.CL3` | `src/mvdm/dos/v86/doskrnl/dos/getset.asm:432:;hkn; NLS_YES, NLS_NO, NLS_yes2, NLS_no2 is defined in msdos.cl3 which is` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/doskrnl/dos/MSDOS.CL4` | `src/mvdm/dos/v86/doskrnl/dos/divmes.inc:16:include msdos.cl4			;M018` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/doskrnl/dos/msdos.skl` | `src/mvdm/dos/v86/doskrnl/dos/dos.tag:87:		 MSDOS.SKL 	drive.` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/conio.h` | `src/mvdm/dos/v86/cmd/mem/mem.c:62:#include "conio.h"			/* need for kbhit prototype */` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/copyrigh.h` | `src/mvdm/dos/v86/cmd/mem/makefile:51:                $(hinc)/copyrigh.h` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/ctype.h` | `src/mvdm/dos/v86/cmd/command/buildmsg.c:13:#include "ctype.h"` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/direct.h` | `src/mvdm/dos/v86/cmd/setver/parse.c:38:#include	<direct.h>` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/dos.h` | `src/mvdm/dos/v86/cmd/command/buildmsg.c:9:#include "dos.h"` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/fcntl.h` | `src/mvdm/dos/v86/cmd/setver/setver.c:66:#include <fcntl.h>` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/io.h` | `src/mvdm/dos/v86/cmd/command/buildmsg.c:11:#include "stdio.h"` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/stdio.h` | `src/mvdm/dos/v86/cmd/command/buildmsg.c:11:#include "stdio.h"` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/stdlib.h` | `src/mvdm/dos/v86/cmd/command/buildmsg.c:12:#include "stdlib.h"` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/string.h` | `src/mvdm/dos/v86/cmd/command/buildmsg.c:10:#include "string.h"` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/version.h` | `src/mvdm/dos/v86/cmd/mem/makefile:50:                $(hinc)/version.h               /` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/h/versionc.h` | `src/mvdm/dos/v86/cmd/mem/memex.c:19:#include "versionc.h"` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/bdsize.inc` | `src/mvdm/dos/v86/doskrnl/dos/makedos.cmd:3:out -f ../inc/bdsize.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/cmacros.inc` | `src/mvdm/dos/v86/inc/version.inc:30:; Use the following switches to control cmacros.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/dirent.inc` | `src/mvdm/dos/v86/cmd/command/makefile:39:               $(inc)/dirent.inc        /` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/dseg.inc` | `src/mvdm/dos/v86/cmd/keyb/commsubs.asm:73:	INCLUDE DSEG.INC` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/ea.inc` | `src/mvdm/dos/v86/cmd/command/makefile:285:               $(inc)/ea.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/intnat.inc` | `src/mvdm/dos/v86/cmd/command/makefile:481:               $(inc)/intnat.inc        /` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/oemnum.inc` | `No literal basename reference in searched DOS v86 tree` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/struc.inc` | `src/mvdm/dos/v86/cmd/graphics/grbwprt.asm:117:INCLUDE STRUC.INC			;					;AN000;` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/inc/umb.inc` | `src/mvdm/dos/v86/doskrnl/bios/sysconf.asm:33:	include	umb.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/make.inc` | `src/mvdm/dos/v86/cmd/append/makefile:10:include ../../make.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/messages/comimsg.inc` | `src/mvdm/dos/v86/cmd/command/makefile:504:	       $(msg)/comimsg.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/messages/comrmsg.inc` | `src/mvdm/dos/v86/cmd/command/makefile:193:	       $(msg)/comrmsg.inc` | Pending: guest build input alone does not establish excluded capability |
| `dos/v86/tools/src/h2inc/h2inc.c` | `src/mvdm/dos/v86/tools/src/h2inc/h2inc.mak:14:h2inc.obj:	h2inc.c $(H286)/ctype.h $(H286)/io.h /` | Pending: historical tool source; independent tool scope needs proof |
| `dos/v86/tools/src/stripdd/stripdd.c` | `src/mvdm/dos/v86/tools/src/stripdd/sources:9:SOURCES=stripdd.c` | Pending: historical tool source; independent tool scope needs proof |
| `dos/v86/tools/src/stripz/stripz.c` | `src/mvdm/dos/v86/tools/src/stripz/sources:9:SOURCES=stripz.c` | Pending: historical tool source; independent tool scope needs proof |

Country build specifics: `nine-way/part-2/dos-country-semantic-reconciliation.md` documents the `msdos.skl`/country message generation chain. It does not establish which locale/data the shipped kernel selected. Existing shipped MEM/COMMAND witnesses do not by themselves bind each input or prove all of its branches. `shard-2/dos-v86-build-tools-review.md` documents the three historical tool recipes; it does not make evidence of guest image provenance or prove every tools profile is irrelevant.

## Reproducibility correction

The observer normalizes and sorts rg results before truncating. Two consecutive observations had SHA-256 A5B4ECCA494E8332B745606A39E2EF20D26840948749E310E4E07F0CE8E06314. The frozen ledger hash matches Git revision 7d365c9bb and is saved in guest-quality-r4-ledger-frozen.csv. guest-quality-r4-replay.py reproduces the frozen sample byte-for-byte, SHA-256 3D38FD11634269985221E0304751182F5C393E725C1A4407060654310D1100AD. Current ledger changes are not applied to the frozen sample.
