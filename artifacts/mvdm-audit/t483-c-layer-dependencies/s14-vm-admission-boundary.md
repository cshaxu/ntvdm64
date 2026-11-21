# C0621 synchronous admission boundary

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
Other BOP operations still reaching F include application transfer 13/14; the
complete chain must be reconciled before closure. B/C direct counts stay 0/0;
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
