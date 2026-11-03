# Proposal — multitasking guest VDM frontends and VxD bridge

## Status and owner request

On 2026-10-31 the owner initially requested that the multitasking-payload project be
added at the end of the queue; the owner subsequently moved it as recorded in
[Queue](../states/QUEUE.md). This proposal is
unnumbered and is not an active task; it does not change the active packet or
authorize production implementation. A numeric T and bounded S packets are
assigned only when the project is admitted under the then-current execution
and source policies.

The owner-directed candidate designs are the [host frontend and task-chain
model](../../artifacts/multitask-compressed-io-model-zh.md) and the
[guest VxD bridge design](../../artifacts/multitask-guest-vxd-architecture-zh.md).
They are research proposals, not current product architecture or evidence of
completed integration. At admission, reconcile them with the current project
authorities, selected source baseline, package and source policy; record any
required architecture or policy decision explicitly before implementation.

## Objective and ownership

Enable a DOS payload in Windows 3.1 386 enhanced mode or Windows 95 guest to
run a multitasking phase with multiple DOS VMs, each providing an independently
associated internal VDM frontend and child task chain. Each chain's text-task
I/O, completion and return reach its actual originating VM while the guest
continues to manage its own DOS windows, presentation and focus. A payload may
enter and leave a multitasking phase while its worker remains alive; worker
type is not a permanent single-task or multitask label. Windows 3.1 standard
mode is explicitly outside this candidate.

The worker hosting the guest is both a backend on its upstream chain and the
provider of frontend endpoints for its guest VDMs. NTSRV owns authenticated
frontend association, task-chain admission, handoff and lifecycle decisions;
it does not relay DOS-window text, screen data or user I/O. The guest Windows
environment retains control of its VMM, windows, focus and drawing. Nested
multitasking payloads have separate worker/VMM scopes and their own VDM
frontends; internal chains do not directly take over the outer NTCON.

## Required scope

1. Audit the then-current NTSRV frontend identity, authentication and
   capability model, task/worker associations, handoff protocol, NTCON,
   NTVDM and NTVWM Console contracts. Specify and implement the smallest
   justified extensions for concurrent VDM frontend branches and nested
   task chains. Retain the existing single-task, GUI and redirection behavior;
   do not assume byte pipes reproduce a native Console.
2. Design and implement the project-owned guest VxD bridge with appropriate
   Win3.1 enhanced-mode and Win95 loading, registration, capabilities,
   failure and teardown. In the exact context of each guest call, obtain the
   VMM's current VM identity and bind it to the worker instance, payload/VMM
   instance, VM handle, lifecycle generation and unique request ID. Handle
   VM handle reuse, pre-existing VMs, missing notifications, system VM and
   unknown or transitioning environments without guessing identity from CR3,
   process ID, window focus or the most recently scheduled VM. Do not modify
   guest Windows itself or introduce a resident DOS agent.
3. Cover both package-owned COMMAND services `54:08` and `54:0A` through a
   common environment decision, precise identity binding and frontend/task
   handoff, retaining their respective COMMAND, COMSPEC, standard-handle,
   return, flag and reentrancy semantics. Audit other launch paths separately;
   do not claim those two services exhaust all launches.
4. Decouple request submission and completion from the host call stack.
   Establish independent per-request continuation and VM wait/resume state;
   prove that waiting can leave any unreturned `host_simulate()` / BOP
   dispatch stack. If the existing boundary prevents this, change the
   appropriate boundary and prove it, rather than asserting that a queue or
   additional threads solve the problem. No per-VM thread model is presumed.
5. Bind asynchronous stdin, stdout, stderr and completion to the correct
   request and frontend. Background host I/O may notify the scheduler, but
   guest memory, CPU state, VMM services and display access must occur only
   in the correct legal VM context. Preserve native Console input records,
   screen, cursor, control events, encoding, focus, backpressure, final output
   and lifecycle semantics where applicable; establish exact supported limits
   with evidence rather than assuming experimental BIOS reads are equivalent.
6. Specify and verify VM retirement, cancellation, failed suspend/resume,
   timer revocation and in-flight callbacks, duplicate or stale events, child
   process failure, VxD unload, payload exit with worker reuse, worker death,
   nested descendants and resource release. Coordinate each layer with NTSRV;
   old identities must not target a reused VM or revive a retired endpoint.
7. Audit the selected original NIDDB device-state manager and its BOP B1
   `INSIGNIA.386` protocol as a separate possible dependency of the same guest
   DOS-VM lifecycle. Establish whether the chosen VxD bridge must coordinate,
   reuse, replace, or leave inactive that optional protocol; do not infer the
   answer merely from shared VM identity. Account for per-VM mouse state,
   initialization, VM switching and retirement, reused identities, direct
   video/mouse consumers, and invalid or missing notifications. Preserve the
   ordinary single-instance mouse path. The controlled NIDDB failure and
   current unshipped experiments are recorded in
   [S13 test-failure evidence](../../artifacts/mvdm-audit/s13-test-failure-recheck.md);
   neither establishes actual guest-provider ordering or authorizes a guest
   modification. The tested Win3.1 installation ran without observed B1
   lifecycle controls; this is not a universal absence claim.

## Admission and provenance boundaries

The research evidence demonstrates useful mechanisms under bounded test
conditions, not a production bridge, universal VxD binary compatibility or
the completed target architecture. Preserve the existing research records
and both design documents as inputs; distinguish their facts from proposed
behavior and revalidate applicability against the actual package at admission.

Before source, firmware, guest media or Microsoft-component work, apply the
then-current [source policy](../etc/operations/policy/source-policy.md) and
record provenance, ownership, toolchain, build composition, licensing and
guest-media immutability. Specifically resolve how a project-owned VxD is
built, staged and loaded without changing immutable Windows components or
violating the project's guest-source/build boundaries; if that requires a
policy or architecture exception, obtain its applicable admission before
implementation. The candidate does not override the existing active packet,
source-first and minimum-difference rules, or authorize modifications to real
guest installations.

At admission, scope the candidate into sequential bounded S packets with exact
owned files, ABI/protocol surface, isolation, evidence and stop conditions.
Use owned automated tests and preserve unrelated changes; do not require the
owner to perform manual testing. For Win3.1, the known COM1/COM2/COM4 opening
warning is handled by selecting Ignore in the automated test environment.

## Required evidence and completion

Use versioned, reproducible test entrypoints and exact coherent package
identities. Run a decisive two-VM control in both completion orders: A and B
each launch a host task; while B remains waiting, prove using an independently
held OS process handle that B is still alive when A's output reaches the
correct VM and A's guest caller actually resumes. Exchange A/B and order;
record VM identity, request ID, timing, output, actual exit status and guest
return. Add incorrect and retired identity controls, including reuse, and
prove stale events cannot reach another VM. Host child exit, internal exit
logs, BIOS readback or screenshots alone are not equivalent to guest caller
recovery and correct window interaction.

Acceptance additionally requires evidence for both services, genuine window
focus/input and output routing, concurrent recovery, return codes, cancellation,
VM lifecycle, required nested worker scenarios, real NTVDM/NTVWM Console
integration and single-task/GUI/redirection regressions on the applicable
guests. Verify each OS and loading mode separately; Win3.1 standard mode is
excluded. Report unverified or failed cases as such, including guest setup or
observer limitations. Only declare the complete capability achieved when
all required evidence supports the full scope.
