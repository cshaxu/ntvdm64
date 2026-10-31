# S11 debug context handoff candidate

Status: **published partial baseline; not S11 closure**. The original x86
monitor uses a thread-specific trap frame for a guest MOV DR, but its DPMI
`ThreadSetDebugContext` broadcasts six values to all monitor-registered
threads and supplies active debug state to subsequently created threads. The
selected CCPU has one execution register bank and one set of derived
breakpoint tables. The [monitor audit](s11-monitor-minimization.md) records
the original-source trace and controlled baseline.

Current disposition: the externally-created-thread TLS experiment described
below is held as [a patch](s11-debug-context-evidence/held-external-cpu-thread.patch),
not in the production worktree. It can move a previously failing remote
debugger further into an unbound execution path, so it must not be published
independently of the remote task ownership and cleanup. The stage plan assigns
standalone debugger integration to S18; the S11 debug-register task handoff
remains an execution-foundation change and is independently verified. S18 must
integrate and validate the whole remote path, including the held CCPU first
entry, session binding, original remote task scheduling, FPU/debug context,
exception return and cleanup, using the original debugger contract. The
[held observer source](s11-debug-context-evidence/held-debugger-remote-address-observer.c)
is research material, not a product provider or full integration test.

The candidate registers the existing WOW native task identity, saves the
six CCPU values plus the pending debug event on execution release, and
restores them through the original `MOV_DR` / breakpoint setup on acquisition.
The DPMI service updates every registered task and its new-thread initial
values. Registration rollback and task retirement remove the corresponding
record; the execution owner is still the existing WOW scheduler. The only
mirrored implementation addition is registered as `MVDM-HOST-DIV-478`.

The selected cached x86 worker and WOW32 built successfully. Before the
change, the real two-host-thread `native-nested` fixture failed with
`WOW_DEBUG_TASK_HANDOFF isolated=0`; after the change it passed with
`isolated=1`, along with its existing nested callback and retirement checks.
The debug-binding unit covers initial rollback, duplicate identity, pending
state, broadcast, active/inactive new-thread inheritance and identity reuse.
The existing init-rollback and CCPU modal fixtures also passed. The first
extended unit invocation failed because its newly added test operations were
ordered incorrectly; the test was corrected and rerun. These controlled
checks do not execute a Win16 guest or establish full original semantics.

Two real-CCPU resume cases exposed a further candidate defect: before the
resume check, an instruction breakpoint at the resumed EIP was missed and a
restored pending debug event was delivered only after one instruction. The
candidate now schedules the existing CCPU debug checks at `NEXT_INST` after a
context restore, before executing the resumed instruction. Cases 109 and 110
then both reported a debug exception at EIP 1000 with BX still zero; the
97-case monitor boundary script passed. The added negative controls verify
that RF suppresses the restored instruction breakpoint and that a disabled
breakpoint does not raise an exception. This controlled result does not yet
establish ordering against all other exceptions or reentrant callbacks.

A real-CCPU case broadcasts the DPMI debug-register values from a port
callback during execution and places a breakpoint on the next instruction.
Before the change the next instruction executed without a fault (`fault=0
bx=1`); routing the broadcast through the original `MOV_DR` provider while
disabling CCPU's fast path produced `fault=1 eip=1002 bx=0`. This controlled
callback exercises the selected CPU and broadcast provider; it does not
itself prove the complete guest DOSX BOP path. Two additional cases call
the selected original `DpmiSetDebugRegisters` entry from the running CPU:
the valid DS:SI record arms the breakpoint before the next instruction;
an invalid 24-byte span sets carry, clears the debug registers and proceeds
without a breakpoint. All 100 monitor-boundary cases passed. The service is
invoked by the test callback, so these cases do not establish guest BOP
dispatch or debugger-driven remote calls. A further controlled CPU program
executes the real `BOP 53:0e` dispatcher and arms a breakpoint on its next
instruction: the selected original dispatch/service and CCPU report
`fault=1 eip=1003 bx=0`. This is a synthetic CPU program, not an immutable
DOSX guest or a debugger-driven remote call.

