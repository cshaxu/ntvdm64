# T95 S7 BOP FE Controlled-Stop Audit

## Question

What does the first-profile trace's reached `C4 C4 FE` require, and may it be
implemented as another adapter resume service?

## Inputs and observations

r45 trace 038 reaches `BOP_DOS:36` at `0032:5A70`, resumes normally, switches
to `SS=DS=0D00`, then executes `C4 C4 FE` at `0D00:0100` with no valid return
stack. Its copied instruction window is `C4 C4 FE E3`; the adapter correctly
passes it through today.

`src/opennt/base/mvdm/inc/bop.h` names `0xFE` `BOP_UNSIMULATE`: “end execution
of code in a vdm.” The pinned historical CCPU has an explicit `0xFE` path in
`softpc.new/base/ccpu386/c_main.c:3340-3342`, which invokes
`c_cpu_unsimulate()` and states it never returns. Its definition at
`c_main.c:4864-4884` exits the current CPU invocation via the NTVDM monitor
(`ccpu386Unsimulate`) or an outer simulation context. The SoftPC BIOS table
also assigns `host_unsimulate` to FE (`base/bios/bios.c:650`).

## Decision

FE is an execution-lifecycle terminal event, not an OpenNT host-service
request. A `fault_rip + 3` adapter resume would contradict the original
non-returning contract and execute the following `E3` byte as guest code.
Neither a DOS/DEM handler nor a replacement CCPU/monitor is admitted.

The only compatible next design direction is a versioned, terminal
Bochs-to-adapter stop disposition: exact real-mode `C4 C4 FE` maps to an
observable controlled-stop reason, with no guest-memory access and no CPU
resume delta. CLI may later map that terminal reason to process completion;
that mapping needs its own lifecycle/exit-status contract. This is not yet
implementation admission because the current Bochs `#UD` seam only supplies
pass-through or resume results.

## Follow-up

The audit found a partial predecessor: exception ABI v1 reserves `STOP=2`,
but the active v2 CPU result and `bx_ntvdm_exception_response` accept only
pass-through or resume. `SIM->quit_sim()` is process termination/cleanup, not
a CPU-loop return channel. In contrast, native `kill_bochs_request` plus the
existing `async_event` check is already the Bochs-controlled CPU-loop return
mechanism (`cpu/event.cc:82-88`, `cpu.cc:77-96`, `main.cc:1087-1093`). Thus a
later implementation needs one registered, default-off generic exception-seam
stop disposition; it must not call `quit_sim`, decode FE in Bochs, or add a
monitor. The adapter recognizes the exact copied FE bytes and returns a typed
stop reason; Bochs only sets those existing mechanical stop flags and longjmps
to its existing CPU loop. Admission and focused negative verification precede
any patch; CLI owns the later normal exit-status mapping.

## Implementation and verification update (2026-08-11)

The admitted implementation carries a terminal result through the existing
typed boundary without placing any FE, BOP, DOS, or OpenNT term in Bochs.

- Adapter-only `bx_ntvdm_controlled_stop_service_v1_dispatch` accepts exactly
  a valid real-mode vector-6 window whose first three copied bytes are
  `C4 C4 FE`; it returns `BX_NTVDM_CPU_RESULT_V2_STOP` with zero resume RIP,
  GPR mask and EFLAGS masks.
- The v3 action conversion represents that same result as a zero-range STOP
  action. The existing v4 pass-through fallback reaches v3; it does not issue
  a guest read for a terminal result.
- The registered generic Bochs exception seam accepts only the typed STOP
  disposition. It sets the pre-existing `async_event` and
  `kill_bochs_request`, then uses the existing exception `longjmp`; the
  native CPU loop observes the request and returns. It neither advances RIP
  nor changes a register, flag, guest byte, device state, or calls `quit_sim`.

Focused static boundary verification passed via
`tests/bx-ntvdm-adapter/Test-BochsUdInterceptBoundary.ps1`. The dedicated
controlled-stop C11 fixture compiled before the action-test assertion update
and passed its exact-positive, wrong-selector, and non-real-mode cases in
`artifacts/build/t95-s7-controlled-stop-c11-002`. The same GCC fixture could
not subsequently rebuild the action targets: its `cc.exe` child exited 1
without a diagnostic even for a standalone source compilation. That is
retained as toolchain evidence, not a code verdict.

The fresh same-island MSVC/x86 r47 named target did compile and link the
adapter service and generic Bochs seam. Its binary SHA-256 is
`FC874263F5CC3D77731EE0237ECDA71064C8C018F2CE830F8614824872CB1A1F`.
Trace 039 is deliberately negative runtime evidence: it reaches a prior
real-mode stack failure (`SS=00A7`, `SP=0003`, then unresolved third
exception) before any FE invocation, is watchdog-terminated, and records no
`adapter controlled stop accepted` line. It therefore does not prove terminal
runtime behavior and authorizes no further BOP/service/device work. The next
source-led investigation remains the earlier stack/control-flow failure.

## Deferred-POST runtime result (trace 040)

Fresh r48 restored the already proven deferred native-POST handoff, preserving
the option-ROM rendezvous and all r45 guest inputs. Its source-built binary is
`D2A65AC3D062145E1026175A1C8511C0C6168B85F525DE9A89001214A6EEC7AE`
(2,561,024 bytes).

The immutable trace at
`artifacts/analysis/t95-s7-native-post-handoff-040-20260811-001/bochs.log`
records this terminal sequence:

```text
... selector=54 service=01 ...
... selector=54 service=0e ...
... selector=54 service=04 ...
... selector=50 service=36 ...
... selector=fe ... cs=0d00 ...
ntdos64 adapter controlled stop accepted
cpu loop quit, shutting down simulator
```

This proves the exact copied FE window reached the adapter, generated the
typed no-effect STOP result, and returned through Bochs's native CPU-loop
termination mechanism. It does not prove an end-user CLI completion: the
observer still recorded `watchdogTerminated=true` after 30 seconds and exit
code 1, so the text/UI process lifetime after the CPU loop remains a separate
CLI/Bochs lifecycle contract. No guest stop is retried, no guest state is
mutated, and no additional BOP is admitted by this result.

## Process-lifetime diagnosis

The watchdog result is not evidence that the CPU loop failed to return.
`refs/bochs/main.cc:1087-1132` logs the observed CPU-loop message, calls
`bx_atexit()`, and returns zero from the simulator entry. The outer Win32
`bxmain` wrapper at `main.cc:357-364`, however, calls `fgets(stdin)` to wait
for ENTER whenever `bx_user_quit` is false. The observer starts a hidden
`cmd.exe` child without redirected/closed standard input, so that original
interactive convenience wait remains pending after the already completed CPU
stop.

This identifies a CLI-process composition concern, not an adapter/BOP,
firmware, CPU, or guest defect. The appropriate future contract is for
`src/cli` to own child standard-input policy (normally an EOF-capable closed
or redirected input for non-interactive execution) and to surface the
simulator's normal return as a CLI result. It must not make the adapter set
`bx_user_quit`, fake a power-off, call `quit_sim`, or change guest behavior.
No source change or rerun follows from this diagnosis.
