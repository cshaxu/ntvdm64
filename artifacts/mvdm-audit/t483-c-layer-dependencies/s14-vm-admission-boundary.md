# C0621 synchronous admission boundary

## P159 approved callback composition

Owner rejects queues for same-thread logical decoupling and approves typed
inner-owned callbacks for outer product actions. P158 is superseded, not
implemented. [P159 evidence](s14-vm-admission-callback-owner.json) records the
final ownership, real guest results, package and remaining chains.

B now owns the BIOS-slot register adapter, bridge and complete guest request
preparation. It calls its typed submit interface synchronously. F registers
submit_vm_command with the existing executor context after initialization and
before CPU entry; cleanup unbinds before executor drain/destruction. F retains
executor orchestration, original threadpool submission, frontend registration,
launch and retirement. Underlying capture, continuation, text and I/O mechanisms
remain B-owned. No whole executor migration or runtime-peer independence is
claimed. Failed submission leaves launch/text owned by B for existing rollback.
Success still means threadpool submission succeeded before token publication.

C0621 is resolved by this actual migration and approved callback boundary.
B/C known direct outward symbols remain 0/0; this chain has one reviewed B-to-F
runtime callback. C0156 (nine D callbacks) and C0619 (cross-thread product
request protocol) are not closed by this delivery. Original mirror/guest delta
is zero, including non-error mirror points and lines.

Cached x86/x64/reference builds and all nine A/B graph gates pass. Unit fixtures
pass 73 ingress, 3189 notification and 81 execution assertions. The published
package passes real COMMAND/native EDIT/DOS MEM return and real Win3.1 two-VM
production native 54:08 AH0/AH1, token/completion, 320 callback identity/stack/
register/flags checks and normal exit. Guest input and 54:0A are not covered by
that two-VM test. Original installation bytes are unchanged; its test copy is
removed. Package c6f232df-b4f3-4fd1-81bf-bc885b2ef1c7 has 11 images and 22 matching
published hashes. Commit/push receipt is the enclosing P159 commit.

## P158 synchronous admission design (superseded; not implemented)

The remaining C0621 edge is not a missing declaration. F's executor submit
allocates a job, creates the B continuation and B I/O, checks closing/count/
duplicate generation under the existing rundown lock, inserts its input node,
then calls TrySubmitThreadpoolCallback with the F execute body. Only after
successful submission does it increment active and expose the token. Failure
unlinks the node, abandons/releases the continuation and leaves launch/text
owned by the caller. Moving this body with that callback does not close C0621.

Recommended replacement: a B-owned ready-job queue sharing the existing
rundown/job identity, with a pre-established F consumer. B synchronously owns
all allocation, continuation/I/O, duplicate/closing checks and ready publication.
F takes admitted objects and owns frontend registration, launch and retirement.
B stores no F function pointer or F host/payload pointer. An event wakes the
consumer; there is no polling, new guest ABI or parallel lifecycle registry.
The existing input-job list and rundown remain authoritative, not a second
active list with competing completion. A pending marker/index only distinguishes
unclaimed jobs within that authority.

This changes the admission primitive: success means a ready consumer's queue
accepted a fully owned request, rather than a new TrySubmitThreadpoolCallback
having succeeded. Startup must create/arm the consumer before exposing B
admission. Consumer setup failure prevents startup; allocation/closing/duplicate/
queue-publication failure before admission returns an error and no token.
Product execution errors after admission continue to be completion errors.
There must be no post-return allocation required merely to schedule an accepted
job. A shared threadpool wait can consume ready work, but its rearming and
parallel callback guarantees require verification before implementation.

The F consumer must allow independent VMs to run concurrently; a serial drain
that blocks on the first child's exit is invalid. Same-VM older-generation
ordering and exact-token input lookup remain unchanged. Shutdown first rejects
new admission, invalidates continuations, then drains both pending and claimed
jobs. It must not cancel the wait callback before pending jobs can be retired.
Join callbacks before freeing the queue, host or I/O. Preserve the current
no-ICA/executor-lock inversion; do not introduce a CPU-thread wait for F.

Named production evidence: mvdm_vm_executor.c::submit/execute/create/destroy;
mvdm_standalone_worker.c publishes rundown before CPU entry and destroys the
executor before the frontend registry; compat/mvdm_softpc_worker_exit.c closes
that same rundown. Existing executor tests cover independent VMs, duplicate
generation, token input, closing rejection, retained failed inputs and retirement
join. New tests must additionally cover no consumer, pending shutdown, publication
failure rollback, concurrent rearming and no job lost between drain and rearm.
Final proof still requires real guest operation-11 status/token plus two live
VM requests, not only the mock-heavy notification fixture.

Native mechanism evidence: build/t483-s14-admission-design/wait_rearm.c,
compiled MSVC x86 /W4 /WX /MT, passed 32 rounds. Each round keeps the first
callback blocked, rearms the same wait, proves a second callback starts,
then disarms and joins both before closing handles. This supports concurrent
consumption without per-job TrySubmitThreadpoolCallback, but does not prove
queue lock/order/cancellation correctness. The production design must avoid
rearming after close and preserve readiness while pending jobs exist.

