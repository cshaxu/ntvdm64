# S12 Ordered Host Boundary Repair Progress

Admitted after [S11 closure](s11-progress.md#s11-closure), under the existing
[repair plan](../../operations/m0-t485-h-repair-plan.md). Baseline c205781fe,
published runtime26,Full23/23. Scope48files645frozen points,not fresh XP
migration. Review in relative-path DIR order and original point order;complete
each file before delivery. Default original valid;retains need real evidence.

Reviewed48/48files645/645points;restored-final-verified432,restored-local0,
residual-final-verified213,residual-local0,unreviewed0.
Delivered/pushed48files645points through 1930f0852.
Current byte-sensitive census is measured separately from frozen points.
P1-P3 cached build,dual publication,targeted verification and commit/push complete.

| Order | File | Frozen points | State |
| ---: | --- | ---: | --- |
| 1 | src/mvdm-host/dos/command/cmd.h | 4 | P1 delivered806a8c45c |
| 2 | src/mvdm-host/dos/command/cmdconf.c | 2 | P1 delivered806a8c45c |
| 3 | src/mvdm-host/dos/command/cmdenv.c | 2 | P1 delivered806a8c45c |
| 4 | src/mvdm-host/dos/command/cmdexec.c | 20 | P1 delivered806a8c45c |
| 5 | src/mvdm-host/dos/command/cmdkeyb.c | 2 | P2 delivered c43ad57bb |
| 6 | src/mvdm-host/dos/command/cmdmisc.c | 8 | P3 delivered d5e4a83f6 |
| 7 | src/mvdm-host/dos/dem/dem.c | 2 | P4 delivered d74c534ca |
| 8 | src/mvdm-host/dos/dem/dem.h | 1 | P4 delivered d74c534ca |
| 9 | src/mvdm-host/dos/dem/demgset.c | 4 | P4 delivered d74c534ca |
| 10 | src/mvdm-host/dos/dem/demhndl.c | 12 | P4 delivered d74c534ca |
| 11 | src/mvdm-host/dos/dem/demlfn.c | 2 | P5 delivered3b1d831b6 |
| 12 | src/mvdm-host/dos/dem/demlfn.h | 2 | P5 delivered3b1d831b6 |
| 13 | src/mvdm-host/dos/dem/demsrch.c | 5 | P6 delivered ae9241746 |
| 14 | src/mvdm-host/inc/dbgexp.h | 1 | P7 delivered c1b02e99f |
| 15 | src/mvdm-host/inc/demexp.h | 3 | P7 delivered c1b02e99f |
| 16 | src/mvdm-host/inc/doswow.h | 1 | P7 delivered c1b02e99f |
| 17 | src/mvdm-host/inc/memapi.h | 5 | P7 delivered c1b02e99f |
| 18 | src/mvdm-host/inc/mvdm.h | 2 | P7 delivered c1b02e99f |
| 19 | src/mvdm-host/inc/softpc.h | 28 | P7 delivered c1b02e99f |
| 20 | src/mvdm-host/inc/suballoc.h | 1 | P7 delivered c1b02e99f |
| 21 | src/mvdm-host/inc/vdmredir.h | 1 | P8 delivered e25380fe2 |
| 22 | src/mvdm-host/inc/vrnetapi.h | 1 | P8 delivered e25380fe2 |
| 23 | src/mvdm-host/inc/vrnmpipe.h | 1 | P8 delivered e25380fe2 |
| 24 | src/mvdm-host/oemuni/file.c | 19 | P8 delivered e25380fe2 |
| 25 | src/mvdm-host/oemuni/process.c | 65 | P9 delivered c3dca3705 |
| 26 | src/mvdm-host/softpc.new/host/src/config.c | 38 | P10 delivered 38b11ba6c |
| 27 | src/mvdm-host/softpc.new/host/src/nt_bop.c | 16 | P10 delivered 38b11ba6c |
| 28 | src/mvdm-host/softpc.new/host/src/nt_com.c | 90 | P10 delivered 38b11ba6c |
| 29 | src/mvdm-host/softpc.new/host/src/nt_cpu.c | 2 | P10 delivered 38b11ba6c |
| 30 | src/mvdm-host/softpc.new/host/src/nt_emm.c | 29 | P10 delivered 38b11ba6c |
| 31 | src/mvdm-host/softpc.new/host/src/nt_eoi.c | 2 | P11 delivered e0b9f26d5 |
| 32 | src/mvdm-host/softpc.new/host/src/nt_error.c | 26 | P11 delivered e0b9f26d5 |
| 33 | src/mvdm-host/softpc.new/host/src/nt_keycd.c | 7 | P11 delivered e0b9f26d5 |
| 34 | src/mvdm-host/softpc.new/host/src/nt_mem.c | 8 | P11 delivered e0b9f26d5 |
| 35 | src/mvdm-host/softpc.new/host/src/nt_msscs.c | 111 | P11 delivered e0b9f26d5 |
| 36 | src/mvdm-host/softpc.new/host/src/nt_pif.c | 14 | P12 delivered 88cde1caf |
| 37 | src/mvdm-host/softpc.new/host/src/nt_reset.c | 34 | P12 delivered 88cde1caf |
| 38 | src/mvdm-host/softpc.new/host/src/nt_rez.c | 1 | P12 delivered 88cde1caf |
| 39 | src/mvdm-host/softpc.new/host/src/nt_rflop.c | 25 | P12 delivered 88cde1caf |
| 40 | src/mvdm-host/softpc.new/host/src/nt_sound.c | 15 | P12 delivered 88cde1caf |
| 41 | src/mvdm-host/softpc.new/host/src/nt_thred.c | 1 | P13 delivered1930f0852 |
| 42 | src/mvdm-host/softpc.new/host/src/nt_unix.c | 1 | P13 delivered1930f0852 |
| 43 | src/mvdm-host/softpc.new/host/src/stubs.c | 4 | P13 delivered1930f0852 |
| 44 | src/mvdm-host/vdd/mscdex/vdd/vcdex.c | 1 | P13 audited; no change |
| 45 | src/mvdm-host/vdmredir/vrica.h | 1 | P13 delivered1930f0852 |
| 46 | src/mvdm-host/vdmredir/vrnetapi.c | 14 | P13 delivered1930f0852 |
| 47 | src/mvdm-host/vdmredir/vrnmpipe.c | 9 | P13 delivered1930f0852 |
| 48 | src/mvdm-host/vdmredir/vrremote.c | 2 | P13 delivered1930f0852 |

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
coverage,not new gameplay/normal-exit evidence. P3 delivered/pushed d5e4a83f6.

Sixfiles38points final verified;9restored29residual607unreviewed.
Nonerror750/31731,down53points78lines from S12 baseline.

## P4 files7-9 source review, file10 pending

- dem.c11/13->4/6 (-7/-7);two DBG exclusions retained;package NTDOS path
  provider required by original demmisc loader. Only line directives changed.
- dem.h28/33->6/6 (-22/-27);nine exclusions retained. guest_location is unused
  by this header and moves to sole DEM consumer demhndl.c (+1/+2). R00078
  remains a relocated dependency,not counted as a complete restoration.
- demgset.c10/37->4/31 (-6/-6);zero exclusions. Keep BootDir overlay,DOSWOWDATA
  lifecycle/CDS owner projection and proven FileSystemType padding correction.
  Register failure can leave prior state;explicit clear is not redundant.

Combined with demhndl include move,batch reduction34points38lines.
S12 current716/31693 from803/31809,cumulative reduction87points116lines.
Reviewed9files45points;9restored36residual600unreviewed. Delivered remains
6files38points. No build triggered for the completed format/reference edits;
demhndl.c whole-file12point audit is next,then the required10-file Full checkpoint.
All latest hashes and both registers synchronized;P4 is not delivered.

## P4 file10 and ten-file checkpoint

demhndl.c28/85->15/60 including moved include (-13/-25). Two excluded
branches retained;original spacing and correct XP lines restored,execution
tokens unchanged except required include relocation. Existing original
extended-error record writes retained. Twelve reviewed residuals have explicit
first-byte alias,checked-page-copy and every-exit lifecycle justification.
Do not claim new runtime failure equivalence from source inspection alone.

Four-file batch77/168->29/103 (-48points/-65lines). S12 current702/31666,
baseline803/31809,reduction101points143lines. Tenfiles57points reviewed,
9restored48residual588unreviewed;delivered still6files38points.
Owned background20704 runs cached full build,actual compiler line check,
dual runtime5 publication and Full via the existing successful runner.
No file remains partially audited before this batch build. Final acceptance
and commit/push remain pending. Reuse successful Full gates on unchanged code.

P4 cached full build PASS. Initial line checker classified three added cleanup
gotos as the sole original literal;source locations and preprocessor output
confirm they are added statements,not original witnesses. Checker corrected,
246original statements PASS with zero drift. No rebuild or preprocessing rerun.
Continuation owner5304 published runtime5 to both locations (26hash checks),
then started the first Full for this final package. It remains running;no
Full success,commit or delivery completion is claimed. Logs:p4/final/full.log.

### P4 final verification

[Receipt](s12-p4-result.json):runtime5 cached full build,dual26hash publication,
246original-line witnesses and first Full23/23 PASS. Includes three desktops,
enhanced Win3.1/Win95 same-window fullscreen roundtrips,WOW windows,command and
lifecycle gates,and real NESticle menu pixels/Escape response. Win95 retains
approved knownExitOne boundary. No source changes or whole-suite retries after
Full;no original guest change. Initial line-checker false positives remain in
the attempt record. Documentation and relative-link gates passed before receipt.
Four files77/168->29/103;S12 current702/31666,reduction101points143lines.
Tenfiles57points final verified;9restored48residual588unreviewed. Commit/push
pending;S12 remains active and next file is demlfn.c.

### P4 delivery receipt

Product and Full checkpoint committed/pushed d74c534ca. Tenfiles57points
are now delivered;9restored48residual588unreviewed. Last Full checkpoint is
file10,runtime5,23/23 PASS. Remaining38files;next11demlfn.c. Census remains
702points31666lines,down101points143lines. Only receipt state changes after
Full;no repeated build,publication or runtime verification is required.

## P5 LFN ABI whole-file restoration

File11 demlfn.c2/4->0/0:restore XP pointer casts at both ANSI/OEM bindings.
R00098/R00099 are restored locally;the historical failure belongs to the
calling convention in the typedef,not the presence of these casts.
File12 demlfn.h2/4->1/2:restore original PVOID argument (R00101);retain
NTAPI (R00100). Native x86 pointer argument layout is unchanged. This is the
admitted MSVC x86 ABI,not a claim of portability across arbitrary C ABIs.
Both files have zero error exclusions;line counts and logical lines remain XP.
Production object code/relocations are compared before/after the final build;
final published package runs existing private-desktop WOW startup coverage.

Batch4/8->1/2,reduction3points6lines. S12current699/31660,cumulative104/149.
Reviewed12files61points:9restored-delivered3restored-local49residual584unreviewed.
Delivered still10files57points. Owner21176 runs cached full build,dual runtime6
publication and targeted verification. Full just passed at file10/runtime5;
not repeated. No guest changes or new provider/shim. Next13demsrch.c after
P5 delivery. P5 commit/push remain pending.

### P5 final verification

[Receipt](s12-p5-result.json):cached full build,dual runtime6 publication26hashes,
83production code sections20743bytes plus relocations identical,and actual
private-desktop WINMINE window PASS. Source file is byte-exact XP;header only
NTAPI remains. Both retain original lines and have no error exclusions.
No Full repeat,new test framework,wrapper or guest modifications. Three frozen
points restored;one ABI residual remains pending owner decision. Commit/push pending.

P5 product/receipt delivered in3b1d831b6 and pushed. Twelvefiles61points
delivered;12restored49residual584unreviewed. Current699/31660,reduction104/149.
Last Full remains file10/runtime5;next file13demsrch.c.

## P6 file13 directory search

R00102-R00106 retain bounded directory continuation:include,state,end-status
repair,capture and deleted-anchor retry. XP still closes underlying enumeration
at end while retaining buffered entries;reclamation discards them. Its original
slow-reset loop still requires the saved anchor and explicitly notes deleted
anchor failure. The existing indexed kernel route is unavailable under the
admitted user-mode provider;do not invent a replacement or broader mutation
promise. Existing T478 real provider and guest pressure evidence applies to
these unchanged owner algorithms;new verification targets final runtime7.

Restore redundant/stale line directives only:43/80->16/39,reduction27/41;
11error exclusions and execution tokens unchanged. S12current672/31619,
cumulative131points190lines. Reviewed13files66points:12restored54residual,
579unreviewed. Delivered12files61points. Background18092 runs cached build,
actual compiler line check,dual publication and existing provider/pressure/FCB
tests. Next14dbgexp.h after delivery;Full not due at13files.

### P6 final verification

[Receipt](s12-p6-result.json):cached build,dual runtime7 publication26hashes,
285original-line witnesses zero drift PASS. First provider compile failed because
the NT4-era fixture omitted XP FillFCBSrchBuf IsOnCD;pass FALSE for actual owned
hard-disk files. Only that failed gate retried;provider115/115 PASS,then three
previously unexecuted DOS pressure cases and actual FCB first/next/end/missing
with receipt37 PASS. No production changes/rebuild/republish after this test-only
fix. No Full rerun. Mirror43/80->16/39;all11exclusions retained. Commit/push pending.

P6 committed/pushed ae9241746. Thirteenfiles66points delivered;12restored,
54residual,579unreviewed. Census672/31619,reduction131/190. Next14dbgexp.h.

## P7 files14-16 shared ABI headers

dbgexp.h1/6 unchanged:three DBGNotify declarations are required cdecl between
WOW /Gz callers and worker dbg.obj providers. Actual current product link maps
confirm these symbols;not merely an adaptation citing itself.
demexp.h1/108->3/13:restore XP LF from CRCRLF,keep two NTIO constants and five
cdecl declarations,add#line1 after prefix. Constant need is demLoadDos
unconditional sizeof expression;nt_msscs references are FE_SB-guarded and do
not prove a non-FE requirement. Three necessary residuals remain. Formatting
splits one whole-file block into three real blocks;owner permanently approved
this class. No execution/preprocessing tokens changed apart from line mapping.
doswow.h1/6 unchanged:three typed callbacks flow from wdos.c via wkman LFNInit
to original DEM cdecl call pointers. Correct prior generic provider-direction
wording;explicit typedef convention matches actual cdecl provider symbols.
All three files have zero error exclusions;unmodified headers retain XP lines.

Threefiles5frozen points reviewed,all residual. S12reviewed16/48files71/645points,
12restored59residual574unreviewed. Delivered13files66points;current674/31524,
cumulative129points285lines reduction. Only demexp formatting changed;accumulate
into next complete-file batch per owner rule,no build or runtime gate rerun.
Next17memapi.h. No new guest/provider/algorithm/ABI choice.

## P7 files17-18 memory/shared declarations

memapi.h5/10 unchanged. R00228-R00232 correspond to five actual cdecl WOW
imports from worker nt_mem.obj/dpmiselr.obj. WOW /Gz defaults conflict without
explicit declarations;keep minimal existing ABI declarations,not wrappers.
mvdm.h10/18->1/1:restore original DOS EOF marker and newline tail (R00233-234),
original macro line endings,and remove redundant stale line directives. Two
error exclusions retained;one required post-exclusion#line remains. No
execution token changes;actual compilation of restored EOF awaits whole batch.

Reviewed18files78points:12restored-delivered2restored-local64residual567unreviewed.
Current665/31507,cumulative138points302lines reduction. Delivered13files66points;
P7 pending only format/EOF edits in demexp.h and mvdm.h. No runtime tests or
build repeated. Next19softpc.h,then20suballoc.h triggers ten-file Full checkpoint
once both whole-file reviews complete.


## P7 files19-20 and actual ABI correction

softpc.h:1/1632 ->26/122 (zero exclusions). Restore XP LF, original SB
prototype location, redundant declarations and harmless spacing. Four frozen
points R00235/236/243/255 restored. Twenty-four residual points retain explicit
cross-image ABI declarations and opaque PVDM_TIB; original two-argument
cpu_createthread is already restored by S7. No new wrappers or algorithms.

Counter-review: the first cached full build disproved the initial claim that
the i386 setter branch was unused. src/ntvdmd-dll/target.cmake selects /Di386;
its 12 setter imports require the existing WINAPI exports supplied by
src/ntvdm-exe/debugger/register_exports.c. Reinstated those qualifiers before
retry. Initial build.log preserves LNK2019/LNK2001; none of these eleven frozen
points is counted restored. Runtime verification has not yet passed.

suballoc.h:1/226 ->2/4;one historical i386 error exclusion retained. Keep
_M_IX86 selecting original Intel4096 commit geometry, backed by nt_mem.c
4096-page ownership and suballoc commit/decommit callback sizes. Restore LF
and redundant line directives; do not claim a reproduced 65536 failure.

Whole P7 batch20/2006 ->39/162 (+19points,-1844lines), including approved
format restoration splitting whole-file blocks. S12 current691/29775,
reduction112points/2034lines from803/31809. Reviewed20/48files107/645points:
12restored-delivered,6restored-local,89residual,538unreviewed. Delivered remains
13files66points. First owner4340 failed link; retry28264 owns cached build and
line checks. Publication and file20 Full checkpoint still pending. Next file21
vdmredir.h only after this checkpoint delivery.


P7 build retry passed; NTVDMD map confirms all restored stdcall imports. First
line-check attempt selected dem.c, which does not include mvdm.h/softpc.h;
zero witnesses correctly failed the audit. Reused actual NTVDMD preprocessing
instead, without product changes/rebuild. Final witnesses:demexp4,mvdm4,
softpc28,suballoc15,zero drift. Continuation26488 owns first Full on runtime8.
Dual assets/release and O:/WINNT/SYSTEM32 publication verified26/26hashes.
Initial link failure and audit-helper failure remain recorded,not erased.


### P7 final verification

[Receipt](s12-p7-result.json): cached full build, dual26-hash publication and
first runtime8 Full23/23 PASS. All three Windows desktop gates, enhanced
same-Window Alt+Enter round trips, WOW and DOS4GW/NESticle gates passed.
Win95 retains approved exit-one boundary. Actual line witnesses51,zero drift.
No Full retries/reruns. Six restored points and35residual points in this batch;
total20files107points final-verified,18restored89residual538unreviewed.
Mirror batch20/2006->39/162;S12total691/29775,reduction112/2034.
Commit/push pending. Next21vdmredir.h;28files remain;S12 remains active.

P7 product committed/pushed c1b02e99f. Twentyfiles107points delivered;
18restored89residual538unreviewed. Work proceeds to file21vdmredir.h.


## P8 files21-23 redirector headers

vdmredir.h22/23->4/5,12exclusions unchanged. R00264 retains slavePICline3,
paired with unchanged original SP3 guest SHA256
8ecf05026ec3b12b93a0809040e45a43902b16f4a392305be21b2a8417850126.
Installed identity rechecked;H032 actual INT72 timeout/INT73 successful async
callbacks is hard runtime evidence. Remove redundant line directives only.
vrnetapi.h1/301->0/0:byte-exact original LF/EOF restored,R00265 local restoration.
vrnmpipe.h4/6->1/2:one exclusion preserved;R00266 opaque state remains for
asynchronous staging beyond guest lease. Reviewed submission/failure,completion,
no-ANR and callback disposal paths. Existing fixture covers payload-before-count,
failed writes and stale epoch. No claim these tests reran in P8 yet.

Batch27/330->5/7,reduction22points323lines. S12current669/29452,
reduction134points2357lines. Reviewed23files110points:18restored-delivered,
1restored-local,91residual,535unreviewed. Delivered20files107points through
c1b02e99f. All local edits format/line-only;accumulate as authorized.
Next24oemuni/file.c;25files remain. Last Fullfile20/runtime8 unaffected.


## P8 file24 OEM file thunks

Whole-file nonerror1/2323->18/43. Restore LF and original FE_SB framework;
five excluded FE_SB bodies now use#error. Production source-groups selects
profile_1 and fixtures select oem_test_profile,neither definesFE_SB. WOW/Gz
FE_SB belongs to different units. Selected DBCS fixes remain;restore original
file-part pointer-base addition after computing OEM byte offset. No extra helper.
R00275/276/278/279/283/285/293 framework/format restored;12residual points retain
byte-capacity,required-size and conversion-failure fixes with explicit existing
CP932 assertions. No claim of final fixture success until it runs.

P8 allfourfiles28/2653->23/50 (-5points/-2603lines). S12current686/27172,
reduction117points4637lines. Reviewed24files129points:18restored-delivered,
8restored-local,103residual,516unreviewed. Delivered20files107points.
Owner11864 runs cached full build then actual OEM/redirector preprocessing.
Dual publication and targeted OEM/async/real guest verification follow. Last
Fullruntime8/file20 remains valid;no Full repeat atfile24. Next25process.c.


### P8 final verification

[Receipt](s12-p8-result.json):cached full build,dual26hash publication and223
original-line witnesses PASS. Existing OEM family/failure/DBCS fixtures,
asynchronous publication fixture and actual runtime9 DOS FCB guest PASS.
Broad oem-tests aggregate failed unrelated pif-fault layout and font-retry DEM
constant declarations. Selected three OEM targets passed;not an aggregate
pass. Carry those stale test dependencies into process.c/S13 reconciliation.
No Full rerun;last file20runtime8 Full23/23 retained. One added zero-length
statement falsely matched a later original statement in audit helper;corrected
classification without product rebuild. Initial failed audit retained.
All24files129points final-verified,26restored103residual516unreviewed.
Mirror total686/27172,reduction117/4637. Commit/push pending;next25process.c.

P8 product committed/pushed e25380fe2. Actual compiler FE_SB negative profile
rejected at restored#error as required. Delivered24files129points;26restored,
103residual,516unreviewed. Next25process.c includes stale PIF fixture follow-up.

## P9 in progress: file25 oemuni/process.c

Original-format restoration only: nonerror1/1500 ->28/142; token sequence
and the one excluded branch unchanged. The block increase reveals dispersed
adaptations previously hidden by whole-file CRCRLF differences; owner has
approved this class of restoration. Source is local and not yet built,
published, tested or delivered. No frozen point is closed by this interim
record. Whole-file SearchPath framework and OEM failure semantics review
continues before the batch build. Evidence: build/t485-s12/p9/format-result.json
and process.c.before/process.c.xp. Delivered totals remain24/48files,
129/645points;26restored,103residual,516unreviewed. Local census713/25814,
versus delivered686/27172. No Full rerun.

## P9 final verification

[Audit](s12-p9-audit.md),[receipt](s12-p9-result.json):25/48files194/645points
verified;71restored,123residual,451unreviewed. Current715/25797; cumulative
reduction88points/6012lines. Cached build,dual publication,four OEM fixtures
and runtime10 DOS FCB pass. Commit/push pending; no Full rerun.

P9 product c3dca3705 pushed;25files194points delivered. No local restoration
pending. Documentation governance and links PASS after audit-index and UTF-8
correction; runtime tests were not rerun. Next file26 in DIR order.

## P10 local file26 config.c

38points reviewed:37format restorations,1package-registry binding residual.
Nonerror1/1983 ->19/20;23exclusions unchanged, C tokens identical. Final
preprocessing/build/publication not yet run: pure format can accumulate.
Audited26/48files232/645points;delivered25/48files194/645points. Restored
delivered71,local37;residual verified123,local1;unreviewed413. Local census
733/23834;reduction70points7975lines. Delivered census715/25797 unchanged.
Next27nt_bop.c;whole-file processing before next build.

## P10 local file27 nt_bop.c

16points reviewed:1format restoration,15evidence-backed residuals.
Nonerror1/1795 ->21/44;32exclusions and C tokens unchanged. Retained ABI
contracts match WOW /Gz and FARPROC exports; V86 mode requires !VM for PM
selector interpretation. SAS read owns user-page permissions and guest fault
service, unlike a raw VdmMapFlat dereference. B page-domain and synchronous
COMMAND ingress providers are named in points.csv; loader failures use
FreeLibrary, not CloseHandle. No new product logic or copied algorithm.

Audited27/48files248/645points;delivered25files194points. Restored71delivered
and38local;residual123verified and16local;397unreviewed. Local census
753/22083,reduction50points/9726lines. Whole-file final preprocessing and
delivery pending; pure format batch continues to file28nt_com.c.

## P10 five-file checkpoint ready for final verification

Files26-30 complete:config.c38points,nt_bop.c16,nt_com.c90,nt_cpu.c2,
nt_emm.c29.149restored,26residual;all175points have conclusions in points.csv
and pending decisions synchronized. Nonerror batch118/6282 ->74/153,
reduction44points/6129lines. Final per-file metrics:config19/20,bop22/44,
com7/8,cpu9/11,emm17/70. A format-helper correction preserved original
blank lines instead of swallowing them before #line;these final metrics
supersede local intermediate counts above. C tokens/exclusion counts unchanged.

Local census671/19668,reduction132points/12141lines from S12 start.
Audited30files369points;delivered25files194points. Restored71delivered plus
149local;residual123verified plus26local;276unreviewed. Build owner25776
runs cached full and actual per-TU preprocessing. Dual publication then Full
required at this file30checkpoint;not yet delivered. No S closure claimed.

### P10 verification running

Cached build25776 PASS;503original logical-line witnesses PASS. First line
check falsely counted two nt_bop.c form-feeds as newlines;initial evidence
retained under build/t485-s12/p10/final/initial-*. Only the checker changed;
no source rebuild. Runtime11 staged and dual-published; Full owner17924
running after line check retry. No Full restart or repeated passing gates.
Delivery remains25files194points until Full/commit/push complete.

P10 Full follow-up: owner17924 remains live. Win31 standard/enhanced desktop
subtests output PASS with normal exit; Win95 desktop outputs PASS under the
previously approved exit-code boundary (normal-exit=False),not a claim of
newly repaired normal exit. Governance11308 and relative links PASS. Full
remaining gates and final timings receipt are pending;no repeat run started.

## P10 final verification

[Receipt](s12-p10-result.json):cached full build,503original line witnesses,
dual26hash publication and runtime11 Full23/23 PASS. No Full rerun. Initial
checker form-feed error retained;checker-only fix,no product rebuild. All
30files369points final-verified:220restored149residual276unreviewed.
Current671/19668,reduction132points12141lines. Commit/push pending;
last delivered25files194points c3dca3705. Next31nt_eoi.c.

P10 product 38b11ba6c pushed. Delivered30/48files369/645points;220restored,
149residual,276unreviewed,zero local restorations awaiting delivery.
Runtime11 Full23/23 and dual26hash publication retained;no Full rerun.
Current671/19668,reduction132points12141lines;next31nt_eoi.c.

## P11 file31 local whole-file audit

nt_eoi.c36points51lines ->6points10lines, reduction30/41. R03379 restores
unused original monitor declarations; R03380 retains the typed int/long pointer
boundary: vrinit.c owns int, ica.h requires IS32*, insignia.h defines long,
and ica.c writes the selected line back. All8error exclusions unchanged.
Source audit complete;final preprocessing/build/publication pending. No behavior
change;declaration/line-only batch may accumulate. Census641/19627; cumulative
reduction162/12182. Reviewed31files371points;220restored delivered plus1local;
149residual verified plus1local;274unreviewed. Delivered30/48,369/645.
Next32nt_error.c;lastFullruntime11 unchanged,not rerun.

## P11 files32-33 local whole-file audit

nt_error.c1/2514 ->10/16;20format restorations,6retained ABI/provider points.
CreateThread requires WINAPI. DOS modal wrapper transfers real input ownership;
WOW presenter retains five-word XP hard-error and real response semantics.
No C-token changes;7error exclusions unchanged. Product preprocessing154original
line witnesses PASS. nt_eoi.c preprocessing50witnesses PASS,zero mismatch.
nt_keycd.c1/1347 ->5/9;5format restorations,2retained bounds fixes: >= rejects
index133/84 at table size whereas original > reads one past. Actual-object
keycode_object_contract_fixture.c is the existing relevant coverage;not rerun yet.
No C-token changes;5error exclusions unchanged. Final keycode line check pending.
Batch38/3912 ->21/35, reduction17points3877lines. Census654/15791,
cumulative149points16018lines. Reviewed33/48files404/645points;restored220verified
plus26local,residual149verified plus9local,241unreviewed. Delivered30/369unchanged.
Declaration/format-only batch accumulates;no Full rerun. Next34nt_mem.c.

## P11 file34 local whole-file audit

nt_mem.c1/2773 ->21/26,5format restored,3mapping-owner boundaries retained.
Original void VdmSetPhysRecStructs cannot report allocation failure;existing B
preallocates slots before InitIntelMemory success and removes aliases before
VirtualFree. No C-token change;26exclusions unchanged. Repeated DEBUG_MEM
conditions exceeded old metric matcher assumptions: original #line anchors now
individually bind all26excluded bodies,without altering production exclusions.
Evidence:build/t485-s12/p11/nt_mem-exclusion-anchors.json and nt_mem.c-result.json.
Current674/13044; cumulative reduction129points18765lines. Reviewed34/48,
412/645points;restored220verified+31local,residual149verified+12local,233unreviewed.
Delivered30/369 unchanged. Pure declaration/format batch continues;next35nt_msscs.c.

P11 final selected-product preprocessing:nt_eoi50,nt_error154,nt_keycd54,
nt_mem131 original line witnesses;389total,zero mismatches. This is source/line
verification only,not cached full build,publication or runtime acceptance.

## P11 file35 and delivery start

nt_msscs.c1/2872 ->11/88;107format restorations,4residual points.
R03797 partially restores original registry error dialog:only missing optional key
is silent;out-of-memory and other open errors report original ED_REGVDD. dwType
holds status before its original value query overwrites it. No new state/provider.
R03764/R03766 retain package directory/registry boundaries;R03767 keeps no-op
instead of original native boot-root IO.SYS/MSDOS.SYS creation. No synthetic guards.
All10error exclusions retained. Entire five-file batch finished before cached
build owner9780 starts(final/build.ps1). Build,publication,targeted tests pending.
Five-file batch40/9557 ->53/149;format recovery splits old whole-file hunks,
net+13points/-9408lines. S12census684/10260,reduction119points21549lines.
Reviewed35/48files523/645points:restored220verified+138local,residual149verified
+16local,122unreviewed. Delivered30/369unchanged. Next36nt_pif.c after delivery.

P11 build attempt1 owner9780 terminated:PowerShell Stop treated resource-tool
stderr ntvdm.src as NativeCommandError. No source diagnosis or build pass claimed.
Retry2 owner14228 redirects stdout/stderr inside CMD and checks actual exit code;
source unchanged. Initial log/error retained. No Full launched.

P11 build retry14228 PASS;five-file product preprocessing514witnesses,zero errors.
Stagingruntime12 PASS. Publication1080 stopped at missing Get-FileHash module;
retry26660 refused nonfresh staging root. Resume22996 reused verified staged
manifest and completed both locations;26hashchecks PASS under test owner28492.
Actual complete SetupInstallableVDD extracted into build-only fixture passes11cases
(optional absence,four real errors,success,query/allocation/load failures,cleanup).
Physical-mapping-observation PASS. Original-external-memory failed C0000602 after
original add;fixture omitted production ntvdm_dpmi_initialize. Test-only change
adds canonical execution header and one real initializer assertion after GDP setup.
Retry15948 rebuilds/retests only this failed fixture,then runs outstanding hard-error
and real guest FCB gates. Product bytes unchanged;no Full rerun. Initial failures
retained;do not claim test repair verified before retry exits.

External-memory retry exits0 after production initializer;actual SAS reversed-page
loads/stores/move,EMS cross-window copy,allocation grow/shrink,exports,DIB sharing
and teardown all PASS. Product unchanged. Owner15948 continues unrun gates.

P11 final runtime12 cached build,dual26hash publication,514line witnesses,VDD11cases,
physical mapping,external memory retry,hard-error six groups and real guest FCB PASS.
Receipt:s12-p11-result.json. All35files523points final-verified;358restored165residual,
122unreviewed. Commit/push pending. Full not rerun;next file36nt_pif.c.

P11 product e0b9f26d5 pushed;35/48files523/645points delivered.
358restored165residual122unreviewed;zero local restoration awaiting delivery.
Runtime12 targeted acceptance complete;no Full rerun,next checkpointfile40.
Next36nt_pif.c;S12 remains active.

## P12 file36 local audit

nt_pif.c1/1082 ->8/33;9format restorations,5residual boundaries.
R03880 actual include order selects C1 pif.h before MVDM pif.h;C1 lacks the
PIFEXTHEADER family consumed here. Keep exact original header selection.
R03881/R03883 preserve package system/default PIF path and capacity checks;
R03882 converts original OEM result for actual ANSI callers;R03886 frees old
owning config/autoexec fields before replacing them. No token changes.
Four exclusions retained. Final product preprocessing/build/publication pending.
Census691/9211,reduction112points22598lines;review36files537points,
358restored verified+9local,165residual verified+5local,108unreviewed.
Delivered35files523points unchanged. Next37nt_reset.c;format batch accumulates.

## P12 files37-38 local audit

nt_reset.c1/809 ->10/16;30format restored,4lifecycle points retained.
Real termination callback closes before longjmp and bounded return closes again;
keep active/consumed guard and B authorization before device cleanup.12exclusions
unchanged. nt_rez.c1/388 ->1/4;one ROM provider point retained. Original rom.c
requests ROMS_REZ_ID;B reads three fixed embedded RT_RCDATA identities with size
checks. No external file fallback or callback.1exclusion unchanged.
Both whole-file C token streams unchanged;final product preprocessing pending.
Census700/8034,reduction103points23775lines. Reviewed38/48files572/645points;
restored358verified+39local,residual165verified+10local,73unreviewed.
Delivered35files523points unchanged. Next39nt_rflop.c;format batch accumulates.

## P12 file39 local audit

nt_rflop.c1/3594 ->35/37:22format points restored,3thread ABI points
retained. Original void cdecl fdc_thread is called via B typed bridge and A
WINAPI thunk; no original body/signature or lifecycle algorithm changed.
34exclusions and C token stream unchanged. Logical line directives restored;
final product preprocessing pending. Format recovery splits whole-file hunk.
Census734/4477;reduction69points27332lines. Reviewed39/48files597/645points;
restored358verified+61local,residual165verified+13local,48unreviewed.
Delivered35files523points unchanged. Next40nt_sound.c,then required Full
checkpoint for complete P12 batch. No build or test claimed for this file yet.

## P12 file40 and checkpoint

nt_sound.c1/917 ->6/16;10format points fully restored,5session-query
points residual. Two formerly mixed points R03967/R03976 are fully restored
brace/indentation changes. Current NtCurrentPeb returns finite B PEB without
SessionId; retain public query, not a cast to private native layout. Query
failure does not open session-zero device;LazyBeep uses existing public Beep.
Three exclusions and C tokens unchanged. Final preprocessing pending.
Five-file P12 batch5/6790 ->60/106 (+55blocks/-6684lines),format recovery
splits whole-file hunks;no functional algorithm changes. Census739/3576,
reduction64points28233lines. Reviewed40/48files612/645points;restored358
verified+71local,residual165verified+18local,33unreviewed. Delivered35/523.
All five files complete before required cached build,dual publish and Full.

P12 cached build owner26444 PASS;selected-product preprocessing387unique
original-line witnesses,zero mismatches. Governance25132 PASS. Publication19524
PASS:runtime13,13host components copied to assets/release and O:/WINNT/SYSTEM32,
26hashchecks PASS. Full owner9272 now running against this exact package;
no runtime pass or delivery claimed yet. Receipt:s12-p12-result.json.

P12 Full owner9272 finished PASS23/23 on runtime13,zero reruns. Win3.1 standard
and enhanced,Win95 desktops;enhanced/Win95 strict same-Window Alt+Enter roundtrips;
WOW,actual command return codes,GUI,RPC,lifecycle,nested handoff and real
DOS4GW/NESticle menu/Escape gates PASS. Win95 knownExitOne boundary retained.
All40files612points final-verified:429restored183residual33unreviewed.
No local restorations await runtime verification. Commit/push pending.

P12 product88cde1caf pushed.40/48files612/645points delivered;429restored,
183residual,33unreviewed. Census739/3576;next41nt_thred.c.
Runtime13 Full23/23 checkpoint complete;no unchanged-code Full rerun.

## P13 file41 local audit

nt_thred.c1/342 ->1/3;one frozen lifecycle point remains residual,not
counted as whole-point restoration. Original ccpu386exitthread frees thread
simulate stack; B teardown executes before it. Keep explicit void cdecl
provider declaration and preparation call. Existing thread-start fixture
asserts cleanup-before-CPU order;not claimed rerun for P13 yet. Both error
exclusions and C tokens unchanged. Final product preprocessing pending.
Census739/3237;reduction64points28572lines. Reviewed41/48files613/645points;
429restored verified,183residual verified+1local,32unreviewed. Delivered40/612.
Next42nt_unix.c;pure formatting accumulates,do not rebuild this partial batch.

## P13 files42-43 local audit

nt_unix.c1/868 ->3/4;original _sys_errlist extern restoration fails C2090
with actual product flags because UCRT macro expands it to a function-return
array declaration. Keep one omitted declaration,original consumers unchanged.
Two exclusions unchanged. Census741/2373,review42files614points,31unreviewed.
stubs.c1/730 ->9/10;restore three redundant NT includes and two format points.
Keep vdm.h:original cpu_createthread needs PVDM_TIB. All-four-header removal
fails C2146/C2081;vdm-only compile passes. No shim/algorithm change. Eight
exclusions retained. Four points:3restored1residual;final product verification
pending. Census749/1653,reduction54points30156lines. Review43files618points;
429restored verified+3local,183residual verified+3local,27unreviewed.
Delivered40/612unchanged. Next44vcdex.c;declaration/format batch accumulates.

## P13 files44-45 local audit

vcdex.c1/2 unchanged:profile18 cdecl default,actual link entryVDDInitialize@12
requires explicit WINAPI. vrica.h1/3 ->1/2 restores original EOF Ctrl-Z.
Actual imported symbol selects mvdm_public_ica_interrupt stdcall bridge,not
underlying cdecl ica_hw_interrupt. Keep void/stdcall declaration. Both residual
points retained;no whole-point restoration credited for partial EOF cleanup.
Census749/1652,reduction54points30157lines. Reviewed45files620points;
429restored verified+3local,183residual verified+5local,25unreviewed.
Delivered40/612unchanged;next46vrnetapi.c. Final batch verification pending.

## P13 file46 local audit

vrnetapi.c163/274 ->78/125:remove85blocks149lines of redundant logical-line
scaffolding/format.14frozen semantic points remain,with per-point owner/boundary
evidence,not a blanket generic-hardening rationale. Preserve sequential guest
far-target reads,empty initialization,DBCS byte-length commits,copy error status,
and original NetAPI free-once order. Existing provider tests cover actual mirror
bodies,aliasing,invalid mapping,OEM capacity and BX0 semantics;final batch rerun
pending. C tokens and74exclusions unchanged. Census664/1503,reduction139/30306.
Reviewed46files634points;429restored verified+3local,183residual verified+19local,
11unreviewed. Delivered40/612unchanged;next47vrnmpipe.c.

## P13 file47 local audit

vrnmpipe.c183/222 ->90/128:remove93blocks94lines of redundant line/format
scaffolding;C tokens and80exclusions unchanged. Nine semantic points retain
per-point evidence:overlapped read error,owned request staging,completion
publication and three release sites. Provider rejects stale owner/epoch and
commits payload before error/count;failed publication cannot notify ANR success.
Original cross-thread queue remains,no new state machine. Existing publication
fixture covers failure order,thread binding and epoch;P13 rerun still pending.
Census571/1409,reduction232points30400lines. Reviewed47/48files643/645points;
429restored verified+3local,183residual verified+28local,2unreviewed.
Delivered40/612unchanged;next48vrremote.c.

## P13 file48 local audit

vrremote.c21/33 ->6/18;15blocks15lines reduced with identical C tokens and
8exclusions. Keep UTF16-to-ACP conversion and allocation failure cleanup;
original cast truncates multi-character names. Actual VrRemoteApi fixture
checks two name conversions,capacity rejection and NetAPI failure;it is not
remote-server acceptance. Long-name rejection remains an explicit boundary.
All48files645points reviewed:429restored verified+3local,183residual verified
+30local,zero unreviewed. Census556/1394,reduction247/30415. Delivered40/612.
P13 final cached build,publication,targeted tests and closure Full pending.

## P13 final package verification in progress

[P13 package receipt](s12-p13-result.json) records the final package identity.

Cached full build27440 PASS;runtime14 dual13-image publication27628 PASS.
979 original product-preprocessing line witnesses match;one added cleanup
statement is recorded separately,not an original-line witness.48file latest
hashes reconcile. Initial targeted link failed because the fixture omitted
the production stdcall register export bridge;observation-tests.cmake now
links that existing object and matches the two exported symbols. No product
algorithm changed. Retry236 passes NetAPI24,OEM14,remote-name4,async27 and
thread lifecycle/module cleanup assertions. Initial failure retained under
build/t485-s12/p13/final/focused-build-initial-failure.log.
Full owner21668 is running against the published runtime14;not yet passed.
No Full rerun is authorized without changed inputs;retry failed gates only.
Commit,push and S12 closure remain pending final results.

## S12 final verification

P13 runtime14 Full23/23 PASS,zero reruns. Cached full build,dual26hash
publication,979 original line witnesses,targeted tests and final-package Full
complete. Batch372/2474 ->189/292,reduction183points2182lines.
All48files645points verified:432restored213necessary residual0unreviewed.
S12 census803/31809 ->556/1394,reduction247points30415lines.
Commit/push remains the last delivery step;S13 admission follows closure.

## S12 closure

1930f0852 delivered and pushed to origin/main. All48files645points have final
code,hash,evidence and delivery dispositions:432fully restored,213qualified
residuals,zero unreviewed or local unverified. Runtime14 cached full build,
dual13-image publication,26hash checks,targeted tests and Full23/23 PASS.
979original line witnesses match. No original guest modification/rebuild.
Nonerror byte-sensitive census803/31809 ->556/1394,reduction247/30415.
The initial targeted ABI-link failure was repaired using the existing product
bridge;no Full retries. Win95 known exit1 and remote-name capacity boundaries
remain explicit. S12 is closed. S13 reconciles all preceding repair evidence
and final code before N004;no unchanged-code Full rerun is authorized.
