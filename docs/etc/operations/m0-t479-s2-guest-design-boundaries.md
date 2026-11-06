# T479 S2 guest bridge: resolved design boundaries

## Status and evidence limits

Owner acceptance closes S2/shared-library-import at 967ef3d31. This record
resumes S2/guest-bridge, following the [S1 closure](../evidence/m0-t479-s1-guest-research-closure.md)
and [original call-chain research](m0-t479-s1-guest-callchain.md).
This is a source-reviewed design checkpoint, not executable bridge acceptance.
No runtime, guest, mirror, protocol or published image changes accompany it.

## Finite findings and selected corrections

| Finding and actual source | Selected design; acceptance obligation |
| --- | --- |
| `src/ntvdm-exe/frontend/vm_host.c`: retire/end hold the host SRW lock while `vm_frontend.c` retires a frontend, performs RPC and joins its service thread. | Under the registry lock only lookup, pin, mark closing and detach. Perform RPC and joins outside it, off the CCPU thread. Retain context until outstanding callbacks and channel dispatch finish. Test VM A retirement while VM B registers and operates, including reentrant teardown. Do not add a thread per request or duplicate lifecycle authority. |
| `src/common/frontend-base/channel.c`: synchronous provider dispatch has no stop context; stop sets an event and cancels synchronous I/O, then joins. A future provider waiting for guest application would not be released by I/O cancellation. | Give dispatch a borrowed cancellation context from the existing channel lifecycle. Wait for operation completion OR channel stop/worker death/VM invalidation/broker loss. Mark and signal invalidation before joining. `finish` cannot be the only wakeup because it follows dispatch. No timeout or polling; test cancellation during each wait and exactly-once completion. This is an interface gap for the planned provider, not proof the current instant provider hangs. |
| `vm_frontend_origin` returns a frontend capability; `src/common/native-hook/installer.c` requires frontend and execution capabilities together. | Reuse `OpenNtBaseClientAcquireConsoleContext`; NTSRV `frontend_registry.c::OpenNtBaseServiceAcquireConsoleContext` already has an authenticated embedded-owner branch creating an independent chain. Build a request-local pair, install it before child resume, close local copies on all failures. Never replace NTVDM's upstream context. Test two VMs and rejected/missing/stale capabilities through actual RPC. |
| `src/mvdm-host/dos/command/cmdexec.c::cmdCreateProcess` uses global command/environment/drive/result and process-wide `SetStdHandle`; `cmdExec32` freezes the event path and runs a synchronous reentry loop. | Keep the ordinary path. The new route captures caller-owned command, environment, drive/CWD, service selector and standard-stream classification before yielding. Duplicate real file/pipe handles; pass explicit per-child startup handles through existing launch services. Reuse preparation logic where separable, not the global freeze/reentry loop. Test parallel commands with different directories, redirections and exit results. CF=1 is the original DOS reentry contract, not a generic launch failure. Its downstream DOS handling remains an explicit S3 proof. |
| VMM lifecycle distinguishes non-executable VM from later destruction. Original `O:/repos.external/ms-dos-6/dev/smartdrv/sdvxd/vmm.inc` defines VM_Not_Executeable before Destroy_VM, including forced termination. | Stop admission and guest delivery at non-executable notification; invalidate generation before delayed completions can run. Notify NTSRV and retire host resources asynchronously. Destroy_VM finalizes the handle association, not guest instruction execution. Cover normal/forced exit, handle reuse, delayed completion and whole-guest exit. |
| A V86 callback entered by redirect is not a far CALL; restoration can reenter callbacks. | Do not borrow a sample's `Simulate_Far_Ret` without its pushed frame. Capture the complete VMM client state and the post-service-byte continuation; mark RETURNING before restoration, hold a reference through nested callbacks, acknowledge once. Prove both BOP service continuations, unchanged SS:SP and nested interrupts on each supported guest. |
| Queuing only a target-VM event to a VM asleep on the very semaphore being completed risks circular waiting. | Notification IRQ must cause a legal VMM wake operation independently of that VM's execution. Perform guest memory/output work only later in valid target context. No host worker calls VMM, no ISR performs RPC, no host thread waits while owning CCPU execution. Exact service legality remains a runtime proof, not an assumption from a header. |

The callback/restoration observations are also documented in the original
[Microsoft VxD DDK](https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/):
V86 callbacks supply the VM and client frame; simulated far return assumes a
matching call frame; restoring client state can deliver callbacks. That 3.0
manual does not establish the 3.1/Win95 semaphore ABI. Local declarations and
isolated execution must establish the selected guest contract.

## Authority and request flow

NTSRV owns association, request outcome and lifecycle decisions. VMM owns VM
identity, scheduling and client state. NTVDM owns its host projections and
translated guest access, frontend-base owns transport cancellation, and the
VxD owns legal VMM entry. The notification device is a wake mechanism, not a
second command protocol or lifecycle authority.

