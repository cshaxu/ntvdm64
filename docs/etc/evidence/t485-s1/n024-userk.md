# S15 P82 N024 userk.h migration

Starting state after P81 HEAD56823f8bf19206c3b07b19663abbffb2fb4c78bd/runtime97.
N0248/9; S15 11/14; overall59/159. Six completed files since P75 Full24/24.

## Source and measured starting state

Fixed XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/windows/core/ntuser/kernel/userk.h,
blob7ee4335799af86fa2de42c04c883cf60abd48ffa.
OpenNT blob0bf5dbcf391f5332cc9ba756fd2f309542129149.
Raw Git objects, before image and three-way comparisons are saved under
build/t485-s15/n024-userk. Before byte-sensitive nonerror diff1point/10213lines.
Global1332files/792exact/803nonerror-identical/529different/8044points/61645lines.

## Whole-file audit inventory

The original XP file has170 preprocessor arms; current file has60.
These are syntactic arms including parent regions, not independent diff counts.
Ignoring diagnostic #line directives for inspection only, old-baseline comparison
has33 adaptation/omission groups and39 groups containing #error. Raw-byte metrics
remain authoritative. branches.json and local-adaptations.json retain ranges.
Do not copy the old exclusion map by line number: XP has reorganized conditions.

All three actual source consumers must be preserved: queue.c and taskman.c use
WOW_ORIGINAL_TASK_ORDER; cleanup.c includes the same header through its full
relative path under WOW_USER_OBJECT_FLAGS_ONLY. The latter does not supply the
scheduler's execution-owner fields and must not instantiate those inline bodies.

## Confirmed migration obligations

- Keep P79's XP task initialization, profile IDs and four-argument OpenCacheKeyEx
  declarations; do not roll them back while replacing the surrounding header.
- Restore XP EnterWowCritSect/ExitWowCritSect text and retain the two existing
  client_context calls at their existing ownership boundaries. Their provider
  transfers debug/client CPU context; they are not disposable diagnostic hooks.
- Preserve the cleanup-only declaration view while retaining all harmless XP
  declarations, including unused ones. Do not replace retained branches by
  custom subset conditions merely to avoid declarations.
- XP has no old CheckForClientDeath macro; P81 removed that scheduler contract.
- XP corrects the retail BEGINATOMICCHECK spelling, so the temporary identical
  P81 local definition can be removed once its real header provider is active.
- Inventory current CURSOR/WOWTHREADINFO/WOWPROCESSINFO/SMS aliases against the
  already admitted USER layout providers before introducing duplicate layouts.

## Actual prerequisite identities

New csrmsg.h reference is windows/core/ntuser/inc/csrmsg.h, not the already
migrated public/sdk/inc/ntcsrmsg.h. It defines USER API message structures atop
that existing CSR header; importing declarations does not implement LPC service.
New heap.h reference is windows/core/w32inc/heap.h (private WIN32HEAP interface),
not base/ntos/inc/heap.h or a CRT header. It includes nturtl.h and conditional
heap diagnostics. Import only required original declarations/dependencies.
ptag.h is generated from windows/core/ntuser/kernel/ptag.lst; confirm the original
generator contract before introducing a replacement. ntddvdeo.h already exists
in non-mvdm/public/ddk/inc. _WINBASE_ already selects native declarations; preserve
its original wbasek.h conditional and the selected build's exclusion evidence.

## Validation state

P82 production edits and failed three-consumer ABI compiles are in progress.
No successful full build or publication yet; runtime97 remains current.
This inventory is not delivery or a claim that all residual diffs are necessary.
Next: resolve header/declaration dependencies, apply the complete XP migration,
then cached full build, dual publication, affected targeted tests, commit/push.

## Prerequisite implementation checkpoint

Five required originals imported byte-for-byte: USER csrmsg.h, w32inc heap.h,
and kernel ptag.lst/ptag.tpl/ptag.bat. Identities are recorded in
build/t485-s15/n024-userk/prerequisite-imports.json; no new allocator algorithm.
Original w32inc/makefile.inc selects listmung then ptag.bat. Reused the existing
built XP listmung and preserved both original template/batch bytes. A small cmd
launcher supplies relative temporary filenames in the build directory, so the
original FOR/F parser does not receive quoted paths as literal input strings.
The three C consumers depend on the generated ptag.h; no manual label table.
The generator-only check passed, matching every name/value/index against the
original list and confirming removal of its temporary file. This is NOT a
product build or runtime acceptance. Whole-header migration remains in progress.
Five additional mirrors are byte-identical; final census integration is pending
with the complete userk.h batch, not falsely counted as directory delivery.

## Header migration and first ABI compile