Resume-order case 117 exposed a candidate regression when reset and a
restored execution breakpoint were simultaneously pending: the candidate
first delivered the breakpoint (`fault=1 eip=1000`). The resume path now
consumes reset first and branches to the existing CPU reset body, matching
the normal boundary's reset-before-debug order. The same case then reaches
the reset vector without the old breakpoint (`fault=0 bx=0`), and the
original-CCPU HALT/reset fixture passes. This change postdates the initial
staged package and its Full/game/Win3.1 results below; the subsequent reset
candidate has separate evidence below.

Important unresolved boundary: the original monitor also registers
`W32RemoteThread` and its thread broadcast covers all registered monitor
threads. The current candidate only registers ordinary WOW lifecycle tasks;
the remote debugger path has not been integrated or proved equivalent.
The selected WOW32 image contains `W32RemoteThread` and
`WK32KillRemoteTask`; original `VDMCallRemote16` uses `CreateRemoteThread`
with the address supplied by `DBGNotifyRemoteThreadAddress`. Its original
`cpu_createthread` call reaches a no-op in the selected non-MONITOR build.
Thus this is a real selected-code boundary, not an unused source file;
whether the debugger entry and its special task switching are operational in
the current product still needs an end-to-end witness.
The remote routine invokes `cpu_createthread` itself and later enters
`host_simulate`; its teardown calls `host_ExitThread` after restoring the
prior callback context. Unlike normal `host_CreateThread`, this call site
does not invoke `ccpu386newthread`. The current `ccpu386SimulatePtr` only
allocates a newly discovered thread's simulation stack when that flag is
set; a real remote-entry witness is required to establish its initial flag,
execution ownership and failure/teardown sequence. Merely registering the
debug snapshot would not connect those responsibilities.

The selected original CCPU thread provider now has a controlled host-thread
witness for the initialization gap: a thread created by `CreateThread` calls
the actual `cpu_createthread` export and `ccpu386SimulatePtr`, without a
`host_CreateThread` notification. Before the candidate the TLS lookup failed
and the process returned 1; after `MVDM-HOST-DIV-479` lets the existing
discovery run when TLS is missing, four create/entry/exit rounds pass. The
same witness was extended to execute a synthetic instruction sequence through
`c_cpu_simulate` and exit via the original CPU path, four times on externally
created threads. `tests/observation/verify-ccpu-debug-context.ps1` runs it,
the debug binding cases and ordinary HALT/reset successfully in that experiment. This establishes
CPU simulation setup, execution and teardown; it does not establish actual `W32RemoteThread` execution
ownership, register isolation or debugger-driven guest callbacks. The
change was later than both staged candidate packages and is now held for S18.

The current release archive does not contain `WOWDEB.EXE` or `VDMDBG.DLL`;
it does contain `TOOLHELP.DLL`. The already-present original reference
`O:/repos.external/OpenNT/base/mvdm/wow16/wowdeb/WOWDEB.EXE` is 3,952 bytes
and has SHA-256 `B0156DC5FA0233FC92D577A3988A5BE548445347BABD841620F1E0C0421D79C0`;
the reference `O:/repos.external/opennt-src-2/binaries/nt/vdmdbg.dll` has
SHA-256 `65BE513058745A41A12F9E40F9E0B73CACD2CA45814F7B18F777CAE26FA992BD`.
Original WOWEXEC source selects `WOWDEB.EXE` when `WOWQueryDebug() & 1`, and
WOWDEB allocates its own communication block before `WOWKillRemoteTask`.
These are available reference inputs, not evidence that the installed
immutable WOWEXEC actually launches them or that this product supports the
remote call. Any witness must use isolated, unmodified original inputs and
must not patch or rebuild guest code.

