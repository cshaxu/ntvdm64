# M0 T291 S4 P1 — `NtVdmControl` capability separation

## Question

Can the only currently reached `NtVdmControl(VdmQueryDir)` path retain its
historical entry/status shape without sharing the BaseSrv-shaped
`GetNextVDMCommand` session route?

## Source and boundary

Original `dos/dem/demsrch.c` calls `NtVdmControl(VdmQueryDir, ...)` while
advancing a directory enumeration. That is a VDM monitor/control operation;
it is not a command broker request. The historical destination was a private
kernel VDM service, unavailable on modern Windows.

`adapter-mvdm-host-out/monitor` now owns a separately typed,
thread-bound `VdmQueryDir` capability. It retains `NtVdmControl`,
`VDMSERVICECLASS`, `VDMQUERYDIRINFO` and `NTSTATUS` shape. A callback may bind
only while its owning session is active and bound to the current thread. All
other service classes, missing binding, stale owner and bad input retain
`STATUS_NOT_IMPLEMENTED` rather than falling into command routing.

## Verification

The formal `New-T280VdmControlNinja.ps1` graph rebuilt and ran on both host
architectures:

- MSVC x86 `/MT`: `PASS: VdmQueryDir adapter contract`;
- MSVC x64 `/MT`: `PASS: VdmQueryDir adapter contract`.

The fixture proves unavailable before session/bind, unavailable when bound
without the typed capability, rejection of another service class, successful
typed `VdmQueryDir` and explicit unbind before session disposal.

## Remaining S4 work

This closes only the current monitor-control route conflation. It neither
implements all original `NtVdmControl` classes nor a kernel VDM service.
`GetNextVDMCommand` broker semantics, per-call-site `host_simulate`
postconditions and expanded WOW task lifecycle remain separate S4 groups.
