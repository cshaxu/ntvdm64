# T194 S10 one-slot observation input preparation 001

Date: 2026-08-12  
Packet: M0 T194 S10  
Disposition: execution inputs prepared; no process started.

## Bundle

`artifacts/build/current/t194-s10-v6-observation-bundle-r1` binds the S8 r2
engine with SHA-256
`D9060D2201BE0DD8F650D44ADFD1F503D9549C1699952F6ACE2F84AC623E758C`.
Its BIOS, VGA ROM and option-ROM hashes are preserved in the fixed
`ntdos64-bochs-engine-bundle-v1.txt` manifest. Bundle creation copied inputs
and wrote hashes only; it did not execute the engine.

## Controller

`tools/probe/Invoke-T194S10V6BoundedObservation.ps1` is intentionally separate from
the retained v5 controller. It accepts only profile JSON declaring both
`ntdos64-byob-profile-v6` and
`nt4-en-us-command-normal-return-v6`; writes T194/v6 input and outcome
schemas; records hashes for runner, shim, engine, profile and target; refuses
to overwrite evidence; and has `retryPolicy = 'none'`. Its only timeout action
is a one-time `taskkill /T /F` of the wrapper process tree.

Static parsing and term checks verified the v6 schema/profile names, v6 wrapper
name, no-retry record and watchdog path, while rejecting stale v5 and old
launch-kind/tail terms. No controller invocation occurred.

## Follow-up

T194 S11 may admit exactly one invocation after it identifies an exact v6
profile/root/target asset set and records why the selected TARGET maps to the
normal-return candidate. No run is implied by this preparation record.
