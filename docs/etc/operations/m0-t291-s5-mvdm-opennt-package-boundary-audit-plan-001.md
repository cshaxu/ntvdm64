# M0 T291 S5 P5 — MVDM/OpenNT package-boundary and final-disposition audit

## Purpose

Replace symbol-led recovery with a complete package-first decision for the
selected MVDM host union and every non-MVDM OpenNT package directly reached
from it. This is an audit-only packet. It deliberately stops before provider,
adapter or broker implementation so the owner can review the complete boundary.

## Governing decisions

1. `mvdm-host` retains the complete selected non-guest MVDM host-package
   union. A source file not enabled in the first runtime profile remains an
   exact mirror with a final disposition; it is not discarded.
2. A non-MVDM package is examined at complete-package scope, but only its
   minimum required original slice enters `opennt-host`. The slice must have a
   selected direct MVDM caller, preserve a non-trivial original service/state
   machine, and have a finite outward closure through public modern Windows
   APIs, existing bounded adapters or a specifically admitted small adapter.
3. CSR/CSRSS, NTDLL CSR transport, kernel VDM, full BaseClient/Kernel32,
   Win32k and USER/GDI server are stopping boundaries. Their source may prove
   an interface contract but cannot be recursively imported as a host-runtime
   dependency.
4. Existing `app`, `session`, `broker` and adapter code is reviewed as a
   possible reversion target. It is not a reason to retain a parallel provider
   once a selected original package can own that behavior.

## Required outputs

- Classify every selected MVDM host file in
  `mvdm-file-recovery-ledger.tsv` as `exact-direct`,
  `exact-adapter-bound`, `minimal-binding-diff`, or
  `mirror-only/profile-excluded`, with a package-level explanation where a
  family shares a single exclusion.
- Expand the interface ledger for every outward edge of each selected MVDM
  package: original declaration/caller, owner, modern binding or failure,
  prerequisite and final state. Textual declaration rows do not count.
- Complete package and build/profile rows for all selected MVDM host packages;
  record every package-to-package edge, external candidate and source cycle.
- Maintain `opennt-non-mvdm-package-boundary-ledger.tsv` for every candidate
  outside MVDM, including rejected NT4 product-shell packages.
- Add a reversion row for every current project-owned implementation that an
  accepted original external package could replace. A row may be `not-yet-
  eligible`, but may not be omitted.
- Produce an evidence report with counts, accepted package roots, rejected
  package roots, finite closure proof, unresolved rows and successor ordering.

## Initial candidate perimeter

The current evidence identifies the following initial perimeter; the audit
must prove or revise it rather than treating it as a source import list:

- Base VDM service vertical slice: `base/win32/server/srvvdm.c` and its VDM
  records, `base/win32/client/vdm.c` VDM calls, and their directly required
  declarations. This is the only currently accepted external-source candidate.
- `base/ntdll/csrutil.c` and `base/subsys/csr/server`: reject as the private
  CSR transport/server product shell, while recording each reached call as an
  `adapter-opennt-host` boundary.
- `base/ntos/vdm`: reject as kernel VDM; preserve `NtVdmControl` declarations
  and use the MVDM monitor adapter rather than importing kernel implementation.
- Remaining BaseClient/BaseSrv, Win32k, USER/GDI server, private graphics and
  Shell product packages: reject unless the audit finds a distinct direct MVDM
  package edge whose entire closure passes the admission test.

## Method

For each selected package, use original `sources`/`makefile`, direct includes,
declarations, called symbols, declared libraries and cross-package source
definitions. Resolve edges in this order: selected MVDM package; accepted
OpenNT-host package; original declaration-only carrier; existing adapter;
finite new specialist adapter; public modern API; explicit stopping boundary.
No compile error and no trace determines the ordering.

## Required execution sequence

This packet is one audit closure, but its work is deliberately sequenced so a
newly noticed symbol cannot create a second, untracked import route.

1. Reconcile the immutable T276 1,689-path final-disposition evidence into the
   live file, package, build and divergence ledgers; retain the historical
   ledger as evidence rather than maintaining a parallel live conclusion.
2. Audit every selected `mvdm-host` package at complete package/build scope:
   source membership, internal edges, external interface families, profile
   exclusions and current autonomous reversion candidates.
3. For every discovered external OpenNT candidate, audit the entire upstream
   package boundary before selecting a required original slice or rejecting the
   package. Record all selected and rejected source paths, outgoing edges and
   stopping boundaries in the external ledger.
4. Expand every reached outward edge to a child interface row with original
   caller/declaration/layout, same-shaped owner, x86/x64 mapping effect,
   unavailable failure contract and prerequisite. A family summary is not an
   interface closure.
5. Reconcile build/profile input rows and reversion rows, report exact counts
   and unresolved items, then stop for owner review. The next queue package
   consumes this manifest to create static source closures; it does not use a
   compile error or a trace as discovery.

## Stop and exit criteria

Stop immediately for owner review when all tracker rows have a final package,
file, interface and build disposition, each accepted external package has a
finite closure proof, each rejected candidate names its stopping boundary, and
every possible reversion target is recorded. No implementation, source-body
change or formal runtime build is part of this packet.