An initial isolated real-debugger registration witness used the current
candidate package, an unchanged reference WOWDEB copy and an x64 native
debugger (the original x86 observer cannot start x64 Run16 with DEBUG_PROCESS:
Windows returns error 50). The debugger observed seven VDM events from NTIO,
NTDOS, HIMEM and COMMAND, but no DOSX/WOW module event or remote registration
in its 30-second observation. Both launching Winmine and explicitly launching
WOWDEB showed this same limited frontier. The observer also read the worker's
mapped `lpRemoteAddress`/`lpRemoteBlock` from the linked image and found zero;
seven event info records were readable, but their all-zero remote fields do
not independently validate the cross-bitness layout. This does not show
why startup stopped at COMMAND and is not evidence that WOWDEB's registration
itself fails. Probe source: `tests/mvdm-host/debugger_remote_address_observer.c`;
test-run scaffolding: `build/t473-s11/dr-context-study/remote-address-run.ps1`.
A subsequent run constrained inherited PATH to the previously proven short
test environment and did reach MSCDEXNT, REDIR and DOSX; it saw 13 VDM events
and three further event-kind-zero notifications attributed to DOSX, but still
did not observe remote registration within 30 seconds. Thus the initial
COMMAND frontier was affected by the test environment; neither run proves a
WOWDEB defect or remote-thread behavior.

The real cause of the remaining DOSX frontier was the x64 test debugger's
event disposition: it returned `DBG_EXCEPTION_NOT_HANDLED` for the x86
`STATUS_WX86_BREAKPOINT` (`0x4000001f`), and the worker exited with that
status. With WOW64 breakpoint/single-step events handled, the unchanged
reference WOWDEB loaded TOOLHELP and registered a nonzero remote address and
communication block in the real worker. A subsequent experimental remote
thread was started without first writing a valid WOWDEB communication
request; it led to guest-fault events and worker exit, so it is **not a valid
remote-call test** and is not attributed to the product. The observer no
longer offers that uninitialized invocation. A proper test must use the
original VDMDBG communication protocol before starting the remote thread.
[Registration witness](s11-debug-context-evidence/remote-registration.txt).
Original `VDMCallRemote16` writes its request through `WriteItem` and
`InternalGetPointer`; its x86 translation uses `GetThreadSelectorEntry` and
assumes the guest selector layout is visible to the host. The selected
CPU40 keeps guest addressing and selector state in the software CPU, so
loading the historical debugger DLL alone does not establish that this
selector lookup can address its guest buffer. This translation and the
request/response contract must be verified before repeating the invocation.
The real x64 debugger witness now calls `Wow64GetThreadSelectorEntry` on the
stopped x86 worker thread with the registered guest communication selector;
for selector `1797` the call failed with error 31. This directly rules out
that host selector lookup for the observed case, but does not by itself
prove every selector or complete the required CCPU guest-memory translation.
[Selector witness](s11-debug-context-evidence/remote-selector.txt).
The [stopped-worker diagnostic](s11-debug-context-evidence/remote-guest-map.txt)
then reads the selected CCPU's LDT register, CR0/CR3, page tables and physical
memory backing using the candidate's linked map. For the observed selector
`1797`, it finds a communication header at guest linear `000357a0`; the
header's own far pointer matches the registered block and its length is
4096 bytes. This is an identity-bound, read-only diagnostic page walk, not a
supported external debugger ABI; it does not cover stale TLB mappings or prove
the original `VDMGetPointer` interface works on this host.

