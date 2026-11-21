# C0621 synchronous admission boundary

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
