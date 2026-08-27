# Proposal: OpenNT Historical-Interface Adapter Convergence

## Decision sought

Before the Bochs-core diff/overlay minimization package, converge the three
general, historical OpenNT dependency facades into one `adapter-opennt`
component. This is a source-layout and build-ownership recovery package; it
does not add a BOP, machine, monitor, filesystem or child-process capability.

## Target boundary

`adapter-opennt` provides the unavailable external interfaces called by
imported `opennt-mvdm-host` source. It preserves original OpenNT spelling,
parameter layout, calling convention, ordering and failure contract whenever
the selected source reaches them. Its internal source topology is explicit:

- `win32/` — historical Win32/NTDLL/OEM façade implementations over public
  modern Windows APIs;
- `softpc/` — historical SoftPC/CCPU/SAS façade implementations, reaching
  only typed `adapter-bochs` mechanics and session-owned bounded services;
- `monitor/` — historical `NtVdmControl`/VDM-TIB/V86 event façade family.

`adapter-opennt` does not own an alternate DEM, COMMAND, XMS, DPMI, Redirector
or WOW provider. Their original algorithms remain in `opennt-mvdm-host`.

## Explicit exclusions

- `adapter-bochs` remains a Bochs-only mechanical boundary and does not gain
  OpenNT naming or policy.
- `adapter-bop` remains the copied machine-event/typed-completion bridge; it
  is not an unavailable historical OpenNT API façade.
- `adapter-redir`, `adapter-wow`, `adapter-vdd` and `adapter-debugger` remain
  separate specialist package boundaries. They are not prematurely merged
  because each represents a distinct historical product/broker contract.
- `session`, `broker` and `app` retain their current ownership. No new
  generic `compat`, `common`, `adapter-host` or catch-all adapter is created.

## Source-first basis

The original caller bodies remain selected in `opennt-mvdm-host`; only their
missing external dependency interface is supplied elsewhere. The existing
`adapter-win32`, `adapter-softpc` and `adapter-vdm-monitor` implementations
are audited recovery input, not a mandate to bulk-copy their trees. Each
retained source is assigned to exactly one of the three `adapter-opennt`
subfamilies with its original caller, interface form, replacement dependency,
failure behavior and x86/x64 evidence.

## Proposed S sequence

1. Build a complete per-file/per-public-symbol owner and dependency manifest
   for the three existing roots and all selected host consumers.
2. Admit the `adapter-opennt` root and its three internal subfamilies,
   including public include and static-library boundaries; update architecture
   authorities before moving source.
3. Migrate the Win32 façade inputs one audited family at a time, preserving
   original header/interface shapes and build selection.
4. Migrate the SoftPC/CCPU/SAS inputs; verify that every machine operation
   still goes only through `adapter-bochs` and no Bochs type reaches an
   imported MVDM source file.
5. Migrate the VDM-monitor family; preserve its declared deterministic
   unavailable operations without recreating NT kernel, CSRSS or BaseSrv.
6. Update all consumer include/build manifests, remove the three superseded
   roots, and prove no current source/build/test input refers to them.
7. Run formal x86 and x64 static composition/fixture regressions and perform
   a final dependency-direction and source-provenance audit.

## Admission evidence and exit criteria

Admission requires the current interface and package trackers plus a full
enumeration of live consumers. The task closes only when every live source
and public header formerly owned by the three roots has one final owner or a
documented deletion; no OpenNT algorithm has moved into the adapter; no
`adapter-opennt` source calls `bochs-core` directly; all selected x86/x64
graphs are updated and pass; and the architecture, coding, source-policy,
README and build manifests name the converged boundary consistently.

## Deferred work

This proposal deliberately does not decide whether future specialist adapters
should be implemented. Their creation remains tied to a complete original
owner-package and ABI audit.
