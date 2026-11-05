# T479 S1 guest bridge: call-chain audit and implementation decisions

Status: source/design work in progress against 56be696ac. This record does
not admit an unverified runtime bridge or close S1. The owner requests design
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
