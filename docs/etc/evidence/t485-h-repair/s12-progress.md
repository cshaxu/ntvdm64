# S12 Ordered Host Boundary Repair Progress

Admitted after [S11 closure](s11-progress.md#s11-closure), under the existing
[repair plan](../../operations/m0-t485-h-repair-plan.md). Baseline c205781fe,
published runtime26,Full23/23. Scope48files645frozen points,not fresh XP
migration. Review in relative-path DIR order and original point order;complete
each file before delivery. Default original valid;retains need real evidence.

Reviewed6/48files38/645points;restored-delivered8,restored-local1,
residual-reviewed29,unreviewed607.
Delivered/pushed5files30points through c43ad57bb.
Current byte-sensitive census is measured separately from frozen points.
P1/P2 cached build,dual publication,targeted verification and commit/push complete.

| Order | File | Frozen points | State |
| ---: | --- | ---: | --- |
| 1 | src/mvdm-host/dos/command/cmd.h | 4 | P1 delivered806a8c45c |
| 2 | src/mvdm-host/dos/command/cmdconf.c | 2 | P1 delivered806a8c45c |
| 3 | src/mvdm-host/dos/command/cmdenv.c | 2 | P1 delivered806a8c45c |
| 4 | src/mvdm-host/dos/command/cmdexec.c | 20 | P1 delivered806a8c45c |
| 5 | src/mvdm-host/dos/command/cmdkeyb.c | 2 | P2 delivered c43ad57bb |
| 6 | src/mvdm-host/dos/command/cmdmisc.c | 8 | P3 final verified/published; commit pending |
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

## P1 first three files

| File | Before nonerror points/lines | Local after | Reduction | Frozen disposition |
| --- | ---: | ---: | ---: | --- |
| cmd.h | 23/32 | 8/8 | 15/24 | 4 restored locally |
| cmdconf.c | 9/11 | 4/6 | 5/5 | 2 evidence-backed residuals |
| cmdenv.c | 2/6 | 1/2 | 1/4 | 1 restored,1 residual |

Completed source reviews total34/49 ->13/16 (-21points/-33lines).
The coupled cmdexec.c signature is restored locally using the existing
void/cdecl/parameter thread bridge;the incoming NULL and owned snapshot are
unchanged. It currently36/116 ->20/93,but its20frozen points are not yet
counted reviewed. The source-order fixture follows the restored signature.
Current four-file batch70/165 ->33/109 (-37/-56),not delivered.

Header format restoration matches exact original token values: in particular
TRUE_IF_WIN32 remains0 in the selected else branch. Nine header exclusions and
two config exclusions remain. Static token/logical-source position comparison
passes for unchanged header tokens;product preprocessing/compilation remains
pending. No original guest modification and no runtime claim.

USERENV evidence is the recorded actual export/COMMAND control in
[H002-H004](../t485-s1/h002-h004.md);the optional named query keeps the original
fallback. The ordinal constants are harmless declarations and restored.
Package-root evidence is GetConfigAutoexec's achSysRoot/SHELL construction and
compat/system_root.c loaded-image derivation;restoring the native Windows root
would select a different system-file cohort. No new provider or algorithm.

Next:complete cmdexec.c in file order,including every retained point and line
anchor,before cached full build/dual publication/targeted tests/commit/push.
Previous runtime26 Full23/23 remains historical;it does not verify these changes.

P1 continuation:cmdexec.c also restores the original unused cmdExec32
PSTD_HANDLES declaration,removes its unconsumed shadow-registry include,and
removes18numerically redundant line directives. Registry call search found no
NtOpenKey/NtQueryValueKey/NtEnumerateValueKey/NtClose consumer in this file.
Original parameter and snapshot remain;no additional source owner is created.
Whole-file20point review is still in progress;no completed-file count increase.

S12 byte-sensitive baseline census:48files803points31809lines at545d04836.
Current local census766points31753lines,reduction37points56lines. Evidence:
build/t485-s12/p1/census.json plus cmdexec-local.json;the latter updates the
cmdexec row after the initial census781/31773. The inherited exclusion matcher
was extended for an exact original directive matched by its adjacent #line
(DEBUG_MEM);the original-directive equality assertion remains mandatory.
This census is distinct from645frozen audit points. First census attempt
rejected that non-CPU40 anchor;no partial result was presented as the total.

## P1 complete source review before build

The four whole-file reviews are complete:4/48files28/645frozen points,
7restored-local21residual617unreviewed;0verified/delivered. cmdexec.c final
nonerror36/116 ->20/95 (-16/-21). Batch70/165 ->33/111 (-37/-54).
S12 current766/31755 from803/31809. Previous local counts above are snapshots.

XP logical-line audit found pre-existing old-baseline anchors:cmdconf.c used
132/622 instead of182/788,and cmdexec.c had several old offsets. These are now
corrected. cmdexec gets explicit anchors for the inserted snapshot declaration,
common status branch and native-thread creation. Static unique-original-statement
check matches141header,255config,399environment statements;two cmdexec added
error statements share original literal text and are not original-position
witnesses. Final product preprocessing follows the actual worker compiler flags.

Retained point dispositions and source/ABI/failure evidence are now in both
CSV registers. Tests planned:existing x86 thread lifecycle,COMMAND capture,
source-order guard,real native EDIT launch/input/exit followed by DOS MEM.
Only affected targeted gates run for this P;Full remains due at10files/S closure.
Owned cached-build/publication process12380 uses final/deliver.ps1 under
build/t485-s12/p1,with stdout/stderr and stage logs. Publication is not yet claimed.

## P1 final package verification

[Receipt](s12-p1-result.json):cached full build PASS,runtime1 publication
26/26host-image hashes PASS. Original guest unchanged. x86 thread lifecycle,
167COMMAND capture assertions,and real EDIT screen/Ctrl+Q/CMD echo/DOS MEM
return all PASS. Existing native-return contract expects launcher exit1;not
represented as exit0. Final product preprocessing checks620unique original
statements with zero line drift (104header,121config,295environment,100exec).
One added error statement shares an original literal but is not an original
position witness. Nine header and two config exclusions remain.

First runner failed before preprocessing due to CMD relative slash parsing.
Second passed preprocessing/thread/capture but the old source guard rejected
XP's original NEC_98 block. Guard corrected only for the unselected platform;
failed guard retried and previously unexecuted native test run once. No source
or package rebuild and no successful gate repeated. Full not due for this P.

Files1-4 verified/published;7restored21residual617unreviewed.
Next file5 cmdkeyb.c after P1 commit/push. No migration batch count increment.

P1 delivered and pushed806a8c45c. Governance/relative links PASS.
Current4/48files28/645points delivered;7restored21residual617unreviewed.
No local source restorations awaiting verification. Nextfile5 cmdkeyb.c.

## P2 file5 cmdkeyb.c

Entry2points6lines;two frozen points. Real linked original-reader control
returned native Windows system32 KEYBOARD.SYS instead of its package
system32 path:exit6 after four prior cases PASS. First control build exposed
missing fixture RcMessageBox/TerminateVDM failure exits;they now abort the test,
never suppress product failures. The controlled directory comparison then failed
exactly as expected. No host keyboard/Registry or guest binary modified.

Restore both original directory-call spellings and bind once in this TU to
the existing package-directory provider. Keep the package Registry projection.
Final1point5lines (-1/-1);body identical to fixed XP except the five-line
integration preamble. XP logical line20 replaces stale17;stale166 disappears.
No alternative KDF algorithm,second directory fallback or new runtime owner.
Extend existing reader fixture with real package-directory resolution and
insufficient-buffer length/no-write assertions;existing integration runner
requires its new marker. Full cached build/publication process236 active.

Reviewed5/48files30/645points:8restored(7delivered1local),22residual,
615unreviewed. Delivered4files28points. Current765/31754;baseline803/31809,
cumulative reduction38points55lines. P2 not yet verified or delivered.

### P2 final verification

[Receipt](s12-p2-result.json):runtime2 cached full build and26dual-location
host hashes PASS. Actual original-reader counterexample exit6 now passes with
matching package KDF path;small-buffer size/no-write assertions PASS. Existing
four reader branches PASS;real keyboard PIF MEM output and exit0 PASS.
Product preprocessing113unique original statements,zero XP line drift.
Initial preprocessing output-directory typo corrected by relocating its owned
output;no compilation/publication repeated. Guest binaries unchanged.
No claim of interactive non-US keyboard layout installation. Full not due.
Five files30points final verified;8restored22residual615unreviewed.
P2 delivered/pushed c43ad57bb;next file6 cmdmisc.c. Current765/31754 (-38/-55).

P2 documentation governance and relative links PASS after correcting the native-path prose;exact control output retained in build evidence. No product test repeated.

## P3 file6 cmdmisc.c

Whole-file review23points116lines ->8points93lines (-15/-23),one existing
CHECK_IT_LATER error exclusion retained. Original variable spelling and
spacing restored;stale NT4 logical lines replaced by XP anchors. Executable
tokens are identical after the local CurDir-to-lpszCurDir rename. No added
function,algorithm,shim or guest change. R00071 fully restored;seven residuals
have explicit provider/ABI/runtime evidence in both registers.

The261-byte advertised current-directory buffer requires the existing261-byte
local storage,not original68. Guest current-directory direct pointer has no
whole-range mapping contract;retain bounded staging/SAS copy. WOW initial
SYSTEMROOT projection remains guest-only and its actual1185/1184 overwrite
requires the existing capacity return;XP guest handles CF/AX retry unchanged.

Source-reviewed6/48files38/645points,9restored(8delivered1local),29residual,
607unreviewed. Current750/31731 from803/31809;reduction53points78lines.
Fivefiles30points delivered;P3 cached build,dual publication,product line
verification and existing guest-location/package/WOW targeted tests follow.

P3 initial product preprocessor check rejected a line anchor inside the
unselected CHECK_IT_LATER branch. Move it after endif (line750);same8/93
metric. Three added environment cleanup statements match older original text
but are not original-position witnesses. The checker records those separately.
First runtime3 build/publication succeeded;runtime tests had not started.
Rebuild/publication of corrected source targets immutable runtime4;no successful
runtime gate is repeated. Initial failure retained under p3/final/attempt1.

### P3 final verification

[Receipt](s12-p3-result.json):runtime4 cached build and dual26hash publication
PASS. Product preprocessing151original statement witnesses,zero drift;three
added cleanup statements are not original witnesses. Existing guest-location
lifecycle/failure fixture,ten package-resource cases including WOW initial
environment projection,and real private-desktop WINMINE visible window PASS.
No original guest modification,rebuild or Full run. This is startup/window
coverage,not new gameplay/normal-exit evidence. P3 commit/push pending.

Sixfiles38points final verified;9restored29residual607unreviewed.
Nonerror750/31731,down53points78lines from S12 baseline.