Microsoft API contracts: [SetThreadpoolWait](https://learn.microsoft.com/en-us/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-setthreadpoolwait)
requires re-registration for subsequent signals; disarming does not cancel
already queued callbacks. [WaitForThreadpoolWaitCallbacks](https://learn.microsoft.com/en-us/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-waitforthreadpoolwaitcallbacks)
joins outstanding callbacks and optionally cancels unstarted ones. Do not use
cancellation to discard accepted jobs during drain. The native probe is a
mechanism experiment only, not guest acceptance or a delivered scheduler.

Decision required before replacing production scheduling: approve this bounded
queue/consumer contract as an inward-owned admission mechanism, with F retaining
product execution. This is not approval for an outer implementation callback,
asynchronous later admission, or moving Run16/NTSRV policy into B. No production
or guest change is included in P158. Existing P157 package remains current.
C0156's nine D transport callbacks and C0619's native product launch are separate
open chains; this proposal cannot close them or imply a completed architecture.

## P157 application and I/O ownership

The complete application publish/commit/dispatch implementation moves to
compat/mvdm_vm_application.c with its header. The existing notification,
continuation and text providers already belong to B. The implementation
continues to wait for the exact guest acknowledgement, preserves cancellation
ordering and discards an uncommitted frame on failure.

VM I/O allocation, title/input-mode state, dispatch, input access and disposal
move to compat/mvdm_vm_io.c. The F unit retains only the frontend operations
table and parameter adaptation: it maps four borrowed cancellation handles
and frontend reply metadata, then calls B. B contains no F callback, window
registry, broker transport or product launch. The original BOP operations
13/14 already use B mvdm_vm_guest_io.c; the P156 contrary statement is corrected,
not counted as a new elimination. Operation 11/executor submission remains open.

The existing notification/application/I/O fixture passes 3189 assertions on
final code, including acknowledgements, cancellation, staged output and rundown;
it mocks the PIC and is not a real enhanced-mode acceptance claim. Final
cached x86/x64/reference builds pass. A same-named header initially selected
the wrong B interface in the F unit; an explicit local include fixed it without
broadening search roots. All nine compiled A/B dependency graphs pass.
Package 5f0e7136-5ea2-48ee-8540-03add3d0806f is published to both locations;
22 image hashes match. Real COMMAND/native EDIT Ctrl+Q/DOS MEM and launcher
completion pass on that package. Evidence: build/t483-s14-application-io.
Non-error mirror change:
0 points/0 lines; original guest files are unchanged.

B/C direct symbol counts remain 0/0. C0156, C0619 and C0621 remain open indirect
implementation chains; these prerequisite migrations do not close them.

## P156 snapshot ownership

mvdm_vm_command_snapshot now joins service/capture in compat/mvdm_vm_capture.c.
Its descriptor/screen validation, copy ordering, capture failure cleanup and
output contract are retained. The existing C guest-memory provider replaces
F session wrappers, using the same active bound context and lease pool described
below. The F snapshot source and header are removed, not retained as forwarding.
No mirror or original guest file changes (non-error delta 0 points/0 lines).

Capture tests pass 160 assertions, including no-bound-context and read-failure
rejection with NULL outputs and no outstanding lease. The existing ingress
fixture passes 64 selection/ownership assertions; its CPU/broker are mocked,
so it is not evidence of real enhanced-mode token delivery. The reference graph
initially retained a deleted F source; explicit CMake regeneration corrected it.
The final cached products and runtime package evidence live in
build/t483-s14-vm-snapshot. Package c5276fb8-2e13-429a-b529-eb6ec8f21f83 is published to both locations (22 verified hashes); real COMMAND/native EDIT Ctrl+Q/DOS MEM and launcher completion pass.

C0621 remains open at mvdm_vm_executor_submit: it allocates a continuation/I/O
object, rejects closing/duplicate generation, links the job and must successfully
submit the existing threadpool execution before publishing a token. The F execute
body performs frontend registration, product launch and retirement. A generic
callback or merely moving the wrapper would preserve the prohibited dependency.
Provider correction: operations 13/14 already resolve to B mvdm_vm_guest_io.c.
The separate channel-side application publisher remained in F at P156. B/C direct counts stay 0/0;
C0156/C0619/C0621 indirect implementation chains remain unresolved.

## P155 text-buffer implementation

The complete frontend-base/video and ntvdm-exe/frontend/vm_text units move
into compat/console_video_buffer and compat/mvdm_vm_text, with headers under
compat/include. Bodies, symbols, state and failure ordering are unchanged;
only includes and build ownership change. Rendering and worker orchestration
remain outside B. No additional callback or duplicate state machine is added.
The two B units use libraryFlags, not nativeServiceFlags; this removes the
unnecessary generated BaseSrv include search path caught by the boundary gate.

Final cached x86/x64/reference builds pass. VM text has 247 passing assertions;
COMMAND capture has 154; video commit/cancel/configuration/serial checks pass.
Real COMMAND/native EDIT Ctrl+Q/DOS MEM and launcher completion pass on the published package. All nine A/B build graphs pass the compiled dependency gate. Package
 ef831118-16ca-41ad-8118-9abfbcf29782 is published to assets/release and
O:/winnt/system32, with all 22 image hashes checked. Build/publication evidence:
build/t483-s14-text-buffer. No original guest or mirror file changes: non-error
mirror delta is 0 points/0 lines.

This removes the text object's E provider dependency as a C0621 prerequisite.
It does not close C0621: F still owns snapshot acquisition and synchronous
operation-11 admission/executor composition. C0619 and C0156 also remain open.
B/C direct symbol counts remain 0/0; the remaining indirect implementation
chains must not be reported as eliminated by this relocation.

## P154 capture implementation

mvdm_vm_command_service and mvdm_vm_command_capture now live in
compat/mvdm_vm_capture.c. Their symbols and input/output shapes are retained.
They use the existing inner guest-memory acquire/release functions and
guest-location API. The 101-line capture unit has no session, frontend or
executor dependency. The F snapshot operation still creates the presentation
object and calls the moved capture function.

The existing capture fixture passes 154 assertions using the shared memory
provider. A former F-only release-failure substitute is replaced by a read
failure at that provider for service decoding. A standalone release-failure
assertion for the moved service function is not claimed by this result.

Cached x86, x64 and reference products built successfully. The dependency
gate passed after reference compilation completed. Package
8ccc064a-121b-4ccb-bcd6-1202f717ebef is published at both release locations.
Evidence is in build/t483-s14-vm-capture. This is partial migration within
C0621, which remains open. No C mirror or original guest source changed.
Final publication hashes and governance checks passed. The published package
also passed real COMMAND/native EDIT/DOS return. Enhanced-mode runtime
acceptance of the remaining admission chain is still outstanding.

The current BOP dispatcher calls mvdm_vm_command_admit in
src/ntvdm-exe/softpc/mvdm_vm_ingress.c for operation 11. The preparation
function already belongs to B. Remaining F dependencies precede and follow it.

Before preparation, mvdm_vm_command_service and mvdm_vm_command_snapshot in
src/ntvdm-exe/softpc/mvdm_vm_command.c obtain guest bytes through
session_thread_current and session_guest_memory_acquire/release. These
operations must use the existing inner guest memory owner when migrated.
They also build frontend text state, whose ownership needs a separate split.

After preparation, mvdm_vm_executor_submit obtains the current worker's
executor and allocates a command_job. It allocates the continuation, captures
its generation, creates VM I/O, then checks closing, active-count overflow
and an already active job for the same VM/generation under the rundown lock.
It inserts the job and submits the existing threadpool execute callback.
Only successful submission increments active and publishes the token.
Submission failure removes the list entry. Any failure destroys I/O,
abandons and releases the unexposed continuation, and frees the job.
The caller then destroys launch/text on failure and releases the capture.

The asynchronous execute body owns frontend registration, product execution,
frontend retirement and input return. Moving the BOP wrapper alone would
still require these F-owned objects and implementation callbacks.

The required migration boundary is synchronous admission through successful
publication, including failure rollback and shared rundown state. Product
execution can consume an admitted request after this boundary only when
the existing submission-failure and ownership guarantees are preserved.
A notification that cannot report admission failure before BOP returns
does not preserve the current contract.

This is source evidence. C0621 remains open. No product code or guest image
was changed by this inspection.

The existing inner entry points are mvdm_softpc_guest_memory_acquire/release,
declared in compat/include/guest_memory_access.h and implemented in
mvdm-host/softpc.new/base/ccpu386/ccpusas4.c. The F wrappers additionally
check session_valid and SESSION_STATE_ACTIVE before borrowing the session's
lease pool. Their replacement must preserve an equivalent active-lifetime
check through the selected inner context.

The earlier directory-based classification of frontend_video as presentation
was too broad. Complete-body inspection finds buffer validation, staging,
commit/abort and allocation only; no window, renderer or outer callback.
Both frontend_video and vm_text can move intact into B, avoiding a second
text state machine. Product frontend creation and rendering remain outside B.

The lifetime checks have now been compared. session_thread_current derives
its session pointer from compat_host_context_current; both paths therefore
select the same thread-local context. session_initialize assigns
host_context.guest_memory to that session's guest_memory_lease. C's acquire
and release check compat_host_context_valid and COMPAT_HOST_CONTEXT_ACTIVE
before using that pool. F additionally checks session magic, ABI version,
size, identity, backend and thread-hook bookkeeping in session_valid.
The migration can retain the guest-memory lifetime checks through the C
entry points, while the product remains responsible for its surrounding
session invariant. This is the specific distinction to verify at the
capture test and worker bind/unbind boundary.
