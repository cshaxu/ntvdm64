# S11 Ordered WOW Repair Progress

Admitted after [S10 closure](s10-progress.md#s10-closure), under the existing
[repair plan](../../operations/m0-t485-h-repair-plan.md). Scope:48 files and452
frozen points; no whole-repository or unconverted XP audit. Review complete
files in relative-path order, trace actual providers and preserve original
guest binaries. Residual differences require concrete necessity evidence.

Initial progress:0/48 files,0/452 points reviewed;452 unreviewed. No changes,
restorations or deliveries claimed. Measure each file at entry; do not use
frozen point counts as current byte-sensitive diff counts.

| Order | File | Frozen points | State |
| ---: | --- | ---: | --- |
| 1 | src/mvdm-host/inc/wowcmdlg.h | 1 | Delivered0c20d5b19 |
| 2 | src/mvdm-host/inc/wowkbd.h | 1 | Delivered0c20d5b19 |
| 3 | src/mvdm-host/inc/wowkrn.h | 1 | Delivered0c20d5b19 |
| 4 | src/mvdm-host/inc/wowsnd.h | 2 | P2 delivered 3bb82967a |
| 5 | src/mvdm-host/wow32/isvwow.h | 1 | P2 delivered 3bb82967a |
| 6 | src/mvdm-host/wow32/precomp.h | 3 | P2 delivered 3bb82967a |
| 7 | src/mvdm-host/wow32/walias.c | 4 | P2 delivered 3bb82967a |
| 8 | src/mvdm-host/wow32/walias.h | 6 | P3 delivered 952408b8b |
| 9 | src/mvdm-host/wow32/wcall16.c | 4 | P4 delivered 9000f7c3f |
| 10 | src/mvdm-host/wow32/wcntl32.c | 33 | P5 delivered 1587bccad |
| 11 | src/mvdm-host/wow32/wcommdlg.c | 11 | P6 delivered 94a576739 |
| 12 | src/mvdm-host/wow32/wdde.c | 25 | P7 delivered f97262387 |
| 13 | src/mvdm-host/wow32/wdde.h | 4 | P8 delivered 874e19e83 |
| 14 | src/mvdm-host/wow32/wdib.c | 1 | P8 delivered 874e19e83 |
| 15 | src/mvdm-host/wow32/wdos.c | 6 | P8 delivered 874e19e83 |
| 16 | src/mvdm-host/wow32/wgdi31.c | 1 | P8 delivered 874e19e83 |
| 17 | src/mvdm-host/wow32/winsockp.h | 3 | P8 delivered 874e19e83 |
| 18 | src/mvdm-host/wow32/wkernel.c | 10 | P8 delivered 874e19e83 |
| 19 | src/mvdm-host/wow32/wkfileio.c | 6 | P8 delivered 874e19e83 |
| 20 | src/mvdm-host/wow32/wkman.c | 22 | P8 delivered 874e19e83 |
| 21 | src/mvdm-host/wow32/wkman.h | 1 | P9 delivered 23731005a |
| 22 | src/mvdm-host/wow32/wmdisp32.c | 79 | P9 delivered 23731005a |
| 23 | src/mvdm-host/wow32/wmdisp32.h | 2 | P10 delivered 77e8752be |
| 24 | src/mvdm-host/wow32/wmsg16.c | 37 | P10 delivered 77e8752be |
| 25 | src/mvdm-host/wow32/wmsgcb.c | 5 | P10 delivered 77e8752be |
| 26 | src/mvdm-host/wow32/wmsgem.c | 3 | P10 delivered 77e8752be |
| 27 | src/mvdm-host/wow32/wmsglb.c | 3 | P10 delivered 77e8752be |
| 28 | src/mvdm-host/wow32/wole2.c | 5 | P10 delivered 77e8752be |
| 29 | src/mvdm-host/wow32/wow32.h | 1 | P11 delivered e5db62755 |
| 30 | src/mvdm-host/wow32/wow32fax.c | 7 | P12 delivered b65051565 |
| 31 | src/mvdm-host/wow32/wowhooks.c | 39 | P13 delivered e1824a049 |
| 32 | src/mvdm-host/wow32/wparam.c | 4 | Pending |
| 33 | src/mvdm-host/wow32/wsdata.c | 19 | Pending |
| 34 | src/mvdm-host/wow32/wsext.c | 4 | Pending |
| 35 | src/mvdm-host/wow32/wshell.c | 1 | Pending |
| 36 | src/mvdm-host/wow32/wshimdb.c | 5 | Pending |
| 37 | src/mvdm-host/wow32/wspool.c | 2 | Pending |
| 38 | src/mvdm-host/wow32/wstruc.c | 3 | Pending |
| 39 | src/mvdm-host/wow32/wuclass.c | 7 | Pending |
| 40 | src/mvdm-host/wow32/wuclip.c | 16 | Pending |
| 41 | src/mvdm-host/wow32/wudlg.c | 5 | Pending |
| 42 | src/mvdm-host/wow32/wuman.c | 5 | Pending |
| 43 | src/mvdm-host/wow32/wumsg.c | 32 | Pending |
| 44 | src/mvdm-host/wow32/wumsg.h | 1 | Pending |
| 45 | src/mvdm-host/wow32/wuser.c | 5 | Pending |
| 46 | src/mvdm-host/wow32/wuser31.c | 2 | Pending |
| 47 | src/mvdm-host/wow32/wutmr.c | 1 | Pending |
| 48 | src/mvdm-host/wow32/wuwind.c | 13 | Pending |

## P1 complete-file source review

Three files reviewed,3/452 frozen points locally restored;0 delivered,449
unreviewed. Original XP Git a2f6d7c93aa4f11efc51147566d5961c59a8ef89 and
files.csv hashes verified. No guest binary/source changed.

| File | Before non-error points/lines | After | Disposition |
| --- | ---: | ---: | --- |
| inc/wowcmdlg.h | 1/397 | 0/0 | Restore LF and final DOS EOF; byte-exact XP. |
| inc/wowkbd.h | 4/4 | 1/1 | Restore EOF; retain one line62 anchor and original NULLSTRUCT exclusion. |
| inc/wowkrn.h | 1/3333 | 13/13 | Remove148-line local frame block; restore LF and13 anchors following the original endif. |

Total6/3734 ->14/14: +8 points,-3720 lines. Standing format approval applies
to block growth exposed by LF restoration. Frozen review points are not the
current diff-block count. Fourteen remaining lines are diagnostic anchors,
not new behavior or retained local algorithms.

Production wkgthunk.c consumes PICALLPROC32W16; wkernel.c and wkman.c use
WOWFREERESOURCE16/WOWOUTPUTDEBUGSTRING16, also indexed by wktbl2.h. No added
legacy frame name has a C/C++ production consumer. The sole host test use
of ICALLPROC32 changes to the existing identical-field ICALLPROC32W16;
its inputs and test logic are unchanged. Legacy guest assembly names cannot
justify this XLATOFF block: it does not generate guest declarations, and
original guest is immutable. Removing the complete block avoids partial
fragmentation. All13 NULLSTRUCT exclusions remain. The final anchors execute after the
excluded arms, preserving selected original source line mapping. Cached full build/publication runner20652 started;
final tests,commit,push pending. This is source review,not delivery.

P1 final runtime1: cached fullbuild PASS; dual publication26/26 hashes PASS.
Existing real WOW generic-call test reports three guest phases,zero errors,
nested callbacks and three releases; actual WINMINE visible window PASS.
No original guest rebuilt; existing independent probe target was cache-clean.
Independent source-check.json verifies all original matched logical lines
(199/113/1586 respectively),unchanged error directives and pinned XP hashes.
All3 files/3 frozen points final verified; commit/push pending. No Full claim.


P1 pre-commit correction: the first local line-anchor candidate placed anchors
inside NULLSTRUCT; the compiler skips them in the supported selection. That
candidate is superseded,not final acceptance. Put anchors back after endif
(wowkbd line62;13 original anchors in wowkrn),keeping the same14/14 census.
No functional adaptation added. Source-check.json now checks placement after
excluded arms and selected original line mappings; discarded excluded endif
line numbers are not claimed as selected-code verification. Final runtime2 is
being rebuilt and republished;runtime1 test successes are candidate evidence
only. Final-code tests/commit/push remain pending.


P1 final acceptance: runtime2 cached fullbuild and dual publication PASS,
26/26 published hashes verified. Final generic-call probe again passed all
three guest phases,zero errors,nested callback counts and three releases;
actual WINMINE visible window PASS. Evidence:build/t485-s11/p1/final.
Runner16956 completed. These results supersede runtime1 for delivery.
3/48 files and3/452 frozen points final-verified;449 points remain unreviewed.
Current reviewed-file census14/14 vs6/3734: +8 blocks,-3720 lines. No functional
residual from these three frozen points;14 necessary line anchors remain.
Commit/push completes this P;Full remains S13.

P1 receipt:0c20d5b19 committed and pushed;3/48 files,3/452 points delivered.
No local source pending. Final runtime2 tests passed. Documentation gate initially
failed because this new record lacked the top-level etc index; the receipt
adds that index and reruns governance before its own commit. No Full claim.


## P2 accumulated format review

wowsnd.h,HXP-R-00270/00271:22/23 ->7/7. Restore exact XP EOF tail
(including LF following0x1A),remove14 redundant anchors;seven error arms and
post-endif anchors remain. No selected declaration or behavior changes.
Both frozen points locally restored; source-check validates retained line
mapping and unchanged exclusions.

isvwow.h,HXP-R-04078:2/168 ->1/2. Restore LF;retain one APIENTRY qualifier.
wmmedia.c's public definition and DLL export use stdcall. Original bare
prototype only works under default /Gz;existing public ABI fixture compiles
under /Gd and checks exact APIENTRY pointer type. The minimal qualifier avoids
requiring public consumers to inherit private build defaults. It remains in
pending-decisions,not owner-approved or fully restored. No new shim.

5/48 files,6/452 points reviewed:3 restored/delivered,2 restored locally,
1 residual pending decision/final verification,446 unreviewed. Reviewed-file
census30/3925 ->22/23 (-8/-3902) including newline/EOF. The P2 pair alone
24/191 ->8/9 (-16/-182). No fresh build/publication/test/commit for these pure
format/line changes yet; accumulate under owner cadence. Next file is
wow32/precomp.h; its three points have only been read,not completed.


### P2 precomp.h

HXP-R-04079/04080 retained with concrete source/ABI evidence;04081 restored.
Current3/14 ->2/12: remove redundant line21 and trailing blank. Retained
original logical source lines are unchanged; no error arm is present here.
The fixed XP wow32.h lines374,544,554 prove why simply restoring _X86_ is
incorrect for CCPU: it selects native TEB->Vdm registers and zero memory base
instead of simulated registers/IntelMemoryBase. The native SDK declarations
are loaded through nt.h first;wkman's vdmdbg.h ABI explicitly brackets _X86_.
A wholesale conversion to extra per-site CCPU guards would enlarge mirror
adaptation without restoring the required original native service. Keep this
single selection;this is evidence-backed residual,not owner approval.

Four shared declaration headers have real consumers:USER/GDI private constants
and interfaces;wuclip's successful-open retirement;wkernel/wow32 package paths;
wkman/wole2 ThreadBasicInformation. SDK22621 winternl.h THREADINFOCLASS only
contains16 and38,not0. Current diff has no ntseapi omission;that old suspicion
is superseded. Shared includes avoid duplicating declarations at every caller.

Menu scope binds WU32LoadMenuIndirect ->ConvertMenu16 ->original
CreateMenuFromResource. Original clmenu.c:73 checks TIF_16BIT for owner-draw
payloads;modern USER has no bound guest CLIENTINFO. Existing synchronous
provider retains the original parser and rollback,not a replacement parser.
Do not move identical macro complexity merely to improve mirror counts.

6/48 files and9/452 points reviewed:3 restored/delivered,3 local restorations,
3 residual pending decisions/final verification,443 unreviewed. Reviewed-file
census33/3939 ->24/35 (-9/-3904). P2 has only format/line changes and remains
accumulated before the next substantive whole-file delivery. Next:walias.c.


### P2 walias.c and delivery boundary

HXP-R-04082/04083/04085 are evidenced residuals;04084 restored. Non-error
11/32 ->6/22 (-5/-10). Preserve two DEBUG error arms;remove redundant anchors,
restore original GetIClass comments/condition layout around one borrowed fnid
read. Current original-compatible WW has no fnid field (wowuserp.h:282);
WND state carries it via existing state_fnid callback. Restoring pww->fnid
cannot compile against the actual ABI. Do not duplicate it into WW.
The existing entry bridge registers the two scalar stdClasses projections;
standard-class cleanup uses provider name/atom,so no second class table is
introduced. Original bounds/fallback and one query remain unchanged.
Independent source-check.json confirms602 matched original logical lines.

7/48 files,13/452 points reviewed:3 restored/delivered,4 local restorations,
6 residual decisions/final verification,439 unreviewed. Reviewed-file census
44/3971 ->30/57 (-14/-3914). P2 four-file batch38/237 ->16/43 (-22/-194).
Whole files complete. Cached build/publication runner5024 started for runtime3;
final ABI/standard-class/real WOW tests and commit/push remain pending.

P2 runtime3 cached fullbuild and dual publication PASS;26/26 hashes verified.
Runner20944 is executing public ABI,standard-class and real WOW tests.
Source checks PASS;runtime acceptance and commit/push remain pending.

### P2 targeted-fixture XP contract reconciliation

Runtime3 public-memory ABI and actual Win16 generic-call/WINMINE checks pass
(`build/t485-s11/p2/runtime-test-result.txt`). The standard-class fixture first
failed compilation/linking against old NT4 contracts. Its current XP bindings
now use the actual FASTCALL convention, product apphelp/comdlg32 import libraries,
userenv, and the existing register_exports.c callee-pop wrappers. No replacement
CPU implementation is introduced. XP W32Init leaves pfnInitDlgCb null, and
FindPWW is now the GWL_WOWWORDS macro; the fixture follows those contracts.
The subsequent run reached production class registration and rejected its old
0x81234567 procedure word with ERROR_INVALID_PARAMETER. The current XP guest
procedure tag requires both high bits (WOW_WINDOW_PROC_TAG); fixture inputs and
callback expectations now use that constant. Final standard-class rerun remains
pending. This is not a passing standard-class result or P2 delivery; product
runtime3 and mirror metrics are unchanged. No guest was changed or rebuilt.
The 15:41 rerun compiles/links and proves class registration now passes:
`WOW_CLASS_COMMIT_CCPU errors=0 cases=2`. It subsequently reports 14 failures
at native-class-projection (fixture lines 1691,2432,2444,2805-2824), before the
standard-class-owner checks. This remaining failure is not yet attributed;
no passing standard-class or completed P2 claim is made. Evidence is in
`build/t485-s11/p2/standard-class-xp/{stdout,stderr,fixture-exit}.txt` (exit 1).
The next action is to compare those projection expectations with the XP
WW/creation contracts, retaining the original assertions' intended behavior.
The 15:45 rerun now passes native-class projection (errors=0). The prior
failures were NT4-tag inputs and expecting old FindPWW side effects instead
of calling XP GetIClass. The managed-reuse DC assertion now follows XP
wreldc.c: ReleaseCachedDCs clears state/count even after native failure; it
has no alias-retirement call. The fixture explicitly retires its own alias.
This does not prove general product GDI alias lifetime. The next stage,
production-window-borrow, reports three assertions at lines 1066,1076,1160:
two more implicit alias-retirement expectations and a DC-font compatibility
case. They remain under investigation; no standard-class PASS or P2 delivery.
All these test edits leave the four product mirror files and runtime3 unchanged.
The remaining DC checks use the same cache/explicit-alias split. The font
check used NT4 TD word13. XP adds dwUserWOWCompatFlags/dwWOWCompatFlags2 and
production also defines FE_SB, adding dwWOWCompatFlagsFE: ulLastDesktophDC
is word16, not word15. Confirmed from current wow32.h and actual product
build.ninja /DFE_SB flags. The intermediate word15 candidate failed the font
assertion and is superseded. No product source change is needed for this input.
The 15:48 run passes production-window-borrow (errors=0), reaching the
standard-class-owner stage for the first time. That fixture likewise used
0x92370056 for its private shadow and nine public class targets; XP rejects
this NT4 single-high-bit encoding with error87. Both inputs now use the
same current WOW_WINDOW_PROC_TAG, retaining all cleanup/restoration checks.
Rerun in progress; earlier 30 cascading class failures are not reported as
product regressions or as cleared until the new run completes.
### P2 final verification

Runtime3 cached fullbuild and dual publication PASS (26 image hashes).
Public memory ABI PASS; actual Win16 generic calls and WINMINE window PASS.
Standard-class final run PASS: cleanup/restoration9, resolution9, storage9,
private-name cleanup and CCPU page-domain lifecycle all errors=0. The original
XP class cleanup selects CSF_WOWCLASS plus procedure module in adwWOW[1],
not the NT4 fixture's selector-only setup; it restores procedures without
clearing the retained WOW words. The fixture now supplies that exact metadata
and retains all actual restoration checks. Test edits do not change product
source or runtime3 identity. Test-only DC aliases are explicitly retired;
these results do not claim general product alias lifetime coverage.

Seven of48 files and13 of452 frozen points are final-verified:7 restored,
6 residuals awaiting owner decision,439 unreviewed. P2 four-file census
38/237 ->16/43 (-22/-194); cumulative44/3971 ->30/57 (-14/-3914).
Source/exclusion/#line checks and final governance run gate commit/push.
Next file remains walias.h; Full remains S13 and migration39/159.
P2 product commit3bb82967a is pushed to origin/main. Final source checks and
26/26 publication hashes verified;worktree clean immediately after push.
Seven restored points and six owner-review residuals are delivered;S11 remains active.

### P3 walias.h entry and local restoration

File8/48, six frozen points HXP-R-04086..04091. Entry non-error census10/18.
Whole-tree production/test search finds FREEHDC16/FREEHOBJ16 only at their
new definitions: both removed (04088/04089 locally restored, not delivered).
Removed redundant #line63/83/118; added the required #line21 after the alias
include. Original DEBUG #error preserved. Local census5/14 (-5/-4).
The header remains under whole-file review; no build is triggered yet.
GDI original hConvert16to32 reads gpGdiHandleInfo, initialized to(PVOID)-1;
modern alias requirements and original-provider availability remain to be
reconciled. Message fields are used by failure cleanup and DDE post rollback;
calling those adapters is not alone proof that the fields must remain.
Continue complete-chain/source-failure evidence before closing the four
remaining decisions. P2 delivered totals remain7files/13points; local P3
restorations are not included as delivered. Including local changes, reviewed
census reduction is19points/3918lines, versus delivered14/3914.
### P3 walias.h whole-file conclusion

Four residual points now have source/runtime evidence; two are restored.
The actual x86 GDI32 exports GdiQueryTable but it returns NULL; the original
16-byte ENTRYWOW roundtrip faults c0000005. The earlier absent-export rationale
is false and has been corrected in both ledgers. Exact probe log:
build/t485-s11/p3/gdi-original-probe.log. Message status distinguishes HWND0
success; captured allocation keys survive partial conversions and DDE post
failure (existing complete-body failure evidence and original alias authority).
The four residuals remain owner decisions, not approved unchanged diffs.
Whole-file census10/18 ->5/14; original DEBUG error retained. Build runner5420
started for runtime4. Final line/source checks,publication,tests and delivery
remain pending. Source-reviewed8/48 files,19/452 points:7 delivered restores,
2 local restores,10 residuals (6 delivered),433 unreviewed. Cumulative reduction
including local P3 is19/3918; delivered reduction14/3914. Next:wcall32.c.
P3 line verification found the post-include anchor must be20, not21.
Corrected before final acceptance:178 original mapped lines now match.
Runtime4 build/publication passed but is superseded;runtime5 final rebuild
runner12396 started. Tests must use runtime5, not the earlier package.

### P3 final verification and owner rule update

Final runtime5 cached build/dual publication PASS;26 hashes verified against
its manifest. Pure LF restoration after build does not change preprocessing.
Source audit maps178 original retained lines exactly, DEBUG exclusion intact.
GDI alias fixture PASS. COPYDATA59,filter14,queue18 cases pass with zero errors;
actual Win16 generic-call test PASS on runtime5. The old COPYDATA fixture lacked
XP cStackAlloc16, used two-argument FindPWW and an empty stackfree16 macro.
It now projects original XP StackFree16 unchanged through the existing source
projection helper; no copied stack algorithm or product adjustment. The class
lookup double fails if reached outside this fixture's deliberately null-window
lookup scope. Earlier failure logs are retained; final test-result PASS supersedes.

Eight files/19 frozen points final-verified:9 restored,10 owner-review residuals,
433 unreviewed. Current reviewed-file census54/3989 ->35/71, net-19/-3918.
P3 reduces5points/4lines (10/18 ->5/14). Commit/push pending.
The owner now requires full five-step delivery at every S closure; this is
updated in EXECUTION,CURRENT,and the T485 plan. Ordinary P targeted cadence
continues; S11 closure requires Full, not a deferral until S13.
### P3 delivery receipt

952408b8b committed and pushed; all eight reviewed files are delivered.
The ordered list next entry is wcall16.c (four frozen points), not wcall32.c.
Earlier next-file prose was a naming error; no out-of-order audit is claimed.
S11 remains active:8/48 files,19/452 points;9 restored,10 owner-review
residuals,433 unreviewed. Full remains mandatory at S closure.

### P4 wcall16.c entry and line minimization

File9/48: four frozen points HXP-R-04092..04095. Entry byte-sensitive
non-error census37/98; nine excluded branches remain unchanged. Removed26
line directives which set exactly the logical number already in effect;
local census12/72 (-25points/-26lines). Evidence:
build/t485-s11/p4/line-minimization.json; original bytes preserved in
build/t485-s11/p4/wcall16.before. No functional changes or build yet.

The real compat/wow_user_task_lifecycle.c callback_enter rejects invalid
binding and non-resource cleanup admission. Without04092 the public DWORD
entry returns its input size4; without04093 the BOOL entry reports TRUE.
The existing wow_public_callback_rejection_fixture directly checks both.
Production wow32-dll-entry-bridge.c registers the guarded original body;
normal original cleanup occurs after host_simulate and cannot run on unwind.
Existing original-frame/abort fixtures exercise that failure boundary.
The precise kernel-resource whitelist and all remaining anchors still need
whole-file conclusion before this file is counted audited or built.

Delivered remains8files/19points (9restored,10residual),433unreviewed.
Including this local format-only change, nine-file census91/4087 ->47/143,
reduction44points/3944lines; delivered reduction remains19/3918.
### P4 whole-file conclusion

All four points have a source-backed residual decision, not an approval to
retain unchanged. Original callback dispatch table at wow16cal.asm:131ff
and its resource bodies599ff distinguish local/global heap, resource,
selector/DIB, directory and handle-flag operations from application-procedure
calls, WinExec and task termination. The cleanup scope additionally requires
resource_cleanup_live; it is not blanket callback admission during teardown.
Production extension_provider binds this classification and guarded original
body. Normal original frame cleanup remains authoritative; exceptional unwind
uses the existing guard, restoring control only while worker state is active.
Moving the same guard into another shim or build macro would not simplify it.

463 matching original source lines have exact logical line numbers after
removing26 redundant anchors. Nine excluded branches unchanged. Non-error
census37/98 ->12/72; reduction25/26. Source-reviewed9/48files,23/452points:
9 restored,14 residual owner decisions,429 unreviewed. Final delivered totals
remain8/48 and19/452 until P4 verification/commit/push. Nine-file cumulative
census91/4087 ->47/143, net-44/-3944. Cached build/publication runner13672
started; final runtime6 and targeted contracts/CCPU/real Win16 results pending.
### P4 final verification

Runtime6 cached full build and dual publication passed;26 publication hashes
match. Callback extension, original multimedia and public-refusal contracts
pass. Complete original-frame four phases and eleven abort cases pass; real
CCPU control/selector/continuation tests pass. Actual Win16 generic-call
integration passes on runtime6. Final result:build/t485-s11/p4/test-result.txt.
Earlier fixture compile failures are retained, not product test failures:
XP uses cStackAlloc16 rather than TDF_INITCALLBACKSTACK. Tests now preserve
reserved-stack and teardown assertions with that counter. The complete TU's
unselected stack/common-dialog services fail closed; RTL uses ntdll.lib.
No product functional modification was introduced to accommodate fixtures.
Nine files/23 points final-verified;9 restored,14 owner-review residuals,
429 unreviewed. Census91/4087 ->47/143, reduction44/3944. P4 delivered/pushed 9000f7c3f.
### P5 wcntl32.c whole-file review

File10/48, HXP-R-04096..04128:33 frozen points. Entry33points/89lines;
removed14 redundant line anchors, final33/75 (-0points/-14lines), two
#error branches unchanged,537 original source lines logically exact.
04096 declares the lifetime provider;14 acquisition/release pairs capture
heap keys before copying and consume them before normal/exception release.
04109..04112 distinguish CB_DIR borrowed map references from owned heap.
Original normal-only output unthunk is not valid exception cleanup: copying
can fail before reaching free, and replaying unthunk can copy output twice.
The existing call scratch and original allocation/map algorithms remain.
No separate allocator/registry or moved complexity. All33 residual decisions
remain pending owner; none is counted fully restored by line-anchor cleanup.
Evidence: original/current transaction349/101 comparison in
m0-t447-s4-message-source-audit.md; current full-body heap_transactions covers
14 delegated controls, and combo cases exercise map and heap acquisition.

Runtime7 cached full build and dual publication passed. Transaction fixture
still used NT4 INIT flag and empty stackfree; now reuses original XP
StackFree16 and checks cStackAlloc16. A first local run found one incorrectly
changed test expectation: notification_proc moves the cursor directly, so
its old-4 assertion was restored. Product code was not changed for this.
Final runner5744 is active; final-package acceptance and commit remain pending.
Source-reviewed10/48files,56/452points:9restored,47residual,396unreviewed.
Census124/4176 ->80/218, reduction44points/3958lines. Delivered remains9files,
23points and44/3944 reduction until P5 passes and is committed/pushed.

P5 final runtime7:transaction191scenarios/0failures and real Win16 three
phases/0errors/3releases PASS. Ten files/56points delivered and pushed as1587bccad. No Full claim; Full remains required for S11 closure.

### P6 wcommdlg.c whole-file review

Entry16points/44lines, eleven points04129..04139. Restore original pragma
spacing, else comment (04130 fully restored), and four-line function signature
layout while retaining the registered original-provider name. Remove redundant
anchors and correct logical lines after the cleanup scope. Initial source had
+3 line drift in dialog body and common callback; final2449 original retained
lines match, two excluded branches unchanged. Census14/28, reduction2/16.
04129 is only partially restored: lifetime include remains necessary.
Three original provider names compose native callbacks with existing task and
borrow scopes plus resource-destroy binding; original algorithms remain in C.
Message acquisition/release and initialized scratch/finally prevent partial
SHAREVIOLATION conversion leaks and unacquired cleanup, while completion marks
normal stack semantics. Existing transaction tests cover that allocation family;
common-dialog call/reference and real Win16 tests will use final runtime8.
No new queue, state machine, guest change or copied algorithm.

Source-reviewed11/48files,67/452points:10restored (one local),57residual,
385unreviewed. Delivered remains10files/56points until P6 verification/push.
Reviewed census140/4220 ->94/246, net-46points/-3974lines; delivered reduction
44/3958. Build/publication runner24948 active; no final-package pass yet.

P6 final runtime8:cached full build,dual publication26hashes,transaction191/0,
common-dialog7leaves/42scenarios/0errors,native template2modes/0errors and
real Win16 three phases PASS. Eleven files/67points final-verified:10restored,
57residual,385unreviewed. Census140/4220 ->94/246, reduction46/3974.
P6 delivered/pushed 94a576739; next wdde.c. S11 Full still required before closure.

### P7 wdde.c whole-file review and ten-file Full checkpoint

File12/48, points04140..04164 (25). Entry26points/161lines, final25/159:
remove two redundant line anchors,719 original retained lines map exactly,
zero exclusions. No functional change or shifted complexity. All25 functional
residuals remain pending owner; no point is counted restored merely by removing
its redundant line anchor.

Initiator acquisition status prevents nested failed allocation from deleting
an older same-window initiator. Original conversion ignores inner-pair failure;
DIB/metafile allocations need their existing per-call rollback if conversion,
packet publication or posting fails. At most two inner metafile objects plus
one POST packet fit the existing three slots. Existing-pair reuse acquires none.
HDDE remains sole long-lived identity; stack records only this call's new pairs.
Rejected mark callbacks remove the new alias; successful guest zero results are
not rejected. Empty delete was reproduced by original-body counterexample.
COPYDATA_HEADER prevents payload/header identity collisions in both directions.
Evidence: actual wmdisp32/wmsg16/wumsg callers, current alias/conversion fixtures,
and m0-t447-s4-message-source-audit.md original/candidate DDE comparisons.
These native fixtures do not prove every guest DDE conversation.

Source-reviewed12/48files,92/452points:10restored,82residual,360unreviewed.
Census166/4381 ->119/405, reduction47points/3976lines. Delivered remains
11files/67points and46/3974 reduction until this package passes and is pushed.
Owner now requires Full every ten completed files in addition to every S closure.
P7 completes current file before build and runs cached full build,dual publish,
DDE targeted assertions and Full against runtime9, then commit/push. Pending.

P7 verification update: runtime9 cached build and dual publication PASS,
26 published image hashes checked. Initial DDE fixture compile failed on XP
WOWGLOBAL macros and added service references; fixture-only mappings now use
existing allocation doubles, verify unknown-handle shell-drop delegation, and
fail closed for unselected ACK allocation services. Alias96/0,Execute38/0,
conversion16 four cases/0 and conversion32 four cases/0 PASS. Existing conversion
fixtures are now dependencies of wow-dde and run through its existing verifier.
No production change was needed. Full runner24724 started 2026-11-26 16:54:38;
Full/commit/push remain pending. Earlier failed attempts remain in p7 evidence.

P7 final runtime9: Full23/23 PASS in613946ms; identical13-image runtime,
dual publication26/26 hashes. Win3.1 standard/enhanced desktop and exits pass;
enhanced Win3.1 and Win95 roundtrips both report window-input/fullscreen-text/
returned-window-input/second-fullscreen/same-window=1. Win95 keeps the approved
knownExitOne boundary (not a normal zero exit). WOW three frontiers, command
matrices, RPC/version checks, actual EDIT return, relaunch/isolation/retirement,
nested window and NESticle pixels plus Escape all pass. DDE96/38/4/4 assertions
pass. No original guest modification. Governance and relative links pass.
All12files/92points now final-verified:10restored,82owner-review residuals,
360unreviewed; census166/4381 ->119/405, reduction47/3976. Commit/push pending.
Full checkpoint covers files1..12; next count checkpoint is file20, plus S closure.

P7 delivered and pushed as f97262387. Worktree clean after delivery.
Next:wdde.h;12files/92points delivered,47/3976 cumulative reduction.

### P8 accumulating line-only batch

File13 wdde.h:4/27 ->4/26, remove redundant #line95,78 original mapped
lines without mismatch,no exclusions. Four frozen points04165..04168 remain
pending owner: bounded per-call conversion acquisitions,COPYDATA header identity,
initiator acquisition result,and actual conversion/rollback/mark declarations.
Complete providers and their failed-acquisition evidence were reviewed in P7;
this header changes no ABI or behavior and does not move types to another owner.

File14 wdib.c:5/6 ->1/2, remove four redundant line anchors,579 original
mapped lines without mismatch,two #error bodies unchanged. Point04169 retains
DeleteObject for HBITMAP produced by CreateDIBSection; the failure path deletes
the DC first and normal same-file retirement uses the same GDI destructor.
CloseHandle cannot substitute for that object release.

Source-reviewed14/48files,97/452points:10restored,87residual,355unreviewed.
Census175/4414 ->124/433, reduction51points/3981lines. P8 local line-only batch
is not built/published yet; delivered remains12files/92points,47/3976 reduction
through P7 f97262387. Next wdos.c. No next Full required before file20 or S closure.

P8 source review continues in order (all local changes remain line/format-only):

| File | Before points/lines | After points/lines | Original mapped lines | Exclusions |
| --- | ---: | ---: | ---: | ---: |
| wdos.c | 10/16 | 7/13 | 561 | 1 |
| wgdi31.c | 4/8 | 2/6 | 665 | 1 |
| winsockp.h | 1/410 | 3/20 | 147 | 0 |
| wkernel.c | 17/39 | 13/35 | 429 | 1 |
| wkfileio.c | 17/29 | 9/21 | 1481 | 3 |

All original-line checks have zero mismatch. winsockp restores original LF;
its +2point/-390line change is covered by owner formatting approval.
WOW /Gz and DEM cdecl callbacks/imports are evidenced by doswow.h,wkman/demlfn
registration,actual Ninja flags and linked _dem symbols.
Kerning copies returned entries rather than requested capacity; existing native
font test asks count+3 and checks untouched tail. Winsock owner serial and four
state bits support same-alias ownership and cancel/completion interleavings in
the original context/list; existing rundown tests cover these conditions.
Package profile selector keeps explicit filenames unchanged and uses one owned
SYSTEM.INI shadow; system_root resolves package directory,not host Windows.
Two old rationales corrected: module filename is NOT MULTI_SZ; its real defect
is strlen on uninitialized/unterminated native failure output and invalid
capacity mapping. Cache node is NOT freed by FreeMapFileCache; its lookup moves
the node to list head,so next must be saved before that link rewrite. Original
loop also omits tail. No functional code changed based on these corrections.

Source-reviewed19/48files,123/452points:10restored,113residual,329unreviewed.
Census224/4916 ->158/528, reduction66points/4388lines. Delivered remains
12files/92points,47/3976 reduction. P8 seven local files await batch validation.
Next wkman.c is file20; complete its whole audit before next big-five checkpoint.


P8 file20 wkman.c:86/203 ->39/134, reduction47/69;23 exclusions unchanged,3437 mapped original lines exact. Original two-argument cpu_createthread restored with coupled softpc.h declaration; selected worker map resolves stubs.obj, whose original body already takes both arguments. Frozen04206 and04211 restored locally;20 residuals have provider/caller evidence recorded in points/pending. Two inherited line drifts after process-exit and idle-hook substitution corrected. Full checkpoint now due; no file21 before delivery. Source-reviewed20/48files,145/452points:12restored,133residual,307unreviewed. Main census310/5119 ->197/662, reduction113/4457. Coupled softpc.h raw newline-sensitive census1/1630 ->1/1631 (one restored original parameter line, not new behavior); combined reduction113/4456. Build/publication/Full/commit/push still pending.

P8 build R1 failed before publication: restored PVDM_TIB parameter is not visible in WOW software-CPU header selection (C2146/C2081). R2 adds only a pointer typedef for the original _Vdm_Tib tag, without importing its layout. Supplemental pending S11-P8-ABI-01 records this compile evidence, outside452 frozen points. Coupled header final raw census1/1630 ->1/1632; normalized37/136 ->38/134. Combined raw reduction113points/4455lines. R2 cached full build/publication is running; not yet a pass.


### P8 conditional-line verification correction

The earlier linear #line counter incorrectly treated directives inside skipped
branches as effective. It was insufficient evidence: real MSVC /E found501
unique-statement mismatches in the first wkman candidate (including actual
line187 for original return line226). That claimed minimization is withdrawn.
Restore conditional-region and join anchors, retain unconditional reductions,
original CPU parameter restoration and the two inherited line corrections.
Actual per-file product MSVC flags now check wdib153,wdos113,wgdi3183,
wkernel30,wkfileio292,wkman736 unique original statements. No actual mismatch:
the one remaining textual collision is CHMODCACHE expanding to4 at original
line4942, colliding with a different literal loop at2311. No guest modified.

Final proposed metrics (points/lines):wdde.h4/26;wdib.c5/6;wdos.c8/14;
wgdi31.c3/7;winsockp.h3/20;wkernel.c14/36;wkfileio.c12/24;wkman.c78/188.
Main cumulative310/5119 ->246/826, reduction64/4293; coupled softpc.h raw
+2lines makes combined reduction64/4291. Frozen counts unchanged:
20/48 files,145/452 reviewed,12 restored,133 residual,307 unreviewed;
supplemental ABI-01 remains separately pending. Not final delivered yet.
Prior P1-P7 linear-only line evidence must be rechecked using selected compiler
preprocessing before S11 closure; their historical runtime passes remain facts.
The runtime10 run is pre-correction evidence only. Rebuild and publish the
corrected final package, then Full before P8 commit/push.

Pre-correction runtime10 Full completed23/23 (historical only). Final branch-safe runtime11 build/publication started in p8/final. Actual product-flag include probe for04199 fails C1083 opening ntseapi.h, exit2; no security API consumer in wkman. This supplies direct compilation evidence for the existing omission. No additional production code introduced.

Final runtime11 cached full build and dual publication26/26 hashes PASS. Final verification PID20732 is owned by p8/final/verify.ps1; run identity in verify-run.json. Full and targeted results pending; no P8 commit/push or file21 yet.


### P8 final package verification

Runtime11 cached full build,dual publication26/26 hashes,targeted file/conversion and task-close tests,Full23/23 PASS. Full elapsed574294ms. Both enhanced desktop roundtrips retain all five input/display/same-window markers. Win95 uses approved knownExitOne boundary. See [P8 receipt](s11-p8-result.json).20/48 files and145/452 frozen points final-verified:12restored,133owner-review residuals,307unreviewed;one supplemental ABI decision. Main cumulative reduction64points/4293lines,combined with coupled header64/4291. All48 S11 files currently810non-error points/7472lines. Commit/push pending. Next checkpoint file30 and S closure. Earlier P1-P7 line-only proof still requires compiler-sensitive recheck before S closure.

P8 delivered and pushed as 874e19e83.20files/145points delivered;Full checkpoint20 complete. Next checkpoint30. Current S11 census810points/7472lines;combined cumulative reduction64/4291. Next ordered file:wkman.h;before S closure recheck earlier linear-only line evidence.


### P9 file21 wkman.h

1point/4lines unchanged. Point04220 retains two cdecl qualifiers: nt_eoi.obj exports _RegisterWOWIdle/_BlockWOWIdle, WOW imports same symbols under /Gz. Original unqualified prototypes change ABI. No wrapper/relocation or new mechanism. No source edit; current runtime11 build and Full evidence applies to unchanged file. Remains owner decision, not approval to retain.21/48files,146/452points source-reviewed:12restored,134residual,306unreviewed. Delivered20/145. Cumulative reduction64/4291 including coupled header;all48 current810/7472 unchanged. Next:wmdisp32.c.


### P9 file22 wmdisp32.c ongoing review

Whole-file review remains open; no file-completion or delivery count added.
Local line-only cleanup:136points/462lines ->110points/433lines (-26/-29),
10 excluded bodies unchanged. Actual product-parameter MSVC preprocessing
exits0.222 original unique-text matches include five nonstationary collisions:
class/name keys moved before copying, two added dwParam assignments matching
another function, and WinHelp hMem16 initialization matching clipboard.
Stationary matched statements have no remaining drift. Removes34 redundant
anchors and adds5 precise anchors for existing dialog/entry/Activate drift.
Non-#line content bytewise unchanged. Evidence:build/t485-s11/p9/line-cleanup-result.json
and batch-compiler-lines.json. This is preprocessing evidence, not final build.

Frozen04222/04223 cannot be dismissed as blank-line-only changes: prshtp.h
is outside FE_IME because GetNMHDRextensionSize uses PSHNOTIFY/private PSN
constants in the selected non-FE_IME build. Final necessity/minimality review
remains part of this file. COPYDATA evidence must distinguish four allocation
failures and nested ownership tests from a hypothetical USER early-reply
capture lifetime; historical notes explicitly did not prove USER use-after-free.
The existing source-body fixture labels itself guest_proof=0.

After this entire file is complete, owner requires cached full build, dual
publication, Full regression, commit and push immediately, before proceeding.

### P9 file22 source review complete

All79 frozen points retain evidence-backed residuals pending owner review; no functional restoration claimed. Main and pending ledgers updated per group. Final local metric136/462->110/433 (-26/-29),10 exclusions unchanged.22/48 files and225/452 points source-reviewed:12restored,213residual,227unreviewed. Delivered remains20/145. Current48-file census784/7443;combined local cumulative reduction90/4320, of which26/29 awaits delivery. Full required now for final package.

P9 runtime12 cached full build and dual publication completed;26/26 published hashes match. Message transaction191 scenarios and COPYDATA59 scenarios pass (source-body supplementary tests). Final-package Full started under owned PID2112 at2026-11-26T18:12:09+01:00; evidence build/t485-s11/p9/final/verify-process.json and full.log. Full is not yet a pass and no P9 commit/push has occurred. Read-only48-file rescan confirms784points/7443lines.

### P9 final-package result

[Runtime12 receipt](s11-p9-result.json): cached build,dual26/26hash publication,message191/COPYDATA59 and Full23/23 PASS. Both enhanced roundtrips pass all five markers; Win95 approved knownExitOne remains.22/48files,225/452points verified:12restored,213residual,227unreviewed. Current784/7443;combined reduction90/4320. Commit/push pending;no S closure.

P9 delivered and pushed 23731005a021b3fb253fb9b698f74682f4183739.22/48files and225/452points delivered;12restored,213residual owner-review,227unreviewed. Full23/23 final runtime12. Next file:wmdisp32.h;next periodic Full file30 and S closure.

### P10 file23 wmdisp32.h

6points/9lines ->4points/6lines (-2/-3); one exclusion unchanged. Three redundant anchors removed; actual WOW32 MSVC /E matches71 original declarations without drift. Four scratch slots plus dwParam carry five independent CREATE/MDI resources through failure; TLS extern matches native-thread conversion state. Both functional points remain owner-review residuals. No functional edit, so accumulate with next whole file.23/48files,227/452points reviewed:12restored,215residual,225unreviewed;delivered22/225. Current census782/7440;combined local reduction92/4323,including2/3 undelivered. Next wmsg16.c starts60/206,3exclusions,37frozenpoints.

### P10 file24 wmsg16.c

60points/206lines->48points/187lines (-12/-19),3exclusions unchanged. Remove15redundant anchors and restore two comment indents; no functional edit. Real MSVC /E matches159 original statements; two COPYDATA matches are intentionally moved lookup statements, third is original WOWGLOBALFREE macro expansion matching GlobalFree in another case.37 functional residuals retain explicit caller/ownership evidence in both ledgers; no restored functional point claimed.24/48files,264/452points reviewed:12restored,252residual,188unreviewed. Delivered22/225. Current census770/7421;combined local reduction104/4342,including14/22 undelivered. Next:wmsgcb.c.

### P10 files25-27 control converters

wmsgcb.c1/327->9/27 (+8/-300): restore XP LF under owner permanent
format approval; two redundant anchors removed,1exclusion unchanged.
Actual MSVC/E9 original statements match. CB_DIR must release by original
message and guest key after hook changes output; failed/refused AddParamMap
must not decrement outer reference. Five residuals remain owner review.
wmsgem.c7/11->5/8 (-2/-3): remove three redundant anchors and added empty
FREEPSZPTR,repair original continuation anchor143.25 original statements
match;1exclusion unchanged. Correct old erroneous ParamMap reasons: real
issues are text/array allocation failure and signed constructor/free mismatch.
wmsglb.c6/10->5/8 (-1/-2): two redundant anchors removed;13 original
statements match;1exclusion unchanged. Two allocation guards and signed
free condition correspond to actual selected constructors and reverse21
scenario fixture, not ParamMap. Three residuals remain owner review.
27/48files,275/452points reviewed:12restored,263residual,177unreviewed.
Delivered22/225. Current census775/7116;combined local reduction105/4647,
including9/327 pending. No functional source behavior changed. Next:wole2.c.

### P10 file28 module alias name

wole2.c non-error mirror diff12/40 ->9/38, reduction3/2.
Two exclusions unchanged;actual preprocessing75 unique original statements
match logical lines. Original XP passes Unicode byte length as character
count and reserves no NUL byte; early return also leaks the query buffer.
Use existing bounded RtlUnicodeToMultiByteN and one query-buffer cleanup,
removing the temporary ANSI allocation/copy/free. Five residual points remain
owner review; final runtime evidence pending.
28/48files,280/452points reviewed:12restored,268residual,172unreviewed.
Delivered22/225. Census772/7114;combined reduction108/4649,including
P10 reduction12/329 pending. Owner requires Full immediately for this batch
before advancing to file29, in addition to file30 and every S closure.

P10 delivery in progress:runtime13 cached full build and dual13-image/26-hash
publication PASS. Module alias bounds/lifetime,reverse21,message191 and
COPYDATA59 PASS. First reverse fixture attempt failed to compile because
its WW and XP macro dependencies were stale; fix reuses original marker
macros and records dependency hashes. No production change after publication.
Final Full is running; commit/push and whole-batch delivery remain pending.

P10 whole-file non-error XP-relative byte-sensitive accounting:

| File | Before points/lines | After points/lines | Reduction points/lines |
| --- | --- | --- | --- |
| wmdisp32.h | 6/9 | 4/6 | 2/3 |
| wmsg16.c | 60/206 | 48/187 | 12/19 |
| wmsgcb.c | 1/327 | 9/27 | -8/300 |
| wmsgem.c | 7/11 | 5/8 | 2/3 |
| wmsglb.c | 6/10 | 5/8 | 1/2 |
| wole2.c | 12/40 | 9/38 | 3/2 |
| Batch | 92/603 | 80/274 | 12/329 |

wmsgcb.c block growth comes from restoring XP LF, under the owner's
standing formatting approval; no behavioral blocks were added to merge
or hide differences. Fixed48-file census independently recomputed772/7114.

### P10 final-package result

[Runtime13 receipt](s11-p10-result.json): cached build,dual26/26hash publication,module-alias/reverse/message/COPYDATA and Full23/23 PASS. Both enhanced roundtrips pass all five markers; Win95 approved knownExitOne remains.28/48files,280/452points verified:12restored,268residual,172unreviewed. Census772/7114;combined reduction108/4649. Commit/push pending;S11 remains active.

P10 delivered and pushed 77e8752be61eec545b100cd9b476b1e6d0aafed3.28/48files and280/452points delivered:12restored,268residual owner-review,172unreviewed. Full23/23 final runtime13. Extra file28 Full complete; next file wow32.h, periodic Full at file30 and S closure.

### P11 file29 wow32.h

Non-error69/70->52/52, reduction17/18;21 exclusions unchanged.
Actual preprocessing101 original declarations match logical lines.
HXP-R-04373 restored to original FLUSHVDMCODEPTR. Prior read-only lease
rationale does not describe current SIM32 provider: buff is unused, original
video scratch flush remains source-owned, CCPU overwrite is a no-op. Existing
callback fixture now checks one exported call and original parameter shape
rather than demanding a project-specific no-provider macro. No new shim.
29/48files,281/452points reviewed:12restored-delivered,1local-restored,
268residual,171unreviewed. Delivered28/280. Census755/7096;combined
reduction125/4667. Cached build/publication running; runtime proof pending.

P11 initial product link failed: stdcall _Sim32FlushVDMPointer@16 vs actual
CCPU cdecl export. Coupled softpc.h existing declaration now explicitly cdecl,
no wrapper. Supplemental S11-P11-ABI-01 records standing approval and proof.
Raw whole-file format block1/1632 unchanged; independently normalized
38/134->39/136 (+1point/+2lines), not hidden by raw census. This coupled
header edit does not count as the future S12 whole-file audit. Rebuild pending.

P11 runtime14 build and dual publication PASS. Original flush ABI assertion,
callback4 and abort11 PASS. Product attempt1 could not compare WOW results
because the disposable wrapper used nonexistent product58c instead of
full58c; corrected wrapper reruns Product only. Production and publication
unchanged. Runtime14 final Product result remains pending.

### P11 final verification

[Runtime14 receipt](s11-p11-result.json): cached full build and dual26/26
hashes PASS; original callback4/abort11 and real Product gates PASS. Original
HXP-R-04373 restored; cdecl supplemental declaration remains registered.
29/48files,281/452points verified:13restored,268residual,171unreviewed.
Census755/7096,combined raw reduction125/4667. Coupled normalized ABI
increment1point/2lines separately disclosed. Commit/push pending.

P11 delivered and pushed e5db627553079f73d34f6b066593469834988a44.29/48files,281/452points delivered:13restored,268residual,171unreviewed. Next file30 wow32fax.c; Full is mandatory after that whole file. S11 remains active.

### P12 file30 wow32fax.c

Non-error9/37->7/34, reduction2/3. Restore04375 formatting and04376
duplicate zeroing; original XP already zeroes the array before SortedInsert.
Five necessary residuals cover failure buffer cleanup, owned AddPrinter
handle close and operation-correct error classification on the actual worker
thread. Caller GetLastError cannot read worker error. No new relay/state.
Two exclusions unchanged;actual preprocessing263 statements match original
logical lines. Redundant anchors around duplicate zeroing removed; continuation anchors repaired.
30/48files,288/452points reviewed:13restored-delivered,2local-restored,
273residual,164unreviewed. Delivered29/281. Census753/7093;combined
raw reduction127/4670;coupled P11 normalized ABI increment remains separate.
Cached build/publication started; file30 Full mandatory before delivery.

P12 runtime15 cached build and dual13-image/26-hash publication PASS.
Fax isolated provider checks135/0 PASS; no real fax installation claim.
Final Full running; Win3.1 standard and enhanced desktop/exit checks PASS.
No source change after publication; commit/push remains pending.

### P12 final-package result

[Runtime15 receipt](s11-p12-result.json): cached build,dual26/26hash publication,fax contract and Full23/23 PASS. Both enhanced roundtrips pass five markers; Win95 knownExitOne retained.30/48files,288/452points verified:15restored,273residual,164unreviewed. Census753/7093;combined raw reduction127/4670. Commit/push pending. S11 remains active.

P12 delivered and pushed b65051565cc970c32511fc10af90e717209a0c24.30/48files,288/452points delivered:15restored,273residual,164unreviewed. File30 Full checkpoint passed23/23 on runtime15; next Full at file40 or S closure. Next file31 wowhooks.c; S11 remains active.

### P13 file31 wowhooks.c

Entry non-error54points/174lines,2 excluded DEBUG bodies;39 frozen points.
Whole-file review retains original hook dispatch/converters and the existing
narrow native boundary. All39 frozen points have individual necessity and
provider evidence in points.csv and pending-decisions.csv; none is approved
for permanent retention by this review. No new functional code or shim.

Remove four redundant line anchors; repair mouse-hook and reverse-CBT
continuations with three precise anchors. Non-line source is bytewise
unchanged. Result51points/173lines (-3/-1),2 exclusions unchanged.
Actual product-parameter MSVC preprocessing checks219 stationary original
statements with zero drift. First check detected a one-line CBT anchor error;
it was corrected before build. Final receipt must use the corrected hash.

Key retention decisions: TLS avoids cross-thread global hook-state races;
the outer wrapper protects native window/task ownership before conversion;
CallBack16 completion is not the hook return value; resource cleanup releases
only completed acquisitions, including nested callback failure. MsgFilter
memo/DDE state is restored after nested different messages. Its nested
finalizers protect distinct fallible releases, not duplicate algorithms.
A zero HWND is not a failed message constructor. Exact-body fixture evidence
is service-double evidence, not proof of a real guest running every branch.

31/48files,327/452points source-reviewed:15restored-delivered,312residual,
125unreviewed. Delivered30/288 until this batch completes. Current48-file
census750/7092;combined raw reduction130/4671. P11 normalized ABI +1/+2
remains separately disclosed. P13 cached build/publication in progress;
message transaction and COPYDATA targeted tests follow the final package.
Last Full:file30. Next Full:file40 or S closure.

P13 runtime16 cached build,dual13-image/26-hash publication,message transaction,
COPYDATA and Product targeted verification PASS. [P13 receipt](s11-p13-result.json).
31files/327points final-verified;commit/push pending,delivered30/288.

P13 delivered/pushed e1824a0499f8a7cfa7316b9d6142b79988dd3cf0.31/48files,327/452points delivered:
15restored,312residual,125unreviewed. Census750/7092,cumulative130/4671.
Next file32 wparam.c. Last Full:file30;next:file40 or S closure.
