# S12 Ordered Host Boundary Repair Progress

Admitted after [S11 closure](s11-progress.md#s11-closure), under the existing
[repair plan](../../operations/m0-t485-h-repair-plan.md). Baseline c205781fe,
published runtime26,Full23/23. Scope48files645frozen points,not fresh XP
migration. Review in relative-path DIR order and original point order;complete
each file before delivery. Default original valid;retains need real evidence.

Initial reviewed0/48files0/645points;restored0,residual-reviewed0,unreviewed645.
Current byte-sensitive census is measured at file entry,not inferred from frozen
points. No S12 code change,verification or delivery claimed.

| Order | File | Frozen points | State |
| ---: | --- | ---: | --- |
| 1 | src/mvdm-host/dos/command/cmd.h | 4 | Pending |
| 2 | src/mvdm-host/dos/command/cmdconf.c | 2 | Pending |
| 3 | src/mvdm-host/dos/command/cmdenv.c | 2 | Pending |
| 4 | src/mvdm-host/dos/command/cmdexec.c | 20 | Pending |
| 5 | src/mvdm-host/dos/command/cmdkeyb.c | 2 | Pending |
| 6 | src/mvdm-host/dos/command/cmdmisc.c | 8 | Pending |
| 7 | src/mvdm-host/dos/dem/dem.c | 2 | Pending |
| 8 | src/mvdm-host/dos/dem/dem.h | 1 | Pending |
| 9 | src/mvdm-host/dos/dem/demgset.c | 4 | Pending |
| 10 | src/mvdm-host/dos/dem/demhndl.c | 12 | Pending |
| 11 | src/mvdm-host/dos/dem/demlfn.c | 2 | Pending |
| 12 | src/mvdm-host/dos/dem/demlfn.h | 2 | Pending |
| 13 | src/mvdm-host/dos/dem/demsrch.c | 5 | Pending |
| 14 | src/mvdm-host/inc/dbgexp.h | 1 | Pending |
| 15 | src/mvdm-host/inc/demexp.h | 3 | Pending |
| 16 | src/mvdm-host/inc/doswow.h | 1 | Pending |
| 17 | src/mvdm-host/inc/memapi.h | 5 | Pending |
| 18 | src/mvdm-host/inc/mvdm.h | 2 | Pending |
| 19 | src/mvdm-host/inc/softpc.h | 28 | Pending |
| 20 | src/mvdm-host/inc/suballoc.h | 1 | Pending |
| 21 | src/mvdm-host/inc/vdmredir.h | 1 | Pending |
| 22 | src/mvdm-host/inc/vrnetapi.h | 1 | Pending |
| 23 | src/mvdm-host/inc/vrnmpipe.h | 1 | Pending |
| 24 | src/mvdm-host/oemuni/file.c | 19 | Pending |
| 25 | src/mvdm-host/oemuni/process.c | 65 | Pending |
| 26 | src/mvdm-host/softpc.new/host/src/config.c | 38 | Pending |
| 27 | src/mvdm-host/softpc.new/host/src/nt_bop.c | 16 | Pending |
| 28 | src/mvdm-host/softpc.new/host/src/nt_com.c | 90 | Pending |
| 29 | src/mvdm-host/softpc.new/host/src/nt_cpu.c | 2 | Pending |
| 30 | src/mvdm-host/softpc.new/host/src/nt_emm.c | 29 | Pending |
| 31 | src/mvdm-host/softpc.new/host/src/nt_eoi.c | 2 | Pending |
| 32 | src/mvdm-host/softpc.new/host/src/nt_error.c | 26 | Pending |
| 33 | src/mvdm-host/softpc.new/host/src/nt_keycd.c | 7 | Pending |
| 34 | src/mvdm-host/softpc.new/host/src/nt_mem.c | 8 | Pending |
| 35 | src/mvdm-host/softpc.new/host/src/nt_msscs.c | 111 | Pending |
| 36 | src/mvdm-host/softpc.new/host/src/nt_pif.c | 14 | Pending |
| 37 | src/mvdm-host/softpc.new/host/src/nt_reset.c | 34 | Pending |
| 38 | src/mvdm-host/softpc.new/host/src/nt_rez.c | 1 | Pending |
| 39 | src/mvdm-host/softpc.new/host/src/nt_rflop.c | 25 | Pending |
| 40 | src/mvdm-host/softpc.new/host/src/nt_sound.c | 15 | Pending |
| 41 | src/mvdm-host/softpc.new/host/src/nt_thred.c | 1 | Pending |
| 42 | src/mvdm-host/softpc.new/host/src/nt_unix.c | 1 | Pending |
| 43 | src/mvdm-host/softpc.new/host/src/stubs.c | 4 | Pending |
| 44 | src/mvdm-host/vdd/mscdex/vdd/vcdex.c | 1 | Pending |
| 45 | src/mvdm-host/vdmredir/vrica.h | 1 | Pending |
| 46 | src/mvdm-host/vdmredir/vrnetapi.c | 14 | Pending |
| 47 | src/mvdm-host/vdmredir/vrnmpipe.c | 9 | Pending |
| 48 | src/mvdm-host/vdmredir/vrremote.c | 2 | Pending |

## Verification and boundaries

COMMAND/DEM/OEM,redirector/VDD,host configuration,packaging and lifecycle.
Trace actual providers before restoring or minimizing;preserve OEM/DBCS capacity,
error return and cancellation/cleanup contracts. No native boot-file writes,
original guest patch/rebuild,Computer Use,new queues or duplicate state owners.
Each substantive whole-file batch gets cached full build,dual publication,
affected targeted tests,commit/push;every10files and S closure require Full.
Unchanged code after Full retries only failed gates and executes unrun gates once.
Report current file points/lines,batch and cumulative deltas,reviewed/restored/
residual/unreviewed/verified/delivered counts separately.
