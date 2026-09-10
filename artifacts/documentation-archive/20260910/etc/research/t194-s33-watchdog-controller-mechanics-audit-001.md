# M0 T194 S33: Watchdog Controller Mechanics Audit

Packet: M0 T194 S33  
Disposition: controller defect isolated; bounded existing pattern identified.

## Facts

S28 used `Invoke-T194S10V6BoundedObservation.ps1`. On watchdog expiry it calls
`taskkill /T /F`, then unconditionally calls `WaitForExit()` with no timeout.
When the current environment returns access denied, that unbounded wait prevents
the `finally` block from writing `observation.json`. This is a controller
mechanics defect; it neither changes nor invalidates the partial BOP trace.

`Invoke-T188S10CopiedStateObservation.ps1` already has the required bounded
pattern: capture taskkill output and exit code, wait at most 5000ms, record
`cleanupWaitTimedOut`, and write the final evidence record in `finally` even
when cleanup fails. It has a static boundary test that prohibits unbounded
`WaitForExit()`, sleep/retry loops, and requires the terminal record.

## Interpretation

The next change is confined to the T194 controller: use the T188 bounded
cleanup record pattern, preserving the one-run/no-retry policy and all input
identity checks. It must not use a job object, elevate privileges, kill by name,
change runner/shim/Bochs behavior, or conceal a failed cleanup. A focused static
controller boundary test should make this property durable.