XP userk.h is now applied in the main tree with the existing explicit task
context ABI and two client-context handoffs. P81's temporary BEGINATOMICCHECK
macro is removed in favor of the real XP retail declaration. No publication.
Three production object targets were compiled together, including the generated
pool-tag prerequisite. The compile failed; preserved abi-build-first.log and
exit2 are evidence, not a delivery pass.

The first concrete conflicts are: heap.h needs FAST_MUTEX pointer declaration;
XP inline ThreadLockExchange requires complete _TL rather than an opaque pointer;
and a proposed DIAGNOSE_IO exclusion was wrong because XP defines that macro
locally to1. The latter exclusion has been removed, restoring XP's selected
branch. The original _TL owner is windows/core/w32inc/w32p.h:72-83, including
its original debug condition; client/usercli.h's iBogus placeholder is not an
appropriate substitute. Resolve these declaration providers without fabricating
native kernel objects. Remaining diagnostics will be assessed on that result.
The first header application is unverified; no directory/file delivery counted.

## Second ABI evidence

Original TL shape (w32p.h:72-83), EX free-function signature and opaque FAST_MUTEX
pointer declarations now resolve the first type failures. Second compile exits2;
abi-errors-2.json groups the remaining diagnostics. It reaches later portions
of the complete XP header, exposing input-device declarations, IME/kernel object
pointer declarations and projected task-ABI name conflicts. No production pass.

Restore original input declaration providers: existing ntddkbd.h plus required
XP ntddmou.h imported byte-identically (six prerequisite files now). KEYBOARD_ID_EX
comes from public/sdk/inc/wmidata.h:3746-3758; CONSOLE_CARET_INFO comes from
windows/core/w32inc/winuserk.h:91-94. These are the real declaration owners,
not reasons to delete retained device structs. Further declaration integration
and final diagnostic line mapping are still pending; no runtime is published.

## Layout and tail-declaration audit

Fourth ABI compile (PID27660, exit2; abi-build-fourth.log) confirms the earlier
queue/menu layout diagnostics no longer occur. It reaches the later header;
this is not a successful build. Third-log evidence is also preserved.
Nine inline helpers access the original kernel Q/THREADINFO members absent from
the selected wow_task_order_queue/thread views: PtiKbdFromQ, ValidatePtiKbd,
PtiMouseFromQ, ValidatePtiMouse, GetpMenuState, GetpGlobalPopupMenu,
IsInsideMenuLoop, IsMenuStarted and IsModelessMenuNotificationWindow. Repository
source search finds only their definitions/internal helper references. Omit
these incompatible bodies, not the surrounding menu types and other helpers.
IsShellProcess similarly requires kernel rpdeskStartup, absent from the host
process view. No fake fields, reinterpretation or replacement algorithms added.

PtiFromThreadId, zzzReattachThreads and zzzCalcStartCursorHide prototypes collide
with existing explicit-context provider macros. _InnerGetClassPtr has a concrete
host class-view declaration in wow_class_lookup_bindings.h; preserve that ABI.
These omissions do not remove their existing production providers.

Restore real declaration providers rather than deleting their consumers:
winuserk.h DESKRESTOREDATA pointer and USERPROCESSINFOCLASS; published winuser.w
menu/DC constants and POWERSTATEPARAMS; ntosdef.h MODE and ke.h KWAIT_BLOCK pointer.
Retail TAGMSG1 follows the original w32err.h no-op contract. These are declarations,
not new kernel state or execution providers. Final compile is still required.

The private ntuser.h include is omitted at the existing native USER boundary.
Its raw XP object is retained as xp-ntuser.h in build evidence. It declares all
kernel mode entry points and includes usercall.h. Concrete NtUserGetThreadState
and NtUserQueryWindow prototypes conflict with the selected control-client
precomp.h macros (416-417), whose operation-specific native bindings already
serve the selected source. The three userk consumers contain no direct NtUser
calls. This does not authorize removing harmless userk declarations or importing
the unrelated kernel syscall shell. Final header compile/line audit is pending.

## Fifth ABI compile: final header region reached

PID21448 completed with exit2. abi-build-fifth.log and abi-errors-5.json retain
all diagnostics, not a selectively filtered success claim. The compiler now
reaches the final userk.h region and the queue.c source. Earlier queue/menu
conflicts are absent; final-region helpers expose further concrete kernel
shared-heap, foreground-input, display-device and lock-owner dependencies.
Two queue.c diagnostics also require checking macro ownership: pDeskInfo and
gdwPUDFlags. Do not introduce fake globals or disable retained production code.

Next action is one complete tail audit: distinguish original declaration-only
providers from helpers accessing incompatible host layouts, trace actual
consumers, and preserve all original functions that compile without behavioral
or ABI substitutions. IME declarations need their original provider types.
The whole file remains unfinished; no cached full build, product publication,
targeted runtime pass, commit or push is claimed for P82. runtime97/P81 remains
the delivered baseline, and all P82 edits are retained for continuation.

## Final-region and production-provider correction

