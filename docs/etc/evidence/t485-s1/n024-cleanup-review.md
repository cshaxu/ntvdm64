# S15 N024 cleanup migration dependency review

P75 delivered candidate runtime91: cached full build, dual26hash publication,
seven native/CCPU cases, genuine Win16 controls and Full24/24 pass. Commit and
push follow the final documentation gates. Starting HEAD was
2f32f78bc260141b66d89a5ff5e43b57eaa65375 (P74 runtime88).
The review and initial failures below describe the migration chronology;
the final receipt is authoritative for acceptance.

## Fixed original evidence

XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89, prefix
Source/XPSP1/NT. Raw Git bytes and relevant excerpts are preserved in
build/t485-s15/n024-cleanup/paired-source-evidence.json.

| Original path | Blob | Required fact |
| --- | --- | --- |
| windows/core/ntuser/kernel/cleanup.c | 838700d18570bea632435b911711ba91fb7633fb | Separate module/task operations; module selection uses hMod16 |
| public/internal/windows/inc/wowuserp.h | 5be3126f26e295b939694fed1db0383bc106009a | PFNWOWCLEANUP has two arguments; PFNWOWMODULEUNLOAD one |
| base/mvdm/wow32/wuser.c | c016f1ad0d790357df269b5b240d3c67530c8851 | ModuleUnload calls pfnWOWModuleUnload after task cleanup |
| base/mvdm/wow32/wkman.c | 25fed1d6614ac12094077bbae2d9d092fec389bf | Late cleanup uses the two-argument task entry |
| windows/core/ntuser/kernel/init.c | e094a14ab0a48e4a4c9fd37b9de12ed1f49bc6dd | gpfnwp initialization uses XP ICLS indices, including null slots |
| windows/core/ntuser/kernel/handtabl.c | c343caf84ff3d79104d0437091dcb2f89c08754c | gahti replaces old flag table; CALLPROC is process owned but excluded by task cleanup |

## Pre-migration call chain and missing provider

The existing output table already contains pfnWOWModuleUnload, but
client/clglobal.c::wow_original_registration_exchange does not assign it.
wuser.c::ModuleUnload still calls four-argument pfnWOWCleanup with an NE
segment list. wowuserp.h preserves that old typedef. registered_cleanup then
enters wow_user_task_lifecycle_exit and wow_user_cleanup_bound under the
existing serialized owner, eventually invoking the old combined _WOWCleanup.
These declarations and callers must migrate together, not one at a time.

Current window bindings already retain original WW storage and hMod16.
kernel/getset.c's restore/capture and refresh operations preserve this field;
they do not generate its value. Production source search found no call to
pfnWowGetProcModule: only its declaration/storage exist. WOW32 supplies
pfnIn.pfnGetProcModule16 = WOWGetProcModule16, and the existing helper calls
the immutable guest's original callback. Thus storage alone does not prove
that module-unload selection can work after replacing the selector algorithm.

XP writes the identity at kernel/createw.c:449 and kernel/getset.c:769 using
xxxClientWOWGetProcModule; class.c:1513 supplies the analogous class-procedure
identity. inc/ntcb.h:7796/7814 owns the original callback send/receive form.
The bounded implementation must connect the existing registered module
provider to creation and procedure replacement, committing the returned
identity into the existing WW/WC with the corresponding target. Keep guest
callback timing outside non-callout cleanup/locked destruction. Module unload
only reads committed identity; it must not invent a selector cache, rescan
guest memory, or invoke guest callbacks after module destruction starts.

## Whole-file implementation scope

1. Restore the complete XP cleanup.c algorithm, retaining explicit borrowed
   contexts and real native destruction failure propagation. Restore its
   separate _WOWModuleUnload and _WOWCleanup; do not retain a second selector
   scan. Native window/class procedure updates must still update the actual
   dispatch target rather than only an observation field.
2. Pair the two-argument cleanup ABI, module output slot, WOW32 callers and
   registered lifecycle entry. Module cleanup must never destroy the current
   task or prepare all its windows for exit. Existing task identity, native
   retirement and resource release ordering stay with the current owner.
3. Connect the existing module callback producer at creation/subclass/class
   procedure assignment and store its result with the original owned words.
   Preserve typed guest/native target distinction; numeric high bits alone
   cannot turn a native procedure into a guest callback.
