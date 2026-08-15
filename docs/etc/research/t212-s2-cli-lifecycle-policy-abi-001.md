# T212 S2: CLI Lifecycle Policy and Result ABI

Packet: M0 T212 S2, Ordinary Mode  
Date: 2026-08-15  
Status: source-built closure

## Question

Can the CLI describe a lifecycle budget, a recorded cancellation request and a
terminal presentation without changing the engine contract or allowing a host
event to fabricate a guest result?

## Delivered ABI

`src/cli/ntdos64_lifecycle_v1.[hc]` owns two fixed-width copied records:

| Record | Fields and owner | Explicit exclusions |
| --- | --- | --- |
| `ntdos64_lifecycle_v1_policy` | CLI-owned instruction budget and recorded cancellation-request state, with magic/version/size and zeroed reserved fields | No Bochs object, guest address/register, callback, timer ID or Windows handle |
| `ntdos64_lifecycle_v1_audit` | CLI-owned copied policy state, returned engine terminal/detail, lifecycle classification and presentation classification | No mutable engine state, guest exit result or durable host-I/O ownership |

The classifier consumes the unchanged
`bx_ntvdm_engine_result_v1`. It maps each valid engine terminal to exactly one
distinct lifecycle/presentation value. `ORDINARY_GUEST_COMPLETION` and
`HOST_CANCELLATION` are represented only when the engine actually returns
those values; S2 creates no producer for either. In particular a policy that
records `CANCELLATION_REQUESTED` preserves a returned controlled terminal as
controlled terminal.

Invalid policy and invalid engine results fail closed into separate audit
classifications. The ABI is CLI-owned but includes the existing engine header
only to interpret the already copied result; it adds no member or API to the
engine contract.

## Verification

Command:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T212S2CliLifecyclePolicyProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t212-s2-cli-lifecycle-policy-r2
```

Observed: a fresh MSVC x64 `/MT` build compiled
`ntdos64_lifecycle_v1.c`, the unchanged engine-contract validator, and
`tests/runner/ntdos64_lifecycle_v1_test.c`; the executable exited `0`.

The fixture proves:

- valid and malformed policy records, including zero budget, reserved-field
  and invalid-cancellation rejection;
- every valid engine terminal has one auditable classification;
- cancellation-request state cannot change a controlled engine terminal into
  host cancellation;
- malformed copied engine results fail as `INVALID_ENGINE_RESULT`.

The declared source closure excludes `bochs.h`, `src/bx-core`, `src/bx-vdm`,
guest execution and Windows `HANDLE` values. A focused source scan found the
excluded words only in a boundary comment, not in the ABI data or code.

## Limitation and follow-up

The active native CLI still forms its hard-coded budget and prints the legacy
two numeric engine fields. S2 intentionally does not alter that public path.

T212 S3 may integrate the existing native CLI with this ABI: form the default
policy, derive its existing engine request budget from it, classify the
returned copied engine result after `engine_run_v1` returns, and present the
separate classification. It may not add in-flight cancellation, host process
termination, durable audit I/O, a normal-completion producer, BOP behavior or
any engine/mantle/Bochs change.
