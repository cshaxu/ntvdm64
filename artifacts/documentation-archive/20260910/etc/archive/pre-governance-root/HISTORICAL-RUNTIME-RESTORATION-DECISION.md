# Historical Runtime Restoration Decision

Status: accepted research architecture decision, 2026-08-06.

## Decision

The research runtime must first restore the historical responsibility chain:

```text
CCPU / SoftPC monitor
  -> BIOS[]
  -> nt_bop / MS_bop_0
  -> DemDispatch
  -> DEM
  -> NTDOS
```

The slash in `CCPU / SoftPC monitor` denotes two historical execution entries
into the same BIOS table, not a requirement to route CCPU through the V86
monitor. The direct CCPU interpreter uses its historical `bop(n)` macro to
call `BIOS[n]`; the V86 monitor uses `VDMEVENTINFO` and `EventVdmBop`. The
selected build profile must preserve one of those original routes without
inserting a self-authored BOP dispatcher.

## Current Execution Disposition

R5 established that the forced CCPU initialization probe stops at
`InitNtCpuInfo`. The original NT4 build definitions show that this is not an
x86 production-profile omission: x86 selects the hardware V86 `MONITOR` path,
whereas `CPU_40_STYLE`/CCPU-generated state belongs to non-x86 profiles. The
historical CCPU/BIOS/BOP/DEM material is therefore an oracle and trace source,
not the current executable x86 profile. See
`M53-HISTORICAL-GENERATED-ABI-RECOVERY-AUDIT.md`.

The normal research runtime has no DOS execution engine today. It continues to
exclude processor V86 mode, `NtVdmControl`, a kernel VDM service, V86-monitor
compatibility shims, replacement DOS SVC dispatchers, replacement BIOS BOP
tables, and replacement PC/AT device semantics. The next executable route is
the separately named owned user-mode monitor, which must consume historical
traces as conformance oracles without claiming to restore this composition.

## Frozen Instrumentation

The following existing self-authored material is bounded startup-trace
instrumentation only:

- CCPU adapter/lifecycle harnesses;
- individual BIOS BOP or DEM service handlers;
- scaffold IVT/BDA/ROM initialization profiles;
- instruction/BOP observations and bounded execution probes.

They may be retained as repeatable tests that answer a narrowly stated
startup-trace question. They must not acquire new DOS services, device
behavior, boot policy, or compatibility claims, and they must not sit on the
normal runtime path once the original historical component is available.

## Modern Host Shim Admission Rule

A shim is admissible only when each implementation item records all of:

1. the exact historical caller, symbol, or missing host facility it replaces;
2. the retained historical caller/callee boundary and data flow;
3. the private modern implementation boundary; and
4. a bounded verification that distinguishes the replacement from a new DOS,
   BOP, or PC/AT behavior.

Permitted shim subjects are unavailable host APIs and their necessary context,
event, console, process, filesystem, media, input, timing, and scheduling
bindings. The current evidence identifies `NtVdmControl` execution/event
production and related monitor context as central examples.

The shim must not define an NT-specific SVC table, assign meaning to a BIOS
BOP selector, or silently supply PC/AT semantics that a historical module is
expected to own.

In particular, an individual BOP reaching a self-authored handler is only
startup-trace evidence. It is never normal-runtime evidence. Each new shim
must name the specific historical host dependency it replaces; "one more BOP
progressed" is not an admissible capability claim.

## Acceptance Evidence

Progress is measured by restored historical chain links, not by a count of
handled BOPs. A claimed runtime capability must identify the original module
that executed it and the shim dependency, if any, that enabled that module.

The first end-to-end target remains a private, bounded smoke sequence:

```text
NTIO.SYS -> NTDOS.SYS -> COMMAND.COM /C <owned test command>
```

It requires a command exit status, an auditable historical-chain trace, and
no host-global installation, injection, registry mutation, or protected guest
artifact redistribution.

## Non-Adopted NTVDMx64 Comparison

The read-only NTVDMx64 experimental HAXM monitor is useful only as a
comparison point. Its `cpu_simulate` loop turns execution exits into an
`EventVdmBop` call and still dispatches through `BIOS[bop]`; that corroborates
the historical table boundary above. It is not an implementation source for
this project: the event producer is HAXM-specific, it recognizes the historic
`C4 C4` encoding directly, and its loader patches `ntdll!NtVdmControl` with
`VirtualProtect` and in-place writes. The last behavior is categorically
outside the non-invasive CLI research profile.