4. Replace the transitional NT4/XP paired class-index mapping with one XP
   ICLS order, and supply gpfnwp from the existing shared cleanup binding.
   No global first-call cache or new registry. Retain XP's explicit CALLPROC
   exclusion and the actual supported object-owner flag semantics.
5. Delete obsolete selector allocation/probe plumbing and unused ClientDied
   code/tests once the paired production calls no longer consume them. Adapt
   existing cleanup tests to module identity, including foreign module/task,
   native-vs-guest procedure, standard-class restoration, refusal and retry.
   Tests must exercise a real provider-derived hMod16, not only hand-fill it.

This is one coupled file batch, not permission to build a partially migrated
cleanup path. Reuse existing cleanup, standard-class, task-lifecycle,
process-retirement, init-notification and real Win16 controls coverage. Compute
final mirror changes for all paired sources before publication. No original
guest modification or rebuild is required or authorized.

## Progress and next action

S15 remains11/14 directories, overall59/159. Eight completed files since
P69 Full24/24. Full non-guest census remains1332files,792byte-identical,
803nonerror-identical,529different,7948points/64273lines;1314XP/18OpenNT.
This review changes neither migration counts nor product acceptance state.
Next action is implement the connected producer and both original cleanup
entries as one batch, then cached build, dual publish/hash, targeted tests,
commit and push. Ten-file and S-closure Full requirements remain in force.

## Local implementation checkpoint

The working batch restores XP cleanup.c's three routines and removes the old
ClientDied, selector scans, selector allocation/probing and static first-call
procedure table. The two-argument PFNWOWCLEANUP and separate module-unload
output are now paired with wuser.c, wkman.c and registered lifecycle entries.
Native destruction refusal remains an explicit failing cleanup result; it
cannot fall through into successful guest module release.

Creation and procedure replacement query the existing registered WOW32 module
callback before publishing the target, then retain hMod16 in the existing WW/WC.
The synchronous helper drops/reacquires an already held USER lock around that
callout using existing runtime operations. Native saved procedure entrances are
resolved through the existing immutable entrance owner, not high-bit guessing.
The actual registered callback/tag-removal/lock contract is added to the existing
init-notification fixture. Low-level cleanup policy fixtures still use explicitly
supplied owned-word views; these alone do not prove a real guest producer.

The standard-class table uses one XP ICLS ordering. gahti is a finite projection
of XP process ownership in the imported XP TYPE order; it does not import
kernel handle allocation. CALLPROC has process ownership and retains XP's
explicit exclusion. Native-only desktop/menu/switch/icon-title/tooltip/ghost
classes are not published as this process's WOW control classes.

Local raw-byte nonerror metrics (not delivery): cleanup.c1/985 to14/107;
wkman.c78/188 to77/186; wuser.c7/23 to5/14; wowuserp.h3/9 to2/7;
clglobal.c8/162 to8/172. Aggregate delta+9points/-881lines; whole census
1332files,792byte-identical,803nonerror-identical,529different,7957points,
63392lines. cleanup.c original LF recovery unmasks its bounded adaptations;
the other four already use XP baselines. Original cleanup.c has no conditional
exclusion; restored XP removes the obsolete selector-probing exclusion.

Cached full build was started after the coupled source and fixture edits.
No test, publication, committed delivery or directory completion is claimed.
Five changed mirrors plus the prior eight completed files require the next
Full checkpoint. Preserve runtime88 until the new product builds successfully.

## Initial verification and correction

First cached full product build passed. runtime89 was dual-published with all
26 manifest hashes verified. Initial registration test failed because its
handwritten WOW tag used the wrong RPL mask; using the original MarkWOWProc
macro fixed the fixture, and actual registered callbacks then passed including
lock release/reentry. No product change was needed for that correction.

The standard-class driver first failed to compile because the old fixture
included XP user.h directly instead of the existing client-layout boundary.
That fixture now uses wow_user_client_view_layout.h. The resulting real CCPU
run then caught an implementation defect: WW-only attachment (no supplied new
procedure target) overwrote already owned module words with zero. Restricting
module replacement to an actual target preserves the input WW identity. Its
managed native-window creation row also had an obsolete injected-module
expectation; native procedures have no guest module, so it now expects zero,
while the storage-only row still requires the supplied identity unchanged.

