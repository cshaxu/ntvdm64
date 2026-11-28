# S15 N024 queue migration audit

P79 runtime95 verified for delivery. Starting HEAD
b9deef43dcdd333e211ff2c385790ec01eacbfd4, accepted runtime94.
Current queue.c is OpenNT1point/5211nonerror lines. Global delivered census
1332files/792byte-identical/803nonerror-identical/529different/7990points/62434lines.
N0245/9, S15 11/14, overall59/159. Three files since P75 Full24/24.

## Fixed source

XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89, path
Source/XPSP1/NT/windows/core/ntuser/kernel/queue.c, blob
acb3619e95949290c2ad8b9174f86a6ed58aa3b1. OpenNT blob
0f5ad9b0d285838840847eb7bf64ca5dd0d75132. Raw originals and initial local
bytes are saved in build/t485-s15/n024-queue. No original guest changes.

## Verified call-chain differences

1. XP zzzInitTask:1114 accepts module and base-file UNICODE_STRING inputs,
   removes the old unused show argument, copies Length plus an explicit WCHAR
   terminator, and sets MaximumLength accordingly. Current xxxInitTask copies
   Length into a MaximumLength-sized allocation without explicitly appending
   the terminator. Restore the XP contract rather than adding another copy path.
2. XP stores LOWORD(hTaskWow), but tests HTW_IS16BIT/HTW_ISMEOW before queue
   attachment and scheduling. Production wkman.c:1545 already supplies
   HTW_IS16BIT. registered_init_task currently strips those bits and chooses
   base_name instead of module_name before calling the one-name lifecycle ABI.
   Merely replacing queue.c would silently skip Win16 scheduling. Pair the
   registration bridge, lifecycle capture and provider signature in this batch.
   Keep the lower task identity used by startup notifications distinct from
   the flagged initialization input; do not change request identity.
3. XP SetAppCompatFlags:819 is explicitly Win32-only (source history and
   assertions): it reads a process PEB/shim database and calls the IME/setup
   classifier. The current selected old function has no production caller;
   do not port its old WOW registry lookup into the XP initializer. WOW already
   supplies both compatibility flag words. The existing flag-mask contract is
   preserved by the original initializer.
4. XP SetAppImeCompatFlags:709 reads PMAP_IMECOMPAT, native locale and the
   optional setup-name array; it is called before scheduler enrollment. This
   must be evaluated as a concrete original dependency, not replaced by an
   unconditional success/no-op. P80 corrected the original setup.c comment:
   XP defines CreateSetupNameArray in kernel/server.c:1673. P79 did not import
   or call that population function; the null-array path does not prove setup
   classification is implemented.
5. XP SetAppStarting:1057 adds an RIT master-timer arm to the same original
   linked-list insertion; CheckAppStarting uses kernel process creation times.
   Current native carrier owns only the list and flags, not the RIT timer.
   Preserve the original list algorithm and explicitly account for the timer
   boundary; never add a second timer/queue just to imitate kernel ownership.
6. XP DeferWinEventNotify / zzzEndDeferWinEventNotify bracket input attachment
   and TDB insertion. Their stated purpose is preventing callback retirement
   while the local TDB is still needed. Current provider is
   wow_user_task_lifecycle.c::reattach -> wow_input_group_commit, which applies
   original AddAttachment/Recalc2 and native AttachThreadInput using the already
   prepared input plan. It does not implement the private USER WinEvent queue.
   No empty deferral callbacks have been introduced; the provider/notification
   boundary must be settled before claiming complete migration.
7. ParseReserved:1462 is independently restorable: XP protects the original
   parser with W32ExceptionHandler(FALSE,...), already provided by the existing
   exception-filter header. ClearQueueServerEvent:1453 also uses existing
   finite queue fields and native event reset. Preserve harmless originals
   where the current representation genuinely supports them.

## Paired implementation scope

- queue.c: selected original task initialization, starting list/show behavior,
  pure accessors/parser and supported original declarations. Enumerate omitted
  kernel process/thread/Q/input/desktop/CSRSS ownership; do not invent broad
  exclusion conditions for unconditional omissions.
- Existing B task structs/initialization/provider headers: exact new fields
  and changed function signature; original names and original parameter roles.
- Existing registration bridge and lifecycle: carry two captured names and
  flagged initialization handle through the same call; preserve notification
  identity, initial-publication retries, input-plan rollback and task ownership.
- Paired profile boundary: P78 deliberately retained legacy private section
  IDs14/5 pending this queue migration. Reconcile to original XP PMAP values
  when migrating this caller; do not silently index the XP table with old IDs.
- Existing fixtures: initialize/name-copy failures, Win16 flag, compatibility
  masks, shared/separate waiter ownership, journal attachment, ordering,
  initial publication and startup notification; real Win16 controls and package
  lifecycle remain required. No substitute probe or new test framework.

## Outstanding evidence before delivery

Resolve the actual IME/setup and WinEvent provider boundaries, finish all points
in this file and paired callers, compute final nonerror mirror census and #line
mapping, then cached full build, dual publication, targeted tests, commit/push.
No compile or runtime pass is claimed by this audit. It establishes the exact
ABI mismatch and prevents a compile-only replacement from skipping task startup.

