# mvdm

MVDM-HOST-DIV-466: BOP 57 and BOP 58 pass the actual segment-address
mode to SIM32: PE with VM clear selects descriptors; real and V86 execution
select segment arithmetic. The original PE-only decision mistakes V86 for
protected mode. This changes neither SIM32's explicit-mode contract nor the
redirector and ISV dispatch algorithms. Focused and Windows 3.1 enhanced-mode
validation are required before delivery.

DIVERGENCE(VIDEO7-PACKED-SURFACE), owner-authorized on 2026-10-25 after
the test publication displayed a 1280x480 mostly empty DIB for a 640x480
guest desktop. `host/src/nt_graph.c::check_win_size` uses the selected packed
painter's byte width, matching standalone SoftPC's same function.
`host/src/nt_vga.c::nt_v7vga_hi_graph_std` adopts SoftPC T60's actual
DIB/source-extent bounds instead of rejecting full 480-row updates.
Both retain the original painter and copying algorithm; no guest or CPU
bank/access behavior is changed. The captured chain4/seq-chain4 state is
already packed, so unrelated T70/T88 stride and painter-selection changes
are not imported speculatively. `tests/observation/verify-video7-packed-painter.ps1`
links the actual painter for full-frame, pitch, clipped-damage, source-end,
invalid-input and guard checks. Baseline/candidate Win95 host-DIB evidence,
native layout offsets and process/hash identities are retained under
`O:/assets/win95-ntvdm-research/display256/published-failure`.
Guest GDI readback or BIOS mode success alone is not host-output acceptance.
Local acceptance passes the actual painter fixture (original source fails),
32 BIOS settings and Win95 desktop/256-colour witness. Captured host output
changes from 1280x480 with 50 populated rows to 640x480 with all 480 populated
rows and later desktop content. The exact candidate worker is test-published
in `O:/WINNT/logs/video7-packed-surface`; physical input and full DOS/WOW/Win3.x
regression remain unclaimed.

DIVERGENCE(VIDEO7-PRESERVE-MODE), owner-approved standalone correction on
2026-10-25: `softpc.new/base/video/v7_video.c::v7vga_extended_set_mode`
validates BL's seven-bit mode number before using its independent bit-7
preserve-memory flag. This matches standalone SoftPC T70 S10 (`62eec11`,
source `O:/repos.hobby/softpc/src/app-softpc/softpc.new/base/video/v7_video.c`);
original OpenNT validates the unmasked byte and rejects E7h. Retain the
original mode setup, banking and clear/preserve algorithms; no guest or
profile-specific branch is added. The actual BIOS regression is
`tests/observation/verify-video7-mode-flags.ps1`: the affected 67/E7 pair,
pixel preservation/clearing and invalid requests. Both focused acceptance and
the optional all-mode sweep passed locally: 16 legal modes with/without bit 7
(32 settings), plus preserve/clear and two invalid requests.
Source hashes, baseline/fixed build inputs and real Win95 256-colour readback
are retained under `O:/assets/win95-ntvdm-research/display256/hostfix`.
The T460 S3 coherent package subsequently included this source and was
published; see `docs/history/m0-t460-closure.md` and the frozen-source record
in `docs/etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md`. The historical
mode sweep is not a claim that it was rerun against every later package.

MVDM-HOST-DIV-455 (T473 S9 delivered baseline; S9 remains open): the three original WOW DC acquisition
thunks pass the acquired native HWND to the original `StoreDC` cache. Modern
USER handles are not NT4 sign-extended HWNDs; resolving again at deferred
release could select a recycled window. Original cache search, delayed release,
GDI aliases and cleanup remain intact. See the S9 wrapper audit for the caller
trace, actual thunk and cache fixtures, HWND reuse and normal guest paths.
The r10 ten-image package was published after Product and Control gates and
application probes. Acquisition-induced reentry and every failure path remain
unproved; see the evidence record for the exact scope.

MVDM-HOST-DIV-454 (T460 S3 candidate): original `wow32/wuclip.c` and
`wuser31.c` adapt only their public Clipboard HWND operands/results through
the existing worker object table and its already published original shared
window observation. Modern USER cannot consume the original NT4
index/uniqueness token. The original registered full-handle callback retains
that representation for all other WOW consumers. A returned native window has
a guest alias only after its exact current identity is in either existing
publication; foreign or retired identities are never truncated, proxied or
assigned a new row. This adds no Clipboard owner, cache or scheduler. Details
and lifetime evidence are in `docs/etc/evidence/m0-t460-s3-clipboard-deferred-lifetime.md`.

MVDM-HOST-DIV-461 (T460 S3 delivered): original
`softpc.new/base/keymouse/keyba.c::KbdEOIHook` retired the single 8042 output
slot as soon as IRQ1 was acknowledged.  That assumes the normal port-60-read
then EOI order, but Win95 VKD/VPICD can EOI first and thereby loses the unread
make code.  Two controller-local facts retain the existing single slot until
both its actual port-60 consumption and its normal IRQ1 EOI have happened.
Status-port reads, PIC-reset cancellation, direct controller replies, the
original delayed IRQ path, scan queue and guest keyboard driver retain their
separate contracts.  The actual production controller/PIC fixture covers both
orders and the source/ownership matrix is
`docs/etc/evidence/m0-t460-s3-keyboard-early-eoi-plan.md`.

MVDM-HOST-DIV-460 (T460 S3 candidate): original
`softpc.new/base/system/ica.c::SWPIC_outb` ICW1 clears the selected PIC's
IRR/ISR/count state without retiring device-side EOI ownership.  In the NTVDM
host, timer, keyboard, RTC and mouse devices use the existing
`host_EOI_hook(irq, -1)` cancellation contract to release that ownership; a
stranded timer `EoiPending` otherwise blocks later IRQ0 delivery.  Snapshot
the original adapter's IRR|ISR while holding its existing ICA lock, retain the
original state-reset sequence, and issue one existing cancellation notification per
retired line before unlock.  New work requeued synchronously by a device
callback is post-reset work, not a resurrected snapshot.  No timer state,
IRQ, guest, Win95-specific branch or alternate PIC implementation is added.
The independent source/contract matrix and required pending/in-service,
cascade, repeated-reset and device regression evidence are in
`docs/etc/evidence/m0-t460-s3-pic-eoi-reset-plan.md`.

MVDM-HOST-DIV-453 (T460 S2): for selected original
`wow32/wuclip.c::WU32SetClipboardData` text-like formats, preserve the
original guest-to-host copy and original cache owner only after public USER
accepts the copied `HGLOBAL`. The unchanged source cached the guest handle and
returned it even when `SetClipboardData` failed because the Clipboard was not
open; it also left the rejected copied host allocation unowned. On rejection,
free only that just-created host copy, retain the prior cache unchanged and
return zero. This does not change delayed rendering, structured formats or
guest media. The isolated guest/native five-phase transfer witness proves
accepted bidirectional conversion, cache retirement/reuse and rejected-input
ownership.

MVDM-HOST-DIV-452 (T460 S2): original NT4 USER invoked the registered
`pfnEmptyCB` whenever a native writer invalidated WOW's original 16-bit
Clipboard cache. Modern USER rejects that private registration ABI. The small
`wow32-dll` adapter therefore records only the public Clipboard sequence after
the original `WU32CloseClipboard` releases the lock; before a cached guest
handle is reused, a changed sequence enters the original
`W32EmptyClipboard` retirement owner. No Clipboard bytes, handles, format
registry, listener window or second cache are introduced. The same five-phase
witness proves an external native replacement retires the old guest cache and
performs the original native-to-guest conversion.

MVDM-HOST-DIV-451 (T453 S2 candidate): original
`wow32/wkfileio.c::WK32FileSetAttributes` rejects a non-root trailing path
separator before calling `NormalizeDosPath`, but then reaches its ordinary
named-pipe cleanup predicate.  That skipped original path leaves the local
pointer and ownership flag indeterminate.  Initialize only those two cleanup
locals; retain the original trailing-separator error precedence, DOS attribute
masking, native call/result encoding and all successful normalization paths.
No guest, pathname algorithm, handle, cache or alternate cleanup owner is
added.  Focused branch evidence and the composed W04 package determine
closure.

MVDM-HOST-DIV-450 (T452 S2): retain original WOW32 KERNEL k49's module versus
HTASK-alias selection, original `NtQuerySystemInformation` observation and
ANSI conversion, but make the caller-declared guest byte count the sole
output/flush bound. A zero or signed-invalid count returns zero before guest
mapping; nonzero output starts as an empty string and flushes exactly that
range. The historical 16 KiB stack snapshot becomes one transient
size-query/allocation/populated-query sequence; an allocation, query,
conversion, retirement or process-list-change failure returns zero without
retry, cache, process handle, alternate alias table or guest pointer escape.
K233/K234 retain the selected original CCPU40 `VdmAllocateVirtualMemory` /
`VdmFreeVirtualMemory` pair and are not redirected through this change.

MVDM-HOST-DIV-449 (T449 S3 candidate): the selected original WOW task
lifecycle has the exact `BaseSrv` task number and 16-bit `HTASK` together only
in `wow32/wkman.c::W32Thread`; bind the project-owned opaque management ticket
there after original `InitTask` succeeds.  Retire that ticket only from the
same original owner's `W32DestroyTask`, after original `W32FreeTask`,
`ExitVDM` and `pfnWOWCleanup` retain their normal ordering.  The adapter holds
only `{ticket, HTASK}` for an authenticated NTSRV close request and invokes
the original local hung-task entry on a new local thread because that original
entry exits its caller.  It neither owns a task scheduler nor substitutes a
PID/request number for `HTASK`; guest sources and semantics are unchanged.
The two original user-cancel exits in `W32HungAppNotifyThread` leave the task
alive, so their owner-local hooks only reopen that same authenticated ticket;
they never retry or replace original hung-task policy.
The original handler has one global completion event and target-TDB slot, so
the adapter admits only one such original termination transaction at a time;
a concurrent request reports failure for an explicit later retry rather than
running two source handlers against shared state.  Its serial gate ends when
the original handler thread ends, while normal cleanup still owns ticket
retirement; a handler that finds no task reopens its own still-published
ticket rather than stranding it.
Separate WOW has no surviving original task record and receives zero at this
original hand-off, so it remains worker-close-only rather than acquiring a
fabricated task row.  The matching host adapter is
`wow32-dll/source/wow_task_termination.c`.

MVDM-HOST-DIV-362 (T448 S12 candidate): on directory-buffer reclamation,
retain one bounded unconsumed successor name/index. If the original slow
reset cannot find its remembered anchor, a user-mode adapter searches for
the successor and leaves it unconsumed. This is not equivalent to the NT4
kernel indexed query under arbitrary deletion, rename or index reuse. The
user-mode adapter lives in `ntvdm-exe/session`; the real-provider, DOS pressure
and full package regressions are recorded in the
[S12 evidence](../../docs/etc/evidence/m0-t448-s11-window-startup.md).

MVDM-HOST-DIV-361 (T448 S12 checkpoint): `VrGetUserName` rejects an
explicitly checked zero-byte capacity with the existing `NERR_BufTooSmall`
result instead of underflowing `CX-1` and writing to the guest buffer.
Unchecked `BX=0`, positive capacities, conversion and buffer release remain
unchanged. The original-provider fixture covers zero, empty and nonempty
names, exact bounds, unchecked mode and failure cleanup; the guest is unchanged.

MVDM-HOST-DIV-360 (T448 S12 checkpoint): when `demsrch.c::FileFindClose`
discards unconsumed directory-buffer entries after the underlying enumeration
already reached its end, mark the search for its original reopen/reset path.
Preserve other failure statuses and the empty-buffer end status. DOS INT21
under buffer pressure and the real DEM provider verify the non-deletion case;
the deleted reset-anchor limitation remains unresolved.

MVDM-HOST-DIV-359 (T448 S12 checkpoint): `keyba.c` installs the actual 8042
controller response in the output register and status without issuing a keyboard
interrupt; command C0 supplies the emulated input-port byte. The existing
NTVDM path otherwise returned stale scan output. Actual controller/PIC tests
cover replies and EOI-owned queued input.

MVDM-HOST-DIV-358 (T448 S12 checkpoint): `vga_prts.c` records CRTC cursor
start register writes even when only bit 5 changes and applies that disable bit
independently of scan-line geometry. It preserves the existing split-cursor
geometry. Register readback and real frontend-frame verification pass.

