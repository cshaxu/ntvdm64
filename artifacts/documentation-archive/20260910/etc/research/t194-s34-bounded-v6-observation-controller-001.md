# M0 T194 S34: Bounded V6 Observation Controller

Packet: M0 T194 S34  
Disposition: controller mechanics corrected and statically verified; no observation run.

## Change

The T194 controller now follows the pre-existing bounded T188 cleanup pattern.
At watchdog expiry it captures `taskkill /T /F` output and exit code, waits at
most five additional seconds, records `cleanupWaitTimedOut`, and reaches its
`finally` record regardless of cleanup success. It no longer calls unbounded
`WaitForExit()` after a denied taskkill. The runner invocation remains one-shot
and `retryPolicy` remains `none`.

No process was launched or killed. There is no elevation, job object,
kill-by-name action, runtime input change, or Bochs/adapter/provider change.

## Verification

`tests/bx-ntvdm-adapter/Test-T194S34ObservationControllerBoundary.ps1` passed.
It requires the exact taskkill scope, 5000ms bounded wait, captured cleanup
fields and terminal record in `finally`; it rejects unbounded `WaitForExit()`,
sleep/retry loops and dynamic execution. `git diff --check` also passed.

## Follow-up

The next observation packet may use the S32 register-listener engine only
after assembling a new immutable bundle. It may make exactly one invocation;
if taskkill is denied again, the controller will retain a bounded terminal
record rather than hang or retry.