A subsequent isolated test waited for the Winmine task-start VDM event,
used the same stopped-worker diagnostic mapping to write and read back a
zero-argument `KERNEL!GetVersion` request in WOWDEB's existing communication
block, and then created the registered remote thread. The request readback
passed but the call did not return: one run reported `STATUS_INVALID_UNWIND_TARGET`,
another a native access violation, and a later run emitted a guest GP at
`021f:00006ffc` with no successful response. The
[bounded result](s11-debug-context-evidence/remote-call-readback.txt) is a
controlled failure, not a proven product root cause: the test uses a diagnostic
page walk in place of the original external selector API and its timing has
not yet been matched against VDMDBG's event choreography. It changes only
the debugger's intended guest communication data in a disposable test worker;
no guest file or code is patched. Do not claim the remote path is fixed.
In a separate [owner-suspended control](s11-debug-context-evidence/remote-paused-owner.txt),
the debugger explicitly suspended the task-start event thread before
continuation. The remote call still did not complete and the worker exited
with `000000ff`, so merely preventing that thread from running concurrently
does not establish a fix. This additional suspension is a diagnostic, not a
claim about the original debugger's choreography; task-start may still be
too early to represent a stable running application.
The [ready-window control](s11-debug-context-evidence/remote-ready-traced.txt)
waited for the Winmine window before issuing a debugger break and the same
read-back-verified request. It still exited the worker with `000000ff`. The
existing termination witness recorded `terminate-vdm`; relocation against
that run's actual worker base and current linker map places its return at
`CALLF+0xd2`, immediately following CALLF's indirect BOP-dispatch call. This
identifies the termination route but not the selected BOP or the reason it
terminated; optimized tail calls and simultaneous execution prevent treating
the return address alone as the root cause.
An x86 hardware execution breakpoint on `TerminateVDM`, installed only on
the isolated remote test thread, captured `EBX=51` at that [termination](s11-debug-context-evidence/remote-bop-watch.txt); the
linked CALLF disassembly uses BL as the BOP table index, identifying WOW BOP
51. Its existing stage witness reported `page-domain-enter-failed`. Source
tracing identifies several possible reasons, including missing thread-local
session ownership; no individual branch has yet been observed. A further
[ready-window paused-owner control](s11-debug-context-evidence/remote-ready-paused.txt)
kept the Winmine window thread suspended before issuing the call; the remote
thread did not complete within the test deadline and did not reach the
termination breakpoint. This suggests execution ownership matters, but is
not proof of a particular lock or a viable production solution. Temporary
branch logging used during the investigation was removed from the worktree.

After explicitly clearing the resume-check marker on CPU reset, the cached
CCPU boundary fixture passed all 97 cases again. The selected original-CCPU
halt/reset fixture also passed its divide-fault cancellation, RF, descriptor,
TSS task-switch and PIC sentinel checks. These are focused fixture results,
not a complete DOS, WOW, Win3.x or remote-debugger acceptance run.
Re-running the linked native nested-task fixture after the CPU reset change
also passed: the debug handoff reported isolation and broadcast, followed by
the existing nested callback, exception-unwind and task-retirement checks.
The verifier's `native-nested` case uses current worker and WOW provider
objects, but it is not itself a Win16 guest debugger test.
The same-object `task-lifecycle -InitRollback` fixture was also rerun and
reported `WOW_INIT_ROLLBACK errors=0 failed_allocations=2 name_releases=2`.
The separate debug binding unit now injects failure at its sole snapshot
allocation: registration returns `ERROR_NOT_ENOUGH_MEMORY`, the unregistered
identity is rejected on exchange, and registration succeeds after allocation
is restored. This unit uses a test heap wrapper; it does not inject this
failure inside the full task initialization path.
The x86 and x64 cached builds completed. A staged coherent ten-component
candidate then passed all 16 Full gates, including WOW startup observations,
Console/Window-17, RPC and lifecycle controls. [Timings](s11-debug-context-evidence/full-timings.json)
and the [package manifest](s11-debug-context-evidence/package-manifest.json)
bind the result to the staged package; its NTVDM SHA-256 is
`BA9441FCDFE49AF9B6A632ADAC3F4F4DD17F3FD6E429A5D89248BEEFDAC9B0D3`.
The first attempts failed before or during the RPC gate due to test setup:
missing short-stage permission, unpackaged test fixtures, Windows PowerShell
instead of PowerShell 7, and selecting an x64 monitor fixture rather than the
previously successful byte-identical x86 fixture. These are not counted as
passing runs. The Full WOW frontier only observes startup, not gameplay.

