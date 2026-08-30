# M0 T313 S5 P2 — Selected Direct-Exit Sweep

## Scope

This is a source audit of the selected x86 CCPU40 host/source manifests, not
of every preserved historical tool or condition-unselected mirror file.  The
goal is to ensure that selected in-process host paths cannot bypass the S5/P1
session-controlled terminal boundary by directly ending the application.

## Reached selected paths corrected

The following original process-exit sites belong to the same NT4
dedicated-`ntvdm.exe` product-shell assumption as `host_terminate`:

- `host/src/nt_reset.c`: rejected startup argument form;
- `host/src/nt_reset.c`: final `host_terminate` action;
- `host/src/nt_event.c`: console-close handling after the original
  `CNTRL_PUSHEXIT` condition;
- `host/src/sim32.c`: selected CCPU40 `InitIntelMemory` allocation failure.

All four now call the one
`mvdm_softpc_terminate_current_session` adapter boundary.  Normal bound
execution escapes immediately with the source exit code.  The CCPU40 SAS
allocation caller additionally returns `NULL` if invoked without a bound
session, retaining an ordinary allocation-failure result instead of ending a
test or host process.

The shared local marker is `MVDM-HOST-DIV-147`; no new generic shim or device
backend is introduced.

## Deliberately retained non-product paths

- `host/src/nt_term.c` is not selected by the formal host `sources` manifest;
  it remains original mirror evidence.
- `base/support/terminat.c` calls C runtime `exit` only after its selected
  `host_terminate` call.  A bound session escapes at that prior terminal call,
  so the historical fallback is unreachable in the product path.
- `host/src/nt_event.c:cmdPushExitInConsoleBuffer` contains the original
  console-window/`ExitThread` product shell.  It is not a direct process exit;
  replacing it would require the separately owned console/presenter lifecycle,
  so it is retained rather than fabricated.
- source tools, debug-only forms, unselected CPU30 forms and comments are not
  claimed as selected runtime behavior.

## Verification

The existing external selected formal graph rebuilt all changed original host
objects and completed successfully:

```text
build/M0-T313/S5/formal-termination-r1
target: original-softpc-candidate
incremental result: 4/4, exit 0
```

S5/P1's bounded CCPU40 witness remains the behavioral proof of the controlled
escape itself.  This P2 adds source coverage for every reached direct process
exit; it does not claim full console shutdown or a booted guest.
