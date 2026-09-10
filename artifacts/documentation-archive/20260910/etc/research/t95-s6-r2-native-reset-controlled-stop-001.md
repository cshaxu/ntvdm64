# T95 S6 r2 Native Reset-Controlled-Stop 001

## Question

Can the newly linked narrow startup-transaction companion execute its native
Bochs PC reset lifecycle and reach a bounded stop without adapter or guest
input?

## Inputs and procedure

`Invoke-T95S6NativeResetControlledStopObservation.ps1` was invoked once with
the r2 binary, `pc-reset-floor-v1`, a new evidence root, and its fixed
`-benchmark 1` command. Its generated wrapper clears `NTDOS64_UD_TEST`,
`NTDOS64_ADAPTER_BRIDGE`, `NTDOS64_ADAPTER_PROFILE`, and
`NTDOS64_ADAPTER_ROOT` before process creation. A second invocation against
the same evidence root refused overwrite before mutation.

## Observations

The authoritative record is
`artifacts/analysis/t95-s6-r2-native-reset-stop-observation-001-20260811-001/observation.json`
(SHA-256 `E80530B852086C1EAB3424816A1F899E09F4D88EFA446485DDF36C4085E152FF`).
It records:

- r2 binary hash `7B261E7F2F9D6CA28E70A6758845C40112DC982A80A248F143B1DCDC7A3A041E`;
- the command `ntdos64-native-startup-transaction.exe -q -f
  intact-native-runtime-profile.bochsrc -benchmark 1`;
- 3,271 ms elapsed, no watchdog termination, and native nogui exit `1`;
- `Bochs benchmark mode is ON (~1 millions of ticks)`;
- original `Reset(HARDWARE)`; and
- `cpu loop quit, shutting down simulator` at tick `10,000,005`.

The copied reset-floor profile hash is
`EA58157828EB29CA4988B925B9F07B1B9222C00FD83B6C0E87E001D1DA4C7FDA`.
The observer's command contains no adapter/guest term, and its record confirms
the four adapter environment variables were cleared.

## Interpretation

The r2 companion has a verified mechanics-only native execution floor:
reset → finite benchmark budget → controlled simulator stop. This is stronger
than its link evidence but deliberately does not exercise the execution-plan
or #UD bridge. No NTIO, NTDOS, COMMAND, BOP, DEM, DOS filesystem, host
capability, CLI, or runnable-NTVDM behavior is established.

## Follow-up

The next independent gate is an exact startup-plan/real-NTIO fixture design:
it must supply immutable BYOB inputs, prove the pre-loop plan application, and
then separately prove the single generic #UD/write/resume transaction. It may
not reuse this no-adapter observation as guest evidence.
