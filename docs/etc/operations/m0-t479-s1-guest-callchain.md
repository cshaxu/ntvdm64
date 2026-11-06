# T479 S1 guest bridge: call-chain audit and implementation decisions

Status: owner-closed research; see the [closure and handoff](../evidence/m0-t479-s1-guest-research-closure.md).
The in-progress statements below are retained research checkpoints, superseded
by that closure for status only; their unproved runtime claims remain unproved.
Initial source/design work was against 56be696ac. The owner requests design
and implementation of the mainline; the existing sequential stage plan still
governs its delivery.

## Confirmed implementation boundary

The selected path is CCPU `c_main.c` BOP dispatch, `MS_bop_4` in
`mvdm-host/softpc.new/host/src/nt_bop.c`, `CmdDispatch` in
`mvdm-host/dos/command/cmddisp.c`, then service 8 `cmdExec` or service 10
`cmdExecComspec32`, both reaching `cmdExec32` in `cmdexec.c`.

`cmdExec32` activates the command capture, assigns process-global command and
environment pointers, changes the shell count, blocks the worker event thread,
sets `fBlock`, starts `cmdCreateProcess`, and waits in `GetNextVDMCommand`.
Its CF/AL result distinguishes reentry from completion. `cmdCreateProcess`
uses shared standard handles, reentry count and exit-code storage. Creating
another thread around this path cannot make it a concurrent per-VM executor.

`MS_bop_4` increments the *current* IP after `CmdDispatch` returns. A bridge
that changes the current VM before unwinding must not let that epilogue
advance the other VM. The bridge must record the original service and return
address, leave the C BOP stack, and resume only its identified client.

The selected CCPU `c_main.c` opcode C4 BOP arm already commits the decoded IP
before `bop(...)`, then executes `CANCEL_HOST_IP()` and `SYNCH_TICK()` after
the host handler returns. This provides a concrete interpreter return boundary
for a redirected guest continuation; adding a hardware V86 monitor is neither
required nor justified. It does not prove callback legality or authorize
changing CPU state on a background host thread. The BOP FE arm instead exits
the current CPU invocation and is not an interchangeable continuation return.

The existing `command/source/mvdm_command_native_child.c` is a synchronous,
session-scoped capture with an epoch and copied command/environment/handles.
It is not a per-VM request store. Retain its validated capture behavior where
applicable; do not keep its singleton or guest mapping leases across an
asynchronous wait. Audit its real-mode location assumptions in the paged V86
case before reusing the capture entrypoints.

## Owners and intended flow

| Object or decision | Sole authority / selected existing boundary |
| --- | --- |
| Guest scheduling, VM handle, client registers | Guest VMM; project VxD reports facts in the actual callback context. |
| Host payload and VM frontend | NTVDM `frontend/vm_host.c` and `vm_frontend.c`; retain their complete returned identities. |
| Task-chain admission, handoff and shutdown | NTSRV through the existing authenticated frontend-origin capability. |
| Pending command and return continuation | NTVDM-owned per-request state, independent of any outstanding BOP C stack. |
| Downstream Console channel | Existing frontend-base service and `ntvdm_vm_io`; host-owned buffers only in its callbacks. |
| Guest screen/input access | CPU-thread guest adapter in a legally selected target VM; never a channel thread. |
| Guest windows, focus and final drawing | Guest Windows/VDD/keyboard components, not NTMON or NTSRV. |

The containing payload retains its upstream frontend. Each guest VM obtains
its own downstream origin through `ntvdm_vm_host_origin`; a child launch must
use that capability without changing the containing worker's upstream origin.
The capability is an owned wait-only duplicate, not the public VM number.
Registration may return an identity with an error after partial admission;
the caller must retain it for retirement before freeing its callback context.

The intended request flow is capture -> exact VMM identity -> authenticated
frontend/child-chain preparation -> independent guest wait -> final I/O and
NTSRV completion -> target-VM continuation. Completion and VM retirement
must compete for the same request ownership. Worker reuse starts a new payload
identity; stale identities must not regain access to a new VM with the same
handle. This flow is an implementation contract, not a passing runtime result.

## Recovered research: useful evidence and rejected shortcuts