1. On the CCPU thread copy caller inputs; retain the original BOP's service-byte
   advancement. Redirect only after the original dispatch frame can unwind.
2. The VMM callback identifies the actual VM and its generation. Register or
   use that VM's frontend; acquire its frontend/execution capability pair.
   RPC and child launch occur outside guest execution and registry locks.
3. Submit once through existing services. Counted completion and wait must
   tolerate completion before the caller waits; do not retry ambiguous submits.
4. A claimed emulated notification resource signals queued work. Pending work
   remains authoritative until acknowledged with generation and sequence.
   PIC cancellation/reset is not normal consumption. No fixed allegedly free
   IRQ, synthetic success, polling, or timed suspend/resume is permitted.
5. Wake through VMM; apply queued I/O in the correct guest VM context. The
   caller completes only after NTSRV's terminal result and the required final
   logical guest-application serial. Restore the saved continuation once.
6. VM death discards pending application/restoration, wakes host waiters with
   failure, and reports to NTSRV. Broker death uses existing local shutdown.
   Resource release waits for references outside the CPU and registry locks.

Generation belongs to the payload/VM registration, not just a reusable VM
handle. Request sequence, committed publication serial and guest-applied serial
are distinct. No cached guest pointer survives a scheduling or mapping change.
System/pre-existing VMs require a serialized initial inventory before READY;
READY means registered resources and inventory, not merely INT 2F detection.

## Complete I/O scope, not a text-only success stub

`src/common/protocol/console_io.h` version 30 has 47 live operations; 38 is
retired. `vm_frontend.c` currently validates and delegates them. It does not
implement guest Console behavior. The provider must cover the following groups:

| Operations | Ownership and obligation |
| --- | --- |
| 1--8, 12--17, 20, 46 | Text, cells, cursor, attributes, scrolling and geometry: caller-visible model with guest VM application, using actual VDD/translated access. No direct global B800 shortcut. |
| 9--10, 22, 35--37, 39 | Modes, code page, display, keyboard layout and text configuration: coherent per-VM state with explicit mapping to guest services. |
| 18--19, 21 | Read/peek/returned input: exactly one consumer and validated receipts. A child must actually receive input; duplicating it into guest BIOS is not proof. |
| 23--31, 42, 47 | Window queries, titles, pointer/clip and fonts: map the guest VM frontend, not NTVDM's unrelated native window or a fabricated Console. |
| 32--34, 43--45 | Video and atomic publication: stage privately, commit complete batches, discard on abort/EOF. Text and graphics both remain in scope. |
| 11, 40--41, 48 | Logical barrier, snapshot and final presentation barrier: distinguish copied, committed and guest-applied state. Never hold a snapshot/publication lock while awaiting the CPU that needs it. |

Existing NTCON's native Win32 Console implementation cannot simply stand in for
this provider. File/pipe redirection remains actual handles, not video. The
47-operation table is a coverage obligation; exact VDD/keyboard/mouse mappings
are not yet implemented or claimed proved. No silent unsupported operations or
weaker presentation contract are approved here.

## Bounded implementation and proof gates

| Stage | Finite delivery and decisive checks |
| --- | --- |
| S2 host foundation | Remove join-under-registry-lock, expose dispatch cancellation, wire request-local capability acquisition; negative lifecycle and concurrent independent-VM tests. No runtime work has been made in this checkpoint. |
| S2 guest bridge proof | Recover/verify the project VxD build and loading path; independently demonstrate callback return, counted wait/wake, resource claim and reset, system/existing VM inventory and forced retirement on Win3.1 enhanced and Win95. These are unresolved executable facts, not open choices delegated to the owner. |
| S3 commands | Both 54:08 and 54:0A, full register/CF/AL contract and per-request capture; two VMs complete in both orders while the other child demonstrably remains alive. Resolve original DOS reentry before declaring native/DOS parity. |
| S4 I/O | All operation groups above, input consumption, final serial, cancellation and nested chain isolation. Guest service mappings must be demonstrated, not inferred from NTCON tests. |
| S5 integration | Coherent publication and existing DOS/WOW/Win3.x regression plus complete guest matrix. No change to immutable original guest components. |

NIDDB remains the original host-device instancing mechanism in
`src/mvdm-host/softpc.new/base/bios/virtual.c`; its instance swaps and selectors
are not frontend identity. Do not enable its hardware-dependent paths merely
to obtain a VM number. Determine the selected device-state dependency with the
S2 guest proof before any mirror change.

## Review and delivery accounting

The selected corrections remove reliance on a global lock, global command
state and implied callback cancellation; they do not establish guest runtime
feasibility by themselves. No original source has been imported in this
checkpoint. Mirror diff: +0/-0. Runtime/compat code: +0/-0. The accepted shared
library package remains unchanged. Documentation governance and diff checks
are required for this checkpoint; executable gates remain with their stages.

Checkpoint validation: documentation governance self-test and verification passed; git diff --check passed. No executable tests were rerun for this documentation-only delivery. Source findings above remain distinguished from pending runtime proof.