The corrected product is rebuilding. The initial logs are retained as
build-first.log, init-notification-first.log, standard-class-first-build.log
and standard-class-first-runtime.log. runtime89 is an interim published
candidate, not accepted delivery. runtime90 is reserved for the corrected
publication, followed by targeted and Full acceptance.

## Further whole-chain audit

runtime90 fixed the WW-only attachment defect. Registered module callback and
standard-class/real CCPU window scope then passed. The existing window scope
now supplies a callback returning the expected module and deliberately starts
with a different WW module value, so its normal/exception/retirement checks
verify production target publication rather than a prefilled field.

Class-procedure testing exposed two stale fixture contracts from before the
USER layout migration: removed GCL/GWL_WOWvpfnWndProc indices and single-high-bit
WOW procedure words. The fixture now uses actual thunk get/set operations and
XP MarkWOWProc (or explicit double-high-bit targets for callback doubles).
Neither reinstates an obsolete product ABI.

Self-review of gahti found this batch initially used NT4 slot numbering even
though user.h now declares XP TYPE_CALLPROC=7 and TYPE_ACCELTABLE=8. The table
is corrected to XP's20-type order: only MENU, CURSOR, CALLPROC and ACCELTABLE
carry the consumed PROCESSOWNED bit; the original cleanup explicitly excludes
CALLPROC. DDE and native-only kernel classes are not erroneously process-owned.
This is a product correction and therefore triggers a new cached full build,
publication and final-package verification, not a test-only retry claim.

## runtime91 verification progress

The corrected cached full product build and dual publication pass with26
matching hashes. Init-notification, standard-class, class-procedure,
task-lifecycle, menu-cleanup, process-retirement and native-creation pass.
Real guest controls and Full remain pending; this is not a P delivery.

Task/process lifecycle fixtures required the existing client-view layout before
their original USER provider includes, avoiding conflicting PQ/PTHREADINFO
declarations. The native-creation fixture used hMod16 as a synthetic test-case
number. Restored module lookup correctly replaced it, so the fixture now passes
that number through CREATESTRUCT.lpCreateParams. Nested creation, rejected
creation, veto and final empty-table assertions all remain and pass. These
fixture corrections do not change runtime91 production code. Initial failures
and successive retries remain in the build-owned evidence directory; passing
unchanged cases were retained rather than rerun.

## Final verification and census

[Delivery receipt](n024-cleanup-result.json) and [mirror census](n024-cleanup-census.json).
runtime91 cached full build and both13-image destinations pass26SHA256 checks.
All seven targeted owner cases and genuine original Win16 controls pass.
Full24/24 passes on the same final published package, including Win3.1 standard,
enhanced and Win95 desktops, SOL Options, WOW frontiers and DOS4GW/NESticle.
Win95 retains the approved known exit-one boundary; it is not a claim of normal
zero exit. Original guest was neither modified nor rebuilt.

| Mirror file | Prior nonerror points/lines | Final points/lines | Delta |
| --- | --- | --- | --- |
| cleanup.c | 1/985 (OpenNT) | 14/107 (XP) | +13/-878 |
| wkman.c | 78/188 | 77/186 | -1/-2 |
| wuser.c | 7/23 | 5/14 | -2/-9 |
| wowuserp.h | 3/9 | 2/7 | -1/-2 |
| clglobal.c | 8/162 | 8/172 | 0/+10 |

Whole non-guest mirror census:1332files,792byte-identical,
803nonerror-identical,529different,7957points,63392lines.
Delta:0files/0exact/0same/0different/+9points/-881lines.
Baselines:1315XP/17OpenNT. Only cleanup.c changes baseline in this batch.
Its LF restoration expands the former whole-file mismatch into actual bounded
adaptations; other paired files were already XP. Original diagnostic mapping
checks269unchanged nonblank lines with no mismatch; #error exclusions remain
accounted for separately by the byte-sensitive census.

This checkpoint covers the five changed mirrors plus eight files since P69;
the completed-file count resets after Full. N0242/9, S15 remains11/14,
overall59/159 directories. Next is exitwin.c; no S/T closure.
