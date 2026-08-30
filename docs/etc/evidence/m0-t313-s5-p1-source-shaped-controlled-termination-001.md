# M0 T313 S5 P1 — Source-Shaped Controlled Termination

## Original path and the product-shell boundary

The selected original SoftPC entry remains
`softpc.new/obj.vdm/ntvdm.c`: `TimerInit`, `CpuEnvInit`, `nls_init`, then
`host_main`.  Its host initialization and error paths call the original
`TerminateVDM`; that retains the original `terminate` cleanup sequence and
reaches `host_terminate` in selected `host/src/nt_reset.c`.

On NT4, `host_terminate` reports to BaseSrv with `ExitVDM` and ends the
dedicated `ntvdm.exe` using `ExitProcess`.  The current product instead runs
the selected host body inside an app-owned session.  Direct process exit would
therefore incorrectly kill the application rather than finish that session.

## Minimal divergence

`MVDM-HOST-DIV-147` changes only this final product-shell terminal action:

- original cleanup and `TerminateVDM -> terminate -> host_terminate` call
  order remain selected;
- `host_terminate` calls the same-shaped
  `mvdm_softpc_terminate_current_session(VDMForWOW, VdmExitCode)` boundary;
- that boundary transfers the original exit code to the currently bound
  session and returns through the app-owned execution escape;
- it does not call `ExitProcess`, create an alternate CPU/device/BOP path, or
  expose a guest address or host identity.

The escape storage and arm/disarm lifetime are session-owned.  It is armed
only around one outer original `host_start_cpu` or original-entry invocation;
recursive CPU returns remain ordinary original CCPU returns.

## Bounded behavior evidence

`tests/mvdm-host/ccpu_bounded_execution_fixture.c` still executes the original
CCPU40 RAM/FPU program through original `host_start_cpu`.  Its existing test
entry seam additionally requests termination with exit code `37`.  The run
proves that the outer source-shaped execution bridge returns, the session is
`COMPLETED`, its completion code is `37`, and the process remains alive to
dispose the session:

```text
build/M0-T313/S5/termination-r1
target: ccpu-lifecycle
result: exit 0
```

The fixture does not claim that `ntvdm.c`, BaseSrv, DOS startup, or a guest
workload completed; the actual `nt_reset.c` body is separately selected by the
formal graph.

## Formal verification

Fresh external Win32/x86 CCPU40 candidate:

```text
build/M0-T313/S5/formal-termination-r1
target: original-softpc-candidate
result: 404/404, exit 0
```

Original diagnostics remain visible.  This confirms the original source body
and its minimal bound-session terminal interface compile together; it is not a
claim that the full entry is safe to execute without the remaining host
capability closures.
