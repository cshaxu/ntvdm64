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
| 4 | src/mvdm-host/inc/wowsnd.h | 2 | P2 source-reviewed; format batch pending |
| 5 | src/mvdm-host/wow32/isvwow.h | 1 | P2 source-reviewed; format batch pending |
| 6 | src/mvdm-host/wow32/precomp.h | 3 | P2 source-reviewed; format batch pending |
| 7 | src/mvdm-host/wow32/walias.c | 4 | P2 final-verified; commit/push pending |
| 8 | src/mvdm-host/wow32/walias.h | 6 | Pending |
| 9 | src/mvdm-host/wow32/wcall16.c | 4 | Pending |
| 10 | src/mvdm-host/wow32/wcntl32.c | 33 | Pending |
| 11 | src/mvdm-host/wow32/wcommdlg.c | 11 | Pending |
| 12 | src/mvdm-host/wow32/wdde.c | 25 | Pending |
| 13 | src/mvdm-host/wow32/wdde.h | 4 | Pending |
| 14 | src/mvdm-host/wow32/wdib.c | 1 | Pending |
| 15 | src/mvdm-host/wow32/wdos.c | 6 | Pending |
| 16 | src/mvdm-host/wow32/wgdi31.c | 1 | Pending |
| 17 | src/mvdm-host/wow32/winsockp.h | 3 | Pending |
| 18 | src/mvdm-host/wow32/wkernel.c | 10 | Pending |
| 19 | src/mvdm-host/wow32/wkfileio.c | 6 | Pending |
| 20 | src/mvdm-host/wow32/wkman.c | 22 | Pending |
| 21 | src/mvdm-host/wow32/wkman.h | 1 | Pending |
| 22 | src/mvdm-host/wow32/wmdisp32.c | 79 | Pending |
| 23 | src/mvdm-host/wow32/wmdisp32.h | 2 | Pending |
| 24 | src/mvdm-host/wow32/wmsg16.c | 37 | Pending |
| 25 | src/mvdm-host/wow32/wmsgcb.c | 5 | Pending |
| 26 | src/mvdm-host/wow32/wmsgem.c | 3 | Pending |
| 27 | src/mvdm-host/wow32/wmsglb.c | 3 | Pending |
| 28 | src/mvdm-host/wow32/wole2.c | 5 | Pending |
| 29 | src/mvdm-host/wow32/wow32.h | 1 | Pending |
| 30 | src/mvdm-host/wow32/wow32fax.c | 7 | Pending |
| 31 | src/mvdm-host/wow32/wowhooks.c | 39 | Pending |
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