The sixth ABI run left only the cleanup view's missing retail diagnostic macro;
the seventh compiled all three actual consumers successfully (PID13584, exit0).
Retained otherwise unused helpers now see 14 exact extern declarations from XP
globals.h, without definitions, instantiated kernel state or new providers.
The six additional omissions access incompatible client/process/queue/thread
layouts: DesktopRebaseToClient, SharedRebaseToClient, LockCaptureWindow,
UnlockCaptureWindow, ThreadLockMenuNoModify and ThreadLockMenuAlwaysNoModify.
All have only their definitions in repository search. Other tail helpers remain.

Restored XP FJOURNALRECORD/FJOURNALPLAYBACK and SET_PUDF macros had overwritten
already delivered explicit host bindings. Remove those three declarations;
queue.c itself is unchanged. XP moved UserAllocPoolWithQuota to usergdi.h:485;
the existing task-initialization process-heap binding now names that entry
instead of obsolete ExAllocatePoolWithQuota. Allocation remains non-raising,
non-zeroing and paired with the same process heap release. Removed temporary
TIF_MEOW/DTM_SETUPAPPRAN definitions in taskman.c now supplied by XP headers.

The first cached full build failed in SoftPC vgaports.h: its MODE typedef
conflicted with the kernel MODE enum added to the generic support header.
Moved only the kernel enum into the selected USER declaration scope. No VGA
source change or type renaming. Second cached full build is in progress.

Line mapping checks 6,240 unchanged nonblank lines. Sixty-five conditional
boundary directives have compressed-body positions; their following #line
executes in the selected continuation. All retained diagnostic-bearing text
maps to its original line. This distinction is recorded, not hidden as an
all-directives-identical claim. Final compiler/link verification still required.

Local census:1338files/798byte-identical/809nonerror-identical/529different/
8148points/51728lines; delta+6/+6/+6/0/+104/-9917. userk.h switches from
OpenNT1point/10213lines to XP105points/299lines; taskman.c17/66 to17/63.
Six original prerequisite additions have zero diff. Newline restoration,
conditional source mappings and the baseline transition explain point growth;
these figures are not yet a delivery census. Seven new files including the
six prerequisites cross the ten-file checkpoint with the previous six, so
runtime98 requires Full after publication, in addition to the affected tests.

## Publication checkpoint

Second cached full build PID6400 exited0. Runtime98 staged and published to
assets/release and O:/WINNT/SYSTEM32; all13 images in both locations match the
manifest (26/26 hashes). Published-hashes.json records each destination/hash.
Delivery coordinator PID17256 continues affected tests then the Full checkpoint.
Publication is not a passing verification claim; no P82 commit/push yet.

## Targeted verification and Full in progress

Controls40/40, task-order, init-notification and ordinary task-lifecycle pass.
The initial InitRollback fixture failed because its allocator still intercepted
ExAllocatePoolWithQuota, which XP no longer calls. Product source and package
remain unchanged. Updated both the native fixture and the separate real-guest
startup-failure provider to intercept UserAllocPoolWithQuota, retaining the
same allocation failures, cleanup error poisoning and release assertions.
Retry init-rollback-r2 passes: failed_allocations=2,name_releases=2,errors=0;
observer installation failures=3 and live=0. Genuine Win16 controls also pass.
Preserve the failed init-rollback logs and all prior passing case evidence.

Full started once under continuation PID24160. Win3.1 standard-mode gate has
passed; remaining gates are not yet claimed. No commit/push until final result.
Exclusion-pairing audit has92 branches,65 explicit continuation anchors and0
anchor mismatches. The seven-header body/diagnostic checks and runtime tests
remain distinct evidence, not an assumption of equivalence.

## Final verified package

[Receipt](n024-userk-result.json) and [census](n024-userk-census.json).
Runtime98 cached build and dual26hash pass. Controls40/40, task-order,
init-notification, task-lifecycle, corrected allocation-injection rollback and
real Win16 controls pass. Full24/24 identifies the same final published package.
The first rollback failure is retained: its old allocation hook no longer
intercepted XP's UserAllocPoolWithQuota call. Both native and real-guest fault
providers now intercept that same actual entry; no assertion was weakened.
No original guest change or rebuild. Publication was available before tests.

Whole census1338/798/809/529/8148/51728; delta+6/+6/+6/0/+104/-9917.
1328XP/10OpenNT. userk.h OpenNT1/10213 to XP105/299; taskman.c17/66 to17/63.
Six new prerequisites are byte-identical. Diagnostic text maps to XP lines;
65 compressed conditional-boundary positions are separately recorded.
N0249/9, S15 12/14, overall60/159. Full checkpoint resets here.
P82 final verification and documentation gates passed. This receipt accompanies the P82 delivery commit; remote publication is verified from Git after push. Next N025; no S/T closure.
