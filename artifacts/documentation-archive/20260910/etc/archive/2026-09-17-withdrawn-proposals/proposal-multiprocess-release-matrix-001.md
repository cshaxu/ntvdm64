# Concurrent-worker release acceptance

> **Withdrawn on 2026-09-17.** The owner confirmed that the existing
> `run16.exe`/`basesrv.exe`/multiple-`ntvdm.exe` architecture already provides
> the intended multi-task capability. `WRITE.EXE` workload recovery is the
> meaningful remaining gate; this retained proposal is not a Queue candidate
> and cannot be admitted without a new owner request.

## Objective

Verify the existing brokered multi-worker x86 product under its release
workloads: concurrent DOS/Win16 tasks, worker and broker loss, Console/stream
isolation, task observation/control, portable media layout and package
publication. This task adds no replacement broker, worker scheduler or process
architecture.

## Boundary

The product already consists of `run16.exe`, `basesrv.exe` and one or more
`ntvdm.exe` workers. Host x64 Windows is an x86-worker acceptance platform,
not a native-x64 worker requirement. This package validates that existing
architecture; it does not reopen its implementation or a one-worker WOW16
owner contract.

## Exit evidence

Record the selected concurrent matrix results, artifact provenance,
supported-host result and every remaining explicit limitation. A failed row
is assigned to a concrete original owner or existing component; it is not
authority to invent a new broker mechanism during release acceptance.
