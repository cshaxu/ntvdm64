# T425 S2 channel ownership and pending handoff

## Scope and provenance

Baseline is delivered T425 S1 2bbc2f570. S2 P1 is delivered by the containing
reviewed commit; T425 remains open. The owner explicitly
allows multiple pending channel requests, with one current frontend owner.

| Current logic / location | Source and sharing decision | Retained independent boundary |
| --- | --- | --- |
| DOS/native owner slots and bind wrappers in NTCON native_console_frontend.c | Project-added presentation adaptation, not original OpenNT. Replace with one channel owner and one bind/release/forget path. | Original worker block/resume and task execution remain worker-owned. |
| dos_pending and DOS-only wait in NTCON/channel activation | Project-added I/O request state. Replace with deduplicated per-channel pending nodes and the same acquire wait for both workers. | Pending nodes contain no task, process, receipt or execution priority. |
| Input queue, ready event, enter/read/prepend in NTCON | Existing project-added shared Console input mechanism. Remove type-specific names/owner decisions. Preserve ordered keyboard return and pending non-consuming barrier. | Original device/native Console interpretation remains local to each worker. |
| Borrowed frame pointers and serials in NTCON | Project-added presentation resource ownership. One active channel frame, detached under io_lock before disposal. | Capability conversion and publication operation consolidation remain S3. |
| Original MVDM/OpenNT-host execution and device algorithms | No edit or extraction. Existing source-shaped boundary retained. | No original execution, completion, VGA/device algorithm is moved into frontend/common. |

Original OpenNT has no independent dual-worker frontend to compose. This
change reuses the existing project-owned channel, handoff event and Console
storage mechanisms; it does not invent a new historical execution provider,
transport, helper, scheduler or lifecycle policy. The remaining prepare_vga
and import_text operations preserve existing behavior pending S3's explicit
operation contracts. Channel kind checks still exist for that publication
work; S2 must not be represented as whole-objective completion.

## State, locks and failure contract

The frontend owns one active channel identity and a FIFO of distinct pending
channel identities. Each blocked caller owns its private manual-reset event;
the root owns pending nodes. io_lock protects the predicate, pending list,
waiter links, frame and input. Waiters reset their event while holding that
lock, then release the lock to wait. handoff_lock serializes presentation-
thread binding requests, not program execution.

Acquisition does not revoke the previous owner. The previous channel's final
publication/release boundary remains required. Non-peek reads cannot consume
queued input while a handoff is pending. Cancel/timeout/peer failure removes
only that request. An unrelated or stale channel cannot release the owner.
Pending requests prevent parking/returning the Console to outer CMD. Stop,
peer death and presentation-thread failure are explicit event-wait outcomes;
the existing ten-second acquisition limit is retained without timer retries.
Channel stop joins its thread and forgets its request before freeing storage.

## Actual attempts

New results are build/M0-T425/S2/r001 and r002, using the validated dependency-
driven MSVC x86 /MT cache in build/M0-T424/S2/r001. Original CCPU40 selection,
APP 0.0.425, protocol/RPC35 and I/O23 remain unchanged. r001 retains preliminary
builds and failed attempts. r002 freezes the final pending-park-barrier build;
its candidate-manifest.json identifies all eight selected x86 products.
Only ntcon.exe differs from S1. After all prepublication gates passed, the
coherent eight-file candidate was published at O:/winnt. Postpublication
smoke also passed against that exact eight-file set.

The first extended fixture failed because its waiter passed GetCurrentProcess
as a pseudo-handle into the multi-object wait. Diagnostic result=6 proved
ERROR_INVALID_HANDLE and explained why its pending request vanished. The
production channel already uses an owned real worker process handle. The
fixture now duplicates a real synchronize-only handle, closes it on completion,
and retains every assertion. No delay or weaker success condition was added.

Final production-linked console_channel_lifetime_test.c, r002/channel-r001.txt:

