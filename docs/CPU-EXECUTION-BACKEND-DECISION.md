# CPU Execution Backend Decision

> **Archived decision (2026-08-08):** This document preserves earlier WHPX
> and external-backend investigation only. It is superseded for the current
> route by `SOFTPC-RECONSTRUCTION-GOVERNANCE.md`: repair and compose the
> OpenNT SoftPC/CCPU path. No external backend, including WHPX, may be
> scheduled, linked, or used as an execution substitute by this repository.

Its fixture observations remain historical evidence, not a preferred execution
candidate or a work order.

## Finding

The checked host has no installed QEMU, Bochs, or Unicorn executable/library.
The current `nvtdm --probe-*` interpreter is therefore retained only as a
bounded test harness. It is not a candidate execution core for `NTIO.SYS`,
`NTDOS.SYS`, `HIMEM.SYS`, DOSX, DPMI, or V86 workloads.

## External Candidates Considered

| Candidate | Relevant capability | Constraint | Decision |
| --- | --- | --- | --- |
| Windows Hypervisor Platform | documented user-mode partition, GPA mapping and virtual-processor APIs | Windows x64 host with an available hypervisor; must remain optional and never enable Windows features itself | selected experimental backend on this host; HLT fixture verified |
| Unicorn | C CPU emulation framework advertising x86 16/32/64-bit and Windows support | GPLv2; CPU framework only, so machine, devices and service traps remain owned work | do not acquire or link until licensing/product decision |
| QEMU system emulation | full x86 machine, CPU and device models, Windows-supported TCG | GPLv2; a complete PC topology is materially different from the desired direct NTDOS adapter | possible disposable research process only; not a normal dependency |
| Historical NTVDMx64/HAXM route | historical VDM execution evidence | requires unavailable source inputs and intrusive host integration that violates this project | excluded from runtime path |
| Handwritten probe | controlled tests of owned loader/service boundaries | not remotely complete enough for 386/V86/DPMI correctness | retained as test scaffolding only |

The upstream descriptions used for this decision are the
[Unicorn project](https://github.com/unicorn-engine/unicorn) and
[QEMU system-emulation documentation](https://qemu-project.gitlab.io/qemu/system/introduction.html).

## Required Decision Before Bootstrap Execution

The selected initial route is WHPX. To execute a real selected NTDOS profile,
the project must still complete its WHPX machine adapter (memory exits, I/O,
interrupt injection, lifecycle and trace) before interpreting this as a
bootstrap-capable result. If WHPX is unavailable on another host, the project
must choose one of these explicitly reviewed alternatives:

1. a separately licensed/compatible reusable CPU backend, wrapped by a narrow
   owned machine interface;
2. a separately distributed GPL research executable/process, if an owner
   accepts its distribution and product boundary; or
3. an owner-approved implementation plan for a complete 386 execution core,
   including protected mode, V86, exceptions, descriptors, A20, IRET, and
   deterministic interrupt timing.

No route may make private Windows loader/console APIs, injection, registry
configuration, or host process patching part of the solution.

## Current Consequence

Continue building the owned WHPX machine adapter, loader, guest-memory, runtime
identity, and host capabilities. Do not claim that the probe or HLT fixture can
execute any BYOB NTDOS binary. The next implementation task is an adapter-level
I/O-exit/bootstrap trace runner, not another arbitrary instruction subset.