The old research worktree has lost its working files. The two research texts
are recoverable as Git objects in commit 4d3bd0f18 at
`artifacts/research/win31-win95-vxd-bridge-design-zh.md` and
`artifacts/research/win31-win95-guest-multitask-bridge-zh.md`. Disposable exact
copies are under `build/t479-s1/research`; use `git show <commit>:<path>` to
reconstruct them. They are historical observations, not this package's test
results or a runtime source dependency.

The detached-continuation records report actual DOS caller recovery in both
completion orders under Win95 and Win3.1 enhanced mode. Their mechanism used
a ten-byte guest-stack trampoline, a fixed small request table and timed
suspend/resume. Limited register/stack and output checks passed. Later VKD
experiments explicitly reproduced duplicate input: a key consumed by the host
was still present in the DOS BIOS queue. The records also distinguish successful
guest-controller results from observers ending at their diagnostic deadlines.

Consequently:

- Do not import the experimental trampoline, cookie authentication, fixed
  slots, synchronous pipe writes or periodic completion polling as production.
- Do not treat BIOS text readback as proof of full Console screen semantics.
- Do not infer keyboard delivery merely from VKD's focus-owner handle.
- Do not use nested `host_simulate` to enter the VxD and keep the original
  command's host stack alive.
- Do not revive the hardware V86 monitor or implicitly activate NIDDB BOP B1.