MVDM-HOST-DIV-357 (T448 S12 checkpoint): original
`softpc.new/base/keymouse/mouse_io.c::mouse_save_state` always stores a zero
checksum, whereas its original restore routine validates the byte sum of the
cookie and context. Keep the original format and restore validation; compute
that same sum over the exact bytes saved. Source-proven real INT33 failure and
test-buffer checksum control, current SoftPC comparison, rejected alternatives
and negative/lifecycle tests are recorded in the
[S12 evidence](../../docs/etc/evidence/m0-t448-s11-window-startup.md#int33-host-correction-proposal).
The full tested thirteen-image checkpoint is published; S12 remains open.

MVDM-HOST-DIV-356 (T448 S3 candidate): retain original SoftPC terminate,
host_applClose and host_terminate bodies and their cleanup order, but obtain
standalone service permission before their first resource/record destruction.
terminate must enter before its repeated-call exit shortcut; host_applClose
and host_terminate also have direct callers. Three one-call insertion sites
share ntvdm-exe/win32/worker_lifecycle.c's cleanup-thread ownership. Recursive
calls by that owner do not submit another request. A natural caller waits on
the existing authenticated service/close handles; acceptance alone is not
permission. Console-close retains its original callback and approved grace.
Original fixtures without a managed worker have no service policy. A final
ExitProcess binding was rejected because device cleanup and ExitVDM precede
it. No guest changes or new control transport. Pinned OpenNT base/mvdm sources:
softpc.new/base/support/terminat.c SHA256
B085166424614A154A41F794BF3505F50A77A2EA60338BCEFCB845BE8E092E53;
softpc.new/host/src/nt_reset.c SHA256
9AA8120237FFA104C87318EF39747A0E66C28BF56BFE9AF03511BDDF13F11B47.
The same registration covers WOW's five whole-process exits in wkman.c and
wow32.c: the parent NtvdmAuthorizeExit export preserves their selected exit code
unless the service has died (ERROR_INVALID_HANDLE). Original ExitVDM and
ExitProcess stay in their original order; no single-task retirement is
intercepted. ExitKernel/FatalExit copy the argument and release its guest alias
before the blocking service call. Original OpenNT base/mvdm/wow32/wkman.c hash:
56BE1989F2BC3ACAFFCFA6345B20E38F5E7312BADF03AF04D4565F8F4A489AC9;
wow32.c hash: 1DFE9900E6DA3BF0E2984C3DF42BDD155B177CFE7707BB24E1324037147D292A.
This candidate does not yet prove partial-initialization safety or all original
entry runtime paths; verification belongs to the
[S3 ledger](../../docs/etc/evidence/m0-t448-s3-close-implementation.md).

The same DIV-356 adds one activation call at nt_cpu.c::host_start_cpu, after
original main has completed host/device/DOS initialization. The adapter retains
early service capabilities but starts cleanup callbacks only here. Original
initialization has no ready callback to adapt; starting receivers in standalone
bootstrap permits cleanup of uninitialized original resources. No original
algorithm moves out of its owner. Original nt_cpu.c SHA256:
3F77F06E497854B00F58ACDE8BFE7B5E67AF7C5E67174DEB579CD76E13F5830F.
Before this boundary, startup failure obtains service permission then releases
only owned partial startup resources; it does not invoke the original Console
handler. Activation failure after the boundary enters original TerminateVDM.

MVDM-HOST-DIV-355 (T448 S3 P18): original OpenNT
base/mvdm/softpc.new/host/src/nt_event.c routes COMMAND fTSRExit by cleaning
the worker and posting WM_CLOSE to its Console window. The admitted standalone
architecture assigns that Console to NTSRV/NTCON, so direct reuse would bypass
the service's close transaction. Keep cmdPushExitInConsoleBuffer's WOW exclusion,
CNTRL_PUSHEXIT identity and calling-thread retirement, but submit Console scope
through the existing worker-base/RPC mechanism. Move its original host_applClose
and ExitVDM(FALSE,0) to CntrlHandler's CNTRL_PUSHEXIT branch, followed by the
original ExitProcess(0). This replaces that branch's obsolete DIV-147 session
escape, not the remaining DIV-147 sites. Source-local ordering is unchanged
within cleanup; its start now follows the service Console-close delivery.
NTSRV resolves the authenticated frontend association and preserves quiescence;
the project-created callback thread has no original Console-server callback
dependency. No replacement Console window, timeout or guest change is added.
The source already composes; a binding alone cannot relocate pre-authorized
cleanup, so this minimal owner-local mirror change is registered. Verification
and remaining original-entry gaps are tracked in the
[S3 ledger](../../docs/etc/evidence/m0-t448-s3-close-implementation.md).

MVDM-HOST-DIV-363 extends DIV-354's conversion-local rollback in
`wow32/wdde.c`, `walias.h`, `wmsg16.c` and `wumsg.c` for T449 S2. The original
`WU32PostMessage` loses its stack-local `DDECONVERSION` immediately after
thunking, even though the actual native `PostMessage` can still reject the
converted packet. Keep one call-local record only until that native result: a
rejected post releases its packed parameter and just-created original HDDE
pairs before the existing private-message fallback; a successful post transfers
them unchanged to the original receiver/ACK path. Do not change synchronous
`WU32SendMessage`, replace the original HDDE list, add a registry, or alter
successful ACK ownership. The ordinary ACK frees a generated
`DDE_EXECUTE_FREE_H16` alias; the same alias must be freed when a rejected post
has no ACK. This is the sole `wdde.c` divergence. The focused source fixture
proves original pair/mark ownership, failed-post cancellation and that exact
generated-alias rule. The recorded candidate package additionally passed
actual guest DDE traffic and the rejected DATA-post control; complete S2
delivery is recorded by its coherent thirteen-image package gate.

MVDM-HOST-DIV-354 registers the remaining S4 classic-DDE construction and
rollback closure in existing wow32/wdde.c/.h, wmdisp32.c and wmsg16.c. Keep
the original HDDE list as the sole persistent alias authority and retain each
original packet/format/ACK/free algorithm. A bounded call-private conversion
record may retain only scalar pairs created by one conversion (direct packet,
DIB inner object, metafile-pict and metafile): it is not a registry, cache or
ABI field. Register each pair before any subsequent failing operation; on
failure, unwind that invocation's pairs in reverse only when the original
HDDE still proves the exact pair is live. The native-to-guest rollback frees
only newly created guest objects; the guest-to-native rollback frees only newly
created native objects, using the existing metafile release route where
applicable. Existing source objects and reused pairs are always borrowed.
Preserve original publication-before-mark ordering; propagate a FALSE
CallBack16 completion only when the current public wrapper rejects before the
original guest callback, then remove the exact just-published pair. Never use
the guest VP return, add a second persistent table, replay a message, broadly
admit DDEML, alter guest/ABI, or replace original transfer/ACK ownership.
Budget 220 executable added/removed lines across the four named originals.
Require exact construction-allocation/registration/pack failure, nested
indirect DIB/metafile, reused alias, ACK/free/reentry, failed post and
successful-transfer controls before the one composed product build; this does
not by itself close S4.

MVDM-HOST-DIV-353 registers the S4 WM_DDE_INITIATE publication-progress
boundary in existing wow32/wdde.c/.h, wmsg16.c, wmdisp32.c and wstruc.c.
The original DDEInitiateList remains the only persistent initiation authority.
`WI32DDEAddInitiator` reports only whether it actually published a node; each
existing per-message invocation records that fact in its already call-private
message storage, and a synchronous normal/recovery unthunk removes a node only
when that same invocation published one. This preserves original successful
LIFO same-initiator nesting and queued-transfer lifetime, while an allocation
failure cannot consume an outer same-key token. It deliberately retains the
original delivery behavior after an allocation failure: this correction adds no
new post/queue failure policy, list, guest change, ABI change or protocol
feature. `putmsg16` initializes its existing local conversion record before
recording this progress, rather than reading indeterminate private slots.
Budget 40 executable added/removed lines. Require complete original-body
same-key nesting, failed-add, direct unthunk, getmsg recovery and queued
transfer controls together with the remaining DDE construction/transfer group;
this is not DDE closure by itself.

MVDM-HOST-DIV-352 registers the S4 EXECUTE ACK node-read boundary in existing
wow32/wmdisp32.c. DDEFindAckNode returns the original live HDDE authority;
capture that node's metadata on this invocation's native stack before the
first guest unmark/free callback, then use only the captured keys/flags for
the existing ACK branches. Reentry may retire the list node; never reread
its allocation afterwards. Keep the original list lookup/deletion, flags,
ACK result, free order and missing-pair disposition. No second alias table,
new generation policy, guest, ABI or native payload lifetime change. Budget
10 executable added/removed lines. Require the complete original ACK normal,
late-free, synthetic-handle and reentrant-node-retirement controls alongside
the existing alias group; this alone is not the whole DDE ownership repair.

MVDM-HOST-DIV-351 registers the S4 original DDE alias retirement boundary
against the pinned OpenNT source and7835ba25e's D1/D2/D5 ledger. Keep HDDE
as the sole persistent alias authority and original message/format/free-veto
algorithms. An empty DDEDeletehandle must return FALSE, just as its existing
missing-pair branch and neighbouring original CopyDataDeleteNode do, rather
than dereference the absent head. No replacement registry, guest change,
protocol feature or ownership transfer policy follows from that guard.
Initial bound20 executable added/removed lines in existing wow32/wdde.c;
extend the registered design before any broader acquisition/retirement edit.
Require exact original-body empty/missing/head/interior/tail/duplicate and
both-direction free/veto controls in the complete DDE acceptance cohort.

MVDM-HOST-DIV-350 registers the S4 DDE conversion-selector cohort against
pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c and7835ba25e's
nine-entry ownership ledger. Existing wowhooks.c and wumsg.c retain original
conversion/callback phase choices and native/guest conversion calls. Capture the
entering TLS selector and restore it on normal, early-return and exceptional
exit, including cleanup failure. Pair narrow GetMessage/PeekMessage putmsg16 intervals
and both PostMessage/SendMessage caller intervals; retain the original scroll
compatibility early return. No allocator, packet transfer/free, message
replacement or ABI change. GetMessage's removed queue item explicitly uses
the existing consuming phase TRUE, independent of its enclosing hook selector;
PeekMessage still derives it from PM_REMOVE. No
new state manager, thread/task ABI or guest change. A lower DDE
allocator cannot restore its caller's operation selector on bypass/failure.
Budget80 executable added/removed lines across these two existing originals.
This is part of the already fixed DDE cohort, not an independently complete
protocol repair: original-body nested/failure proof and the entire grouped
DDE/runtime/publication gates remain required before delivery.

MVDM-HOST-DIV-349 registers the S4 call-owned conversion transaction against
pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c and delivered P21/
native prerequisite860785e88. Six existing originals: wmdisp32.c/.h,
wcntl32.c, wcommdlg.c, wudlg.c and wowhooks.c. Keep original allocation,
conversion, normal output/result and global-lock algorithms. Capture completed
heap/global acquisitions in existing message scratch before any copy can
escape; consume keys before the original release. Pair nested MDI transfer
and the independent CBT creator. Existing outer finalizers include window,
dialog and both message-hook consumers; stack-allocating CBT/journal/mouse
hooks also retain a native call-local cursor/INIT snapshot. Restore only a
failed invocation, while its original task/worker remains live; preserve normal
stackfree16's no-op and SignalProc teardown. Explicitly exclude COPYDATA's
already owned header/data and DDE protocol transfers from generic heap keys.
No field/ABI size change, allocator/registry, output-copyback-on-abort,
idle/scheduler change, guest edit or new original import. ADAPTER-WOW-101
supplies only the finite captured-key release and stack pairing behind these
original consumers; a lower allocator cannot fix their ignored copy failures.
Budget300 executable added/removed lines across these six originals. Complete
the entire original-body fault/normal/nested/lock-count/terminal group and
actual guest plus every-P/coherent13 gates before production publication.

MVDM-HOST-DIV-348 registers the S4 original callback-abort ownership group
against the pinned OpenNT owners and P20/audit92a60fe38. Existing wcall16.c
owns call-local cursor/INIT progress and deferred API-frame return; wow32.c
keeps W32Exception but targets its handled return's actual API entry. Existing
CCPU c_main.c/ntthread.c/.h own only the abandoned native simulation level;
c_xtrn.c always clears its existing interface-active flag after an escaped
native exception. No CPU opcode, guest, callback conversion, normal result,
task-selection or exception-disposition algorithm is replaced. The original
normal callback body and FE longjmp stay; source-owned finally and exact-frame
cleanup pair only an interrupted invocation. Existing SignalProc teardown
INIT/IGNOREINPUT transitions survive. Current descriptors and shared state
remain authoritative: do not replay old VDMFRAME bytes or c_getCpuState.
The bounded CPU-control facade uses original selectors/PE/VM/CPL setters,
preserves LastError, and cannot turn unavailable continuation into success.
Budget180 executable added/removed lines in these six original files. The
indexed S4 audit owns full phase/disposition/terminal evidence and the source
recovery ladder; complete source/tests before one affected build cohort.

MVDM-HOST-DIV-347 registers the complete S4 parameter-map ownership cohort
against pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c and P19.
Seven existing originals: walias.h, wparam.c, wmsg16.c, wmsgcb.c, wmdisp32.c,
wcntl32.c and wuwind.c. Keep the original table, refcounts, alias lookup,
relocation, cache and task teardown. Reject zero pairs; rollback an existing
reference if refresh fails/escapes; do not publish a NULL refreshed pointer.
Use existing call scratch to distinguish actually acquired map references
from direct views/owned heap/DS-stack, clear it before original release and
reuse only the input-only text/CB_DIR unthunkers on abort. Capture the direct
SetWindowText guest key before reentry and release its actual acquisition in
local finally. No generic finalizer, allocator/table replacement, guest edit
or ABI size/layout change. These original consumers alone know acquisition
and release progress; a lower allocator cannot repair ignored failure or an
unconditional deletion of another call's node. Budget180 executable added/
removed lines across these seven files; exact original-source normal/fault/
nested/relocation/teardown witnesses and real guest/retained13 gates precede
publication. The indexed S4 source audit records baseline hashes and scope.

MVDM-HOST-DIV-346 registers the complete reverse/native-buffer constructor
failure cohort against pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
wmsgem.c and wmsglb.c are byte-exact before this cohort, SHA256 respectively
88D05E4770453621472704E90EDCEFD6AD18B618528B4166DA3A9BA1EA1A3619 and
5E1A7DD4140AE6538D4E728CA869013E8CD00D06D5339EE5682319D5FF4BF26E;
prior registered wmsg16.c is
BE05F88C0D422049D7308E5A6711E8E65181C8331A5405547BD547E9B1676B2C.
Only reject missing LocalAlloc/STACKORHEAPALLOC/malloc_w outputs in
EM_REPLACESEL, EM_SETTABSTOPS, LB_GETSELITEMS, LB_SETTABSTOPS and WM_WINHELP.
Pair both tabs releases with their original signed construction predicate;
negative counts acquire nothing and must not free the caller's guest token.
Retain original allocation choice, converters, message/return representations
and unthunkers. LocalAlloc failure currently reaches RtlCopyMemory(NULL);
the other branches falsely report conversion success with no native buffer.
The caller's existing ThunkMsg16 FALSE/fThunkSucceeded contract already
rejects construction failure; no replacement control or cleanup registry.
A lower allocator cannot repair ignored NULL at these original consumers.
No original-source import or guest change. Budget20 executable added/removed
lines across these three original files, with exact selected-source fault
negatives and normal/inline/heap/zero-count/one-release evidence, actual control
messages and retained coherent13 gates required before runtime delivery.

MVDM-HOST-DIV-345 registers the S4 original construction/release cohort against
the same pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c WOW32 owners.
Pre-change canonical mirror hashes: wmdisp32.c
141A504A03FD41517B5585F802B636FD49A04546E538E71A0412F65F9E83BA4D;
wmdisp32.h B26E105AE57608DFE9D30A629496549745BC5B68AC67B7DABA0E84404A95929C;
wmsg16.c 92552D35D187F3D278333C12F0FB47D4D89FE968EC2E2252A9B47D4C0A43C9B2.
Retain full original bodies/allocators/maps and existing Error/unthunk paths.
Guard GETTEXT's absent allocation, propagate nested MDI conversion failure,
release the captured owned MDI title, keep the original unmapped-string/free
fallback on optional map allocation failure and initialize WinHelp's failed
handle0. Existing dwTmp gains two final DWORDs for nested MDI title/structure;
prior member offsets/PARM16 and public/guest/wire formats remain unchanged.
Cleanup uses captured ownership, not mutable guest fields or current WND style.
No facade can correct these original local failure branches or recover an
owned title after it has been overwritten; no later code is imported.
Source-proven minimal host corrections are permanently authorized. Budget60
executable added/removed mirror lines in these three files. Exact-body
fault/ownership negatives and actual MDI/text plus full coherent publication
pass in bounded S4 P18; see the indexed S4 source audit for exact evidence and
remaining M4 obligations. Raw mirror delta28 added/23 deleted lines includes
comments; selected differing-file count remains95. No guest change.

MVDM-HOST-DIV-344 (bounded S4 P13 delivery) registers the COPYDATA/caller group
against pinned OpenNT5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, base/mvdm/wow32.
Original per-file hashes/paths are retained in P12 required-root-foundations-r1
source-registers evidence and its canonical topology manifest. The affected
original owners are wdde.c/.h, wmdisp32.c/.h, wmsg16.c, wumsg.c/.h, walias.h,
wudlg.c, wowhooks.c/.h and wstruc.c; no guest owner is modified.
Retain conversion/layout/dispatch/list algorithms. Add checked acquisition and
rollback through the original unthunk, an empty-list delete guard, call-owned
COPYDATA scratch, explicit synchronous fFree initialization, rejection/status
and finalizer hooks. MSGPARAMEX gains only an appended host-private success
bit: a NULL HWND is not conversion failure. Existing member/PARM16 offsets and
guest data formats stay unchanged; no assembly/worker/wire consumer owns it.
Temporary conversion flags/Hook identity and filter recursion scratch belong
to their native thread's nested call stack. Map the existing private variables
to TLS and restore scoped values; do not make the shared CPDATA list TLS.
Normal original cleanup/order stays in the original caller; new exception-only
finalization mechanics live in wow32-dll under ADAPTER-WOW-095.
Direct original composition exposes ignored allocation failure, missing
initialization and unpaired unwind; neither native USER nor a facade can fix
those omitted owner branches. Source-proven host correction is permanently
authorized. No later source unit is imported; comparative Windows2000 evidence
corroborates the two TRUE initializers only. Initial budget is guard/state/scope
changes in these existing owners, at most300 executable added/removed lines;
retain full functions, no algorithm crop, guest/public ABI or scheduler change.
Review exact-body negatives, all fault stages, normal/nested/Proc32 and actual
guest/caller lifetime tests as one group before coherent publication. Scope,
source identities, limits and final accounting are in the active S4 audit.
P13's conservative nontrivial changed-line upper bound is285 (budget300).
Model27 and actual Proc32/whole caller gates pass; this is not all M4 closure.

MVDM-HOST-DIV-343 registers a definition-only name binding in
wow32/wcall16.c::CallBack16, pinned to OpenNT
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c, original SHA256
4875844ABFB8D22025480C14174481A194779D83759CE3A696E978B68C55A9F0.
The unchanged original frame/CPU/return algorithm is named
wow_original_CallBack16; ADAPTER-WOW-079 supplies its existing public-shaped
entry and pairs the existing execution scope before guest frame resolution.
The original body always returns TRUE, so its public WOWCallback16/Ex callers
previously ignored that result. The newly fallible admission must propagate
FALSE as DWORD0/BOOL FALSE instead of reporting an unexecuted callback as a
success. Budget: definition identifier, those two failure-propagation guards
and local comments; no frame, CPU, resolver, normal-result or guest change.
Original Ex argument prevalidation and preparation order remain. A valid but
rejected Ex call does not promise output preservation after its original
cbArgs store; invalid-size output remains untouched as before.
A whole-unit rename is rejected because
it also redirects20 same-unit memory/resource/public callback calls around the
entry. Late host_simulate gating is rejected because frame writes precede it.
Original-body fixtures explicitly call the retained-body name; separate scope
and real extension/callback/yield/resource/lifetime acceptance must prove the
composed production entry. Registration precedes the mirror edit.

Retrospective MVDM-HOST-DIV-304 register completion: wcall16.c's existing
before/simulate/after callback-frame observations call the already owned
wow_callback_frame_trace. Original resolver, stack selection, host_simulate,
return/unwind and failure policies remain unchanged. Copied scalar CPU/frame
values go only to the existing process-local opt-in log; no alternate
dispatcher, guest-memory read, CPU-register write or new logging interface is
introduced. This fills the missing canonical row for the existing local ID,
not a new mirror edit. Actual callback/lifetime traces remain acceptance data.

MVDM-HOST-DIV-340 registers relocation of the existing default-off window
callback diagnostics from wow32/wmdisp32.c to wow32-dll's existing
wow_user_thunk_scope owner. Keep the raw-procedure scalar capture and two
named scalar-only calls around unchanged CallBack16/BlockWOWIdle. Preserve
the existing environment opt-in, record text/order and LastError on enabled,
disabled and failed-output paths. The same registration marks wkfileio.c's
existing owned file-open-failure trace; it does not change open/error policy.
No callback dispatcher, guest-memory reader or new tracing subsystem is added.
Original pinned owners: OpenNT base/mvdm/wow32/{wmdisp32.c,wkfileio.c}.
Budget: declaration include, scalar capture and two calls instead of the two
embedded diagnostic bodies; existing file trace call unchanged. Focused
actual-owner trace tests and real Win16 callbacks plus grouped regression
are required before delivery. This registration precedes the relocation.

MVDM-HOST-DIV-341 retrospectively registers the already selected GDI handle
boundary in wow32/walias.c/h. Original ENTRYWOW/GdiQueryTable shared state is
unavailable: T422 E8 records native GdiQueryTable returning NULL for a real
bitmap. Existing worker-owned wow_gdi_alias remains the one bounded carrier,
with native GDI still validating real resources; no second drawing engine,
private modern table reader or change to the existing alias policy. Retain
the public conversion/retirement seams and mark the omitted obsolete layout
locally. Pinned OpenNT walias.c/h and T422 E8/E9 retain source/positive and
negative evidence; this adds registration, not a new runtime implementation.

MVDM-HOST-DIV-342 retrospectively registers the one-word APIENTRY correction
in wow32/isvwow.h. Original wmmedia.c already defines WOW32ResolveMemory as
APIENTRY; the original default-/Gz build masked the incomplete declaration.
Keep explicit ABI for callers compiled with another default convention.
No function body or public signature changes. The unnecessary local
WOWGetVDMPointer declaration in wow32.h instead returns to byte-exact source:
ADAPTER-WIN32-042's existing declaration carrier recovers the exact original
public/sdk/inc/wownt32.h prototype. No new memory mapping implementation.

MVDM-HOST-DIV-339 reserves the S3 typed procedure-service correction at six
call sites in wow32/wuwind.c::WU32Get/SetWindowLong and
wow32/wuclass.c::WU32Get/SetClassLong, pinned to OpenNT
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. IsThunkWindowProc already identifies
the native-thunk versus real16:16 branches; preserve that fact at the host
service seam. Modern USER cannot execute an original WNDPROC_WOW word, so the
guest WND branch needs ADAPTER-WOW-072's native ABI entry, while the native WND
branch lets USER normalize its opaque CPD in one assignment. Owned CLS retains
original address-only normalization and zero-window unload restoration.
A DWORD-only
facade cannot recover this source intent after guest selectors and native
CPDs acquire colliding bit patterns. Keep original encoding, return/thunk,
WW/WC update and class-versus-existing-WND algorithms. The decoded result
also carries guest/native origin: an encoded selector7fff can have the same
FFFF high word as a native CPD. Four local return conversions therefore use
origin at both tagged/native and CPD selection, rather than reinterpreting
the decoded guest identity as CPD or a high native address as guest code.
Class setter failure
must stop before publishing its WOW words, matching the existing WND caller's
failure ordering. Budget: six service selections and local guards in these
two original host files; no guest edits. This is registered before those
mirror call sites are changed, not a claim that they are implemented or tested.

MVDM-HOST-DIV-338 (T447 S2, owner-selected thread idle observers):
wkman.c WK32WowSetIdleHook borrows the current lifecycle task's native observer
handle instead of creating a global Hook. W32FreeTask clears that intent but
does not unhook the borrowed observer; task lifecycle rollback/retirement owns
its single release. Preserve original TD registration and RET_FOREGROUNDIDLE
body. Original OpenNT wkman.c, kernel hooks.c and taskman.c are provenance:
NT4's private registering-process filter is unavailable in public modern USER.
Owner accepts thread-before-global ordering, not lost worker-wide coverage.
Owned lifecycle/idle bindings supply thread coverage and native chain forwarding.
The query follows taskman's selected PTI through its lifecycle native-thread ID
to that exact original TD; another task's `hIdleHook` cannot enable this task.
No guest changes; verification pending in the S2 scope audit. This supersedes
DIV-336's unchanged global-registration/cleanup statement only.

MVDM-HOST-DIV-337 (T447 S2, owner-approved original-host exception):
wow32/wow32.c W32Dispatch records when its original TD stack is established
and completes CallBack16's deferred CPU-stack restoration on handled exception
return. Original successful return, W32Exception disposition and CallBack16
remain intact; early setup failure does not read an uninitialized TD, and
CONTINUE_SEARCH/CONTINUE_EXECUTION never execute the handler body. Pinned
OpenNT base/mvdm/wow32/wow32.c and wcall16.c contain this non-fast-path gap;
no matching SoftPC correction was found. FastWOW is comparison evidence, not
a newly selected provider. Owner explicitly approves this minimal correction
and standing registration-first treatment of similar original-host defects;
see the [source policy](../../docs/etc/operations/policy/source-policy.md)
and the [S2 audit](../../docs/etc/evidence/m0-t447-s2-scope-freeze.md).
Added state is one BOOL, one establishment assignment and one guarded use
of the existing SETVDMSTACK macro. Guest unchanged. Runtime acceptance and
publication remain subject to the S2 gates, not implied by this registration.

MVDM-HOST-DIV-336 (T447 S2): rename only wkman.c's
WK32ForegroundIdleHook definition to WowOriginalForegroundIdleHook. Its
original body, registration, TD list and hIdleHook cleanup remain unchanged.
The owned wow_idle_hook_binding.c supplies the original entry name, enforcing
existing WOW task/borrow scopes and local registration before guest access.
Original taskman's IsHooked/xxxCallHook bindings now query that same original
TD registration and invoke this original callback instead of constant absence.
No second registry, scheduler, hook-server import or guest change. This
candidate requires ordering, taskman, guest and worker-isolation acceptance;
see the [S2 audit](../../docs/etc/evidence/m0-t447-s2-scope-freeze.md).

MVDM-HOST-DIV-335 (T447 S2): rename only wcommdlg.c's three native Hook
definitions to WowOriginalDialogProc, WowOriginalPrintSetupDialogProc and
WowOriginalFindReplaceDialogProc. Original declarations/registrations still
name WCD32 entries supplied by wow32-dll/wow_common_dialog_callback.c. Enter
the existing task/borrow scopes before guest access; preserve original message
conversion and resource cleanup. The modeless WM_DESTROY branch performs
resource cleanup, not an app Hook, and during task exit requires the explicit
resource-live phase at original SG_EXIT's task-specific pfnWOWCleanup.
No COMMDLG algorithm or resource registry is replaced. This unverified group
requires modal/modeless, ordinary/terminal and failure evidence before release.

MVDM-HOST-DIV-334 (T447 S2): rename only wow32/wkman.c's W32FreeTask
definition to WowOriginalFreeTask. Both original exit callers still call
W32FreeTask, supplied by wow32-dll/wow_task_cleanup_entry.c. This fallback
precedes the unchanged resource/timer/Hook cleanup for remote/startup paths.
Normal windows instead retire at existing SG_EXIT/pfnWOWCleanup, before
guest GlobalFreeAll; W32FreeTask alone proved too late for modeless resources.
Late session unbind cannot safely run guest-capable window destruction.
No guest change, copied task cleanup or new scheduler. The source trace and
rejected late-only ordering are in the
[S2 audit](../../docs/etc/evidence/m0-t447-s2-scope-freeze.md#a2a3-integration-review-resource-ordering-corrects-late-only-teardown).
Unverified candidate; original cleanup and normal/sibling exit tests required.

MVDM-HOST-DIV-333 (T447 S2): rename only wow32/wutmr.c's W32TimerFunc
definition to WowOriginalTimerFunc. All 27 original timer stubs retain their
W32TimerFunc calls and original identities, including WM_TIMER conversion
through TMR.dwTimerProc32. wow32-dll/wow_timer_callback.c supplies the scoped
entry and invokes the unchanged body. The existing task/borrow owners replace
the unavailable NT4 USER callback ownership, not timer policy or scheduling.
See the [A2 source review](../../docs/etc/evidence/m0-t447-s2-scope-freeze.md#a2-timer-entry-selection-implementation-admission).
This candidate requires grouped provider/guest validation before delivery.

MVDM-HOST-DIV-332 (T447 S2): rename only wow32/wowhooks.c's central
WU32StdHookProc definition to WowOriginalStdHookProc. All 32 original stubs
retain their original target name, now supplied by wow32-dll's
wow_hook_callback.c. The wrapper enters the existing task callback and USER
borrow scopes before the original body reads CURRENTPTD, converts messages or
allocates guest stack, and pairs their release through finally. Hook identity,
chain policy, parameter conversion and dispatch remain original. No global
preprocessor rename, copied original body, guest change or new scheduler.

MVDM-HOST-DIV-331 (T445 S7): owner explicitly approves the reviewed inline
`dos/command/cmdmisc.c::GetWowKernelCmdLine` capacity/retry repair. Before copying
the rebuilt environment, compare its byte count with CMDINFO.EnvSize; on shortage
free both temporary buffers and return the required size in AX with carry set.
Reject resize requests at or above the immutable guest's 32 KiB limit without
16-bit truncation. Leave first-call state unchanged until successful delivery.
This bounded approved inline body is an exception to the three-line insertion
rule, not permission for unrelated mirror expansion. Original guest tcode.asm
already owns expansion and retry; no guest patch or host-environment filtering.
See [writer proof and admission](../../docs/etc/evidence/m0-t445-s7-base-rpc-client-split.md#confirmed-environment-overwrite).

MVDM-HOST-DIV-327 (T430 S2): protected-mode CALL gate, outer RETF and
outer IRET install the new stack pointer with the original set_current_SP
helper after loading SS. This is the exact three-body correction already
present in sibling SoftPC ce5f53515d3e6ce0a64e66a5465aa7a8fbca00f7;
destination SS address width, not instruction operand width, selects SP/ESP.
No validation, privilege transition, parameter layout or VM-return policy
is replaced. [Source and profile evidence](../../docs/etc/evidence/m0-t430-s2-softpc-repair-gate.md).

MVDM-HOST-DIV-326 (T429 S5): software text mouse flush retains
original-owner `calc_update` and copies the completed frame. A dedicated
NTVDM publisher holds only the latest immutable state, compares the full
descriptor/payload with its last successful send, and caps ordinary sends at
50Hz with an event and one-shot timer. Idle state has no recurring timer.
Heartbeat/CCPU timers remain unchanged; the publisher never reads VGA memory
or CPU registers. Final block/resume and route changes quiesce copied sends
before the existing final-frame acknowledgement. Graphics invalidations use
the same complete-copy publisher when Window presentation is active.
T429 S6 relocates that project-owned copied-state engine to worker-base for
NTVDM and NTVWM; the VGA extraction and DIV-326 mirror hooks stay unchanged.
The [S5 evidence](../../docs/etc/evidence/m0-t429-s5-software-video-publication.md)
records supersession of the unpublished heartbeat/FIFO candidate and limits.

MVDM-HOST-DIV-325 (T429 S2): `softpc.new/base/ccpu386/c_main.c::DECODE`
compile-selects the project-added transition and four WOW instruction observers
with `MVDM_CCPU_DECODE_DIAGNOSTICS`. The normal CCPU40 graph does not define
it, so opcode fetch/dispatch incurs no observer call or replacement branch.
An explicit diagnostic compile retains the unchanged attribution block.
The block was added by project commit806598b9b1, not original OpenNT;
instruction execution, interrupt handling and coarse diagnostics are unchanged.
This removes diagnostic work rather than substituting a CPU algorithm.

MVDM-HOST-DIV-322 (T425 S8): CCPU Window presentation uses software
FULLSCREEN without enabling X86GFX/MONITOR or hardware BIOS/regen mapping.
The existing ntvdm-exe/softpc/mvdm_softpc_text_video.c adapter samples frontend
policy on the original video owner and requests the original stream transition.
nt_fulsc.c retains software VGA mode, font and CRTC initialization in both
routes; nt_graph.c retains software backing. DIV-326 restores text flush
extraction on the original owner and separates copied display transport;
ordinary video checks remain original, while guest cursor changes publish.
nt_event.c extracts and acknowledges final software text/cursor before release;
hardware desktop transitions stay excluded. nt_reset.c retains software idle
detection. Original VGA/painter/mouse algorithms remain at their owners; route
ENTER/LEAVE and CPU-owner handoff now call the original draw/undraw boundary;
the per-tick mouse_refresh_pointer compensation is removed from the current
implementation. Natural-path tests and final r046/r045 release identity are
tracked in the [S8 ledger](../../docs/etc/evidence/m0-t425-s8-worker-frame-deduplication.md).
The earlier r040 package remains recovery and owner-accepted performance evidence.

MVDM-HOST-DIV-321: `softpc.new/base/keymouse/keyba.c::Reset6805and8042`
retains the original 6805/8042 reset and delegates only stale host keyboard
IRQ retirement to `ntvdm-exe/softpc/mvdm_keyboard_reset.c`. The original
reset clears queued device bytes but not the standalone delayed IRQ/PIC state;
after a DOS/native handoff that pending IRQ can replay a returned key into
the next DOS command. The adapter clears the pending host IRQ carrier before
the next guest command. It does not own key translation, guest BIOS, or
Console input policy. [S16 evidence](../../docs/etc/evidence/m0-t423-s16-typeahead-handoff.md).

MVDM-HOST-DIV-211 (S11 candidate): the event loop again consumes batches of
five host records. Scan-less keyboard expansion is moved unchanged into the
worker's win32/console_compat.c boundary, with a forty-record stack result
for five worst-case eight-transition keys. Original dispatch, key history,
mouse IRQ/EOI and guest callbacks remain owners. The owner-added scheduling
repair removes the per-batch Sleep(10) packing delay and gives the original
auto-reset suspension event priority over queued input. The alertable wait
still blocks when idle; the original suspend/resume handshake is unchanged.
The companion
ReadConsoleInputExW binding no longer clamps consuming reads to one record.
See the [S11 ledger](../../docs/etc/evidence/m0-t423-s11-interaction-retirement.md).

MVDM-HOST-DIV-318 (retained mouse integration, T425 S8 route repair): original `nt_event.c`
dispatches copied Window-relative mouse records through the worker-owned
bridge; its existing EOI/pending and cancellation boundaries include that
queue. `nt_mouse.c` retains original absolute input as fallback and delegates
only relative ingress conversion to `ntvdm-exe/softpc/mvdm_softpc_mouse_guest.c`.
Original EmulateCoordinates, limits and callback masks remain owners.
The attempted MONITOR/X86GFX cursor hooks were removed after final-link and
source review disproved their availability in this CCPU40 composition.
The reviewed 286d54a3 reference's non-MONITOR `mouse_io.c` software cursor
count, shape/hotspot, geometry, draw/erase and saved-background paths are now
selected in the candidate. S8 replaces periodic route refresh with consumed
ENTER/LEAVE and original CPU-owner block edges calling a three-statement
mouse_pointer_route_changed wrapper over original cursor_update/display/undisplay.
No synthetic motion or callback mask is produced for a stationary route edge.
An ICA-protected worker route snapshot replaces the old presentation binding.
Real guest cursor count, text/graphics draw/erase and reset/position/input
integration retain their S7 evidence; S8 additionally tests actual transmitted
stationary ENTER/LEAVE frames and original CRTC-only changes. Software
FULLSCREEN is separate from excluded hardware takeover; no shared library changes.
Frontend/native handoff, real guest callbacks and Console mouse regression pass;
see the [S7 ledger](../../docs/etc/evidence/m0-t423-s7-window-mouse.md).

MVDM-HOST-DIV-317: `softpc.new/host/src/nt_event.c` retains raw-record
identity through keyboard dispatch and selects pending original history ages
instead of interpreting device-packet count as raw-record count.
`softpc.new/base/keymouse/keyba.c` carries those origins beside existing
6805, held, translation and 8042 slots, with source-shaped reset/EOI retirement.
Internal responses carry no raw origin. The existing KEY_EVENT_RECORD ring,
device bytes, scan translation, locking, BIOS join and Console return remain
owners; bounded metadata arithmetic lives in ntvdm-exe/softpc. No guest,
frontend, broker or CCPU instruction logic changes. This candidate is not yet
accepted: actual metadata lifecycle and negative/real regression gates are
recorded in the [S6 ledger](../../docs/etc/evidence/m0-t423-s6-window-display.md).

MVDM-HOST-DIV-316: `softpc.new/host/src/nt_event.c` joins its original
hardware-history and BIOS returns without synthesizing an additional release
when the first successfully returned hardware event is already the release of
the newest BIOS key. Scan, VK and extended identity must match. Failed/empty
returns, different keys and scanless BIOS sequences keep the original pair.
The one-record boundary identity resets at every handoff and after the newest
BIOS entry; no frontend filter, device policy or guest change is introduced.
Both original return algorithms remain in their original owner. This bounded
host correction is under verification, not delivered; see the
[S6 ledger](../../docs/etc/evidence/m0-t423-s6-window-display.md).

MVDM-HOST-DIV-315: `softpc.new/base/keymouse/keyba.c::keys_in_6805_buff`
restores the `else` accidentally embedded in the original single-byte comment.
Complete single-byte scan sequences must not leave a pending multi-byte marker:
otherwise four unread events count as one and history return drops records.
It also counts complete single-byte events after a partially consumed
multi-byte sequence while preserving the partial-transfer flag; an unmatched
earlier marker must not swallow those independent complete events.
The original ring, history, reset and guest keyboard semantics remain owners;
no new keyboard provider or CPU change is introduced. The extracted-original
x86 test and real integration status are in the
[S6 ledger](../../docs/etc/evidence/m0-t423-s6-window-display.md).

MVDM-HOST-DIV-314: `softpc.new/host/src/nt_graph.c` copies resolved text
colours and completed original refreshes to the independent frontend's
authenticated channel. STREAM_IO asks only whether frontend policy requires
text frames, then uses original `disable_stream_io`; no new mode transition,
guest decoder, font loader or worker UI is introduced. Original update,
palette resolution, cursor batching and error reporting retain their owner.
The added publication trigger follows original `nt_start_update`, not mere
return from `calc_update`: an idle dirty check does not publish a full frame.
Its notification survives between ticks and is consumed on the video owner.
T444 S1's required-regression repair checks the original `CNTRL_VDMBLOCKED`
bit before the added CCPU software-video tick. A queued CPU timer event can
arrive after COMMAND releases its frontend but before its next command BOP;
it must retain dirty/cursor/mode state instead of publishing to a closed pipe.
Original final paint precedes setting that bit, and resume clears it after
reacquisition. This fixes the project-added publisher's integration, not
the original timer's asynchronous suspension contract. No new state or
transport-error suppression is introduced. See the
[T444 S1 evidence](../../docs/etc/evidence/m0-t444-s1-native-indexed16-palette-projection.md).
Software-fullscreen mouse flushes leave VGA dirt for the existing video tick;
they do not execute synchronous copied-frame transport in the guest IRQ.
S12 also copies the resolved text configuration after the original periodic
stream flush, without requesting video mode. This lets NTVWM inherit the
current fonts/palette without changing the DOS stream/cursor path. Explicit
configuration and frame messages are no longer filtered by the adapter. See the
[S12 ledger](../../docs/etc/evidence/m0-t423-s12-ntw32-backend.md).
The bounded copy/packing is in `ntvdm-exe/{softpc,win32}`; frontend owns all
rendering. Source recovery, tests and pending real-guest gates are recorded
in the [S6 ledger](../../docs/etc/evidence/m0-t423-s6-window-display.md).

MVDM-HOST-DIV-313: `softpc.new/host/src/nt_event.c` acknowledges its original
block/resume boundaries to the independent frontend. The block notification
follows original final painting, unused-key return, mode restoration and timer
stop; resume acquires presentation/input before original Console queries and
event-thread release. `ntvdm-exe/win32/console_client.c` carries that finite
notification; `ntcon-exe` serializes native and DOS I/O and returns unread hidden
Console records through the original-shaped native prepend operation. The NT4
shared visible Console did not require a cross-process presentation binding;
the owner-admitted independent frontend split does. No guest, original scheduler,
reentry count or task completion policy changes. S4's actual mixed-chain and
lifecycle verification, and its publication state, are recorded in the
[S3/S4 ledger](../../docs/etc/evidence/m0-t423-s3-hidden-console-ledger.md).

MVDM-HOST-DIV-312: `softpc.new/host/src/nt_event.c::ReturnUnusedKeyEvents`
returns the initialized history slice with its actual count and original
oldest-first ordering, bounded by the existing MAX_KEY_EVENTS array. OpenNT's
loop counter is one beyond the number copied; passing it exposes an extra
uninitialized record. Short history also occupies the array tail rather than
the prefix. The original history lookup, Console prepend and cleanup remain
the owners; no guest change, new queue or adapter-side filtering is introduced.
The source-body test in tests/app/unused_key_return_test.c covers full, partial,
empty and capacity-limited batches; see the
[S2 ledger](../../docs/etc/evidence/m0-t423-s2-console-boundary-ledger.md).
Real nested typeahead and DOS17 regressions pass; the ledger preserves the
earlier intermittent failure and the limits of causal attribution.

MVDM-HOST-DIV-311: `softpc.new/host/src/nt_event.c::nt_block_event_thread`
finishes pending original video-mode selection before its existing final
`update_alg.calc_update` at the stopped-guest handoff. `nt_graph.c` defers
selection by `nt_ega.h::EGA_TICK_DELAY`; a task can complete with that work
pending and otherwise paint through the previous graphics painter. The bounded
loop calls the unchanged original video tick, not a guest timer/interrupt,
and adds no scheduler, sleep, replacement painter or frontend policy.
Source/caller evidence and current verification are recorded in the
[S2 ledger](../../docs/etc/evidence/m0-t423-s2-console-boundary-ledger.md).
This change remains under validation, not a claimed delivery.

MVDM-HOST-DIV-310 (T422 S2, unaccepted): the CPU40 FastBOP carrier binds the
three original native-monitor far-call shapes (inline service, WOW dispatch,
callback return). `dpmi32/modesw.c` also publishes the original `RM_BIT_MASK`
contract through SAS, following `dpmi32/i386/dpmi386.c`, so real-mode callers
retain the original slow BOP fallback. The native monitor's host CS:IP and
native context exchange cannot execute in CCPU. The selector reservation and
complete call/return lifecycle remain under verification; this is not a
completed FastWOW or WOW32 claim. The focused mock mode-boundary fixture
passes and the real guest reaches FlatAddressArray and INT 2Ah publication.
The INT 2Ah carrier now also updates native `FlatAddress` using the original
`GetDescriptorMapping`/IntelBase conversion; the PMODE32 guest view contains
descriptor-derived guest-linear bases, never copied native pointers. Normal
publication, DIB alias retention/invalidation and guest-view isolation pass
focused tests. Rejected-entry guest-write ordering and full callback closure
remain unaccepted.

MVDM-HOST-DIV-300: establish the finite worker-owned paging and descriptor
domain required by the immutable PMODE32 USER client after source observation
proved that the normal DOSX transition reaches WOW with no active CR3.  Original NT paging,
shared-user-data and per-thread TEB publication were kernel context-switch
services, not an MVDM algorithm that can be imported into the standalone
worker.  The binding therefore uses the existing original `VdmAllocateVirtualMemory`
and `VdmAddVirtualMemory` carriers plus unchanged CPU40 CR0/CR3 mechanics to
identity-map the active SAS, publish a read-only clock at `7FFE0000h`, and
withdraw PG at the original DPMI real-mode transition.  It copies the
source-published DOSX GDT, leaving it untouched, and overlays only `KGDT_R3_DATA`
and the current worker's one-page guest TEB in the selected WOW descriptor
view.  The view starts with source-defined empty `SHAREDINFO` pointers and
`TEB+18h` self only; it publishes no host pointer, USER object, handle, task,
desktop or callback state.  Original USER producers and the 21/20 registration
contract remain S42--S44 owners.  See the S41 registration-frontier evidence
and the worker-domain lifecycle fixture.

S42 also binds `wuman.c::WU32NotifyWow`'s original `CallCsrFlag` byte to
that worker domain. USER16's original client macros read, set and clear it;
no native WOW32 static address may be published into the CCPU address space.
The byte remains zero-initialized, per-worker and withdrawn at teardown;
the original client decision logic is unchanged.

MVDM-HOST-DIV-299 is withdrawn.  The former experiment installed a private
CCPU page directory before original `MS_bop_1`; it was removed because DOSX
owns the active CR3/page-table lifecycle and the experiment was not a valid
production carrier.  The subsequent S41 transition observation instead proved
that normal DOSX transitions never create CR3; DIV-300 supersedes the retired
assumption by creating a bounded worker-owned domain only at the reached WOW
boundary, after DOSX/XMS has disabled SAS A20 wrapping.  No USER shared object, host
pointer publication, handle, callback or task policy is claimed by the retired
experiment.

MVDM-HOST-DIV-298: the provider precompiled-header binding redirects original
WU32LoadMenuIndirect's call to ADAPTER-WOW-037, which invokes the recovered
OpenNT menu parser with explicit Win16 template semantics. ConvertMenu16
and the entire original thunk remain unchanged. Native LoadMenuIndirectW
misinterprets packed guest owner-draw pointers as strings; the retained
negative-before/positive-after test proves the binding error. This is not a
guest repair. Actual guest menu callback acceptance remains open.

MVDM-HOST-DIV-297: the final provider precompiled-header binding maps the
four ANSI window/class long APIs to ADAPTER-WOW-033. Only private WW/WC
indices use scoped native storage; ordinary indices still call USER32. Original
FindPWW/FindPWC and SETWL/SETWC bodies are unchanged. Private setter branches
return the previous stored word rather than the original uninitialized dwOld;
selected SETWL/SETWC uses discard it. This does not restore GWL/GCL_WNDPROC
conversion or register the full USER output table. Formal x86 and shared-word
lifetime/read/write fixtures pass; real guest acceptance remains open.

MVDM-HOST-DIV-296: `wow32/wow32.c` routes the two original LPFNW32 calls
in W32Dispatch/W32TryCall through the ADAPTER-WOW-032 synchronous borrow scope.
Original function selection, frame handling, return value and outer exception
policy remain unchanged. The adapter uses the original LPFNW32/PVDMFRAME types
and releases native USER aliases in finally, including recursive calls. No
FastWOW assembly path is selected by this CCPU provider. Formal x86 and typed
nested/exception tests pass; private USER API redirection remains incomplete.

MVDM-HOST-DIV-295: `wow32/wuser.c::WU32LoadBitmap` retains original
GETMISCPTR demand loading, output-table dispatch and original pointer cleanup.
The resolved bitmap payload is replaced only for the synchronous USER call
by the existing explicit-protected read lease with the original DWORD resource
size, then released without writes. Lease failure returns the original zero
result without entering USER. No resource decoder or loading policy is added.
The extracted actual thunk passes success, failed acquisition and null-payload
ordering tests; real guest consumption and output registration remain open.

MVDM-HOST-DIV-294: the worker import declarations in `inc/softpc.h`,
`inc/{dbgexp,demexp,memapi,wowmmcb}.h` and `wow32/wkman.h` explicitly retain
cdecl for existing CPU40 worker exports while WOW32 recovers its original
stdcall compilation default. Types, bodies and source ownership are unchanged.
The original x86 monitor did not require these emulated register calls; the
standalone CPU40 worker does. This is an ABI boundary, not missing function
implementation. No stdcall-to-cdecl symbol aliases or runtime wrappers are used.
`wow32/wdos.c` uses `demexp.h` instead of duplicating its error API prototype.
See S40's package calling-convention and worker import evidence.

MVDM-HOST-DIV-292/293 are retired: restoring the original WOW32 `/Gz` build
eliminates the window callback bridge and explicit dialog WINAPI edits.
Original callback assignments and dialog declarations/bodies are restored.
The earlier `/Gd` negative evidence remains in the
[S40 evidence](../../docs/etc/evidence/m0-t420-s40-wow-user-profile.md).

MVDM-HOST-DIV-291: retail `wow32/wow32.c::W32Init` uses the original
absent-value ThunkNLS default without requiring the NT4 machine-global WOW
registry key. DEBUG source retains its original registry parser and close;
it is not the selected retail product profile. The retail KnownDLL list is
currently empty, and its real loader consequences remain S40 acceptance work.
No host registry or guest file is modified. Original default computation and
provider initialization order remain. The
[S40 evidence](../../docs/etc/evidence/m0-t420-s40-wow-user-profile.md)
records x86 compilation, limits and source-diff minimization.

MVDM-HOST-DIV-290: `dpmi32/int21map.c` routes extended error (59h)
through original `ReturnESBX` segment conversion, selecting DI for this
documented ES:DI result instead of BX. Original DOS error fields and
selector allocation remain owned by the original providers. S38 records
the pre-fix loss of the returned segment during saved-ES restoration.

MVDM-HOST-DIV-289: `dpmi32/int21map.c::LoadExec` returns the eight-byte
EXEC1 stack/entry result on successful AL=1, following original
`dpmi/486/dxmain.asm::dosex4`. It does not change loading, failure, PSP or
environment policy. The [S38 evidence](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md)
records the reproduced omission; load-only lifecycle acceptance is pending.

MVDM-HOST-DIV-288: `dpmi32/int21map.c::IOCTLBlockDevs` maps the original
DEM media-ID (46/66, packed MID=25 bytes) and access-flag (47/67,
ACCESSCTRL=2 bytes) pairs instead of forwarding protected DS:DX untranslated.
Original DEM, device policy, buffer and return owners are unchanged. The
[S38 evidence](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md)
separates actual read-only volume queries from controlled write-side tests.

MVDM-HOST-DIV-287: `dpmi32/int21map.c::IOCTLBlockDevs` supplies the
missing format-track 42h length case: five bytes, or seven with the original
A_FORMATPACKET track-count bit. The dispatch previously admitted the call
but used uninitialized Length. Original mapping and device owners remain;
no physical format is used in the controlled
[S38 verification](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md).

MVDM-HOST-DIV-286: `dpmi32/int21map.c::IoctlReadWriteTrack` reads the
32-bit client's selector from its original ParameterBlock, not beyond the
13-byte real-mode packet copy. Original transfer offset, DOS packet size,
chunking and buffer owners remain unchanged. The
[S38 evidence](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md)
distinguishes controlled device-boundary proof from real-medium acceptance.

MVDM-HOST-DIV-285: `dpmi32/int21map.c::IOCTLMap2Bytes` adds the original
client DX/EDX offset to the selector base, matching adjacent MapDSDXLenCX.
The original two-byte IOCTL mapping otherwise reads/writes DS:0 instead of
the requested buffer. Original driver, bounce-buffer and return owners stay
unchanged; no guest media change. The
[S38 evidence](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md)
records the real PRN query failure and candidate verification.

MVDM-HOST-DIV-284: `dpmi32/int21map.c::NoTranslation` preserves the original
DOS returned ZF for nonblocking input (06/DL=FF and 0C06/DL=FF). The common
CF-only return helper discarded that output bit. Other calls retain their
original flag behavior; original DOS owns polling, input and queue policy.
The [S38 evidence](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md)
records the before/after FLAGS witness and verification state.

MVDM-HOST-DIV-283: `dpmi32/int21map.c::DpmiXlatInt21Call` selects existing
original `ReturnDSSI` for INT21/6300 in the non-DBCS product as well. Original
DOS implements that query in both profiles; the non-DBCS dispatch fallback
restored client DS and corrupted its returned pointer. Other subfunctions
retain original dispatch. No new provider, DBCS profile or guest mutation.
Focused and product acceptance are recorded in the
[S38 evidence](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md).

MVDM-HOST-DIV-282: existing CCPU `c_debug.c/.h`, `c_main.c` and `c_tsksw.c`
separate newly matched debug events from sticky DR6 status. Original data,
instruction and T-bit matching schedules the event; exception continuation
consumes/cancels it and CPU reset clears it, without clearing guest DR6.
Original match tables, access conditions and exception-frame owners remain.
This S38 dependency is experimental until the complete mechanical groups
and real DPMI/product regressions in the S38 evidence pass.

MVDM-HOST-DIV-281: CPU40 composes only the original debug-register service
from existing `dpmi32/i386/dpmi386.c`, replacing its no-op BOP mapping in
`dpmidata.h`. The guest read spans six DWORDs and rejects null before the
original rollback. Worker `ThreadSetDebugContext` binds to original MOV_DR,
not host thread debug registers. Kernel LDT/fast-BOP bodies stay excluded.
[S38 recovery and verification](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md).

MVDM-HOST-DIV-280: `dpmi32/dpmiint.c::DpmiUnhandledExceptionHandler`
places EFLAGS at byte 8 of its original DWORD IRET frame, preserving the
CS at byte 4. Both pinned OpenNT editions write flags over CS; this is
an original-owner layout correction, not a replacement provider. S38
records source comparison and runtime verification status in its
[evidence](../../docs/etc/evidence/m0-t420-s38-dpmi32-capability-review.md).

MVDM-HOST-DIV-279: `dos/dem/demgset.c::GetMediaId` corrects the original
filesystem padding loop's destination field. Its source comment requires
NUL-to-space padding in both fixed-width strings; writing VolumeID leaves
filesystem NULs and can overwrite the label. Real INT21/6900 output reproduced
the missing padding. Keep the original owner, layout, query and failure flow;
no new provider or guest change. See the S37 OEMUNI capability evidence.

MVDM-HOST-DIV-278: `softpc.new/host/src/nt_pif.c` keeps original OEM PIF
expansion, then binds its result to the ANSI configuration-file consumers
through the bounded worker-local media-path conversion. The default ANSI
path is unchanged. `nt_msscs.c` supplies the documented MAX_PATH+12 output
capacity instead of MAX_PATH. No new file or provider enters the mirror.
[S37 evidence](../../docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md).

MVDM-HOST-DIV-277: `oemuni/process.c::GetEnvironmentVariableOem` saturates
the original ANSI/OEM descriptor capacity rather than wrapping at 65536,
checks completion against the allocated capacity, preserves empty output,
stops at the first failed conversion and returns the converted OEM length.
Original BaseClient `base/win32/client/process.c::GetEnvironmentVariableA`
provides the capacity-clamping and converted-length precedent; no new
environment provider is introduced. A complete ANSI query now precedes OEM
byte sizing, so short queries return required OEM capacity rather than ANSI
length. Growth, disappearance and allocation/size failure keep bounded cleanup.
[S37 evidence](../../docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md).

MVDM-HOST-DIV-276: OEM short paths use complete Unicode queries and actual
OEM capacity/length, retaining zero on conversion failure through finally.
Computer names use the original documented maximum Unicode name size and
report OEM conversion failure/required bytes instead of false success.
[S37 evidence](../../docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md).

MVDM-HOST-DIV-275: `oemuni/{file.c,process.c}` bounds current/search/system/
Windows/temp directory conversion by the actual caller capacity and computes
required OEM bytes before conversion. Search returns converted byte length
and prefix offset. Original owner conversion and resource cleanup remain.
Volume-name and filesystem-name conversion likewise retain actual capacity;
original conversion failure and finally cleanup handle insufficient space.
Directory/search temporary Unicode storage is sized by its own full query,
not the OEM caller's capacity. Query growth/failure and unrepresentable
lengths fail before conversion; representable output capacity is saturated
at the original USHORT limit instead of wrapping.
[S37 evidence](../../docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md).

MVDM-HOST-DIV-274: `oemuni/file.c::GetFullPathNameOem` applies the original

MVDM-HOST-DIV-274 (mouse cursor projection): the original transformed
virtual mouse position is also published through the worker WOW page-domain
cursor carrier at the existing mouse-input boundary. This is a separate
behavior from the same-numbered OEM path conversion above; input/callback
and projection lifetime coverage remains subject to the S10 mouse audit.
BaseClient curdir.c DBCS byte-count correction using the existing OEM RTL
size interface, including the file-part prefix. An oversized Unicode result
is not read as a completed path. No new conversion provider is introduced.
[S37 evidence](../../docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md).

MVDM-HOST-DIV-273: `oemuni/process.c::ExpandEnvironmentStringsOem` publishes
the original conversion's OEM result instead of leaving ANSI bytes in the
caller buffer, uses the converted byte count, and separates a borrowed output
pointer from cleanup-owned allocations. Conversion failure returns zero,
rather than the preceding ANSI call's successful length.
[S37 evidence](../../docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md).

MVDM-HOST-DIV-272: `dpmi32/xmem.c::DpmiReallocateXmem` tests the original
BOOL return from DpmiReallocateVirtualMemory as BOOL, not NTSTATUS. This
prevents failed growth from being reported as success and replacing the
still-valid allocation record with address zero. Both original backends
and the original declaration return BOOL.
[S36 evidence](../../docs/etc/evidence/m0-t420-s36-suballoc-capability-review.md).

MVDM-HOST-DIV-271: `xms.486/xmsmemr.c` restores original SAS commit/decommit
callbacks for preallocated CCPU40 backing. Only its flat-base mover calls
the existing bounded mapping-aware worker operation; original signatures,
overlap behavior and cancellation on unavailable movement remain intact.
No zero-on-free policy or replacement allocator is introduced.
[S36 evidence](../../docs/etc/evidence/m0-t420-s36-suballoc-capability-review.md).

MVDM-HOST-DIV-268: `softpc.new/base/ccpu386/c_xcptn.c` applies the five
NTVDMx64 PATCH-007 brace corrections, keeping flag cleanup and immediate CPU
continuation inside the successful protected-mode exception-hook condition.
Unhandled and real-mode exceptions reach original `do_intrupt`; no handler,
stack algorithm, API, opcode or overlay changes. The selected originals have
the same lexical defect and an adapter cannot correct it. Provenance and
focused/native verification: [S5 evidence](../../docs/etc/evidence/m0-t413-s5-exception-scope.md).

MVDM-HOST-DIV-269: `oemuni/{file.c,process.c}` initializes only local
failure sentinels (`ReturnValue`, `FilePart`, `UString` and `lpDstW`) where
original SEH cleanup returns through an otherwise uninitialized local. This
preserves every original conversion and Win32 success route, while ensuring
that invalid input/allocation failure returns the documented zero/FALSE result
without freeing or publishing an indeterminate pointer. Focused source-native
verification: [S17 evidence](../../docs/etc/evidence/m0-t420-s17-oemuni-package-recovery.md).

MVDM-HOST-DIV-270: `softpc.new/host/inc/cfpu_def.h` uses the MSVC x86
floating-point status API for the three guest-visible sticky exception queries,
then clears the x87 status and restores the historical all-masked host control
word.  The OpenNT `_controlfp(0, ...)` spelling reads/changes modern MSVC's
mask control rather than sticky status; a rounding `FISTP` consequently
unmasked host inexact and terminated the worker.  This is the individually
reproduced x86 correction in the comparative NTVDMx64 CCPU patch, not a guest
algorithm replacement.  S21's real `FPU64`, `FPUI32`, `FPUCW`, and `FPUSW`
guest cases cover rounded 64/32-bit stores, initial `037Fh` control word, and
the guest-visible masked precision status bit.

T413 S4 narrows MVDM-HOST-DIV-156: base/cvidc/evidgen.h retains its original
full VideoVector layout and latch/selector macros, but excludes its 76 older
field-dispatch macros under CPU_40_STYLE. Original base/inc/egacpu.h already
declares the exact typed cpu/src/evid/vglob.c interfaces. This seven-line
header seam removes the overlay accessor rebinding and 20 generated wrappers;
it does not add a field algorithm or change the original generated renderer.
The full table cannot simply use the five-slot CCPU header: its latch setter
has a different slot offset. The original C-VID GDP profile remains selected.
The c_main.c instruction-delta cohort is restored to original expressions,
including masking the complete USE16 next-IP sum; no replacement helper remains.

MVDM-HOST-DIV-VDM-TIB-STORAGE: `softpc.new/host/src/stubs.c` omits the
non-MONITOR four-byte `VdmTib` placeholder. The selected original printer
carrier requires the full original `VDM_TIB`, defined by
`ntvdm/monitor/mvdm_vdm_tib.c` and bound at worker startup.
Keeping the placeholder can satisfy static-library resolution without loading
the real storage, turning original printer-field stores into global corruption.

Temporary observation register: MVDM-HOST-DIV-266 adds scalar-only hooks in
softpc.new/host/src/{config.c,nt_bop.c} and softpc.new/base/bios/emm_fncs.c for EMS size,
initialization result, request/result registers and original WOW loader failures. The existing physical
mapping adapter owns bounded first-hit output; original branches, register
results and dispatch are unchanged. This owner-authorized T406 S1 instrument
is not recovered behavior and is removed after upstream coverage is resolved.

Original-relative MVDM host mirror. M0 T311 re-rooted original support
carriers (`dirs`, `makefil0`, `inc`, `oemuni`, `suballoc`) here, beside their
original MVDM host consumers. Standalone tools and selected firmware inputs
remain in their own mirror components. This root has no locally authored
provider replacement.

## Provenance

- [S1 canonical union ledger](../../docs/etc/operations/m0-t274-s1-non-guest-mvdm-union-ledger.tsv)
  selects one source for every host path.
- [S5 topology manifest](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-manifest.tsv)
  records all 1,689 host rows, including imported/deferred disposition, source
  path and SHA-256.
- [S5 closure evidence](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-closure-001.md)
  records the topology/body boundary.
- [S6 host-input manifest](../../docs/etc/operations/m0-t274-s6-mvdm-host-input-manifest.tsv)
  records the 639 S5-deferred selections and their copied-file hashes.
- [S6 closure evidence](../../docs/etc/operations/m0-t274-s6-mvdm-host-package-supply-closure-001.md)
  records full selected-host coverage and verification.

The root carries the current host-owned selection described by the topology
manifests. T310/S4 reclassified executable `softpc.new/base/bios` and
`softpc.new/base/keymouse` packages from the firmware-input component back to
this host mirror: their 24 source/makefile/manifest files are byte-exact
selected-OpenNT mirrors; their 23 historical `obj/` products have no selected
source-root counterpart and are retained as byte-identical project evidence.
Reset, BIOS services and controller algorithms are original SoftPC runtime
code. This physical layout is not by itself a runtime-enablement claim.

T301 S1 P21 additionally preserves missing byte-exact historical build
products from `opennt-src-2` under their original-relative host paths.  These
files are source/product evidence only: they are not host build, link or
runtime-discovery inputs.  The complete per-file provenance is in
[`m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv`](../../docs/etc/operations/m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv).

## Divergence register

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| MVDM-HOST-DIV-463 | Preserve original Win16 asynchronous database-query ownership through cancellation and task rundown. | The selected original WOW32 source returns `WSAEINVAL` from `WSACancelAsyncRequest`, records no creator task serial and tests the wrong allocation result in all six constructors.  A completion after a task exits can otherwise retain/follow former guest-buffer identity. | Add the creator serial to the existing async context, retain the original list/post conversion owners, cancel/detach exactly once on explicit cancellation or task cleanup, reject only stale provider completions, and test the existing allocation's actual buffer result.  No provider, scheduler, callback carrier, guest image or new registry is added. | `wow32/{wsdata,wsext,winsockp}.c`; [T467 S3 evidence](../../docs/etc/evidence/m0-t467-s3-winsock-async-lifetime.md) |
| MVDM-HOST-DIV-462 | Restore a task's default Win16 Winsock blocking hook after a nonblocking operation. | Original `WWS32WSAUnhookBlockingHook` tests the non-null native `WSAIsBlocking` function pointer rather than calling it, so every initialized provider falsely returns `WSAEINPROGRESS`. | Call the same selected WSOCK32 predicate with its original no-argument ABI; preserve original task-TLS hook state, error return and all callback ownership. | `wow32/wsext.c`; [T467 S2 evidence](../../docs/etc/evidence/m0-t467-s2-synchronous-winsock-bridge.md) |
| MVDM-HOST-DIV-461 | Retire a single 8042 scan-output slot after its normal delivery. | The original NT path treats PIC EOI as if port 60 had already been read; early EOI by VKD/VPICD overwrites the unread byte. | Retain two per-slot observations and run the existing `continue_output` only when both the actual port-60 read and the normal EOI occur. Status reads and reset cancellation are not data consumption/normal EOI. | `softpc.new/base/keymouse/keyba.c`; [T460 S3 keyboard plan](../../docs/etc/evidence/m0-t460-s3-keyboard-early-eoi-plan.md) |
| MVDM-HOST-DIV-320 | Overlay a word with named low/high bytes for original port I/O and device registers. | The original non-CPU30 union always orders high then low; selected x86 CCPU40 needs host-little-endian layout when original ios.c assigns X then reads byte.low/high. An external facade cannot change this C union layout. | Owner-supplied LITTLEND conditional selects low then high, retaining the other layout and original algorithms. Scalar and repeated-word original I/O dispatch is covered by ccpu_bounded_execution_fixture; no CPU30 profile is enabled. | `softpc.new/host/inc/host_cpu.h`; [S9 evidence](../../docs/etc/evidence/m0-t423-s9-conpty-migration.md) |
| MVDM-HOST-DIV-321 | Retire stale host keyboard IRQ at original 6805/8042 reset. | Original reset clears the device queue but the standalone delayed IRQ/PIC request remains; on DOS/native return the same key can re-enter the guest after it was returned to native input. | A one-call mirror hook delegates retirement of the host delayed/PIC request only when the 8042 output slot carries a user-key origin; device-internal responses, original keyboard flags, scan, BIOS and command semantics remain unchanged. | `softpc.new/base/keymouse/keyba.c`; `ntvdm-exe/softpc/mvdm_keyboard_reset.c`; [S16 evidence](../../docs/etc/evidence/m0-t423-s16-typeahead-handoff.md) |
| MVDM-HOST-DIV-319 | Complete redundant shared WOWEXEC without starting another task. | Standalone asynchronous launch needs to distinguish this original successful no-op from failed initialization; ExitVDM alone carries no reason. | One call before the unchanged original ExitVDM publishes the authenticated startup acknowledgement through the DLL binding; original classification, completion and error cleanup remain owned here. | `wow32/wkman.c`; `../wow32-dll/source/wow_user_registration_bridge.c`; [S8 evidence](../../docs/etc/evidence/m0-t423-s8-gui-launch-wait.md) |

### Re-rooted MVDM support declarations

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| MVDM-SUPPORT-DIV-001 | Model `DEMEXTERR.ExtendedErrorPointer` as a 32-bit field in the packed DOS data record. | The original x86 `PUCHAR` happens to be 32 bits; on x64 it widens and changes the guest ABI. | Preserve the source record's nine-byte packed layout by storing the guest pointer numerically as `ULONG`. | `inc/dossvc.h` |
| MVDM-SUPPORT-DIV-005 | Declare the internal MVDM hardware/software/fault interrupt registration callbacks. | The original public declarations carried function pointers as `PVOID`, relying on an invalid function/data-pointer conversion that has no safe C ABI meaning. | Retain the original registration entrypoint names and callback parameters/results, but publish three named `int`-returning function-pointer types. | `inc/intapi.h`; `softpc.new/host/inc/nt_inthk.h`; `softpc.new/host/src/nt_inthk.c` |
| MVDM-SUPPORT-DIV-006 | Select original fetch/store macros. | MSVC defines _M_IX86 rather than the historical i386 spelling. | Extend only the original condition; retain both original branches. The x86 product selects direct accesses. | `inc/mvdm.h` |
| MVDM-SUPPORT-DIV-007 | Select original suballocation granularity. | MSVC defines _M_IX86 rather than i386. | Extend only the original condition and retain both branches; selected x86 remains 4096 bytes. | `inc/suballoc.h` |
| MVDM-SUPPORT-DIV-008 | Include the original VDM_TIB declaration through an absolute NT source-tree path. | The historical path cannot resolve outside the OpenNT build tree. | Resolve the selected identical `vdm.h` declaration through the mirrored OpenNT ABI include root; no type, field or ABI is recreated. | `inc/vdmtib.h` |

### Existing MVDM host divergences

DIV-326 uses only minimal nt_graph.c flush and nt_event.c block/resume hooks;
its new cadence, copied state and synchronization live in ntvdm-exe adapters.
The original clock, VGA/mouse algorithms and NTCON protocol are unchanged.

T427 restores `cmdredir.c`'s original `GetWindowsDirectory` temporary-file
fallback (OpenNT `base/mvdm/dos/command/cmdredir.c`): this is host scratch space,
not guest media. The previous product-root replacement and its now-unused
firmware include are removed; original retry/failure order is unchanged.

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| MVDM-HOST-DIV-323 | SafeLoadLibrary preserves original FPU save/load/restore around DLL loading. | Exact internal WOW32/VDMREDIR providers must come from the current product root, not CWD/PATH. | Delegate only the load expression to `mvdm_softpc_load_library` in the existing firmware adapter; common root joins internal names, arbitrary VDD names/explicit paths retain LoadLibraryA. No guest or general host DLL policy change. | `softpc.new/host/src/nt_bop.c` |
| MVDM-HOST-DIV-324 | Preserve GetWowKernelCmdLine environment construction and guest-copy order. | KRNL386 takes SYSTEMROOT from the boot PDB before the submitted Win16 task record; the native worker must retain host SYSTEMROOT. | After the WOW boot guest MULTI_SZ is complete, project its SYSTEMROOT; the existing firmware adapter uses common's bounded projection and returns CRT-owned bytes. Native environment and lpszzVDMEnv32 remain unchanged; original error/termination shape is retained. No immutable guest or scheduler change. | `dos/command/cmdmisc.c`; `../ntvdm-exe/softpc/mvdm_softpc_firmware.c` |
| MVDM-HOST-DIV-222 | Adapt the original `53:01` register-frame handoff to CCPU segment-cache semantics. | The kernel-VDM x86 owner writes passive CONTEXT selectors before PE; CCPU immediately resolves each setter through its active descriptor cache. | Capture the source frame while real-mode SS is valid, clear NT as original `ntos/vdm/x86/vdmmisc.asm::VdmSwapContexts` does with `EFLAGS_USER_SANITIZE` (retain the CCPU IF/IOPL policy), enable PE/CPL-3, then load the same CS:EIP, SS:ESP and DS fields through CCPU. Restore the already selected worker WOW page/GDT view on this reentry path, matching native user-selector lifetime; kernel VDM state bits remain absent. | `dpmi32/modesw.c` |
| MVDM-HOST-DIV-223 | Bind DOSX's source-delivered LDT to CCPU after `53:0F`. | Historical non-i386 only used the table for host address conversion, while CPU40 executes selectors directly. | Derive the guest-linear table base from the existing `Ldt`/`IntelBase` pair and install it in CCPU with an internal-valid LDTR selector. | `dpmi32/dpmi32.c` |
| MVDM-HOST-DIV-224 | Route CPU40 DPMI extended-memory allocation through the original shared XMS suballocator. | CPU40 XMS reserves the complete extended linear range, leaving the generic VDM allocator correctly empty when DOSX requests memory. | Use the existing RISC shared-XMS allocation, free, reallocation and query operations consistently for CPU40 only. | `dpmi32/dpmimemr.c` |
| MVDM-HOST-DIV-225 | Synchronize source `53:00` descriptor writes into CCPU's bound LDT. | x86 uses `NtSetLdtEntries`; the old RISC branch only updates `FlatAddress[]`. | Copy the already supplied DOSX descriptor into the bound CPU40 LDT after original limit normalization and alongside the original conversion cache update. | `dpmi32/dpmiselr.c` |
| MVDM-HOST-DIV-226 | Publish `InitializePmStackInfo` as a DOSX-readable VDM TIB projection. | x86 returns a kernel-owned `VDM_TIB` address; CCPU executes DOSX and requires a guest-linear base for its `SEL_VDMTIB` descriptor. | Allocate one shared-XMS guest block, mirror the exact `VDM_DPMIINFO` fields delivered by DOSX, and return that linear address through `CX:DX`; no host address crosses the boundary. Required allocation failure follows original `cmdmisc.c` memory-error dialog plus `TerminateVDM`, never a successful zero-base publication; S38 fault injection verifies Terminate and Ignore both end the worker without returning a failed publication. | `dpmi32/{data.c,dpmi32p.h,dpmiint.c}` |
| MVDM-HOST-DIV-227 | Supply the original WOW/DOSX `SEL_BIOSDATA` cache to CPU40. | CPU40 treated protected selector `0040h` as an ordinary LDT descriptor, but original DOSX reserves it as a fixed BIOS-data carrier. | Select the original `dxboot.asm` base `0x400`, 64-KiB writable ring-3 data cache for selector `0040h`. | `softpc.new/base/ccpu386/c_seg.c` |
| MVDM-HOST-DIV-228 | Permit the original writable WOW DOSX `DXCODE` carrier. | The source executes its documented self-modifying initialization through selector `00CFh`; CCPU had made every protected code cache read-only. | Retain writability only for the original `SEL_DXCODE|STD_RING` cache while preserving normal protected-code write protection. | `softpc.new/base/ccpu386/c_seg.c` |
| MVDM-HOST-DIV-229 | Restore the source-defined BOP `FDh` real-mode transition in the CPU40 product topology. | The CCPU graph compiled the BIOS BOP table without its monitor-gated `FDh` entry, so the original WOW DOSX five-word continuation frame fell through `illegal_bop` and later returned through poisoned stack storage. | Select the original BIOS table entry for x86 and restore the identical DOSX frame transfer in the existing CPU40 DPMI mode-switch owner, loading cached real-mode segments only after PE is clear. | `dpmi32/modesw.c`, `tools/build/New-T310OriginalSoftpcNinja.ps1` |
| MVDM-HOST-DIV-229 (IRQ publication boundary) | Reject an unpublished hardware interrupt vector before building a guest frame. | CPU40 can receive a real IRQ while DOSX is still publishing handlers; an absent selector must not create an interrupt frame. | Return the existing no-hook result before changing EFLAGS or the stack; this is separate from the similarly numbered BOP FD transition. | `dpmi32/dpmiint.c` |
| MVDM-HOST-DIV-230 | Observe selected original HMA child stack stores. | The reached Disa20_Xfer required direct physical-store evidence; the paired RETF observer was retired with original-file restoration in T413. | The remaining default-off range-gated store helper copies scalar addresses, word value and SAS class only. It cannot map, modify guest memory or influence dispatch. | `softpc.new/base/ccpu386/ccpusas4.c`; `../ntvdm/softpc/{include/mvdm_softpc_machine_observation.h,mvdm_softpc_machine_observation.c}` |
| MVDM-HOST-DIV-230 (real-mode cache boundary) | Reload CCPU pseudo descriptors after the original protected-to-real transition. | The kernel VDM relied on hardware hidden caches; CPU40 otherwise retains protected caches after clearing PE. | Reload CS, SS, DS, ES, FS and GS while retaining original visible segment values and DOSX frame. This is separate from the similarly numbered HMA diagnostic. | `dpmi32/modesw.c` |
| MVDM-HOST-DIV-242 | Enter DOSX real mode before loading its real-mode CS. | CPU40 setCS immediately validates against the current mode, unlike the original passive kernel CONTEXT. | Leave the existing WOW page domain and clear PE before loading the source-supplied DOSX CS; preserve original segment and frame data. | `dpmi32/modesw.c` |
| MVDM-HOST-DIV-243 | Supply DOSX protected CS and SS during protected-mode transition. | CPU40 can execute a helper before the caller restores the passive native CONTEXT; real-mode segment values would otherwise be treated as protected selectors. | Use the source-supplied protected code and data selectors only for the transition, then retain the original caller frame restoration and page-domain reentry. | `dpmi32/modesw.c` |
| MVDM-HOST-DIV-232 (historical integration record) | Historical WOW error presentation description. | The previously documented ADAPTER-SOFTPC-049 route and WOW ERRORPANEL fallback are absent from the current source. | Current WOW retains WowErrorDialogEvents and WOWpSysErrorBox through the NtRaiseHardError boundary and selected hard-error presenter; failed presentation maps to RMB_ABORT. DOS separately uses the original ERRORPANEL. Current lifecycle and every button/error outcome still require verification. | `softpc.new/host/src/nt_error.c`; `../ntvdm-exe/win32/{wow_private_unavailable.c,wow_hard_error_dialog.c}`; `artifacts/mvdm-audit/shard-1/host-error-dialog-audit.md` |
| MVDM-HOST-DIV-233 | Attribute the reached original real-mode #UD interrupt transfer during DOSX bootstrap. | The S11 frontier needs the actual IVT[6] target, but no static source record identifies the runtime-installed vector. | Copy only vector/source stack/target scalars after original `phy_read_word` and before `do_intrupt` mutates the machine; retain the original vector lookup, pushes, cache load and CPU continuation. | `softpc.new/base/ccpu386/c_intr.c`; `ntvdm/softpc/mvdm_softpc_machine_observation.c` |
| MVDM-HOST-DIV-234 | Attribute an original BOP-06 invalid-opcode panel without changing its continuation. | The DOSX/WOW frontier needs the opcode frame which reached the original panel, not a guessed CPU attribution. | After the original panel captures its request, copy only the instruction/stack scalars to an optional host report; the existing one-byte continuation and original error disposition remain unchanged. | `softpc.new/base/system/illegalp.c`; `ntvdm/softpc/mvdm_softpc_machine_observation.c` |
| MVDM-HOST-DIV-235 | Attribute the fixed original DOS-EXEC KRNL386 entry window and any later reached bootstrap selector. | S17 reaches the fixed DOS entry `8AFF:0000` after KRNL386 opens, while the established BOP/DEM reports do not identify its numeric guest code window; assigning it to Kernel31 or DOSX without a descriptor-backed observation would be speculative. | After the original CPU opcode fetch, and only for `8AFF:0000..03FF` or selector `283A` while the existing BOP report is enabled, copy at most 512 linear-address/instruction-byte records through the existing checked guest lease. The helper neither changes a descriptor, register, CPU state, guest byte, mapping, BOP, or control transfer. | `softpc.new/base/ccpu386/c_main.c`; `../ntvdm/softpc/{include/mvdm_softpc_machine_observation.h,mvdm_softpc_machine_observation.c}` |
| MVDM-HOST-DIV-236 | Restore the host-only BOP observation selector after COMMAND constructs its guest environment. | `cmdenv.c` copies inherited variables into DOS only during `SVC_GETINITENVIRONMENT`; leaving a diagnostics path visible earlier changes guest input. | App captures the selector before original startup, then `cmddisp` restores it immediately after the original environment service returns. It never changes the copied environment or BOP result. | `app/report_configuration.{c,h}`; `dos/command/cmddisp.c` |
| MVDM-HOST-DIV-237 | Observe original WOW32 provider initialization and first task handoff with fixed markers. | The ordinary loader snapshot proves module mapping but cannot distinguish mapping from successful original `W32Init` return or later original task ownership. | The provider appends bounded initialization/task-entry markers only when an optional host report path is present, after COMMAND copied guest environment; no thunk, frame, task, thread, or provider result changes. | `wow32/{wow32.c,wow32.h,wkman.c}`; `tools/observation/ObserveSoftpcStartup.mjs` |
| MVDM-HOST-DIV-258 | Attribute original BOP 59 direct-access error entry and return. | The original MS_bop_9 calls host_direct_access_error with AX; current added diagnostics inspect guest SS:SP before and after that call. | The current source performs four guest word reads on each side before the reporter checks its default-off trace setting. These reads are not already-live scalars and can reach CPU memory handlers; an actual guest-visible failure has not been established. Reconcile or remove the observations with an original-call and zero-extra-read fixture before claiming equivalent semantics. | `softpc.new/host/src/nt_bop.c`; `../ntvdm-exe/softpc/mvdm_softpc_termination.c`; `artifacts/mvdm-audit/shard-1/bop9-diagnostic-guest-read-audit.md` |
| MVDM-HOST-DIV-259 | Select the WOW32 Windows and system directories during provider initialization. | Original NT4 host-directory calls resolve the modern host installation rather than the app-selected immutable product media. | Use the shared `NtvdmGetWindowsDirectoryA` / `NtvdmGetSystemDirectoryA` queries, which expose the selected package root and its existing `system32` child; preserve the original short-path normalization and fail initialization when either selected location is unavailable. | `wow32/wow32.c`; `../ntvdm-exe/softpc/{include/mvdm_softpc_firmware.h,mvdm_softpc_firmware.c}` |
| MVDM-HOST-DIV-464 | Resolve the original COMMAND home-drive query against the selected product system directory. | Original `CMDInit` uses `GetSystemDirectory`; an isolated product root may live on a different drive from the modern host system directory. | Preserve the original initialization and drive extraction, replacing only that query with the existing `NtvdmGetSystemDirectoryA` package-root projection. The host directory and product directory can differ; failure and guest behavior require separate validation. | `dos/command/cmd.c`; `../ntvdm-exe/softpc/{include/mvdm_softpc_firmware.h,mvdm_softpc_firmware.c}` |
| MVDM-HOST-DIV-305 | Record an explicitly enabled CCPU far-call diagnostic. | Diagnosing WOW callback handoff requires observing the selected original CALLF before its normal branch. | Only when both `MVDM_WOW_CALLBACK_CPU_TRACE_PATH` and the matching `MVDM_WOW_CALLBACK_CPU_SELECTOR` are set, call the bounded host witness; it reads caller and descriptor data and preserves host last-error. Disabled operation and enabled failure/reentry timing require separate verification. | `softpc.new/base/ccpu386/call.c`; `../ntvdm-exe/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-306 | Observe far-call descriptor validation for WOW diagnosis. | Original far-transfer validation does not collect the selector and descriptor-domain diagnostics. | Calls the host witnesses before the original descriptor type/presence decision. Although reporting is environment-gated, the call arguments perform descriptor reads unconditionally; a real CCPU paging fixture reproduced #PF where original code gives #GP. This known behavioral difference requires S11 correction; do not characterize the full insertion as inert when logging is disabled. | `softpc.new/base/ccpu386/c_xfer.c`; `../ntvdm-exe/softpc/mvdm_softpc_termination.c`; `../../artifacts/mvdm-audit/shard-3/cpu-far-diagnostic-audit.md` |
| MVDM-HOST-DIV-307 | Observe the WOW INT 2Ah LDT publication request. | Original CCPU software-interrupt dispatch has no visibility into the WOW fast-LDT request before a handler is selected. | For INT 2Ah with the original EAX/EBP signature, pass the existing selector and descriptor scalars to a witness enabled by `MVDM_WOW_CALLBACK_CPU_TRACE_PATH`; preserve the original handler dispatch. Enabled diagnostic timing and I/O failure are not equivalent to uninstrumented execution. | `softpc.new/host/src/nt_inthk.c`; `../ntvdm-exe/softpc/mvdm_softpc_termination.c` |
| MVDM-HOST-DIV-308 | Observe exception-hook results and far-destination translation. | Diagnosing WOW fault delivery and far-call address translation requires data from the existing CCPU and host interrupt paths. | The exception witness records handler state and result when `MVDM_WOW_CALLBACK_CPU_TRACE_PATH` is enabled. The separately marked far-transfer witness also performs descriptor reads and `xtrn2phy` before its reporting call; those operations are not gated by the trace setting and their side effects are pending audit. Do not assume disabled tracing makes the complete insertion inert. | `softpc.new/host/src/nt_inthk.c`; `softpc.new/base/ccpu386/c_xfer.c`; `../ntvdm-exe/softpc/mvdm_softpc_termination.c` |
| MVDM-HOST-DIV-309 | Observe WOW INT 2Ah register values. | The original CCPU interrupt dispatch does not itself retain the scalar values needed to trace the WOW kernel LDT carrier. | For INT 2Ah, pass register scalars to a witness enabled by `MVDM_WOW_CALLBACK_CPU_TRACE_PATH`; the original software-handler decision follows. Enabled diagnostic timing and I/O failure remain separate validation boundaries. | `softpc.new/host/src/nt_inthk.c`; `../ntvdm-exe/softpc/mvdm_softpc_termination.c` |
| MVDM-HOST-DIV-297 | Resolve WOW's bare package `SYSTEM.INI` argument through the selected worker system root. | Original USER16 calls the original WOW private-profile thunk with the bare name `SYSTEM.INI`. NT4 resolves that name through the installed VDM system profile; modern Win32 otherwise redirects it to `C:\\Windows\\SYSTEM.INI`, outside the selected product package. | Retain the original private-profile APIs, defaults, errors, and arbitrary caller path behavior. Only an exact case-insensitive bare `SYSTEM.INI` argument uses a session-owned temporary copy of the selected root file when available; on shadow creation failure the original bare name is passed through. Write persistence, fallback and teardown require further verification; the original guest `USER.EXE` is not modified. | `wow32/wkernel.c`; `../ntvdm-exe/softpc/{include/mvdm_softpc_firmware.h,mvdm_softpc_firmware.c}` |
| MVDM-HOST-DIV-260 | Supply the standalone WOW KnownDLL bootstrap policy when NT4's product-global WOW registry key is absent. | The selected immutable Win16 media provides `COMMDLG.DLL` and `OLECLI.DLL` in its system directory, but an empty KnownDLL list makes the original loader search the package root then the modern host Windows installation. | With no WOW key, seed only those two admitted package members into the unchanged original allocation/parser; a present key and its value retain the original registry path. | `wow32/wkman.c`; selected `system32/{COMMDLG.DLL,OLECLI.DLL}` media manifest |
| MVDM-HOST-DIV-262 | Materialize a projected Win16 class's original menu before public native-window creation. | The class menu is a Win16 resource name paired with a packed `HMODINST32` identity. Public `RegisterClassA` cannot resolve that identity as a native module, so the created native window otherwise has no `HMENU`. | Keep original `WU32LoadMenu` resource discovery, conversion and lock lifetime. At the existing class-window boundary, invoke the unavailable private output callback with the already converted Unicode template; its `ADAPTER-WOW-020` implementation uses public `LoadMenuIndirectW` only and never publishes the packed identity. | `wow32/wuwind.c`; `../ntvdm/wow/{include/wow_private_user_compat.h,wow_private_user_compat.c}` |
| MVDM-HOST-DIV-171 | Write the original local computer/user result directly through `LPSTR_FROM_WORDS(ES:DI)`. | The historical x86 host alias has no explicit lifetime and cannot safely cross the current session boundary; the selected public NetAPI carrier returns the user name as Unicode without the historical global `UNICODE` compile setting. | Preserve the original public calls, character-capacity gate and register/status order; replace only the bounded copies with the worker lease. The added OEM conversion also checks positive declared capacity in bytes including NUL and returns existing `NERR_BufTooSmall` before writing; BX=0 remains unchecked. DIV-361 separately corrects the CX=0 underflow. Lease refusal returns `ERROR_INVALID_ADDRESS`. | `vdmredir/vrnetapi.c`; `../vdmredir-dll/include/mvdm_redirector_guest_copy.h`; `../vdmredir-dll/source/mvdm_redirector_guest_copy.c` |
| MVDM-HOST-DIV-172 | Use `VrRemoteApi`'s locally synthesized computer name as the server name for the original RAP branch. | The original source explicitly copied the Unicode `NetWkstaGetInfo` result with `strcpy`, which is not a valid modern C string conversion. | Preserve the fixed local OEM temporary, `NetApiBufferFree`, and original later RAP branch; convert only the public Unicode result through `WideCharToMultiByte(CP_OEMCP)`. Conversion failure releases the original local allocation and returns `ERROR_INVALID_DATA`. | `vdmredir/vrremote.c` |
| MVDM-HOST-DIV-176 | Write the original `I_CDNames` trio of guest far-pointer destinations after obtaining public workstation/user values. | The original treats the packed guest structure and all three targets as durable flat aliases. That crosses the current session boundary without a lease and is invalid on x64. | Keep original NetAPI calls, ANSI conversion, upper-casing and no-return-code behavior. The Redirector adapter reads each destination after the preceding write, clears each non-null target and copies through bounded leases, including destinations aliasing later pointer fields. | `vdmredir/vrnetapi.c`; `../vdmredir-dll/{include/mvdm_redirector_guest_copy.h,source/mvdm_redirector_guest_copy.c}` |
| MVDM-HOST-DIV-177 (retired DEM diagnostic) | Historical `demOpen` observation only. | The original default-off diagnostic calls were removed in T420 S13 (`a2ec4d531`). | Retained for historical traceability; original `demOpen` behavior and active DIV-195 remain. This is distinct from the still-active SIM32 DIV-177 row. | Historical `dos/dem/demfile.c`; see `docs/etc/evidence/m0-t420-s13-dem-and-residual-diff-recovery.md` |
| --- | --- | --- | --- | --- |
| MVDM-HOST-DIV-002 | Persist, compare and release a `PFFINDLIST` pointer across DOS FindFirst/FindNext calls. | Native pointer persistence is width-unsafe and forbidden across the MVDM/guest boundary. | The original source flow publishes, resolves and releases an opaque identity through the S15 ntvdm/softpc facade; `FFindId` stays original numeric data and source resource-failure branches are retained. | `dos/dem/demsrch.c` |
| MVDM-HOST-DIV-004 | Retain and write the DOS extended-error location installed by `demSetDTALocation`. | A native `GetVDMAddr` pointer cannot cross a callback or an x64 ABI boundary. | Preserve DS:CX as an `ntvdm/softpc` numeric guest descriptor; `demRead` takes a fresh nine-byte bounded lease, writes the original fields in order, and commits/releases before it continues. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demhndl.c` |
| MVDM-HOST-DIV-005 | Retain the NTDOS DTA far-address cell and use its current target for DEM search/FCB I/O. | The original `pulDTALocation` is a persistent native pointer, and DTA bytes must not outlive a guest-memory lease. | Preserve DS:AX and its four-byte far-address cell numerically; use fresh bounded DTA leases for FindFirst, FindNext and FCB I/O, committing only guest-write paths. Lease failure follows existing DEM failure flow with `ERROR_INVALID_ADDRESS`. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demsrch.c`, `dos/dem/demfcb.c` |
| MVDM-HOST-DIV-006 | Retain the current DOS PDB scalar for DEM file/find and SoftPC disk/floppy ownership. | The original `pusCurrentPDB` is a persistent native pointer; its two-byte guest value must not survive a callback as host memory. | Preserve DS:DX as a numeric descriptor and take a fresh two-byte read lease at each reached consumer. Existing false/zero/negative error directions are retained when the scalar cannot be read. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demsrch.c`, `dos/dem/demfile.c`, `softpc.new/host/src/nt_fdisk.c`, `softpc.new/host/src/nt_rflop.c` |
| MVDM-HOST-DIV-007 | Retain the DEM SFT chain and original VDD SFT/JFT handle operations. | The original retains pSFTHead and returns direct guest aliases; original SFT_NTHandle is 32-bit. | Keep the SFT head as numeric DS:offset. Current selected VDD functions resolve direct synchronous CCPU guest pointers, and original WOW callers use them; the previously documented shadow/commit wrapper is not present. Guest backing lifetime, reentry, rollback and handle-width safety remain unverified. | `dos/dem/dem.h`, `dos/dem/demdata.c`, `dos/dem/demgset.c`, `dos/dem/demfile.c`, `wow32/wkfileio.c`; `artifacts/mvdm-audit/shard-1/dem-media-sft-lifecycle-audit.md` |
| MVDM-HOST-DIV-008 | Resolve original WOW and installable-BOP exports. | Modern x86 rejects incompatible implicit function-pointer assignments. | Retain explicit FARPROC/MYFARPROC conversions and original call order. Ordinal checks use the restored original ULONG expressions; no extra provider is enabled. | `softpc.new/host/src/nt_bop.c` |
| MVDM-HOST-DIV-009 | Write `cmdGetCurrentDir`'s `DS:SI` result directly through an unbounded `GetVDMAddr` pointer. | A guest pointer cannot be retained as host memory on x64 or across the modern component boundary. | Retain the original drive/environment order and CF/AX outcomes; stage the at-most-67-byte OEM result in a local buffer, then use the session-owned ntvdm/softpc numeric descriptor and exact synchronous guest write. | `dos/command/cmdmisc.c` |
| MVDM-HOST-DIV-012 | Return a DPMI extended-memory allocation record by splitting the native `XMEM_BLOCK *` into `SI:DI`. | The selected Win32/x86 product preserves the original process-local 32-bit pointer carrier. | Retain the original `SI:DI` handle, allocation/failure ordering and source list without a surrogate layer. | `dpmi32/xmem.c` |
| MVDM-HOST-DIV-018 | Return the monitor `VdmTib.PmStackInfo` address in the original `CX:DX` x86 register pair. | The selected Win32/x86 product preserves the original process-local 32-bit pointer carrier. | Return the direct 32-bit address in `CX:DX`; no mapping or projection layer is introduced. | `dpmi32/dpmiint.c` |
| MVDM-HOST-DIV-139 | Define the x86 `LDT_ENTRY` descriptor layout for the non-kernel-VDM DPMI source branch. | Modern public `winnt.h` supplies that same declaration before the historical source-local fallback, causing a duplicate C type definition. | Preserve the original fallback verbatim only when the public declaration carrier is absent; no selector algorithm or descriptor field is changed. | `dpmi32/dpmidata.h` |
| MVDM-HOST-DIV-146 | Deliver a keyboard-controller output-port reset pulse to the selected CCPU40 executor. | The original non-CPU30 branch writes CCPU's private `cpu_interrupt_map` directly and calls the historical host wake hook. This composition intentionally keeps that private bitmap inside `c_main.c`. | Use the original public `cpu_interrupt(CPU_HW_RESET, 0)` entrypoint; it sets the same CCPU40 reset event and clears quick mode for the selected executor. | `softpc.new/base/keymouse/keyba.c` |
| MVDM-HOST-DIV-147 | Provide the historical shared SoftPC support definitions. | `xt.c`'s CCPU/non-CPU30 declaration block is an obsolete monitor-owned duplicate of CCPU40 register, descriptor and machine-status state; selected CCPU40 owns that state privately in `c_main.c`. | Retain the source file and every non-state support definition, but exclude only the obsolete CPU-state declaration block when `CPU_40_STYLE` is selected. | `softpc.new/base/support/xt.c` |
| MVDM-HOST-DIV-155 | Provide the historical non-CCPU parity lookup table. | In the selected CPU40 CCPU product, `c_main.c` owns `pf_table`; `xt.c`'s original monitor-path copy is a duplicate provider and has no separate CPU40 consumer. | Retain the original table for all other profiles but exclude only this duplicate table under `CPU_40_STYLE`. | `softpc.new/base/support/xt.c` |
| MVDM-HOST-DIV-177 | Translate a SIM32 protected-mode selector:offset into a numeric guest-linear address. | The original SIM32 body uses the CPU30 `DESCR/read_descriptor` carrier, while CPU40 keeps compatible descriptor decoding inside its execution core. | Preserve the caller's explicit pm argument, real-mode arithmetic and the -1 invalid-selector result. Protected requests always use original CPU40 selector_outside_GDT_LDT/read_descriptor_linear, even when CPU PE is clear; no hidden-cache selection or CPU30 monitor state is recreated. | `softpc.new/host/src/sim32.c`; `../ntvdm-exe/softpc/mvdm_softpc_effective_address.c`; `softpc.new/base/ccpu386/c_bsic.c` |
| MVDM-HOST-DIV-178 | Retain original `CallBack16` protected-mode resolution, including segment faults and FlatAddress native mappings. | A caller alias must not survive the second potentially guest-reentrant resolution. The former current-mode/SAS lease bypassed the original resolver. | Snapshot only caller wTDB/wLocalBP immediately after original GETFRAMEPTR; never write the snapshot back. Restore original callback resolution, flush/free and post-return reacquisition. Remove the autonomous acquire_vp path. Two CBFRAMEPTR assignments retain the original resolver without the legacy cast-lvalue spelling. Runtime acceptance pending in T447 S2. | `wow32/wcall16.c`; `../wow32-dll/source/wow_callback_frame_lease.c` |
| MVDM-HOST-DIV-179 | Include the original WOW32 precompiled-declaration cohort. | The original `base/mvdm` tree placed `winuserp.h` and `vdm.h` on a private include root; the split product deliberately keeps those declaration-only files in `opennt-abi` instead. | Replace only the two include spellings with the same-named byte-identical ABI carriers. This adds no USER/GDI, CSR, VDM-control or provider behavior. | `wow32/precomp.h`; `../opennt-abi/source/public/internal/{windows/inc/winuserp.h,base/inc/vdm.h}` |
| MVDM-HOST-DIV-180 | Keep the original WOW32 exports as the selected source provider. | The modern SDK's `wownt32.h` declares later public WOW32 exports with incompatible historical type modifiers before original source can define its matching NT4 bodies. | Suppress only that modern declaration carrier for this original WOW32 compilation cohort; no runtime API is removed or substituted. | `wow32/precomp.h` |
| MVDM-HOST-DIV-181 | Select original CCPU40 simulator stack access in WOW32. | Modern x86 headers set `_X86_` for host architecture, but original WOW32 uses that marker to select a native-VDM `pIntelRegisters` alias unavailable in the selected CCPU40 machine. | Undefine the host marker only before `wow32.h`, retaining the original `getSS`/`setSS` CCPU simulator stack contract on both x86 and x64. | `wow32/precomp.h`; `wow32/wow32.h`; `softpc.new/host/inc/host_cpu.h` |
| MVDM-HOST-DIV-150 | Select the original CPU40 recursive-emulation vector. | In the selected CPU40 CCPU profile, original `host_cpu.h` maps `host_simulate()` to `host_simulate_func`, while this historical host source also defines a direct function of that macro name. | Retain the direct body unchanged for non-CCPU builds and omit it only for CCPU, whose original initialization selects `c_cpu_simulate` through the vector. | `softpc.new/host/src/nt_cpu.c`; `softpc.new/host/src/nt_cprgs.c`; `softpc.new/host/inc/host_cpu.h` |
| MVDM-HOST-DIV-151 | Select the original generated-CCPU access-vector initializer for CPU40. | `nt_cprgs.c` historically gates that CCPU initializer on `CPU_30_STYLE`; without the retired macro, CPU40 falls into a different legacy register-state implementation that requires CPU30 globals. | Select the original CCPU initializer for either historical CPU30 or selected CPU40. CPU30 remains undefined and unlinked. | `softpc.new/host/src/nt_cprgs.c` |
| MVDM-HOST-DIV-182 | Install the selected generated-CCPU access vector at original CPU startup. | The historical CPU30 product shell supplied the post-CPU vector-installation step; the CPU40-only product deliberately excludes that shell, leaving the original `host_simulate_func` null even though DIV-151 selects its CCPU40 provider. | Immediately after the original `cpu_init()`, call the existing original `load_sw_cpu_access_functions()` only for selected CCPU40. This retains the original vector table, register accessors and `c_cpu_simulate` recursive entry; it adds no executor, adapter or guest behavior. | `softpc.new/base/support/main.c`; `softpc.new/host/src/nt_cprgs.c` |
| MVDM-HOST-DIV-152 | Publish the actual CPU40 CCPU return widths for control-state vectors. | Old declarations represent `c_getTS`, `c_getEM`, `c_getMP`, `c_getPE`, and `c_getIOPL` as unprototyped `word` providers, while selected CCPU bodies return `ISM32`. | Keep vector storage, names, and calls; give the five selected CCPU entries their exact no-argument `ISM32` contracts. | `softpc.new/host/src/nt_cprgs.c`; `softpc.new/host/inc/host_cpu.h`; `softpc.new/base/ccpu386/c_reg.c` |
| MVDM-HOST-DIV-153 | Preserve original BIOS BOP FE unsimulate entry. | CPU40 CCPU owns the actual `c_cpu_unsimulate` execution unwind, while original BIOS retains the historic `host_unsimulate` public spelling. | Publish only the original-name bridge to the selected CCPU provider; no alternate CPU loop or termination policy is added. | `softpc.new/host/src/nt_cpu.c`; `softpc.new/base/ccpu386/c_main.c`; `softpc.new/base/bios/bios.c` |
| MVDM-HOST-DIV-023 | Start the original `ErrorDialogBoxThread` through the public `CreateThread` call shape. | The selected NT4 source declares the worker as cdecl, but current x86 Win32 requires `LPTHREAD_START_ROUTINE`/`WINAPI`; the source is rejected before the original dialog code can be composed. | Preserve its parameter, return, lifetime and call ordering; apply only the two `WINAPI` declaration/definition annotations supplied by the registered NTVDMx64 `callconv.patch`. | `softpc.new/host/src/nt_error.c`; provenance: [T413 S5 exception-scope evidence](../../docs/etc/evidence/m0-t413-s5-exception-scope.md) |
| MVDM-HOST-DIV-024 | Include the monitor `vdm.h` carrier required by original SoftPC idle-detection state. | The original relative include assumes the unsplit NT4 private-tree depth, which no longer exists after the owner-rooted mirror move. | Replace only that path with the same selected `vdm.h` declaration supplied through the declared ABI include path; source declarations and algorithm remain unchanged. | `softpc.new/base/system/idetect.c` |
| MVDM-HOST-DIV-026 | Keep the CPU-40 SAS memory-span global consistent with its original `sas.h` declaration. | On x86, the implementation's `PHY_ADDR` and header's `IHPE` were both 32-bit by coincidence; after the required x64-safe `IHPE` correction they conflict. | Retain the original global spelling, usages and numeric meaning, but define it as the declared `IHPE`; its one floating-point consumer declaration is kept congruent. No guest or component ABI changes. | `softpc.new/base/ccpu386/ccpusas4.c`, `softpc.new/base/ccpu386/zfrsrvd.c` |
| MVDM-HOST-DIV-027 | Call the existing SoftPC `host_sas_init(sys_addr)` allocator during CCPU SAS setup. | The original x86 compiler accepted the missing function prototype and therefore happened to preserve the 32-bit pointer return; modern x64 C treats it as `int` and truncates the allocated backing address. | Add only the exact existing `UTINY *` declaration; implementation, parameter, return value and source control flow are unchanged. | `softpc.new/base/ccpu386/ccpusas4.c` |
| MVDM-HOST-DIV-029 | Make `check_malloc` call the existing host `malloc` contract for SoftPC allocations such as the CCPU FPU stack. | The historical `<memory.h>` incidentally declared `malloc`; current MSVC declares it in `<stdlib.h>`, otherwise C treats the pointer result as `int` and truncates it on x64. | Add the modern declaration header only; `host_malloc` spelling, allocator, failure loop and all callers remain original. | `softpc.new/base/inc/ckmalloc.h` |
| MVDM-HOST-DIV-030 | Provide the original `host_start_cpu`/`host_simulate` source in a CCPU static link. | `nt_cpu.c` carries two host memory globals that duplicate CCPU SAS definitions; they are not used in its CCPU branch and make the original host wrapper un-linkable beside `ccpusas4.c`. `host_delays` is independent controller timing state and remains required by the original FDC/WD2010 callers. | Retain `Start_of_M_area` and `Length_of_M_area` for every non-CCPU build; exclude only those duplicate definitions under `CCPU`, while preserving original `host_delays` in every profile. The original wrapper names, parameters and bodies are unchanged. | `softpc.new/host/src/nt_cpu.c` |
| MVDM-HOST-DIV-031 | Link the historical i386 SoftPC product aggregate with `monitor.lib`. | The selected `i386 + CCPU` profile must physically exclude the NT4 kernel-V86 monitor; retaining it in the product manifest would permit an accidental second executor. | Remove only the `monitor.lib` line; retain all other original library order. The removed monitor package is available only as read-only external OpenNT reference material, not as a repository copy. | `softpc.new/obj.vdm/sources`; external OpenNT reference tree |
| MVDM-HOST-DIV-032 | Declare the original host_time interface. | Current UCRT defaults time_t to 64 bits even on x86; original nt_unix.c defines long host_time(long *), and the selected NTVDM CMOS caller uses long. | Declare the actual original long ABI explicitly. This is not x64-only residue; changing the whole CRT time profile is not part of this declaration repair. | `softpc.new/base/inc/timeval.h` |
| MVDM-HOST-DIV-033 | Read CRT system-error text from `_sys_errlist`/`_sys_nerr`. | Modern UCRT removed those exported storage symbols. | Preserve `host_get_system_error`'s original `char *` text contract with public `strerror`; only the unavailable CRT-storage access changes. | `softpc.new/host/src/nt_unix.c` |
| MVDM-HOST-DIV-035 | Original external-memory add/remove with PhysicalPageREC translation updates. | Exact lower translation bodies are unavailable in the audited source; CCPU needs a bounded host-page binding. | Original VdmAddVirtualMemory/VdmRemoveVirtualMemory bodies are restored. Only InitIntelMemory/FreeIntelMemory bind the adapter lifecycle, allocating all slots before initialization succeeds and retiring borrowed mappings before freeing normal memory. The void setter does not allocate. No publish/prepare/cancel protocol or surrogate identity remains. Bounded PID-correlated first-hit trace: `pages.initialized`, `VdmSetPhysRecStructs.call`, and `set.*`. | `softpc.new/host/src/nt_mem.c`; `../ntvdm/softpc/{include/mvdm_softpc_physical_mapping.h,mvdm_softpc_physical_mapping.c}` |
| MVDM-HOST-DIV-036 | Resolve an Intel physical byte to CCPU backing memory. | The original CCPU assumes every physical address is an offset in one native `Start_of_M_area` block; an admitted external host-page span must instead remain live through its session-owned mapping identity. | Add one selector-blind adapter probe before the unchanged original RAM calculation. The adapter returns a native pointer only to this synchronous CCPU access path after validating the owning session, mapping identity, span and lifetime; otherwise the original RAM path runs unchanged. Bounded PID-correlated first-hit trace: `resolve.external-hit`. | `softpc.new/base/ccpu386/ccpusas4.c`; `../ntvdm/softpc/{include/mvdm_softpc_physical_mapping.h,mvdm_softpc_physical_mapping.c}` |
| MVDM-HOST-DIV-037 | Locate original SoftPC ROM/CMOS resources through `host_find_file`. | The original body hardcodes the NT system directory, which is neither the project firmware package nor a portable product installation boundary. | Preserve the original function name, ANSI parameters and `host_error` failure order; replace only its system-directory lookup with the session-owned configured firmware-root adapter. | `softpc.new/host/src/nt_unix.c`; `../ntvdm/softpc/mvdm_softpc_firmware.c`; `../session/session.[ch]` |
| MVDM-HOST-DIV-038 | Load the original `ntio.sys` image in `InitialiseDosEmulation`. | The original body hardcodes the NT system directory. The current product stages immutable MVDM system media beside its executable rather than requiring an installed NT4 system file. | Preserve original file validation, `CreateFile`/`ReadFile`, SAS write, virtual-interrupt restore and `CS:IP` entry ordering; replace only directory discovery with the session-owned MVDM-system adapter. A controlled termination that returns now returns `-1`, matching the original terminal failure direction. | `softpc.new/host/src/nt_msscs.c`; `../ntvdm/softpc/mvdm_softpc_firmware.c`; `../app/package_layout.[ch]`; `../session/session.[ch]` |
| MVDM-HOST-DIV-039 | Initialize the fixed virtual-interrupt state for the selected SoftPC machine profile. | Historical `#ifndef i386` chose the original software-CPU (RISC-host) path, while `i386` chose the NT kernel-V86 monitor. This product deliberately selects original `CPU_40_STYLE` on an x86 host; host ISA must not accidentally select the unavailable kernel-V86 product shell. | Preserve the original software-CPU transition by selecting the non-`i386` `MIPS_BIT_MASK` state only under `CPU_40_STYLE`; retain the original V86-monitor clear state for any future non-CCPU profile. The bit remains a historical `FSTI/FCLI` behavior selector only—no MIPS guest code or second CPU is enabled. | `softpc.new/host/src/nt_msscs.c`; `../../tools/build/New-T310OriginalSoftpcNinja.ps1` |
| MVDM-HOST-DIV-040 | Terminate the original SoftPC heartbeat at its alertable wait. | Original NT4 alert wakes with STATUS_ALERTED. The compatibility carrier prefers the native NTDLL NtAlertThread when available; only its public APC fallback returns STATUS_USER_APC. | Preserve the original termination block and accept either status. The APC fallback is not a general replacement for a pending NT alert on arbitrary threads; timer, PIC and session policy remain with their existing owners. | `softpc.new/host/src/nt_timer.c`; `../ntvdm-exe/win32/nt_thread_alert_compat.c` |
| MVDM-HOST-DIV-043 | Publish the original EGA BIOS chain-write callback globals. | `cga.c` used old-style unprototyped global function pointers while the same original interface is already explicitly two-`ULONG` in `video.h`; x64 then loses the checked callback contract when EGA routines are assigned. | Preserve names, storage and call order, changing only the two global declarations to the existing `video.h` `IPT2` shape. | `softpc.new/base/video/cga.c`; original declaration evidence `softpc.new/base/inc/video.h` |
| MVDM-HOST-DIV-044 | Preserve the original NTVDM host-code selection without implicitly selecting a second machine profile. | The original common host header defines `CPU_30_STYLE`, which selects the separate NT4 kernel-VDM V86-monitor contract. The product admits only the original CCPU40 software-executor profile, so a shared header must not re-enable the retired route. | Retain `NTVDM` and the original remaining host declarations; remove only the `CPU_30_STYLE` definition and document the exclusion at its former insertion point. `CPU_40_STYLE` remains the sole build-manifest selection. | `softpc.new/host/inc/host_def.h`; `../../tools/build/New-T313CcpuLifecycleNinja.ps1`; `../../tools/build/New-T310OriginalSoftpcNinja.ps1` |
| MVDM-HOST-DIV-146 | Let the historical ICA header publish its non-CCPU interrupt-map bit macros. | The original combined CCPU40 build also defined `CPU_30_STYLE`, using it incidentally to suppress `CPU_HW_INT` before CCPU's distinct event enum. The product no longer defines the retired CPU30 marker. | Preserve the original non-CCPU branch and prevent the ICA macro only when the original `CCPU` executor is selected; CCPU's original event enum remains authoritative. | `softpc.new/base/inc/ica.h` |
| MVDM-HOST-DIV-055 | Store the selected SAS physical-memory capacity. | The original CPU_40 declaration used the host-sized `IHPE` alias, although the value is produced from a physical-address input and returned through the fixed-width `TYPE_sas_memory_size` interface. On x64 that silently widens a guest capacity and narrows it on return. | Preserve all source control flow while declaring the capacity as `PHY_ADDR`; it remains a guest physical extent and never a host pointer or mapping token. | `softpc.new/base/inc/sas.h`, `softpc.new/base/ccpu386/ccpusas4.c` |
| MVDM-HOST-DIV-056 | Select the original x86 memory-size branch. | The modern MSVC target advertises `_M_IX86`, not the historical `i386` spelling. | Recognize either macro locally; keep both original branch bodies and all calculations unchanged. Only Win32/x86 is a supported target. | `softpc.new/host/src/config.c` |
| MVDM-HOST-DIV-057 | Declare the original `host_simulate` controller re-entry form. | Non-MONITOR sources select `cpu4.h`, which omits the declaration although original keyboard, mouse, BIOS and storage code call the same `void` entrypoint.  Modern C otherwise assumes an `int` result and loses the call ABI. | The shared original host declaration carrier publishes only `void host_simulate(void)`; selected SoftPC/CCPU implementation and all call order remain unchanged. | `softpc.new/host/inc/host_def.h` |
| MVDM-HOST-DIV-058 | Carry mouse range endpoints from the original INT 33h state to the host range notifier. | The caller's `MOUSE_SCALAR *` endpoints and the host declarations' `word *` parameters had equal NT4 width but incompatible signed scalar pointer types; modern MSVC correctly rejects the call shape. | Preserve the four-pointer API, call order and implementation.  Declare both caller and host definitions with the actual `MOUSE_SCALAR *` endpoint type. | `softpc.new/base/keymouse/mouse_io.c`, `softpc.new/host/src/nt_mouse.c` |
| MVDM-HOST-DIV-059 | Read the NTIO keyboard vector table and stream-buffer cells through original SoftPC SAS backing memory. | The original x86 build treated fixed-width `effective_addr` results as host pointers; x64 truncates those values before the cast. | Retain the original table order and all `effective_addr` calculations, but combine each physical number with the existing native-width `Start_of_M_area` backing pointer.  No new mapping manager or device shim is created. | `softpc.new/base/keymouse/keybd_io.c` |
| MVDM-HOST-DIV-060 | Retain the unprovided `Sas_overwrite_memory` member in the original generated CCPU SAS vector. | The source-era generic K&R null-function-pointer cast is incompatible with the selected generated two-argument slot on modern x86/x64 C compilers. | Keep the original unavailable null slot and vector order, but spell it as the existing generated `TYPE_sas_overwrite_memory *` null type.  No provider, wrapper, mapping or behavior is added. | `softpc.new/base/ccpu386/sasCdef.c` |
| MVDM-HOST-DIV-061 | Declare the allocator used by the original EMM manager. | NT4 included its allocator declaration only under MONITOR, while the selected CCPU graph still reaches host_malloc. | Include the standard allocator declaration; original long storage IDs, declarations, tables and host implementations are restored for the sole Win32/x86 product. | `softpc.new/base/dos/emm_mngr.c` |
| MVDM-HOST-DIV-062 | Retain only the selected generated-global view. | The earlier x64 callback typing is retired: x86 restores OpenNT's generic `IHP` callback carriers in `evidgen.h`. The remaining generated `VGAGLOBALSETTINGS` versus `VGA_GLOBALS` view selection is a distinct C-VID profile composition issue. | Compile original generated C-VID table sources directly and select `VGAGLOBALSETTINGS` only for the original `C_VID` profile; non-C-video source keeps its original `VGA_GLOBALS` view. No callback type wrapper remains. | `softpc.new/base/cvidc/ev_glue.c`, `softpc.new/base/inc/egacpu.h` |
| MVDM-HOST-DIV-075 | Publish the original generated C-video runtime-error call contract. | All selected generated `SEVID*.c` bodies call `CrulesRuntimeError`, whose existing original definition is in `ev_glue.c`; the historical generated support header omitted a declaration, producing implicit-`int` calls. | Add only the exact `void (char *)` declaration to the shared original generated support header. No generated body, diagnostic behavior, adapter or mapping-manager route is changed. | `softpc.new/base/cvidc/j_c_lang.h` |
| MVDM-HOST-DIV-076 | Retain exact native C variadic call contracts at selected original SoftPC call sites. | The NT4 compiler accepted implicit `printf`/`sprintf` declarations. On x64, a variadic call must see its real declaration; the original host already supplies the selected `printf` provider and `sprintf` remains a CRT API. | Add the standard C declaration header at each reached original call site only. No custom provider, adapter, format change, mapping-manager use or warning suppression is added. | `softpc.new/base/ccpu386/fpu.c`, `softpc.new/base/ccpu386/popf.c`, `softpc.new/base/cvidc/j_c_lang.c`, `softpc.new/base/cvidc/ev_glue.c`, `softpc.new/host/src/nt_mem.c` |
| MVDM-HOST-DIV-064 | Select the original in-source DWORD fill loop. | The previous width adaptation replaced the original call branch with another loop even though OpenNT already contains a usable direct loop. | Enable that original loop without changing its body; retain original x86 types and alignment. The replaced implementation and 132 alignment/length cases have matching output digests. No new provider is retained. | `softpc.new/host/src/copy_fnc.c` |
| MVDM-HOST-DIV-140 | Publish the original `host_getenv` native pointer result. | The portable SoftPC host header aliases `host_getenv` to the C runtime `getenv`, but the selected CCPU source has no standard declaration in scope. Modern C then assumes an `int` result, truncating the returned host pointer on x64. | Add only the standard C runtime declaration in the original host portability header. The alias, call sites and environment-variable semantics remain unchanged; this private host pointer is not a mapping-manager identity. | `softpc.new/host/inc/host_def.h` |
| MVDM-HOST-DIV-071 | Publish the original CCPU EFLAGS accessor. | `c_bsic.c` defines `c_getEFLAGS`, but its own selected internal header omits the declaration. Callers otherwise use an implicit host `int` return, which is not the source's fixed 32-bit EFLAGS contract. | Add only the exact `IU32 c_getEFLAGS(void)` declaration to the same CCPU header; no caller ordering or flag semantics change. | `softpc.new/base/ccpu386/c_bsic.h` |
| MVDM-HOST-DIV-072 | Publish selected same-package CCPU call contracts. | The original CCPU executor relied on implicit declarations for its existing address, segment, PIC, timer, debug and NPX bodies. Modern C defaults those calls to a host `int`, which is unsafe for fixed-width results and void call contracts. | Include the existing owner headers and declare only the two ownerless same-package `void` functions. The called bodies, call order, CPU state and device semantics are unchanged. | `softpc.new/base/ccpu386/c_main.c` |
| MVDM-HOST-DIV-126 | Publish CCPU `note_486_instruction` contract at the reached `POPFD` caller. | The original same-package function is defined in `c_main.c`, but `popf.c` relied on implicit-int declaration. Modern C would infer a host `int` result despite the original diagnostic being `void`. | Declare the original `void(char *)` signature at the reached caller; it neither changes the 486 diagnostic policy nor changes CPU state. | `softpc.new/base/ccpu386/popf.c` |
| MVDM-HOST-DIV-073 | Pass the CCPU TLS jump frames with the original frame identity. | The helper functions return `jmp_buf *`, but historical C accepted that pointer where modern MSVC requires the referred-to `jmp_buf` array. The old form has an incompatible pointer level on both targets. | Dereference only at the three `setjmp` calls. The same TLS frame, exception path, simulation nesting and long-jump target remain unchanged. | `softpc.new/base/ccpu386/c_main.c` |
| MVDM-HOST-DIV-074 | Declare the selected EVID word-fill glue callback in the original EGA write handler table. | The original declaration says `IU8` while both `fill_word_ev_glue` and the selected `MEM_HANDLERS` slot use `IU16`; historical unprototyped table initialization masked the mismatch, but modern x64 correctly diagnoses an unsafe function-pointer ABI. | Change only that forward declaration to `IU16`, matching the existing original glue body and handler-slot contract. The function, table order and video behavior remain unchanged. | `softpc.new/base/video/ega_writ.c` |
| MVDM-HOST-DIV-141 | Read original `config_inquire` scalar options used by firmware, controllers and display initialization. | The original `void *` result carries either a real object pointer or a small configuration enum; direct casts from that carrier to 16/32-bit scalars truncate a native pointer representation on x64. | Retain `config_inquire` and all original option/result destinations; only scalar consumers first convert through `ULONG_PTR`, then perform their original narrow enum conversion. These values remain private host configuration, not guest or external identities. | `softpc.new/base/{bios/emm_fncs.c,system/rom.c,system/cmos.c,system/cmosnt.c,keymouse/keyba.c,video/video.c}`; `softpc.new/host/src/nt_graph.c` |
| MVDM-HOST-DIV-142 | Size and advance the original DOS ROM scratch-pad message. | `strlen` returns native `size_t`; original assignment to `IU32` can truncate before the existing single-page/scratch-pad guard on x64. | Retain the original guard, input truncation, byte write, `$`/NUL termination and final 32-bit `cur_loc`; use native-width temporary length/end values, and convert only after the guard has bounded the result to the ROM scratch range. | `softpc.new/base/system/rom.c` |
| MVDM-HOST-DIV-077 | Process the original XMS move descriptor and forward-copy its guest byte range. | The original 32-bit process model obtains `SS:BP` and the source/destination linear values as raw `PBYTE` aliases, then truncates those aliases through `ULONG`/`DWORD`. Neither architecture may use a host pointer as an Intel address. | Retain the original descriptor order (`destination`, `source`, word count), overflow rule, forward copy and AX=1 success result. A same-shaped SoftPC adapter reads the twelve-byte descriptor through a bounded lease, then routes its full-span forward overwrite through original numeric CCPU addressing. It never publishes a host alias or applies generic writable-lease hydration; it requests controlled session cancellation for an unrepresentable descriptor/range instead of inventing a guest XMS error code. | `xms.486/xmsblock.c`; `../ntvdm/softpc/include/mvdm_softpc_guest_memory.h`; `../ntvdm/softpc/mvdm_softpc_guest_memory.c` |
| MVDM-HOST-DIV-079 | Select the original CPU_40 C-video EVID string route instead of an unselected legacy `WRT_POINTERS` route. | The historical non-C-video WRT tables carry native string callback pointers through `ULONG`; on x64 those table initializers truncate pointers. The selected `CPU_40_STYLE` profile uses the original EVID tables instead. | Keep the original direct BIOS byte/word callbacks and table storage, but compile the uninstalled direct string move/copy helpers only outside `CPU_40_STYLE`. The selected EVID source and its table contract remain unchanged. | `softpc.new/base/video/egawrtm0.c` |
| MVDM-HOST-DIV-080 | Preserve the original exception-source environment declaration. | The source-era macro redeclared the mapped CRT environment query with mismatched qualifiers. The unused ntstubs facade was restored to original in T413. | Use the existing host_def.h declaration; keep every exception call and original provider. | `softpc.new/base/ccpu386/c_xcptn.c` |
| MVDM-HOST-DIV-143 | Select the original CPU_40 C-video EVID mode-1/mode-2 string route. | Historical mode-1/mode-2 WRT string callbacks are uninstalled under `CPU_40_STYLE` yet carry pointers through `ULONG`. The original private-buffer `ULONG` alignment expressions are restored for x86. | Compile the uninstalled WRT string helpers only outside `CPU_40_STYLE`; preserve selected EVID bodies, direct byte/word callbacks and original alignment expressions without introducing a guest-address mapping. | `softpc.new/base/video/egwrtm12.c` |
| MVDM-HOST-DIV-082 | Publish original CCPU FPU/address/debug and CRT call contracts. | The selected source package has existing `c_addr.h`, `fpu.c`, `yoda.h` and CRT declarations, but three translation units omit the declaration/import selected by their original calls. Modern C then compiles those calls as unchecked implicit forms. | Include only the original owning headers, retain the exact original `FLDENV` declaration shape, and include the standard CRT declaration. `yoda.h` retains the original `PROD` no-op `check_D` choice. No CPU/FPU/memory logic, mapping identity or provider is added. | `softpc.new/base/ccpu386/{zfrsrvd.c,c_page.c,c_reg.c}` |
| MVDM-HOST-DIV-083 | The original SoftPC support startup sequence calls selected `nt_msscs`, `nt_cpu`, and mouse providers without a common declaration carrier; the `CPU_40_STYLE` guard hid one historical declaration. | The selected CCPU40 support call site otherwise lacks the exact provider declarations. | Publish the exact existing `void` / `int` contracts at the original support call boundary. `nt_msscs.c` retains its original implicit-result spelling. Startup order, provider bodies, machine state, return values, and guest behavior remain unchanged. | `softpc.new/base/support/main.c`; `softpc.new/host/src/nt_msscs.c` |
| MVDM-HOST-DIV-084 | The original `nt_msscs` initialization path invokes selected PIF and EMS/DOS providers without including the existing PIF declaration or having a public LIM page-frame declaration carrier. | Their implicit result declarations are invalid on x86/x64. | Include the original `nt_pif.h` provider declaration and publish the exact original `emm_mngr.c` boolean/LIM configuration contract at the machine-initialization use site. No PIF/EMS logic, mapping, or startup order changes. | `softpc.new/host/src/nt_msscs.c` |
| MVDM-HOST-DIV-092 | Publish the selected CPU_40 direct SAS overwrite call. | The original `sas.h` moves to a generated function-vector contract under `CPU_40_STYLE`; the host call sites also need a correctly typed declaration for the external `sas_overwrite_memory` wrapper. The currently selected C-VID wrapper forwards through a vector to the CCPU provider, as recorded separately in DIV-189. Modern C consequently inferred an `int` result at original host call sites. | Add the exact existing `void (PHY_ADDR, PHY_ADDR)` declaration adjacent to the selected generated SAS declarations. The CCPU provider, physical-range semantics and call order remain unchanged. | `softpc.new/base/inc/sas.h` |
| MVDM-HOST-DIV-093 | Format the original private host allocation pointer safely. | The original `nt_mem` diagnostic prints native `IHP` storage with a 32-bit `%lx` format, which is invalid on x64. | Retain the failure path and all diagnostic values, but pass `commitAddr` as a native `%p` pointer. No allocation, guest address, mapping-manager or memory semantics change. | `softpc.new/host/src/nt_mem.c` |
| MVDM-HOST-DIV-095 | Format the retained `sas_manage_xms` stub diagnostic using its actual argument widths. | The original stub reports a native start pointer with `%lx`; x64 cannot pass that pointer through a 32-bit variadic conversion. | Retain the diagnostic and stub result; use `%p` for the private pointer and scalar conversions for its `ULONG` and `INT` arguments. No XMS/SAS behavior or mapping route changes. | `softpc.new/host/src/stubs.c` |
| MVDM-HOST-DIV-096 | Invoke the original EMS page-frame mapping services with a declared status/argument ABI. | `VdmMapDosMemory` and `VdmUnmapDosMemory` were NT4 kernel-VDM services; their user-mode source calls otherwise compile as implicit `int` on both host architectures. | Retain original page-number calculations, calls, status test, and failure result. Add only the source-shaped adapter declaration/import; the adapter implements a session-owned physical-page alias without host pointers. Bounded PID-correlated first-hit trace: `VdmMapDosMemory.call`, `map.*`, `VdmUnmapDosMemory.call`, and `unmap.*`. | `softpc.new/host/src/nt_emm.c`; `../ntvdm/softpc/include/mvdm_softpc_physical_mapping.h` |
| MVDM-HOST-DIV-097 | Resolve the original EMS page-frame physical alias before normal CCPU/SAS backing lookup. | NT4 kernel VDM performed this physical remap outside CCPU; without it a selected SoftPC build sees the frame address rather than the EMS backing page. | Preserve normal/external SAS lookup. Add one numeric, selector-blind translation probe owned by the same adapter; it neither exposes a host pointer nor changes CCPU instruction semantics. Bounded PID-correlated first-hit trace: `translate.alias-hit`. | `softpc.new/base/ccpu386/ccpusas4.c`; `../ntvdm/softpc/{include/mvdm_softpc_physical_mapping.h,mvdm_softpc_physical_mapping.c}` |
| MVDM-HOST-DIV-267 | Preserve original EMS host-copy dispatch when a conventional span crosses a session EMS alias. | Kernel VDM supplied a contiguous host alias; the selected CCPU40 provider correctly resolves each byte but cannot make `get_byte_addr(first)` represent a cross-window span. | Keep `emm_mngr` dispatch and each public `nt_emm` function. The original native fast path remains for unaliased RAM; only an overlapping alias span uses the existing bounded session lease, including overlap-safe move ordering. Lease failure returns the original host `FAILURE` result. Bounded PID-correlated first-hit trace: `ems-lease.load/store/move` and matching `*-result`; the trace is emitted only after the original alias-overlap predicate selects this missing-kernel seam. | `softpc.new/host/src/nt_emm.c`; `../ntvdm/softpc/{include/mvdm_softpc_guest_memory.h,include/mvdm_softpc_physical_mapping.h,mvdm_softpc_guest_memory.c,mvdm_softpc_physical_mapping.c}` |
| MVDM-HOST-DIV-085 | Selected BIOS tape and keyboard controller paths invoke the original XMS A20 providers but their inherited SAS conditional declaration is not effective in the selected compilation units. | Implicit host-int calls are invalid x86/x64 contracts. | Include the original XMS provider header at the two original call boundaries; retain the same A20 call ordering and original `xmsa20.c` implementation. | `softpc.new/base/keymouse/keyba.c`; `softpc.new/base/bios/tape_io.c` |
| MVDM-HOST-DIV-066 | Preserve original error-dialog sequence with public Win32 callback signatures. | SoftPC BOOL expands to UINT and does not match the signed WNDENUMPROC return; dialog callbacks also require exact modern signatures. | Keep typed callback/LPARAM boundaries. Restore original x86 PDWORD enumeration body, thread-ID casts and local int result; retain the separately registered WINAPI worker correction. | `softpc.new/host/src/nt_error.c` |
| MVDM-HOST-DIV-046 | Declare the selected CCPU flag-read and segment-load callbacks consistently across the generated CCPU declaration carrier and `nt_cprgs` selection table. | Original NT4 C builds accepted unprototyped function-pointer assignments; the generated header incorrectly uses `IBOOL`/`IUH` where `c_reg.c` defines `ISM32`. Modern MSVC correctly diagnoses the mismatch. | Keep the original CCPU function names, parameters, results and initialization order. Correct only the selected generated declarations and CCPU table slots to their actual `ISM32` contracts; alternate CPU selections retain their own original declarations. | `softpc.new/base/ccpu386/cpu4gen.h`, `softpc.new/host/src/nt_cprgs.c` |
| MVDM-HOST-DIV-047 | Bind every selected CCPU register/flag setter through the original SoftPC access table. | The original generic table uses old-style empty parameter lists, so NT4 C accepted setter assignments without proving the passed value width. | Preserve original callbacks and initialization order; give only the selected CCPU slots their existing generated `IU8`, `IU16` or `IBOOL` parameter shapes. Non-CCPU profiles retain their own original declarations. | `softpc.new/host/src/nt_cprgs.c` |
| MVDM-HOST-DIV-048 | Bind the selected CCPU control fields through their generated GDP names. | The historical non-PROD `GdpOffsetFromName` assertion is unavailable in the selected source set, and hard-coded packed offsets obscure the original field contract. | Retain the original x86 64 KiB contiguous GDP allocation and generated macro layout; `nt_cpu.c` names the selected generated fields directly and omits only the unavailable assertion. GDP is private SoftPC state and does not use the session mapping manager. | `softpc.new/base/ccpu386/cpu4gen.h`, `softpc.new/base/ccpu386/vglob.c`, `softpc.new/base/cvidc/{ev_glue.c,gdpvar.h}`, `softpc.new/host/src/nt_cpu.c` |
| MVDM-HOST-DIV-050 | Keep original timer and Redirector-to-PIC public forms while entering their selected typed NT/SoftPC contracts. | NT4 C accepted `IS32*` as `PULONG` and `int*` as `LONG*`; modern builds correctly reject the mismatches even though both carriers were historically 32-bit. | The timer uses a local `ULONG` remainder; existing `nt_eoi.c` uses a local `IS32` around original `ica_eoi`, preserving PIC ordering and the public `int*` form. | `softpc.new/host/src/nt_timer.c`, `softpc.new/host/src/nt_eoi.c` |
| MVDM-HOST-DIV-147 | Preserve the selected CCPU40 SAS-allocation failure path without ending an enclosing host process. | The retained `sim32.c` failure path can be reached while a bounded session is active; its historical `ExitProcess` cannot be used by that session boundary. | Replace only that failure's process exit with the same exit-code transfer to the bound session escape. The standalone `ntvdm.exe` worker retains the original `host_terminate` path through `ExitVDM` and process exit; `nt_reset.c` still has project-owned initialization and repeated-close guards, which are retained pending lifecycle and failure-path verification (see `artifacts/mvdm-audit/shard-1/div147-host-close-audit.md`). `nt_event.c` remains its original console-control form and is outside this divergence. | `softpc.new/host/src/sim32.c`; `../ntvdm/softpc/mvdm_softpc_termination.c` |
| MVDM-HOST-DIV-148 | Compile the original LIM byte/word/string writeback bodies needed when CCPU40 encounters `SAS_MM_LIM`. | `host_def.h` selects `LIM`, so the selected original CCPU calls `LIM_b_write`/`LIM_w_write`; the matching original bodies in `emm_mngr.c` are nevertheless excluded by its historical `#ifndef NTVDM` gate. | Retain the entire original writeback block unchanged, expanding only its outer compilation condition for the explicit selected CCPU40 recovery profile. No adapter, substitute write algorithm, or alternate EMS state is introduced. | `softpc.new/base/dos/emm_mngr.c`; selected build flag in `tools/build/New-T310OriginalSoftpcNinja.ps1` |
| MVDM-HOST-DIV-149 | Start a 32-bit COMMAND child with the original selected standard-stream endpoints. | NT4 temporarily changed its dedicated `ntvdm.exe` process-wide standard handles before `CreateProcess`; the current app owns those streams independently even in the admitted single-session profile. | Keep the original `SetStdHandle → CreateProcess → restore` order and parameters. A translation-unit-local Win32 adapter records those selected handles per worker and passes them only in a copied `STARTUPINFO` at the public child-creation boundary. | `dos/command/cmdexec.c`; `../adapter-mvdm-host-out/win32/{include/command_process_compat.h,source/command_process_compat.c}` |
| MVDM-HOST-DIV-067 | Call original same-package video selector and EGA helpers. | NT4 C accepted absent declarations as an implicit `int` contract, which prevents modern compilers from validating the real parameter and return ABI on either host architecture. | Add only exact declarations for the existing `setMarkPointers`, `Glue_set_vid_wrt_ptrs`, index-state, C_VID selector and `ega_read_dot` bodies. Their definitions, selection order and behavior are unchanged. | `softpc.new/base/video/ega_prts.c`, `vga_prts.c`, `cga.c`, `ega_read.c`, `video.c` |
| MVDM-HOST-DIV-087 | Invoke the original VM-instance cleanup and host stream-display update providers. | The split selected source layout carries neither historical declaration carrier, so NT4-era implicit `int` calls cannot validate their `void`/pointer-width ABI on x86 or x64. | Publish only the exact existing provider declarations at the two original call sites; preserve the patch `ClearInstanceDataMarking` provider, `nt_graph.c` display provider, and all caller ordering. | `softpc.new/base/bios/virtual.c`, `softpc.new/base/video/gfx_updt.c` |
| MVDM-HOST-DIV-088 | Publish the selected C-video EVID handler-table and selector contracts. | The original generic `MEM_HANDLERS` table erases the concrete EVID callback signatures, leaving its move-slot initializer incompatible; the selected writers/markers also call original selectors without declarations. | Under the existing `CPU_40_STYLE`/`C_VID` selection only, preserve the six callback shapes from `ev_glue.c`, publish exact selector contracts, and omit the unselected generic A2/debug table whose callback forms are not installed by this profile. Other historical machine selections retain the generic table. | `softpc.new/base/inc/gmi.h`, `softpc.new/base/video/{ega_writ.c,gfx_updt.c}` |
| MVDM-HOST-DIV-089 | Bind selected C-video read glue through the original read-handler table. | The historic `ULONG` table fields and implicit glue imports only agree with EVID's `IU32` forms by 32-bit typedef coincidence, so modern compilers cannot validate the selected call ABI. | Under the existing `CPU_40_STYLE`/`C_VID` selection, give `READ_POINTERS` and its three imports the exact `ev_glue.c` forms. Other historical selections retain their original carrier. | `softpc.new/base/inc/cpu_vid.h`, `softpc.new/base/video/ega_read.c` |
| MVDM-HOST-DIV-090 | Fill selected EGA private plane words through the original host `memset4` helper. | The base declaration used `ULONG *`, while the selected `copy_fnc.c` provider takes `unsigned int *`; their historical 32-bit width coincidence leaves a mismatched pointer contract on current MSVC. | Publish the provider's exact `unsigned int` data/pointer/count forms and use the matching private plane-word cast. The fill algorithm and private video-memory ownership are unchanged. | `softpc.new/base/inc/host.h`, `softpc.new/base/video/ega_vide.c` |
| MVDM-HOST-DIV-098 | Call the original SoftPC ICA lock around speaker state and device-handle transitions. | The historical product aggregate supplied the two `nt_eoi.h` contracts transitively; its owner-rooted source mirror no longer does, so modern C assumes an incompatible `int` return. Full `nt_eoi.h` also selects an unrelated monitor-only `VDMVIRTUALICA` global. | Add only the two original `void` declarations required by this source. The original sound/ICA bodies, lock ordering and failure behavior remain unchanged. | `softpc.new/host/src/nt_sound.c`, original declaration evidence `softpc.new/host/inc/nt_eoi.h` |
| MVDM-HOST-DIV-099 | Expose the original event-manager `PointerAttachedWindowed` external. | The original aggregate let C infer `int`; its provider actually defines the shared SoftPC `BOOL` state. | Publish only the existing `BOOL` declaration. No event, mouse or console behavior changes. | `softpc.new/host/inc/nt_event.h`, `softpc.new/host/src/nt_event.c` |
| MVDM-HOST-DIV-263 | Adapt the selected original stream-I/O Console mode to the public modern Console. | The standalone text-mode worker remains in stream I/O while the original Console Server path later enables mouse/window records. Modern QuickEdit can consume a physical click before the unchanged original event worker reads it. | In the existing `SetupConsoleMode` calculation, retain its source-derived input-mode subtraction and add public mouse/window plus extended-input flags while clearing QuickEdit. The original later `host_disable_stream_io` transition retains its source shape; shutdown restores the original saved mode. | `softpc.new/host/src/nt_hosts.c`; `softpc.new/host/src/nt_fulsc.c` |
| MVDM-HOST-DIV-465 | Close superseded inherited stdout/stderr after opening the worker Console output. | Inherited redirected pipe writers can keep EOF pending after COMMAND cleanup; the DOS request retains separately duplicated streams. | The selected source closes the old handles but the earlier active-output field still holds the original stdout value. A source-linked mocked redirected-output comparison confirms this stale value; real first use, handle alias/reuse, lifecycle and a safe repair remain to be verified. | `softpc.new/host/src/nt_hosts.c`; `artifacts/mvdm-audit/shard-0/nt-host-screen-lifetime-audit.md` |
| MVDM-HOST-DIV-264 | Observe the selected original native-Console mouse callback sequence. | A Console record and an INT 33 callback registration alone do not prove that the original guest callback transfer returns to `mouse_int2`. | Default-off markers copy already-owned scalar coordinates, condition masks and callback CS:IP immediately beside original code; they neither retain input nor change mouse, PIC, CPU, callback or guest state. | `softpc.new/host/src/nt_mouse.c`, `softpc.new/base/keymouse/mouse_io.c` |
| MVDM-HOST-DIV-265 | Enter original DOSX CMOS-shutdown continuations at their supplied CPU40 addresses. | The historical non-CPU30 condition subtracts two through a BOP-era convention, but `dxutil.asm` supplies `DXCODE:enrm50` in the code-9 IRET frame and `DXCODE:enrm45` for the code-0Ah far jump; both begin ordinary guest `FCLI` instructions. CPU40 begins execution at live EIP. | Preserve the historical adjustment outside CPU30/CPU40; under CPU40 alone, retain both original addresses unchanged. No BOP bytes, BOP decoding, continuation layout, or non-CPU40 branch changes. | `softpc.new/base/bios/reset.c`; original source `dpmi/dxutil.asm` |
| MVDM-HOST-DIV-100 | Notify the selected CCPU executor when an original SoftPC application thread starts or exits. | The original `ntthread.h` provider carrier was omitted by the owner-rooted host source, causing implicit `int` calls on both architectures; its unrelated `jmp_buf` declarations require executor-private state not used here. | Add only the two existing original `void` declarations; keep original calls, thread lifetime and failure flow. | `softpc.new/host/src/nt_thred.c`, original declaration evidence `softpc.new/base/ccpu386/ntthread.h` |
| MVDM-HOST-DIV-102 | Publish selected CCPU common register-vector contracts. | NT4 common host_cpu.h, nt_cprgs.c storage and direct nt_aorc.c wrappers use empty parameter lists/default int, while selected c_reg.c providers use precise register, flag, segment and descriptor-result contracts. Those unchecked calls are not a stable x86/x64 ABI. | Preserve all source-named callbacks, operations and dispatch order; make only selected CCPU IU8/IU16/IU32/IBOOL and ISM32 declarations explicit. Guest-width narrowing warnings remain visible and unchanged. | softpc.new/host/inc/host_cpu.h; softpc.new/host/src/nt_cprgs.c; softpc.new/host/src/nt_aorc.c |
| MVDM-HOST-DIV-107 | Represent the DOS Drive Parameter Block next link in its guest-defined 16:16 form. | The original `struct A_DPB *` is a 32-bit x86 process-pointer coincidence; on x64 it widens the packet and corrupts the following DOS fields. | Keep the original field order and writes, but store `Next` as the guest's four-byte far value; terminal and link values remain `FFFFFFFFh` and `segment:offset`. | `dos/dem/demdasd.h`, `dos/dem/demgset.c` |
| MVDM-HOST-DIV-109 | Start the original COMMAND child-execution worker under the correct x86/x64 callback and session contract. | `cmdCreateProcess` is a `void`, cdecl no-argument worker but historical source casts it to `LPTHREAD_START_ROUTINE`; x64 masks the mismatch and the raw worker has no session binding for mapped handles. | Retain the original worker body and `CreateThread` call ordering; a translation-unit-local adapter binding invokes a same-shaped void/cdecl bridge that establishes the creator session only for worker execution, carries a bounded source spelling for a default-off disposal diagnostic, and returns `DWORD 0` to Win32. | `dos/command/cmdexec.c`; `../adapter-mvdm-host-out/win32/{include/thread_start_compat.h,source/thread_start_compat.c}` |
| MVDM-HOST-DIV-111 | Retain COMMAND's SCSINFO, `SCS_ToSync`, `SCS_Is_Dos_Binary`, and `SCS_FDACCESS` source field addresses. | NT4 retained `GetVDMAddr` process aliases and derived real-mode values by truncating those host pointers; neither representation is safe across current x86/x64 session boundaries. | Preserve the packed source structure, field offsets, byte/word updates, and unsigned-word wrap. The mirror records each original 16:16 location once; the adapter takes a bounded session guest-memory lease only while a COMMAND, DEM, disk, or floppy consumer operates. A missing/stale location follows that caller's existing error or termination direction. | `dos/command/{cmd.h,cmddata.c,cmdexec.c,cmdmisc.c,cmdpif.c}`, `dos/dem/demmisc.c`, `softpc.new/host/{inc/nt_fdisk.h,src/nt_fdisk.c,src/nt_rflop.c}`, `../ntvdm/softpc/{include/mvdm_command_guest_state.h,mvdm_command_guest_state.c}` |
| MVDM-HOST-DIV-112 | State original COMMAND binary/registry calls with their public Win32 parameter types. | RegQueryValueEx requires LPBYTE and GetBinaryType requires LPDWORD rather than LPLONG. | Preserve original ULONG lengths, commands, registry bytes, classification and 128-byte tail admission. Keep only the typed Win32 arguments and other registered boundary changes. | `dos/command/cmdexec.c` |
| MVDM-HOST-DIV-114 | Read keyboard-layout registry values and return the original `KEYB.COM` command-tail packet. | RegQueryValueEx requires binary LPBYTE buffers; the one-byte guest tail must not silently wrap. | Preserve original INT lengths and buffer/layout order, state LPBYTE arguments, and retain the checked NoInstallkb16 failure direction and final CHAR conversion. | `dos/command/cmdkeyb.c` |
| MVDM-HOST-DIV-160 | Receive the current host directory returned through original `VDMINFO`. | The original `cmdGetNextCmd` advertises `MAX_PATH + 1` bytes but allocates only the guest-visible 64-byte DOS-directory limit, which corrupts the stack when an installed package root is longer. | Retain the original BaseSrv request and host `SetCurrentDirectory` behavior; enlarge only the host-local receiving buffer to exactly the declared `MAX_PATH + 1` capacity. | `dos/command/cmdmisc.c` |
| MVDM-HOST-DIV-116 | Report an original CCPU simulation-buffer allocation failure. | The original `%d` diagnostic receives `sizeof(ThreadSimBuf)`, which is `size_t` and can be wider than `int` on x64. | Preserve the original allocation, failure branch and diagnostic fields; use C's matching `%zu` conversion on both host architectures. | `softpc.new/base/ccpu386/ntthread.c` |
| MVDM-HOST-DIV-128 | Return a typed failure value from the original CCPU exception-frame accessor. | The original `jmp_buf *ccpu386ThrdExptnPtr()` used a bare `return` on its invalid TLS and missing-TLS paths, producing an undefined pointer result. | Preserve diagnostics, failure branches, TLS lookup and normal frame selection; return the same typed null frame used by sibling `ccpu386SimulatePtr()` failure paths. | `softpc.new/base/ccpu386/ntthread.c` |
| MVDM-HOST-DIV-144 | Retain the original local `DbgBreakPoint` replacement used by the process-level SoftPC entry. | The selected modern NTDLL declaration is `NTAPI`, while the source definition omitted that historical calling-convention spelling; x86 would otherwise expose two incompatible declarations. | Add only the matching `NTAPI` declaration modifier. The original debugger-port check, free-build no-op and `DebugBreak` behavior remain unchanged. | `softpc.new/obj.vdm/ntvdm.c`; declaration carrier `../opennt-abi/host-compat/include/nt.h` |
| MVDM-HOST-DIV-129 | Publish the original alternate-CPU interrupt dispatch result ABI. | `a3_cpu_interrupt` has no result path but used an implicit `int` declaration, which modern C rejects and can misdescribe its call result. | Retain switch cases, interrupt delivery and default diagnostic; state the existing `VOID(int, IU16)` callable form. | `softpc.new/base/ccpu386/ntstubs.c` |
| MVDM-HOST-DIV-130 | Preserve bounded FPU stack-index arithmetic across host widths. | Subtracting two native pointers produces a host-width `ptrdiff_t`, while the original FPU register index is fixed-width `IU32`. | Preserve the original eight-entry FPU stack and `IU32` register computation; state that the bounded private stack offset is converted before addition and is neither a guest address nor host identity. | `softpc.new/base/ccpu386/fpu.c` |
| MVDM-HOST-DIV-131 | Publish the original reserved-NPX dispatch input ABI. | `ZFRSRVD` used a K&R declaration that hides its fixed-width instruction argument from modern C call checking. | Retain the original `IU32` parameter, instruction table and prologue order; express the same signature with a modern prototype. | `softpc.new/base/ccpu386/zfrsrvd.c` |
| MVDM-HOST-DIV-133 | Keep the original resource-write retry inside the app-selected firmware root. | After `host_find_file` had resolved an existing resource, the NT4 retry used only the bare name and could create/write a same-named file in the process current directory. The current product's app/session contract owns resource roots and must not silently escape them. | Preserve the original lookup, first open, retry/create attempt and `host_error` ordering; retry using the same resolved `hff_ret` path rather than bare `name`. No resource bytes, guest state or component ABI changes. | `softpc.new/host/src/nt_rez.c` |
| MVDM-HOST-DIV-134 | Create zero-length `C:\\MSDOS.SYS` and `C:\\IO.SYS` placeholders during a first session for a historical Brief installer probe. | These are installation-only faux boot markers. The selected product may project real host drives but must never mutate a real host boot root merely to satisfy this probe. | Retain the original `scs_init → AddSystemFiles` order and void contract. A selected-profile macro transfers only this side effect to the same-shaped adapter disposition; the complete original body remains in the mirror under the alternate compile branch. | `softpc.new/host/src/nt_msscs.c`; `../ntvdm/softpc/{include/mvdm_softpc_firmware.h,mvdm_softpc_firmware.c}` |
| MVDM-HOST-DIV-119 | Retain the packed original SCS/standard-handle guest ABI while storing COMMAND pipe, thread, critical-section and redirection records as native host allocations. | The original `#pragma pack(1)` remained active past the guest records. x86 tolerated unaligned host fields; x64 reports each address passed to a Win32 synchronization API and the layout is not a guest-visible contract. | Close the packing scope immediately after `STD_HANDLES`; preserve the byte-exact guest structures and leave the later host-only structs naturally aligned. No source algorithm, BOP field, mapping identity, or handle route changes. | `dos/command/cmd.h` |
| MVDM-HOST-DIV-120 | Start original COMMAND pipe workers through the modern `CreateThread` callback contract. | `cmdPipeInThread` and `cmdPipeOutThread` are cdecl `void(LPVOID)` bodies, but the NT4 source force-casts them to `LPTHREAD_START_ROUTINE`; this is ABI-unsafe on x86 and loses the creator session binding. | Keep both original worker bodies and call ordering. A same-shaped adapter captures the current session, calls the cdecl worker through a WINAPI thunk, carries a bounded source spelling only for default-off disposal attribution, then returns `DWORD 0` only at the Win32 boundary. | `dos/command/cmdredir.c`; `../adapter-mvdm-host-out/win32/{include/thread_start_compat.h,source/thread_start_compat.c}` |
| MVDM-HOST-DIV-121 | Declare the selected generated C-video mark rules with their real native host-word ABI. | `evidfunc.h` declared 17 generated `S_*Mark*` definitions with implicit `int` returns and obsolete table-facing argument lists. The generated rule definitions actually take four `IUH` words and return `IUH`; this is a call ABI error even in the selected x86 profile. | Retain the original generated rule names, tables and bodies. Correct only the mirror declarations to the exact existing four-word `IUH` contract, so the original `evidfunc.c` table definitions compile directly. | `softpc.new/base/cvidc/evidfunc.h` |
| MVDM-HOST-DIV-122 | Format generated C-video native host words with their native variadic shape. | The selected C-video rule profile widens `IUH` to `UINT_PTR`, but five diagnostic-only `printf` calls retained `%d`, which is a varargs ABI mismatch on x64. | Use the standard `PRIuPTR` format with an explicit `uintptr_t` view only at those diagnostic calls. Rule arithmetic, table selection, video state, and guest values remain unchanged. | `softpc.new/base/cvidc/{ev_glue.c,j_c_lang.c}` |
| MVDM-HOST-DIV-123 | Preserve generated C-video native-word bit-test width. | Two generated rule paths use an `IUH` bitfield and an `8*sizeof(IUH)` guard, but leave the literal `1` as a 32-bit `int`; x64 then performs a 32-bit shift before the native-word bit test. | Keep the original rule IDs, guard, fallthrough and result paths. Type only the literal as `IUH`, so the generated bit test follows its existing native-word carrier on x86 and x64. | `softpc.new/base/cvidc/{SEVID019.c,SEVID020.c}` |
| MVDM-HOST-DIV-125 (retired) | Original instruction-IP bookkeeping. | The retired x64 delta rewrite also incorrectly masked only the USE16 delta rather than the final sum. | T413 S4 restores all original delta/update expressions, removes the helper, and verifies original-source identity, wrap/control/page/return tests. | `softpc.new/base/ccpu386/c_main.c` |
| MVDM-HOST-DIV-127 | Publish the original CCPU extended fast-BOP fallback call contract. | The selected CCPU source calls `EDL_fast_bop(ULONG)` but no selected OpenNT definition exists, so modern C infers an invalid `int` result. | Declare the exact `void(ULONG)` form at the original caller. Missing provider bodies remain in the named SoftPC adapter boundary rather than adding a mirror file; no BOP semantics or adapter route is introduced. | `softpc.new/base/ccpu386/c_main.c`; `../../ntvdm/softpc/mvdm_softpc_ccpu_fallback.c`; provenance: `O:/repos.external/ntvdmx64` pinned patch. |
| MVDM-HOST-DIV-135 | State the selected original C-VID mark callback result contract. | The historic `UPDATE_ALG` table declared `boolean` results although all selected CGA/EGA mark providers are `VOID` and every reached caller discards that result. The old casts obscure the actual CCPU40 callback contract. | State the five callback forms as `void`, make the local simple marker `VOID`, and retain the existing side effect and table order. No display endpoint, guest address form, or controller state algorithm changes. | `softpc.new/base/inc/gfx_upd.h`; `softpc.new/base/video/gfx_updt.c` |
| MVDM-HOST-DIV-136 | Publish original EGA write-provider parameter contracts. | The original writer tables used empty K&R declarations at local and imported provider boundaries, so modern MSVC could not check the selected value/offset/count forms. | Keep bodies and table order unchanged; replace only those declarations with the exact original later-definition `IPT2`/`IPT3`/`IPT4` signatures. | `softpc.new/base/video/{ega_writ.c,egawrtm0.c,egwrtm12.c}` |
| MVDM-HOST-DIV-137 | Provide selected CCPU shared Gdp/Cpu/Video state without a duplicate SAS vector. | Historical `localfm.c` declares all four globals, but selected `ccpusas4.c` already owns the original `Sas` vector. Linking both would make two SAS states. | Existing `localfm.c` preserves the original Gdp, Cpu and Video declaration spellings and omits only duplicate `Sas`; no algorithm, guest value or host identity is added. | `softpc.new/base/ccpu386/{localfm.c,ccpusas4.c}` |
| MVDM-HOST-DIV-156 | Recover the omitted generated CCPU-to-C-VID vector binder. | The retained CCPU40 manifests contain original slot metadata but omit the historical generated assembly unit; generic include order also selected an incompatible CCPU vector layout for the shared carrier. | Named SoftPC adapter expands original `c2cpusad.h` into typed assignments at the existing setup point; unavailable CPU slots remain explicit. | `softpc.new/base/cvidc/ev_glue.c`; `../ntvdm/softpc/mvdm_cvidc_vector_binding.c`; `../../tools/build/GenerateCvidcCpuBinding.mjs` |
| MVDM-HOST-DIV-105 | Pass the original file-position high offset into the Win32 SetFilePointer call. | NT4 accepted an unsigned ULONG pointer where the public contract requires signed LONG; same-width coincidence hid the incompatible call ABI. | Retain the zero high-offset value and original seek ordering, but declare its local carrier as LONG at both calls. | dos/dem/demhndl.c |
| MVDM-HOST-DIV-154 | Initialize original DEM's DOS directory carrier. | NT4 obtains it from the installed system directory; the product selects immutable root/system32 per session, matching original deployment rather than a flat package root. | Retain DEM's caller-owned directory buffer, original MAX_PATH directory bound and all subsequent path/file logic; fill it through the session-system-directory adapter rather than host `GetSystemDirectory`. | dos/dem/dem.c; ../ntvdm-exe/softpc/{include/mvdm_softpc_firmware.h,mvdm_softpc_firmware.c} |
| MVDM-HOST-DIV-157 | Select original `config.nt` and `autoexec.nt`, including PIF overrides. | Modern Windows has no NT4 system media. The selected CCPU/LIM composition reads CONFIG first for EMS setup, then for DOS boot; the original consuming lookup loses the override between those callers. | Keep original path expansion, caller buffer and error handling. Bind the no-PIF default through the system-media lookup. Retain override strings through both reads, free them before replacement by another PIF, and let process teardown reclaim the final worker-local selection. No new parser or configuration policy. | `softpc.new/host/src/nt_pif.c`; `../ntvdm-exe/softpc/mvdm_softpc_firmware.c` |
| MVDM-HOST-DIV-158 | Expand original CONFIG.NT/AUTOEXEC.NT `%SystemRoot%` paths and write their synthesized COUNTRY/SHELL lines. | NT4 uses its installed Windows directory; the product must instead use the app-selected immutable MVDM system root and must not depend on host installation files. | Preserve original short-path normalization, PIF/config parsing, generated line order, temporary-file ownership and failure direction. Bind only the root acquisition through the existing system-media adapter; stage selected original system-directory-shaped media below `mvdm/system32`. | dos/command/cmdconf.c; ../ntvdm/softpc/{include/mvdm_softpc_firmware.h,mvdm_softpc_firmware.c}; ../../tools/build/Stage-OriginalSoftpcRuntime.mjs |
| MVDM-HOST-DIV-148 | Bind static original Registry reads to a private layered handle. | `NTVDM.REG` overrides only an admitted read-only current-host Registry snapshot; no original caller receives a native handle or may write HKLM. | Preserve each selected original caller's open/query/enumerate/close ordering and fallback/error logic; translation-unit-local bindings expose the overlay and cached host values. Hardware discovery and mutable WOW registry families remain separate dispositions. | `softpc.new/host/src/nt_msscs.c` and S45 receiver matrix; `../ntvdm-exe/softpc/{include/mvdm_shadow_registry.h,mvdm_shadow_registry.c}` |
| MVDM-HOST-DIV-159 (retired diagnostic; include remains) | Historical top-level exception identity observation. | The reporting call was removed in T420 S13 (`a2ec4d531`); the declaration-only include and inline marker remain in `nt_timer.c`. | Retain the historical reference without claiming an active exception writer or changing the original filter/termination order. | `softpc.new/host/src/nt_timer.c`; historical `docs/etc/evidence/m0-t420-s13-dem-and-residual-diff-recovery.md` |
| MVDM-HOST-DIV-162 | Bind the selected CCPU40 C-VID video function-vector before SoftPC configuration begins. | The retained source packages omit the historical generated pre-configuration binder. `config()` reaches EGA setup before the original later `setup_vga_globals()` hook, leaving the C-VID `Video` vector null. | Preserve the original `gfi_init → config` order; insert one declaration-only adapter seam and one idempotent video-only binder call between them. The full CCPU vector is still bound only at the original post-CPU initialization location. | `softpc.new/base/support/main.c`; `../ntvdm/softpc/{include/mvdm_cvidc_vector_binding.h,mvdm_cvidc_vector_binding.c}` |
| MVDM-HOST-DIV-163 | Provide the CCPU quick-event calibration/restart slots referenced by the selected C-VID CPU table. | The retained C-VID generated `accessfn.c` contains the public forwarding shims but the paired generated timing provider is absent. Binding those slots back to the shims recurses indefinitely. | The named SoftPC adapter provides only the otherwise absent table endpoints: calibration reports the source-shaped zero fallback, while restart get/set use selected original `qevnt.c::qevJumpRestart` (initial value 100). Thus the adapter owns no duplicate quick-event state. The generated table binds exactly those slots to the provider; all other retained table entries keep their original mapping. | `../tools/build/GenerateCvidcCpuBinding.mjs`; `../ntvdm/softpc/mvdm_cvidc_vector_binding.c` |
| MVDM-HOST-DIV-164 | Observe an already decoded original DEM, COMMAND, or DPMI BOP at its original ingress and, for COMMAND, its already-completed host return. | The non-debug product has no usable debugger or historical trace shell; the BOP adapter must distinguish a reached original ingress from an invented dispatcher, and the DPMI route must expose an existing `53:xx` scalar without a second guest-memory read. | The adapter emits one ingress record only after original `MS_bop_0` or `MS_bop_4` has decoded its service byte and before original `DemDispatch` or `CmdDispatch`; original `DpmiDispatch` records its already-decoded `Index` only after its unchanged one-byte `IP` advance and before table validation/call. For the existing fatal DEM `50:3D` record only, it additionally copies the already-live 16-bit `CS:IP` and `DS:DX` words. For original COMMAND only, it emits a second record immediately after the unchanged `CmdDispatch` and original `setIP(+1)` sequence; that record copies already-live `CS:IP`, `AX`, `CF`, and `IF`. It is default-off: only the fixed observer's child-only report path receives it, so the guest-facing Console is never used for project diagnostics. It does not read guest memory, route a selector, alter the provider result, IP ordering, CPU, guest, session, selector, service, return or failure state. | dpmi32/dpmi32.c; softpc.new/host/src/nt_bop.c; ../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c} |
| MVDM-HOST-DIV-214 | Preserve CPU40 host-thread event notifications while CPU40 consumes another event. | Original ordinary event-map RMW operations can erase concurrent notifications; a blanket replacement of every observation by a consuming operation also loses HALT RESET. | Atomic snapshot/raise/CAS retain bit identity. HALT only observes RESET/INTR; the normal RESET handler consumes RESET before resetting. Timer/SAD consume before callback. The selected IRQ latch is consumed before PIC acknowledgement, unlike original post-ack clear, so a subsequent raise is not erased by that clear; ICA still owns vector/priority and signed rejection. Do not claim this transport ordering is byte-equivalent to upstream or that latch-only tests prove full PIC execution. | softpc.new/base/ccpu386/c_main.c |
| MVDM-HOST-DIV-330 | Yield a halted CCPU40 guest until the existing pending-event map changes. | The original ring-0 `HLT` loop repeatedly polls its event map, consuming one host CPU even when the guest has no work. | A process-local auto-reset carrier is created before timer/event producers start. `c_cpu_interrupt` first retains the original event bit then signals that carrier; after its unchanged reset/IRQ/timer/quick-event checks, HLT waits only when the map and quick-event count are empty. The event neither defines work nor changes its consumption order. It is not the original `IdleEvent`, does not alter idle accounting, and is closed only after the original host has stopped producers. | softpc.new/base/ccpu386/c_main.c; ntvdm-exe/softpc/mvdm_softpc_ccpu_wait.c |
| MVDM-HOST-DIV-329 | Keep the existing default-off WOW source-descriptor diagnostic out of ordinary CCPU SAS stores. | The project-added diagnostic called `GetEnvironmentVariableA` twice at every byte/word/dword SAS store merely to discover that its optional trace was disabled; a normal WOW XMS copy therefore entered the Windows environment path millions of times. | The standalone worker reads and validates the inherited trace path/selector once before CCPU execution. The five existing CCPU hook sites make one process-local active-flag check; when inactive they make no helper, environment, file, selector, or guest-memory call. When active the existing filter and record shape remain unchanged. Trace configuration is launch-time only, as inherited environment configuration already was in supported observations. | softpc.new/base/ccpu386/ccpusas4.c; ntvdm-exe/softpc/{mvdm_standalone_worker.c,include/mvdm_softpc_termination.h,mvdm_softpc_termination.c} |
| MVDM-HOST-DIV-215 | Identify the original guest keyboard-read boundary for a fixed external Console observation. | COMMAND `54:0E` initializes keyboard/Console state but can return while startup code still has interrupts masked; it is not proof that guest `INT 16h` is awaiting a key. The formal `COMMAND.COM` child reaches the NTVDM-specific `AH=1` status-poll edge rather than `AH=2` waitio. | At original `keyboard_io` cases `AH=0`, `AH=1` and `AH=2`, immediately before their unchanged idle calls, emit one default-off child-only marker. The `AH=0`/`AH=1` report is latched once because source polling can repeat. The observer then uses ordinary `WriteConsoleInput`; it does not inject guest data, route a BOP, alter CPU/PIC/BIOS state or change idling. | `softpc.new/base/keymouse/keybd_io.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-219 | Attribute the original SoftPC idle-event wake after second `COMMAND.COM` reaches DOS `CON` line input. | A successful Console→8042 admission does not itself prove that the CPU leaves its source-owned idle wait; without this distinction an IRQ/IF diagnosis would be speculative. | After the existing source event signal and around the existing `host_release_timeslice` wait, emit bounded host-local scalar observations only after the original `INT 21h/AH=0Ah` marker. The observer neither selects, signals, resets nor waits on an event; it does not modify CPU/PIC/BIOS/guest state. | `softpc.new/host/src/nt_unix.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-325 | Attribute original Windows 3.1 Setup serial/parallel hardware-probe boundaries in the CCPU40 host. | Setup's hardware-collection stall occurs before any delivery PATCH finalizer, but the visual UI cannot distinguish a BIOS `INT 14h`/`INT 17h` loop from a host COM open or the historical VDM parallel-status IOCTL. | When the explicit diagnostic environment variable is set, append a bounded scalar entry/exit record around the existing BIOS boundaries and host device-open/status boundary. The observer neither selects, opens, closes, emulates, suppresses nor changes serial/parallel hardware, guest registers, BIOS return values, Setup arguments, timing policy or lifecycle. | `softpc.new/base/comms/{rs232_io.c,printer_.c}`, `softpc.new/host/src/{nt_com.c,nt_lpt.c}`, `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-325 (INT15 and keyboard guest-stack observations) | Attribute caller stacks at the original BIOS INT 15 and keyboard BOP boundaries. | The added observations evaluate guest SS:SP through effective_addr and sas_w_at before the shared diagnostic reporter checks its opt-in setting. | Current source therefore performs extra guest reads even with output disabled. Source-linked INT15 PROD/PM-off comparison observes two versus zero original reads; actual guest-visible side effects remain unproven. Review these observations as a cohort with DIV-258 and gate or remove only after original behavior and error-path validation. | `softpc.new/base/bios/tape_io.c`; `softpc.new/base/keymouse/keybd_io.c`; `artifacts/mvdm-audit/shard-1/int15-diagnostic-guest-read-audit.md` |
| MVDM-HOST-DIV-327 | Attribute the original COMMAND first-WOW classification boundary while diagnosing the post-copy Windows 3.1 Setup handoff. | The normal Setup page can only show a quiescent real-mode machine; it cannot distinguish an absent `KRNL286.EXE` classification from the source-owned compatibility return that permits the first WOW executable. | With an explicit host-only trace path, copy at most 32 already-computed scalar records: binary type, `VDMForWOW`, first-WOW flag and first-call flag, plus whether the existing first-WOW branch was selected. A separate `lmsw.c` before/after CR0 witness uses the same inline ID, is controlled by `MVDM_WIN31_SETUP_MODE_TRACE_PATH` and emits at most 64 records per thread; it leaves original CR0 operations in place. This witness is distinct from the CALL/RETF/IRET stack repair registered under the same ID above. It neither copies the guest pathname nor changes any original return, flag, command, session or process decision. | `dos/command/cmdexec.c`; `softpc.new/base/ccpu386/lmsw.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-328 | Attribute the original real-mode illegal-op frame and execution window during Setup diagnosis. | The error panel does not itself identify the predecessor guest frame. | After the original four frame-byte reads, the current reporter arguments unconditionally repeat two stack reads and add two more before its opt-in check. A controlled source-body comparison confirms original four reads versus current eight with the same mocked panel, two writes and continuation; guest-visible side effects remain unproven. The separately selected instruction-window diagnostic also needs its own enabled/disabled review. | `softpc.new/base/{system/illegalp.c,ccpu386/c_main.c}`; `../ntvdm-exe/softpc/mvdm_softpc_termination.c`; `artifacts/mvdm-audit/shard-0/illegal-op-diagnostic-audit.md` |
| MVDM-HOST-DIV-221 | Preserve the original PIC's rejected hardware-interrupt acknowledgement. | `ica_intack()` can return its original signed `-1` rejection after a host-thread IRQ notification becomes stale before CPU40 accepts it. Assigning that value directly to the historical unsigned vector carrier turns the rejection into a fabricated interrupt `FFh`. | Preserve the original acknowledge call and every successful vector's transfer/order. Hold the result in a signed local first; only a nonnegative acknowledgement is narrowed into the original vector carrier and transferred. | `softpc.new/base/ccpu386/c_main.c` |
| MVDM-HOST-DIV-222 | Enter a CPU40 INT33 direct guest callback at the registered first instruction. | The recovered non-CPU30 source subtracts `HOST_BOP_IP_FUDGE` under an assumption that the callback is preceded by a CCPU BOP. EDIT registers `371A:0223`; runtime bytes prove `0221` is ordinary `ADD AL,[BX+SI]` and `0223` begins the callback prologue. Selected CCPU starts at live EIP, so the stale adjustment modifies callback input state before its first instruction. | Only `jump_to_user_subroutine` selects the unadjusted registered offset for CPU40, matching the CPU30 direct-callback form. It does not change CCPU, its BOP decoder, `HOST_BOP_IP_FUDGE`, CPU30, callback ABI, event routing, buttons, coordinates, or any other BOP caller. | `softpc.new/base/keymouse/mouse_io.c`; P12 evidence `docs/etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md` |
| MVDM-HOST-DIV-165 | Make selected SoftPC BIOS/video ROM inputs resident before CPU40 firmware calls. | Historical NTVDM+X86GFX assumes the kernel-VDM shell has installed ROM bytes; this product lacks that shell. | Select the original generic rom_init body and retain its copy/protection ordering. Current host_read_resource dispatches ROM requests to the main-module embedded RT_RCDATA provider, not host_find_file; unknown, missing or oversized resources return zero, while rom_init ignores read_rom results. Downstream failure on missing required resources remains unverified. | `softpc.new/base/system/rom.c`; `softpc.new/host/src/nt_rez.c`; `../ntvdm-exe/softpc/mvdm_softpc_firmware.c`; `artifacts/mvdm-audit/shard-1/rom-residency-xt-audit.md` |
| MVDM-HOST-DIV-166 | Attribute an already-selected original top-level VDM return in the fixed non-debug observation container. | After ROM recovery, an unchanged terminal status no longer reached the existing known-thread exception report. Without distinguishing `main` return from direct process/thread termination, the next machine repair would be a guess. | Immediately before the original `main` returns, call a diagnostic-only adapter that writes its already-selected `int` result only when the fixed observer supplies a child-only report path. It does not alter SEH, return, guest state, process lifetime or normal product behavior. | softpc.new/obj.vdm/ntvdm.c; ../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}; ../../tests/observation/console_startup_observer.c |
| MVDM-HOST-DIV-167 | Retain the original async named-pipe request's guest buffer and result locations until host I/O completes. | NT4 stored durable flat `GetVDMAddr` aliases and passed its guest buffer to an asynchronous worker. A session guest lease cannot cross the BOP or worker lifetime. | Preserve the original packed 16:16 values, request queue, completion order and SoftPC ICA route. The Redirector adapter snapshots writes or allocates read staging at submission, then takes fresh leases only to write completion words and read bytes. Stale session memory is discarded, never published as a synthetic completion. | `inc/vrnmpipe.h`, `vdmredir/vrnmpipe.c`; `../ntvdm/redir/{include/mvdm_redirector_async.h,mvdm_redirector_async.c}`; existing `../adapter-mvdm-host-out/{softpc/include/mvdm_guest_location.h,win32/include/thread_start_compat.h}` |
| MVDM-HOST-DIV-173 | Start `VrpAsyncNmPipeThread` through the original `CreateThread` call. | The original cdecl `DWORD(LPVOID)` callback is incompatible with the WINAPI callback ABI on x86; x64 compatibility is incidental. | Keep the original worker body, call expression, arguments and failure branch. The existing same-shaped Win32 thread facade makes only the cdecl-to-WINAPI transition and binds the creator session during the worker lifetime. | `vdmredir/vrnmpipe.c`; `../adapter-mvdm-host-out/win32/{include/thread_start_compat.h,source/thread_start_compat.c}` |
| MVDM-HOST-DIV-175 | Start the original DLC receive-event worker through its original `CreateThread` call. | Its historical cdecl `DWORD(LPVOID)` entry is incompatible with the modern x86 WINAPI callback ABI. | Retain the worker body, original call expression, arguments and error branch; the existing translation-unit-local thread facade performs only the cdecl-to-WINAPI boundary and binds the creator session for the worker lifetime. | `vdmredir/vrdlcpst.c`; `../adapter-mvdm-host-out/win32/{include/thread_start_compat.h,source/thread_start_compat.c}` |
| MVDM-HOST-DIV-176 | Bind original SoftPC graphics DIB output without the NT4 Console Server graphics-buffer protocol. | `graphicsResize` requests `CONSOLE_GRAPHICS_BUFFER`, whose private server implementation supplies `lpBitMap` and a mutex. Modern public Console APIs cannot do this. | Retain the original DIB allocation, palette, resize, CGA/EGA/VGA pixel writers, invalidation calls and handle-close order. Replace only the private allocation call with a same-shaped adapter binding that supplies a bounded session-owned DIB byte plane and duplicate output handle; app sees snapshots only. | `softpc.new/host/src/nt_graph.c`; `../adapter-mvdm-host-out/win32/{include/conapi.h,source/console_compat.c}`; `../session/session.[ch]` |
| MVDM-HOST-DIV-177 (retired COMMAND diagnostic) | Historical COMMAND table-call observation only. | The diagnostic calls were removed in T416 S4 (`52d701d6b`). | Retained for historical traceability; original dispatch and the independently active SIM32 DIV-177 remain. | Historical `dos/command/{cmddisp.c,cmdmisc.c}` |
| MVDM-HOST-DIV-202 | Determine whether the source-owned resident `LodCom_Entry` jump-table binding, HMA/A20 precondition, and post-environment-return execution window remain valid. | `EndInit` transfers through `LodCom_Trap -> LodCom_Entry`; scalar BOP records alone cannot distinguish an invalid resident far pointer, an HMA flag/A20 mismatch, or a later control failure. | After the unchanged `SVC_GETINITENVIRONMENT` table call, a default-off helper reads only `TrnLodCom1_Entry`, `LodCom_Entry`, `MsgRetrv_Entry`, `ComInHMA`, the source-defined `PDB_Version` word at resident PSP offset `0040h`, and eight bytes at the source-defined `LodCom` HMA target through short session mapping leases; it also queries the existing CPU40 A20-wrap scalar. At each already completed `54:0F` BOP return it copies a 12-byte window from that exact post-service CS:IP. The PDB word is startup-state evidence, not an assertion that the observer has proven the `INT 21h/AH=30h` return. It cannot retain a guest alias, write guest memory, alter CPU/BOP state, or route COMMAND. | `dos/command/cmddisp.c`; `softpc.new/host/src/nt_bop.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c,mvdm_guest_location.c}` |
| MVDM-HOST-DIV-203 | Observe the original CCPU `BOP FE` CPU-unwind boundary. | Fixed-container evidence can show that `host_main` is inside original cleanup but cannot otherwise identify the already-decoded guest return site that selected the original `c_cpu_unsimulate` transition. | Immediately before the unchanged CPU40 `c_cpu_unsimulate()` call, copy only the already-live post-decode CS:IP to the existing child-only BOP report when configured. The hook neither routes, resumes, stops, changes registers, writes guest memory, retains a guest alias nor changes the original unwind. | `softpc.new/base/ccpu386/c_main.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-183 | Observe the source-defined end of NTDOS CONFIG processing without changing its notification semantics. | The non-debug fixed container cannot otherwise distinguish a skipped `multi_pass`/`trys` write from a later `commnd` loss before permanent COMMAND loading. | After unchanged `UMBNotify(0)` and `demDasdInit()` in original `MS_bop_E`, a default-off adapter records only selected-map `multi_pass_id` and bounded `commnd` offsets under the already-live original CS. It acquires and releases synchronous read leases before the original BOP returns; it does not retain a guest pointer, route a selector, modify CONFIG/UMB/DEM, or affect CPU/guest state. | `softpc.new/host/src/nt_bop.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c,mvdm_guest_location.c}` |
| MVDM-HOST-DIV-184 | Observe a bounded source-owned CPU40 SAS write span, including byte/word/dword stores. | Selected-image recovery must distinguish an absent original control-word write or transient copy from a later in-guest control transfer, without adding instruction tracing or reading guest memory. | After each unchanged `c_sas_store`, `c_sas_storew` or `c_sas_storedw` completes, a default-off helper matches one explicitly configured linear span and records only write address, width and scalar value. It cannot alter CPU state, guest memory, selectors, routing, or mapping lifetime. | `softpc.new/base/ccpu386/ccpusas4.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-185 | Observe the original DEM file-read scalar contract needed by NTDOS `doconf`. | S7 proves `trys` was not reached; the original `doconf` branch requires a complete `AX == CX` read before it can enter `getcom`. | Default-off calls at original `demRead` entry and terminal success/failure copy only numeric DS:DX, request, file-offset, completed count, AX and carry state. They do not inspect the guest buffer, alter the host read, retain a pointer or alter a DEM/BOP result. | `dos/dem/demhndl.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-186 | Observe the original CONFIG EOF-seek scalar contract before NTDOS `doconf` reads its temporary file. | T355 S8 proves a temporary-file open but no `demRead`; source proof identifies `AH=42h/AL=2` as the only intervening host-service call. | Default-off calls at original `demChgFilePtr` entry and terminal success/failure copy only DOS offset/origin/result/AX/carry scalars. They retain no host handle or guest pointer and do not alter file position or a DEM result. | `dos/dem/demhndl.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-188 | Recover source-defined fast-I/O failure when the NT4 x86 kernel interceptor is absent. | `NTFastDOSIO` is a kernel trap-frame handler and cannot compose in the standalone worker. | The existing `demFastIoUnavailable` mirror owner returns only CF=1 for both fast selectors, so unchanged NTDOS `$READ`/`$WRITE` takes its original normal-service fallback. No user-mode fast-I/O algorithm, lease, mapping or alternate source file remains. | `dos/dem/demdisp.c` |
| MVDM-HOST-DIV-189 | Supply the reached C-VID `Sas.Sas_overwrite_memory` vector member. | The original generated CCPU `sascdef.c` carrier leaves this member null, but the selected CPU40 `ccpusas4.c` supplies an exact same-shaped provider whose original behavior is a deliberate no-op: CCPU has no compiled-code cache to invalidate. | At the existing C-VID vector composition point, the named SoftPC adapter assigns only `c_sas_overwrite_memory`; all other generated unavailable slots remain unchanged. | `../ntvdm/softpc/mvdm_cvidc_vector_binding.c`; `softpc.new/base/{ccpu386/ccpusas4.c,cvidc/{sas4gen.h,accessfn.c}}` |
| MVDM-HOST-DIV-190 | Attribute a reached CCPU40 direct-RAM physical-read failure without changing the original SAS result. | S17 proves the selected startup does not publish an external physical-page binding, but the `phyR` access violation alone cannot distinguish a bad Intel address from an unavailable M-area backing span. | At the already reached `c_GetPhyAdd` normal-RAM fallback, call one default-off adapter recorder after alias/resolver misses. It copies only requested/direct physical numbers, wrap mask, M-area length and private base scalar; exception reporting emits the last record only when the observer configured a child-only path. | `softpc.new/base/ccpu386/ccpusas4.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-192 | Read original permanent-COMMAND `DS:DX` COMSPEC text at `SVC_CMDCOMSPEC`. | NT4 treated the real-mode source as a process-address alias. That alias is invalid outside the session guest-memory owner even on x86. | Keep original `COMSPEC=` construction, `cbComSpec` calculation and `AL` disposition. Copy at most `MAX_PATH` bytes through one synchronous read lease; unavailable or unterminated source returns `CF=1/ERROR_INVALID_ADDRESS` without retaining a guest pointer. | `dos/command/cmdmisc.c`; `../ntvdm/softpc/{include/mvdm_guest_location.h,mvdm_guest_location.c}` |
| MVDM-HOST-DIV-193 | Write original permanent-COMMAND initial environment at `SVC_GETINITENVIRONMENT` `ES:0`. | NT4 wrote the selected environment through a durable low-address alias. | Retain original environment construction, paragraph sizing and `BX` insufficient-buffer result. After that original capacity gate, write the unchanged `COMSPEC` and environment byte order through one synchronous session lease; an unavailable destination returns `CF=1/ERROR_INVALID_ADDRESS`. | `dos/command/cmdenv.c`; `../ntvdm/softpc/{include/mvdm_guest_location.h,mvdm_guest_location.c}` |
| MVDM-HOST-DIV-194 | Exchange original COMMAND `CMDINFO`, executable path, command-tail and transformed-environment guest records at `SVC_CMDGETNEXTCMD`. | NT4 passed these 16:16 records as direct host aliases, including across the Base VDM client call. | Snapshot `CMDINFO`; keep Base VDM fields local; lease guest writes only while copying and commit the original fields. Before first-command PIF expansion, copy its completed guest environment through a bounded read lease, release before host/UI calls, bind the snapshot only during original `cmdCheckForPIF`, then free it. Reject unreadable/unterminated input without exposing an alias; do not replace original expansion. The executable path is committed after original PIF/uppercase transformations. No guest/native pointer survives the BOP. | `dos/command/cmdmisc.c`; `../ntvdm-exe/command/{include/mvdm_command_guest_state.h,source/mvdm_command_guest_state.c}`; `../ntvdm-exe/softpc/{include/mvdm_guest_location.h,mvdm_guest_location.c}` |
| MVDM-HOST-DIV-195 | Supply the original low-dependency DOS `.COM` `DEMOPEN` pathname and `DEMREAD` buffer without retaining a `GetVDMAddr` host alias. | The selected non-`i386` SIM32 form provides a native pointer, but a pathname/read buffer must not outlive the session-owned guest-memory boundary. | Keep original DEM pathname canonicalization, named-pipe branch, file/open/seek/error ordering, opaque `AX:BP` handle identity and CF/AX results. `demOpen` copies only a bounded `MAX_PATH` string; `demRead` takes one writable lease for the synchronous host call, commits only on its original success path, and discards it on every failure path. Lease refusal follows original DEM error direction with `ERROR_INVALID_ADDRESS`. | `dos/dem/{demfile.c,demhndl.c}`; `../ntvdm/softpc/{include/mvdm_guest_location.h,mvdm_guest_location.c}` |
| MVDM-HOST-DIV-196 | Let original `cmdExec32` and its detached `cmdCreateProcess` worker retain the command, environment and standard-handle inputs needed by the original process lifecycle. | NT4 kept durable `GetVDMAddr` process aliases in globals until the detached worker consumed them. Those aliases are invalid across the session guest-memory boundary. | Before worker creation, copy the original bounded command tail, double-NUL environment and three 32-bit standard-handle identities into session-owned adapter state. The original COMMAND worker retains conversion, process, wait, exit-code and re-entry order; no guest pointer crosses the asynchronous boundary. | `dos/command/cmdexec.c`; `../ntvdm/softpc/{include/mvdm_command_native_child.h,mvdm_command_native_child.c}` |
| MVDM-HOST-DIV-218 | Preserve the original `cmdExec` AH=1 and `cmdExecComspec32` distinction at the host-service boundary. | The product's COMMAND service must re-enter through Run16, but the detached adapter otherwise receives only already assembled command bytes and cannot safely infer the original branch from a name or shell text. | The two original COMSPEC branches store their branch and, for AH=1, the already-captured guest tail in session-owned state. At the project-owned BOP adapter call site, that state selects `NtvdmSystemRoot\System32\run16.exe command` for no-argument re-entry or `…\run16.exe <tail>` for AH=1. Thus it replaces the original COMSPEC executable while keeping the original tail for Run16's existing product classification boundary; it does not create a second `COMMAND /c` wrapper. No `COMSPEC` environment value is read, changed, or forwarded by this adaptation; guest construction, wait, exit result and re-entry remain unchanged. | `dos/command/cmdexec.c`; `../ntvdm-exe/command/{include/mvdm_command_native_child.h,source/mvdm_command_native_child.c}` |
| MVDM-HOST-DIV-198 (retired) | Prototype parameter-name normalization, removed during S35 review. | The two parameters are both ULONG; their declaration names do not participate in the C function type or calling ABI. The previous ABI-mismatch rationale was incorrect. | Restore original xms.h verbatim. Actual destination/source argument order, callback definition and suballocator invocation remain unchanged. | `xms.486/xms.h` |
| MVDM-HOST-DIV-199 | Observe the reached original COMMAND `SVC_GETINITENVIRONMENT` `ES:0`/`BX` and immediately-live `EndInit` arena scalar contract before and after its table call. | The current product must distinguish a BOP/CPU address mismatch from an original `SETBLOCK`/environment-layout limit without retaining a guest alias or changing the immutable guest retry. | At the existing original `CmdDispatch` table boundary, copy only `ES`, `BX`, `AX`, carry, `DS` and `SS:SP`; after the call, copy `DS:EnvSiz` (`203Ch`), original `ResSize` (`0592h`), and MCB size at `DS-1:0003h` through short read leases, plus CPU40's existing real-mode `MSW`/`DS` base/limit scalars. The adapter uses the app-captured report path that is removed before `cmdenv.c` reads inherited variables; no guest bytes, pointer, provider state or BOP result is changed. | `dos/command/cmddisp.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c,mvdm_guest_location.c}` |
| MVDM-HOST-DIV-203 | Distinguish a returned original Base VDM record from a post-command DOS error before selecting the next owner repair. | A second `54:01` proves only that immutable COMMAND returned to its acquisition loop; without the already-live `CMDINFO.ReturnCode`, `VDMINFO.ErrorCode` and `VDMState` scalars, it cannot distinguish normal command completion from a DOS/device failure. | At the original `cmdGetNextCmd -> GetNextVDMCommand` call boundary, append default-off copies of only the 16-bit guest return code, `ErrorCode`, `VDMState`, first/repeat flags and success/failure.  No `CMDINFO`/`VDMINFO` pointer, command bytes, guest address, session state or return value is changed. | `dos/command/cmdmisc.c`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-204 | Dispatch the historical normal-file `SVC_DEMFASTWRITE` selector. | On NT4, kernel `NTFastDOSIO` owns the fast-write trap. | The existing `demFastIoUnavailable` owner returns only CF=1. The unchanged `$WRITE` caller then takes its original `SVC_DEMWRITE` fallback; no user-mode fast-write algorithm is invented or enabled. | `dos/dem/demdisp.c` |
| MVDM-HOST-DIV-231 | Bound DEM create path and write-buffer guest access to the active session. | Original create and write services dereference guest aliases directly; the current host requires a bounded synchronous guest-memory lifetime. | Create copies a NUL-terminated pathname within MAX_PATH; nonzero write obtains a read lease and releases it on success and failure. Zero-length write keeps the original SetEndOfFile path; invalid access is reported through the original DEM error channel. T420 S34 is historical scenario evidence, not complete current-package coverage. | `dos/dem/demfile.c`; `dos/dem/demhndl.c`; guest-location provider under `../ntvdm-exe/softpc/` |
| MVDM-HOST-DIV-205 (retired) | Historical DEM create/write diagnostics. | Removed in T420 S13 (`a2ec4d531`). | Retained for historical traceability; the separately active DIV-231 memory boundary is not this diagnostic. | Historical `dos/dem/{demfile.c,demhndl.c}` |
| MVDM-HOST-DIV-206 | Stop the original SoftPC Console event worker during host close. | NT4 process termination made an alert-only request sufficient; a running worker must release its session binding before local teardown. | Pass the original nt_init_called flag to the bounded join. A never-resumed thread has no session binding and stays suspended until process exit; never resume input during teardown. A started thread is alerted and joined using a duplicated source-owned HANDLE. No new lifecycle state or guest semantics. | `softpc.new/host/src/nt_event.c`; `../ntvdm-exe/softpc/{include/mvdm_softpc_event_thread.h,mvdm_softpc_event_thread.c}` |
| MVDM-HOST-DIV-207 | Observe the existing Console-record to SoftPC 8042/PIC/CPU40 interrupt chain during the fixed interactive matrix. | The fixed container proves a Console event is consumed but cannot otherwise distinguish conversion/admission failure from a source-owned IRQ, PIC acknowledgement, BIOS transfer or EOI-continuity failure. | Default-off hooks copy only converted scan code, stopped/disabled/full flags, queue depth, selected PIC IRR/ISR/IMR and scan result, IRQ/vector, selected EOI line, and an already-high PIC suppression scalar immediately beside unchanged original operations. They do not retain input records, alter guest/8042/PIC/CPU/BIOS state, route BOPs, or add an input producer. | `softpc.new/{host/src/{nt_event.c,nt_cpu.c},base/{keymouse/keyba.c,system/ica.c,ccpu386/c_main.c}}`; `../ntvdm/softpc/{include/mvdm_softpc_termination.h,mvdm_softpc_termination.c}` |
| MVDM-HOST-DIV-208 | Wake the original SoftPC CPU from its short idle wait when host activity arrives. | The original NT4 `PulseEvent` notification can be lost on modern Win32 when activity arrives immediately before the CPU marks itself waiting. | Retain the original auto-reset event and all idle counters/timing.  Signal that same event with `SetEvent`, which preserves one pending wake for the next original wait and wakes one existing waiter. | `softpc.new/host/src/nt_unix.c` |
| MVDM-HOST-DIV-217 (retired) | Historical `demChMod` diagnostics. | Removed in T420 S13 (`a2ec4d531`). | Retained for historical traceability; original attribute behavior remains separately owned by DEM. | Historical `dos/dem/demfile.c` |
