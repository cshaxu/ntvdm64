# S15 P67 N022 USER client declarations

## Scope and inputs

Start HEAD362a2feb9, runtime77, N02237/39. Fixed XP SP1 commit
a2f6d7c93aa4f11efc51147566d5961c59a8ef89, original path
Source/XPSP1/NT/windows/core/ntuser/client/usercli.h,
blob07145f7769ad44c7c0df715c8009323e4bf9230c,2985 lines.
Previous OpenNT blob211a72359d28a17695e827bd8afe97081329e167.
No original guest change or rebuild.

## Whole-file reconciliation

Restore the complete XP declaration order, constants, control state and
function signatures. The existing lookaside-first/full-control second include
passes remain: the former needs IN_STRING/LARGE_IN_STRING/LOOKASIDE, native
heap macros and lock macros before control state is available. The latter
adds EDIT and the rest of the USER client declarations. Repeated structure
definitions remain prevented by their existing two guards, consolidated into
contiguous groups instead of the previous scattered switches. Original
conditional branches keep their spelling, #error exclusion and XP #line.

The XP ACCELCACHE body now resides in this original header. Its existing
B declaration becomes an opaque forward declaration; clglobal.c/globals.h
only declare a pointer and do not require a second body. Existing x86 view
bindings supply KPULONG_PTR, the original MSGMASK layout and opaque keyboard
table pointer. HFill reuses the existing WOW help payload pointer declaration.
No new storage, private object provider or runtime state machine is added.

Original GetControlBrush/GetControlColor aliases resolve through their
NtUser-prefixed names to the same existing B providers. Retail UserAssertMsg0
uses the existing no-evaluation assertion contract. The remaining OpenNT
winmgrc.c GetAppVer caller retains its unchanged current-task version binding
in the existing control precomp, pending that next file's migration.

## Residual boundaries and actual counterexamples

| XP range | Disposition and evidence |
| --- | --- |
| 21-40 | Retain finite precomp/header binding. Full USER private include closure is not the selected host build. Existing native/public headers and scoped original user.h/userrtl.h providers supply reached declarations. |
| 240 PtiCurrent declaration | Existing EDIT binding represents only thread-ID equality; it is a function-like macro to GetCurrentThreadId, not a kernel THREADINFO provider. Restoring this declaration produces C2061/C2059. Preserve the existing call binding, not a fabricated kernel thread object. |
| 2272,2274-2275 | Omit private DDE client, CSR call and full ntuser aggregation includes. Actual restore first fails C1083 on absent ddemlcli.h. Restore original globals.h at2273: this already-migrated provider supplies USER hook globals. No DDE/CSR transport imported. |
| 2666-2786 | Omit USER's private IMM loading/dispatch table. Actual restore produces missing LPIMEPRO/PINPUTCONTEXT/PIMEDPI types and conflicts with current native IMM functions. Existing control precomp owns native IMM API binding; it must not be overwritten by a second private table. Following independent declarations and original CUAS branch remain. |
| 2805-2831 | Omit private shared-memory rebasing and private display/monitor accessors. Actual restore gives C2039 for ulSharedDelta, pDispInfo and pMonitorPrimary, absent from the retained local views. Existing dialog monitor binding supplies native snapshots. |
| 2860-2877 | Omit private loader-lock accessors. Actual restore gives C2039 for LoaderLock in the surrogate PEB; manufacturing that field would not provide the real loader lock. Original unrelated declarations/macros and independently compilable inline functions remain. |

These unconditional omissions are visible nonerror differences, not hidden
behind newly invented exclusion conditions. The original DBG, platform,
message-pump and other unselected conditional bodies are recorded separately.

The first compile also exposed the USER pfnLocalSize macro expanding inside
the public RPC/COM header. The public COM and private USER CLIPDATA names also collide (C2011) in real cleanup/server builds. Preserve both layouts and scope the public declaration names during SDK inclusion; no data conversion or new implementation is introduced. Load public ole2.h before installing the existing
USER callback-name macros in the shared view binding. Do not modify the SDK
or duplicate COM declarations. Diagnostic attempts remain in
build/t485-s15/n022-usercli/compile-check-*.log; these are real product-profile
object builds and are not substitutes for final publication verification.

