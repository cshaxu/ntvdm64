# Proposal: MVDM-Host Historical-Interface Adapter Convergence

## Decision sought

Before the Bochs-core diff/overlay minimization package, converge every
OpenNT-facing historical dependency facade into one `adapter-mvdm-host-out`
component. This is a source-layout and build-ownership recovery package; it
does not add a BOP, machine, monitor, filesystem, Redirector, WOW, VDD,
debugger or child-process capability.

## Target boundary

`adapter-mvdm-host-out` provides the unavailable external interfaces called by
imported `mvdm-host` source. It preserves original OpenNT spelling,
parameter layout, calling convention, ordering and failure contract whenever
the selected source reaches them. Its internal source topology is explicit and
family-preserving:

- `win32/` — historical Win32/NTDLL/OEM façade implementations over public
  modern Windows APIs;
- `softpc/` — historical SoftPC/CCPU/SAS façade implementations, reaching
  only typed `adapter-bochs` mechanics and session-owned bounded services;
- `monitor/` — historical `NtVdmControl`/VDM-TIB/V86 event façade family.
- `redir/` — historical VDMREDIR/Redirector product-interface façade family.
- `wow/` — historical WOW32/WOWEXEC product-interface façade family.
- `vdd/` — historical VDD product-interface and host-callback façade family.
- `debugger/` — historical BDE/DBG/VDMDBG/VDMEXTS product-interface façade
  family.

`adapter-mvdm-host-out` does not own an alternate DEM, COMMAND, XMS, DPMI,
Redirector, WOW, VDD or debugger provider. Their original algorithms remain
in `mvdm-host`.

## Explicit exclusions

- `adapter-bochs` remains a Bochs-only mechanical boundary and does not gain
  OpenNT naming or policy.
- `adapter-mvdm-host-in` remains the copied Bochs-to-MVDM machine-event and
  typed-completion bridge; it is not an outgoing historical API façade.
- `session`, `broker` and `app` retain their current ownership. No new
  generic `compat`, `common`, `adapter-host` or catch-all adapter is created.

## Source-first basis

The original caller bodies remain selected in `mvdm-host`; only their missing
external dependency interface is supplied elsewhere. The existing
`adapter-win32`, `adapter-softpc`, `adapter-vdm-monitor`, `adapter-redir`,
`adapter-wow`, `adapter-vdd` and `adapter-debugger` implementations are
audited recovery input, not a mandate to bulk-copy their trees. Each retained
source is assigned to exactly one named `adapter-mvdm-host-out` subfamily with its
original caller, interface form, replacement dependency, failure behavior and
x86/x64 evidence.

## Proposed S sequence

1. Build a complete per-file/per-public-symbol owner and dependency manifest
   for the seven existing OpenNT-facing roots and all selected host consumers.
2. Rename the product components exactly once: `adapter-bop` becomes
   `adapter-mvdm-host-in`; the seven mirror roots become `mvdm-guest-dos`,
   `mvdm-guest-win16`, `mvdm-softpc-firmware`, `mvdm-host`, `mvdm-support`,
   `mvdm-tools` and `mvdm-platform-abi`. Update every live code/build/document
   reference before an adapter source move.
3. Admit the `adapter-mvdm-host-out` root and its seven internal subfamilies,
   including public include and static-library boundaries; update architecture
   authorities before moving source.
4. Migrate the Win32 façade inputs one audited family at a time, preserving
   original header/interface shapes and build selection.
5. Migrate the SoftPC/CCPU/SAS inputs; verify that every machine operation
   still goes only through `adapter-bochs` and no Bochs type reaches an
   imported MVDM source file.
6. Migrate the VDM-monitor, Redirector, WOW, VDD and debugger families;
   preserve their declared deterministic unavailable operations without
   recreating NT kernel, CSRSS, BaseSrv or private product brokers.
7. Update all consumer include/build manifests, remove the seven superseded
   roots, and prove no current source/build/test input refers to them.
8. Run formal x86 and x64 static composition/fixture regressions and perform
   a final dependency-direction and source-provenance audit.

## Admission evidence and exit criteria

Admission requires the current interface and package trackers plus a full
enumeration of live consumers. The task closes only when every live source
and public header formerly owned by the seven roots has one final owner or a
documented deletion; all seven MVDM mirror roots use their approved names; no
OpenNT algorithm has moved into the adapter; no `adapter-mvdm-host-out` source
calls `bochs-core` directly; all selected x86/x64 graphs are updated and pass;
and the architecture, coding, source-policy, README and build manifests name
the converged boundary consistently.

## Deferred work

This proposal does not authorize a generic compatibility layer. The seven
historical interface families remain separately named directories, manifests,
static-library inputs and test groupings inside one component; a source may
not cross from one family to another merely because they share the component
root.
