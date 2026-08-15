# T203 S1 passive normal-return observation

## Question

Does the current source-built CPU5 native fixture reach the original COMMAND
normal-return BOP (`C4 C4 54 0B`) before its existing declared-plan terminal?

## Inputs and procedure

- Current `src/bx-vdm` composition manifest and the locked S93 source-built
  `COMMAND.COM` / `SHARE.EXE` fixture inputs.
- `bx_ntvdm_normal_return_observation_v1`: default-off, copied-data-only test
  observation.  It records only an already accepted `54:0B` `RESUME` outcome
  whose resume IP is the existing `fault_rip + 4`; it has no guest-memory,
  CPU, timer, provider-selection, or result-mutation path.
- `tools\probe\Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1` with build
  root `artifacts/build/t203-s1-normal-540b-native-r5`.

The command rebuilt the entire current composition manifest under MSVC x64
`/MT`, linked successfully, and ran the bounded CPU5/Pentium-MMX fixture once.

The focused copied-observer regression also compiled and passed under MSVC x64
`/MT`.  It proves disabled observation cannot capture, a non-`54:0B` window
cannot capture, a wrong resume target cannot capture, and one valid existing
`54:0B` resume is copied with its DX-to-AX evidence.

## Observation

The emitted JSON records `runExitCode: 2`; compilation and linking succeeded.
The final line of `run.log` is:

```text
t203-s1 normal-return observed=0 captured=0 cs=0000 eip=0000 dx=0000 ax=0000 disposition=0 resume=0
```

The same run retains the pre-existing bounded terminal fact: `status=4`
(`BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP`) and terminal CPU position
`95ab:03c3`.  The source map identifies the terminal as the current COMMAND
session's typed `54:01` declared-plan-exhaustion STOP: after one target slot
has been delivered, `get_next.delivered >= launch_plan.slot_count` returns
`bx_ntvdm_cpu_result_v2_stop` in
`src/bx-vdm/bx_ntvdm_command_package_session_v1.c`.

## Interpretation and confidence

This is a reproducible, passive native **non-observation** of `54:0B` on the
current fixture path.  It is stronger than the superseded r3 result because
the recorder does not use the old first-generic-UD stop assertion and reports
only an already accepted normal-return resume.

It is not an engine terminal-result proof: the fixture calls the finite-run
mechanic and its test bridge mirrors the bound composition, rather than
entering through `bx_ntvdm_engine_run_v1` and the product CLI.  Therefore it
does not authorize a terminal-result ABI, a synthetic `54:0B`, or any BOP
provider change.

## Follow-up

T203/S1 remains limited to terminal transport admission.  A future qualified
normal-return test must use the engine's copied request/install/lifecycle path
without converting any unrelated event into STOP.  Until then the engine's
`ORDINARY_GUEST_COMPLETION` enum remains unproduced and no DOS exit value may
cross to the CLI.