- PASS all four incoming/outgoing VGA-conversion combinations, pending input
  barrier, peeking, ordered Q delivery and stale-owner isolation.
- PASS two simultaneous pending channels, FIFO acquisition, repeated-request
  deduplication and independent head/tail cancellation.
- PASS pending Console-return rejection; park succeeds only after release.
- PASS pending timeout and real suspended peer process death without releasing
  the active channel or preventing a replacement request.
- PASS handle count unchanged across cancellation/acquisition/failure: 439
  in channel-r001 and 433 in the final-source channel-r002 run.
- PASS retained real-pipe EOF/publication rollback, logical grid/viewport/
  cursor, ten original VGA thresholds and private Console handoff cases.

The fixture links production NTCON storage and includes its real channel/pipe
dispatcher; only broker attachment is substituted. It is Console/API/transport
evidence, not RPC authentication or actual guest acceptance. Existing text-
handoff fixture reports 32 checks/0 failures, presentation 426/0 and real
pipe input-return 688/0. These do not certify physical RDP capture.

The seven-fixture input runner's first r002 attempt used a relative BuildRoot;
its observer subsequently could not locate the native fixture after changing
working directory (exit 2/image identity unavailable). r002/focused-r001 is
not counted as a pass. Re-running with absolute build/observer/log paths in
focused-r002 passes all seven unchanged assertions. The warning-only signed
comparison in the added timeout fixture was corrected with an explicit DWORD
cast; channel-r002 retests that final test source.

## Verification and delivery

Latest-build r002 Console17 and Window17 each pass all 17 established
assertions. Actual modern EDIT returns to CMD and DOS MEM; the same outer CMD
also completes DOS/native/cooked return. Independent-session survival, both
workers' worker/frontend loss, eleven RPC cases and five GUI routes pass.
Five version/identity negatives pass; WINMINE's visible application window and
SOL/WRITE's existing memory-error frontiers match S1. These are observation,
not gameplay or SOL/WRITE functional passes. Strict repeated DIR reports
entered-dos=1, dirty-prompt=0 and final=0. Coherent publication has completed;
postpublication smoke passes twelve native/DOS rapid-relaunch pairs, twelve
interactive CMD relaunches, DOS/native/cooked return and GUI startup/default
return plus explicit wait exit 37. All eight published hashes still match.
Preliminary r001 product gates cannot certify the later pending-park production
change. r002 is the selected final release; S1 recovery was preserved and
verified before replacement.
S3 publication/client consolidation and S4 whole-objective audit remain open.
No physical desktop manipulation, guest/configuration or imported-library
change is made by this stage.

Diff review finds no original mirror/ABI/imported-library delta against S1;
all 47 pinned imported-library inputs in its release manifest still match.
The removed owner slots/wrappers do not remain in production source. Link-
ownership negatives and documentation governance/relative-link gates pass.

## Publication identity

r002/freeze-publish.ps1 verifies 236 source-input hash rows and eight x86
cache/staged products. The previous coherent S1 files are retained in
r002/accepted-s1-recovery with recovery-manifest.json. Only the eight product
files are replaced; guest/configuration and user data are not overwritten.
published-manifest.json matches candidate-manifest.json after replacement.

NTCON SHA256:
`4C60A4AAC0CC7968C1D6F7CC1D15B274D4F0D56EB668AE2409C7BCDBFE51E45A`.
The other seven hashes match the S1 ledger and its published manifest exactly.
No RPC/wire change was made; protocol/RPC35 and I/O23 are retained.

Final review checks pending ownership under io_lock, independent cancellation,
wait-event reset under the predicate lock, stale releases, final publication
before acquisition, and frame detachment before channel disposal. No task
completion or component-retirement authority is added. S3 retains the explicit
prepare_vga/import_text operations and publication kind checks for consolidation;
they are not claimed removed here. Source-input and publication hashes were
rechecked before P formation. Governance, relative links and git diff --check
pass; the containing P is pushed to main before S3 admission.
