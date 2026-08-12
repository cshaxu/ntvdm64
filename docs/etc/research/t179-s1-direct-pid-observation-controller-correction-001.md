# T179 S1 direct-PID observation-controller correction 001

## Change

Invoke-T179DirectBochsObservation.ps1 is an observation-only tool. It does not
belong to the product CLI, adapter or Bochs composition. A future caller must
supply the already immutable Bochs binary, profile, BYOB root, configuration
and an empty evidence directory.

The tool starts the exact declared Bochs executable through ProcessStartInfo.
It sets only six declared adapter values for the duration of that child launch:
profile, root, launch kind, launch tail, bridge and trace. It restores their
prior process values in finally. The native log is the declared
EvidenceRoot/bochs.log path; no stdout/stderr pipe is opened.

## Process and timeout contract

The started Process object is the sole owned process. Its PID is recorded in
the result. Only after WaitForExit(watchdog) returns false does the tool call
Process.Kill() on that same object, and it then waits for that object. It has no
cmd wrapper, taskkill call, process-name/path scan, tree kill or Start-Process
indirection.

The caller must provide an outer timeout of at least watchdog plus 15 seconds
of evidence collection grace. The tool records watchdog, outer budget and grace
in its result. It refuses a result/log collision.

## Static verification

Test-T179DirectBochsObservationController.ps1 parsed the controller through the
PowerShell AST and verified the direct executable assignment, exact-PID timeout
branch, required environment allowlist and outer-timeout guard. It rejects
cmd.exe, taskkill, Start-Process, ReadToEnd, Get-Process and
EnvironmentVariables references. The test passed on 2026-08-12 and launched no
executable.

## Limitation and next gate

T179 is not a runtime result. It neither reruns T177 nor observes SHARE, DOS,
54:11 or any BOP. T180 must create a fresh source-identical evidence root and
make one separately admitted direct-PID observation, with a host execution
budget larger than the recorded 50-second outer controller budget.

