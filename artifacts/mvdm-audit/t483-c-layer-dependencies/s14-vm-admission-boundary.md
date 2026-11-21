# C0621 synchronous admission boundary

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

The text object also contains frontend_video from E, alongside its committed
and staged text buffers. Its create operation belongs to the capture chain,
while begin/stage/commit/abort/dispose call E presentation code. Moving the
whole vm_text implementation into B would therefore introduce a B-to-E
dependency. Split the captured text data and its ownership from presentation
before moving the command capture and admission bodies.

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