The original [Microsoft VxD DDK](https://www.pcjs.org/documents/books/mspl13/win/w3ddkvxd/)
documents a V86 callback with the current VM handle and client-register frame.
It also makes Suspend_VM a counted operation that rejects the system VM and
critical-section owner. Resume_VM can fail, leaving the VM suspended. These
are constraints on the design, not proof of Win95 or Win3.1 runtime coverage.
No completion path may assume that scheduling work on a suspended VM wakes it.

## Finite S1 decisions still requiring proof

| Item | Required decision and proof before runtime admission |
| --- | --- |
| Entry/return ABI | Prove a VMM-owned callback/client-frame route can avoid executing injected guest-stack code; enumerate exactly saved registers, IP adjustment and page/mapping rules. If not, record the precise unsupported dependency before selecting another route. |
| Event delivery | Establish a legal event-driven host-to-VMM wake path, including HLT, suspended target and critical sections. A host event alone does not invoke guest VMM; periodic experimental timers are not the answer. |
| Environment transitions | Trace original Windows start/exit notification and actual 54:08/0A system-VM callers. Missing driver registration must not masquerade as ordinary DOS. |
| Command composition | Split capture/command preparation from singleton wait/launch state while preserving COMSPEC, standard-handle and CF/AL/reentry semantics; retain the ordinary single-task route. |
| I/O provider | Map every consumed Console opcode to owned buffering and legal guest screen/input operations; prove no duplicate keys, final-frame ordering and independent frontend flow control. |
| Driver packaging | Recover the successful LE toolchain and loading recipe, pin provenance, author the add-on separately, and stage only in owned guest copies. Do not modify original Windows binaries. |
| NIDDB | Audit the original device-state switching contract independently of frontend identity; do not send INSIGNIA lifecycle messages merely because a VM was registered. |

These are the remaining S1 design tasks, not seven new product features.
Implementation stages remain S2 registration/lifecycle, S3 independent
command continuations, S4 I/O/nesting and S5 integrated delivery. No new public
Run16 flag, broker transport or permanent per-VM host thread is required by
this design.

## Acceptance that implementation must supply

Both services on both guests; two VM requests in both completion orders with
an independently held sibling process handle and actual guest-caller result;
wrong/stale/reused identities; VM close during admission and completion;
payload exit/reentry; NTSRV/worker loss; interactive input without duplicate
guest delivery; final screen/cursor before return; two-level nested chains;
ordinary DOS, GUI and redirected file/pipe regressions. Tests must exercise
real NTVDM/NTVWM child chains, not replace them with byte-pipe demonstrations.

This checkpoint changes documentation only. No runtime, guest media, mirror
source or publication bytes have changed, and no new runtime test pass is
claimed.

Documentation verification ran in owned background process 27652, started
2026-11-05T23:57:55.0298301Z, and exited zero. Documentation governance and
`git diff --check` passed; logs and result are under `build/t479-s1`.
An earlier detached launch (7232) produced no result and is not counted as a
passing check; the successful supervisor retained process ownership through
completion. No production rebuild is needed for this documentation checkpoint.

## Follow-up: event-driven candidate and corrected source conclusions

Research baseline: 3adad2816. This section supersedes assumptions, not the
runtime admission gate above. No candidate driver or production modification
has been exercised in this follow-up.

### Command memory is already translated

The complete selected chain is `mvdm_guest_location_acquire` ->
`session_guest_memory_acquire` -> `mvdm_softpc_guest_memory_read` ->
`c_sas_loads` -> `c_sas_hw_at` -> `bios_read_byte` in `ccpusas4.c`.
The latter uses `xtrn2phy` when paging is active and has the original
`biosDoInst` fallback. Therefore segment-times-16 is a *linear* address here,
not proof of a physical-memory bypass. Do not add another page-table walker.
The missing asynchronous contract is ownership of the current VM and lifetime
of the address, not absence of SAS translation. Capture must finish on the CPU
thread in the originating context; no mapping lease or guest address may be
used by a channel thread after yielding. The fallback can execute guest code,
so capture/reentry and failure require explicit tests before splitting the
current singleton into independent requests.

### Selected candidate to validate

1. Preserve original command preparation for both services. Before the global
   blocking/launch portion, copy command, environment, standard-handle
   interpretation and return state into a request. Do not publish a child yet.
2. Finish the existing `MS_bop_4` service-byte IP increment. Redirect execution
   to a VMM-allocated V86 callback and return normally from the host BOP. The
   callback supplies actual VMM identity and client state. A saved register
   carrying a request reference is restored before guest return; it is not
   authentication. No executable bytes are written into the DOS stack.
3. Bind the request to the live payload/VM generation and existing authenticated
   frontend origin. Submit through NTSRV. The original host BOP stack is gone
   before any wait for admission, child execution or I/O completion.
4. Use a VMM semaphore wait in guest driver context, with interrupt-service
   flags validated on each supported guest. Do not use `Block_Poll`, periodic
   suspend/resume, or hold a VMM critical section across a host operation.
5. Host arrival of output/completion queues an identified event and requests a
   guest-visible device interrupt through the existing ICA/CCPU. A project VxD
   claims that device's IRQ through VPICD. Its interrupt handler acknowledges
   the device and schedules/signals only through permitted VMM services; it
   does not perform RPC, wait for host I/O, or touch arbitrary VM memory.
6. The awakened target context applies pending I/O. Only after the final
   presentation serial is applied and NTSRV completes the child chain may it
   restore the caller and return the original service's CF/AL result.

This is a proposed small emulated notification device, not a recovered NT4
hardware V86 monitor. It adds real implementation work: IRQ allocation,
pending/acknowledgement ownership and PIC-reset cancellation. Do not hardcode
an apparently unused IRQ as safe. Registration must fail without acquiring
the resource; it must not steal a guest device's interrupt. No new host broker
transport is implied. The host pending queue is authoritative; interrupt
edges merely announce pending work. An enqueue racing acknowledgement must
remain observable, and a stale event must never revive a retired generation.

The 3.0 DDK's reserved-semaphore note is not evidence that 3.1 lacks the service.
Read-only Microsoft interface evidence includes MS-DOS 6
`dev/smartdrv/sdvxd/vmm.inc` (services, wait flags, callback/API hooks) and
Win2000SRC `private/shell/inc/win95/vmm.h` (the Win95 semaphore wrappers).
Microsoft [KB Q75864](https://www.betaarchive.com/wiki/index.php/Microsoft_KB_Archive/75864)
also identifies Signal_Semaphore among the asynchronous services for the
documented Windows versions. Interface existence does not prove our callback
can wait safely or that all required virtual interrupts are serviced.

### Input and output are separate admission proofs

Do not promote the historical fixed-character VKD experiment into a Console
keyboard implementation. The input contract is one owner per event: forward
an accepted event to the active child *or* leave it to the guest, never both.
Keep make/break ownership across focus changes and handoff. Preserve the
original path for inactive VMs and guest system shortcuts. A host queue failure
is not permission to report a key consumed successfully. Native input records,
layout/encoding, repeat, control keys and mouse events remain required.

The documented VMM interrupt-chain mechanism can observe simulated V86
interrupts, providing another candidate observation point after VKD routing.
Its availability alone does not prove correct keyboard consumption/EOI or
replace the PM API experiment with another untested hook. Compare the two
points with the same event: record target VM, make/break, actual child read,
and post-return BIOS input. Select the smallest point that proves single
consumption on both guests and both full-screen/windowed paths. Do not install
two production consumers or use an all-keys hotkey workaround.

For output, retain existing frontend-base transaction and serial contracts.
Host callbacks may buffer cells/geometry/cursor/input, but only a legal target
VM context may apply guest presentation changes. Direct host writes to global
B800 memory are not a VDD update contract. `PRESENT_BARRIER` must acknowledge
the specified committed presentation in the guest model, not merely queue
receipt; it cannot require an occluded window to become physically visible.
File and pipe redirection bypass this presentation boundary. Every consumed
Console opcode needs a provider disposition, including graphics, font,
geometry, mode and input operations: a text-only success stub does not satisfy
the admitted nested-payload scope. No unsupported-operation fallback or
silent truncation has been approved in this research.

### Environment, NIDDB and teardown

Guest `doskrnl/bios/msint13.asm` and `doskrnl/dos/mscode.asm` distinguish Windows
enhanced startup/exit notifications using INT 2F and DX bit 0. These are
transition evidence, not proof that a usable bridge has loaded. Driver
registration establishes READY; absence is not sufficient to classify a
paged V86 guest as ordinary DOS. A future observer must be on the actual CCPU
interrupt path, not assume the DPMI32 software-interrupt reporter sees it.

NIDDB is a distinct mouse/device-state concern. Its B1 DeviceInit reaches
`c_SasRegisterVirtualSelectors`, which writes two guest LDT descriptors.
Do not forward that initialization merely to obtain VM registration. The
current `virtual.c` already contains DIV-467 retirement restoration; older
research calling that change unshipped is stale relative to this baseline.
Any needed instancing must use a separately justified lifecycle adapter;
neither activating the complete old protocol nor declaring all instancing
unnecessary follows from the frontend design.

VM destruction first closes admission for that generation, then reports
retirement to NTSRV and disables guest delivery. Child/process handling remains
NTSRV-owned. Pending host results for a retired VM are discarded as stale,
never applied through the current VM. Guest semaphore/callback storage cannot
be freed while a waiter or handler can still reference it. Static loading is
the initial candidate on both guests; do not promise dynamic unload safety
based on the historical dynamic-loader experiment. Payload shutdown must
disarm the notification device before worker reuse.

### Finite proof gate before freezing the ABI

| Proof | Positive and negative observations required |
| --- | --- |
| Callback/wait | Both 54:08/0A, both guests; full caller state restored; sibling continues; signal-before-wait, signal-during-wait, VM destruction while blocked; no host BOP frame retained. |
| Notification device | Claimed resource; masked/unmasked IRQ, HLT, early completion, enqueue/ack race, PIC reset and disarm; no idle polling or interrupt storm. |
| Input owner | Both keyboard candidate observation points compared; make/break, repeat, modifiers, focus switch, handoff, no duplicate BIOS input; do not count a key-injection trace as child consumption. |
| Presentation | Opcode inventory against actual DOS/native consumers; frame serial applied in exact VM before return; different VM geometry and nested graphics included. |
| Lifecycle | System/pre-existing/reused VM, failed/partial registration, missing driver, payload exit, NTSRV loss; NIDDB mouse state independently checked. |

These gates are prerequisites, not claimed passing tests. The research now
identifies a concrete event-driven implementation candidate and rejects the
old timed suspension path, but S1 is not closed and no universal feasibility
claim is made for input/presentation. The next work is these bounded executable
proofs and provider mapping, not additional unbounded alternative designs.

Read-only comparison hashes (no source imported):

- MS-DOS 6 VMM.INC: `28BD6EDABA10434DFCAD26BFCDAB8DBD1A3CC28100CD6910C67397D8C05F92E9`.
- Win95 VMM.H: `A6F6B6178BD21ADB1CF5F30C791E875632FDB9BC5ACEEE9890425B3EDC85EC9F`.
- OpenNT-4.5 `nt/private/os2/client/thunk/include/vkd.inc`:
  `11BECFF50B3FED4F080A795925788F670C2BDE6F645926C8BF6198B61211B947`.

Follow-up documentation validation: owned hidden process 8068 exited zero;
documentation governance and `git diff --check` passed. Logs are
`build/t479-s1/verify-followup.stdout` and `.stderr`. This delivery changes only
this research record and its Current summary, not runtime or guest bytes.