With the same staged package, the existing [gameplay and options probes](s11-debug-context-evidence/game-runs.json)
passed for Winmine, SOL, WRITE and four SOL Options rounds. The isolated
[Win3.1 matrix](s11-debug-context-evidence/win31-matrix.json) passed standard
and enhanced modes with DOSONLY 0 and 1; the original and test-copy trees
both retained all 86 previously recorded binary hashes. These results cover
the listed operations, not the external debugger's remote-entry path.

After the reset-order change, a newly built and staged package passed all
[16 Full gates](s11-debug-context-evidence/reset-full-timings.json), the
[gameplay and options probes](s11-debug-context-evidence/reset-game-runs.json),
and the four-case [Win3.1 matrix](s11-debug-context-evidence/reset-win31-matrix.json).
The original and isolated Win3.1 trees again matched all 86 binary hashes.
The [new package manifest](s11-debug-context-evidence/reset-package-manifest.json)
records NTVDM SHA-256
`CF558CD72E8FD4CC0C5CAF75123AD0305A94B51A05F880D8FCDF6C71C923EC70`.
The Win386 [high-rate mouse input-sink witness](s11-debug-context-evidence/reset-mouse-observations.txt)
also observed fresh pixels during the burst and on all eight later movements.
It does not cover physical mouse or Raw Input delivery.

Other outstanding checks include nested CPU/debug exception ordering,
allocation failure during full task startup, true guest debugging and
coherent ten-component publication. Do not claim all-thread or remote
debugger recovery until the whole boundary is resolved.

### Remote entry: ownership and teardown review

The original `W32RemoteThread` (`src/mvdm/wow32/wkman.c`) is entered by a
debugger-created native thread. It calls `cpu_createthread`, installs its
stack-local `TD`, saves and changes CPU registers, and only then calls
`host_simulate`. The currently selected non-monitor `cpu_createthread` in
`softpc.new/host/src/stubs.c` is empty. Normal project-created threads instead
pass through `thread_start_compat.c`, which captures the existing session and
binds its thread hooks; the externally created thread does not pass that
thunk. The session hook binds the per-thread WOW runtime, whereas
`mvdm_softpc_wow_page_domain_enter` explicitly requires the current session,
protected mode, the matching domain owner and successful desktop publication.
These source facts establish missing attachment on the remote route; they do
not establish which page-domain check caused every observed failure.

The original remote cleanup is also a paired operation: `WK32KillRemoteTask`
restores the previous CPU context, publishes the remote entry and calls
`host_ExitThread`. The selected host exit invokes the CCPU cleanup adapter,
which first runs the existing session-thread finish hook. Any added remote
attachment must be established before the first shared CPU-state access and
be released on every return and exit path. Merely calling the session bind at
`host_simulate` would be too late; merely binding before it would also leave
CPU scheduling and rollback unproven. The test in `ccpu_halt_reset_test.c`
proves first CCPU TLS discovery for an externally created thread, not the
original debugger's task identity, execution ownership, exception handling,
or complete remote call.

The debug binding unit additionally checks that selecting the current task
does not replace its live register bank or pending exception with the saved
snapshot, that a transfer to an unknown task neither changes the live bank
nor captures a new snapshot, and that the next successful transfer captures
the new live value. The x86 cached Ninja fixture was rebuilt and these checks
passed. This is a same-process boundary unit with mocked CPU registers, not a
remote-debugger integration result.

The current source was subsequently rebuilt through the cached x86 worker
`product-programs` and `dpmi-debug-tests` targets and the WOW32 target; the
x64 cached product build was also checked. A fresh ten-image candidate and
matching identity-stamped test cache were staged. An initial Full run reached
the native GUI cases but ended without its final timing or exit record when
its controlling session disappeared; it is not passing evidence. Full was
restarted as a separately owned hidden background process with explicit logs
and an exit record. Its final result and subsequent game and Win3.1 checks
must be associated with that new package before any publication decision.

