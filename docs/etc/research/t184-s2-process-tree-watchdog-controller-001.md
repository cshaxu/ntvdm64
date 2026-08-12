# T184 S2 process-tree watchdog controller 001

Date: 2026-08-12  
Packet: M0 T184 S2  
Disposition: controller source is statically closed; it has not launched a process.

## Controller contract

`tools/Invoke-T184V5FiniteSequenceObservation.ps1` receives explicitly named
runner, shim, bundle engine, profile, BYOB root, target and empty evidence
root. Before start it resolves and hashes all file inputs and writes
`inputs.json` with a literal `retryPolicy: none`.

It writes a small `cmd.exe` wrapper that invokes the runner with the fixed
`--engine shim --bochs bundle-engine --byob-profile profile --byob-root root
target` contract and redirects both streams. The wrapper PID is the one
process-tree root. On timeout the controller invokes:

```text
taskkill.exe /T /F /PID <wrapper-pid>
```

so the runner, shim and native-Bochs descendants are all inside the explicit
termination scope. The wrapper writes its normal child exit code. The
controller's `finally` writes `observation.json` for normal exit, timeout and
launch-error paths, together with stream hashes where those files exist.

## Static verification

PowerShell's parser accepted the controller with zero parse errors. Source
inspection confirms the pre-launch `inputs.json`, `stdout.log`, `stderr.log`,
unconditional `observation.json`, literal no-retry policy and the exact
`taskkill /T /F` line.

This is controller evidence only. It proves neither process launch nor tree
termination in a real observation. S3 must first materialize the fixed v5
profile/root and admit exactly one invocation of this controller.