## Local implementation checkpoint

The XP initializer, IME compatibility lookup, accessors and reserved parser are
now present. The registration bridge keeps the flagged initialization handle
separate from its lower16-bit notification identity. The existing lifecycle
captures module/base names together and frees both on its common cleanup path;
the provider and existing fixtures use the paired XP signature. No second
startup transaction or scheduling queue is introduced. The diagnostic allocation
failure provider is paired too, preserving the lower task identity in its log.

Profile consumers now use XP identifiers10/4/31 for compatibility, desktop and
IME compatibility. The native read-only provider opens the original matching
registry locations; it does not manufacture values, policy or impersonation.
The paired userk declarations remain individually OpenNT-accounted until that
whole file reaches its own migration slot. Its existing newline format is kept.

The RIT master-timer arm and private WinEvent-queue brackets remain explicit
omissions in this candidate. Native input attachment uses the existing prepared
plan; it does not dispatch the private USER notification queue. The original
optional setup-name list is retained with its original empty-list behavior;
population of that list is not implemented or claimed by this batch.

Original-line audit checked438 matching nonblank lines with no mismatch.
Local unverified census:1332files/792byte-identical/803nonerror-identical/
529different/8012points/63527lines; versus P78, delta+22points/+1093lines.
queue.c changes baseline from OpenNT1/5211 to XP23/6300; profile.c stays11/781;
paired userk.h stays1point and changes10185 to10189 byte-sensitive diff lines.
These are not delivery results or a claim of net reduction: the queue original
baseline also changes. Source identities, hashes and raw/normalized metrics
are in build/t485-s15/n024-queue/local-census.json. Cached full build is owned by
the PID and command in build-process.json, with build.log and build-exit.txt.

First build stopped at the native lifecycle's unavailable MAXUSHORT spelling;
MAXWORD supplies the same16-bit string-size limit. Second build reached final
link and proved a client/kernel collision: winmgrc.c and queue.c both define
GetAppCompatFlags, but read different client/thread views. A one-line queue-only
symbol binding retains the complete XP kernel body under
wow_kernel_GetAppCompatFlags, leaving the selected client provider unchanged.
This adds one mirror line and no new point; no weak-symbol selection, function
deletion or replacement algorithm is used. Both failed build logs/exit codes
are retained as build-first and build-second. The third cached build is live;
no publication or passing runtime test is claimed yet.

## Final source and ownership review

Selected XP code retains the original module-name copy/terminator, compatibility
flags and masks, startup-list algorithm, TDB enrollment, scheduler handoff,
startup presentation, reserved parser, IME query and both compatibility accessors.
The otherwise unused GetAppCompatFlags2 and ClearAppStarting are retained with
their original inline/list dependencies in userk.h. Names that originally lived
in separate client/kernel modules have local compile bindings in queue.c; no
body is replaced, no client provider is redirected, and no queue is added.

Omitted original kernel ranges remain unselected: process/thread creation and
deletion own ETHREAD/EPROCESS callouts, kernel locks and CSRSS desktop assignment;
Q/lookaside/message allocation owns kernel pool and private QMSG/SMS storage;
input wake/sleep/post-message dispatch owns kernel input queues and wait objects;
console/window-station/process-information entrypoints own kernel handles and
security. Existing native USER and bounded lifecycle carriers continue owning
those host resources. The original Win32-only SetAppCompatFlags reads PEB/shim
state and is not the WOW initializer; guest-supplied WOW flags remain authoritative.
These unconditional omissions are counted as differences, not hidden behind
invented exclusion branches. The one retained original DBG exclusion uses #error.

The native IME query now uses XP PMAP_IMECOMPAT31. Original setup-list inspection
is retained; CreateSetupNameArray population is not imported or claimed. The
kernel RIT master timer and private WinEvent notification queue remain outside
this native startup carrier, as described above. Native AttachThreadInput commits
the existing prepared plan without dispatching the private WinEvent queue.

## Final verification and delivery

[Receipt](n024-queue-result.json) and [census](n024-queue-census.json).
Cached full build, dual13-image publication/26hash checks, controls40/40,
task-order, init-notification, task-lifecycle, init-rollback and genuine Win16
controls pass against runtime95. No original guest was modified or rebuilt.
The first two builds failed; their logs remain. Later compile reviews removed
native declaration collisions and paired the remaining old OpenCacheKeyEx
prototype. Intermediate successful builds do not replace final-build evidence.
Original-line audit checks457 matching nonblank lines, with no mismatch.

Final whole census:1332files/792byte-identical/
803nonerror-identical/529different/
8012points/63536lines.
Commit delta0files/0exact/0same/0different/+22points/+1102lines.
1319XP/13OpenNT. queue.c changes original baseline from OpenNT to XP; the larger
XP kernel-only remainder is counted. This is not reported as a net reduction.
N0246/9; S15 11/14; overall59/159. Four files since P75 Full24/24.
Next server.c; no S/T closure.