That restarted run completed: all 16 [Full gates](s11-debug-context-evidence/remote-current-full-timings.json)
passed for the [identity-stamped candidate](s11-debug-context-evidence/remote-current-package-manifest.json),
whose NTVDM SHA-256 is `16809CA79E59AC91F5639684F77D634BCEF8C37D200B9A0D167D637D8F924BBE`.
The [game probes](s11-debug-context-evidence/remote-current-game-runs.json)
passed Winmine, SOL, Write and four SOL Options rounds. The isolated
[Win3.1 four-mode matrix](s11-debug-context-evidence/remote-current-win31-matrix.json)
passed with [86 guest binary hashes unchanged](s11-debug-context-evidence/remote-current-win31-integrity.json)
in both source and test-copy trees. The Win386 input-sink burst produced
[fresh pixels](s11-debug-context-evidence/remote-current-mouse-observations.txt)
during and after movement; the observation harness terminated that run at its
budget, so it is not evidence of natural Win386 exit or physical mouse input.
None of these tests proves the debugger-created remote task path.
On the same rebuilt worker, `verify-ccpu-debug-context.ps1` passed its
binding, reset and four external-thread first-entry cases; separately,
`verify-ccpu-monitor-boundary.ps1` passed all 102 selected cases. The latter
includes restored breakpoint and reset-priority controls, but does not prove
all simultaneous timer, trap, interrupt or nested-callback orderings.

The later S11-only rebuild removes the held external-thread experiment and
has a different package identity; earlier passing results do not certify it.
Its first Full attempt failed `Window-17` at `nested-mem`: the observer timed
out with `input-delivered=0` and the last console snapshot showing a partial
`COMM`. Four earlier gates, including Console-17, had passed. This is a real
non-pass, not evidence yet identifying whether the cause is input delivery,
the candidate, or the test host. Preserve the failing witness and use the
same selected case against both the candidate and prior passing package before
deciding how to proceed; do not publish on the earlier candidate's results.
The [failure](s11-debug-context-evidence/final-full-initial-nonpass.json)
and its [input timing](s11-debug-context-evidence/final-window17-nested-failure-input.txt)
and [console snapshot](s11-debug-context-evidence/final-window17-nested-failure-console.txt)
are retained. Three isolated repetitions each against the prior passing
package and the S11-only candidate [passed](s11-debug-context-evidence/final-nested-controls.json).
They do not identify the original failure's cause; the full candidate gate was
restarted and must complete before delivery.
The S11-only candidate subsequently passed all 16 [Full gates](s11-debug-context-evidence/final-full-timings.json),
the [game probes](s11-debug-context-evidence/final-game-runs.json), and the
[Win3.1 four-case matrix](s11-debug-context-evidence/final-win31-matrix.json),
with [86 guest binary hashes unchanged](s11-debug-context-evidence/final-win31-integrity.json)
in both trees. The isolated Win386 input-sink witness showed
[fresh pixels](s11-debug-context-evidence/final-mouse-observations.txt)
during the burst and on all eight follow-up movements; it is not a physical
mouse test, and its budget termination is recorded separately. The exact
[candidate manifest](s11-debug-context-evidence/final-package-manifest.json)
records NTVDM SHA-256
`1D381FFBC38421B4AF79A58C502BDAD9EB62A954DF7E3C1E9D402A6AB0585619`.
The single earlier Window-17 input failure remains unlocated and is not
claimed fixed by the subsequent six controls and Full pass.

Source commit `f4400d311` was followed by the coherent ten-image
[publication](s11-debug-context-evidence/final-publication.json); both
destinations matched all 20 expected hashes. A post-publication
[owned-Console smoke](s11-debug-context-evidence/final-installed-smoke.txt)
ran the installed `run16 cmd /c ver`, exited zero and displayed the
[host version](s11-debug-context-evidence/final-installed-smoke-console.txt).
An earlier direct invocation from the tool process returned 87 without an
owned Console; it is not a passing or comparable Console-launch witness.