## Full-build dependency correction

The first full build failed because the old precomp order loaded userrtl.h
before XP's TL declaration, suppressing it through _USERRTL_. Move the same
existing scoped include after the shared declarations; retain XP's condition
and type body unchanged. MSGFLAG_SPECIAL_THUNK moved upstream to user.h:2517;
its exact value is supplied by the existing message ABI pending that header's
ordered migration. The second full build required restoring the original globals.h include and keeping FDEFWINDOWMSG/FWINDOWMSG in the first shared include pass, before the existing default-window provider installs its binding. Preserve original macro bodies and existing provider order. The restored globals declarations must precede callback-name macros as well: actual full builds showed those macros expanding in extern declarations. Include the same original globals header in the shared view binding before installing callbacks; the retained original include remains guarded. #line corrections are placed after excluded-arm boundaries so the selected preprocessor path executes them. These failures and the corrected final build are distinct
evidence; no failed build is counted as delivery.

## Required MDI layout dependency

The complete product compile also proved that wow_control_registered_extent
requires MDIWND. XP moves MDI and MDIWND from usercli.h to user.h:3150-3165.
Those exact XP declarations are inserted into user.h's existing selected
control-layout region; x86 KERNEL_UINT_PTR preserves the prior pointer width.
No algorithm or allocation policy moves. user.h remains an OpenNT whole-file
baseline:63points/491lines becomes63points/508lines (+0/+17), within the owner's
standing approval for a single small mirror adjustment. usercli.h decreases
170/592 to28/245; combined delta-142points/-330lines. No new guest binary.

## Targeted fixture correction

The initial control cohort failed at fixture link, before executing any case:
its old GetAppVer assertion referenced a macro no longer present in XP
usercli.h. Remove that obsolete assertion and its private mock; retain the
GETEXPWINVER callback checks and all memory/dialog cases. Product sources and
runtime78 remain unchanged. Preserve the failed control/build.log and retry
the previously unexecuted cohort in control-retry. That cohort passed39/40;
its remaining memory fixture still supplied the old one-bit NT4 WOW procedure
marker. XP IsWOWProc requires both high bits, matching production wcntl32.c
MarkWOWProc usage. Use the XP-marked fake procedure and original predicate in
the fixture; retry only that case. control-final-result.json aggregates39
unchanged passing cases and the corrected fixture result.

The mixed CCPU lifecycle fixture uses /Gd, whereas production USER uses /Gz.
Newly restored original LPK declarations exposed that test-only mismatch.
Scope WINAPI spelling to those two declarations in the fixture, following
its existing GetWindowBorders treatment; retain product declarations and
implementations. Preserve standard-class/build.log and retry only this gate
in standard-class-retry. That attempt exposed the newly restored private
GetWindowPlacement alias leaking into the fixture native HWND observation.
Undefine that alias after the original header; keep the existing public
GetWindowPlacement test calls. Final lifecycle evidence uses standard-class-final.

## Verification and census

Final cached full build, runtime78 dual13-image26hash publication, controls40/40,
real CCPU standard-class lifecycle, SOL Options and genuine Win16 controls pass.
[Receipt](n022-usercli-result.json), [census](n022-usercli-census.json).

usercli.h: OpenNT170points/592lines -> XP28points/245lines,
delta-142points/-347lines; paired user.h +0points/+17lines;12 original conditional exclusions, XP logical-line
audit passes. Total1330files/789byte-identical/800nonerror-identical/
530different/7965points/65210lines. Delta0/0/0/0/-142/-330;1306XP/24OpenNT.
N02238/39; S15 9/14 directories, overall57/159. Eight completed files since
P61 runtime70 Full24/24. No Full rerun required at this checkpoint.
