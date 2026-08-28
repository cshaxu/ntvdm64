# M0 T306 S1 P3 — BaseClient / BaseSrv cohort boundary audit

## Question

Does the physical `baseclient-candidate` cohort justify importing a larger
BaseClient/Kernel32 product package, or does each reached original form stop
at a modern public API, an existing source-shaped Base VDM seam, or a later
owner package?

## Inputs

- The 54 physical `baseclient-candidate` rows in
  [the signature-seed package ledger](../operations/m0-t306-s1-signature-seed-package-ledger.tsv).
- The selected original client forms: `base/win32/client/vdm.c` lines 340,
  731, 804 and 1014, plus `base/win32/client/gmem.c:31`.
- The selected original server forms:
  `base/win32/server/srvvdm.c` lines 130, 457, 486 and 569.
- The byte-exact current `opennt-host` VDM client/server mirrors and the
  existing bounded `adapter-mvdm-host-out/basesrv` local DOS record seam.

## Procedure

Retained every physical call count from the signature-gated ledger. Grouped
only after source/path identity had already been proved, then read each
reached client/server body through its first non-composable product boundary.
Public Windows API forms were classified as leaves rather than treating their
historical BaseClient implementation files as a required import. No source
body, provider or adapter behavior was changed.

## Observations

- `GetNextVDMCommand` has six physical callers. Its client capture/capacity/
  retry order and matching DOS server record algorithm are already byte-exact
  mirror evidence. `CsrClientCallServer`, capture-buffer transport and the
  CSRSS console/DOS record shell prevent direct execution; the existing local
  `basesrv` seam is therefore retained as an explicitly source-derived,
  DOS-only implementation.
- `SetVDMCurrentDirectories` (one caller) and `ExitVDM` (fourteen callers)
  likewise cross CSR into console/process/WOW server state. Their source body
  remains the required contract, but their modern implementation belongs to a
  later Base VDM broker plus session/monitor lifecycle closure, not an import
  of the BaseClient product shell.
- `RegisterWowExec` has one caller and needs CSRSS process identity plus a
  shared WowExec notification window. `RegisterWowBaseHandlers` has two
  callers; its small `gmem.c` body only records a WOW hook. Both are retained
  for the WOW owner package and do not enlarge the current broker.
- `WaitForSingleObject` (14), `ReleaseMutex` (1), `LocalAlloc` (6),
  `LocalFree` (8), and `SetFilePointer` (1) are public Windows contracts on
  the current supported hosts. Their historical BaseClient source files are
  not selected runtime imports.

## Decision

There is no general BaseClient/Kernel32 runtime import. The exact Base VDM
client/server source already mirrored under `opennt-host` remains the primary
algorithm and ABI evidence. The required current implementation is the named,
bounded `adapter-mvdm-host-out/basesrv` local seam; it is neither a generic CSR
facade nor an app-owned command protocol. The full physical grouping and final
disposition are recorded in the companion
[cohort ledger](../operations/m0-t306-s1-baseclient-cohort-ledger.tsv).

The later Base VDM broker package owns the source-shaped directory and exit
forms. The later WOW package owns both WOW registration forms. A full CSR/CSRSS
or Kernel32/BaseClient product reconstruction is a non-invasive hard boundary.

## Confidence and follow-up

High confidence for the 54 physical source bindings and public-leaf split;
medium confidence for future `ExitVDM` lifecycle shape because it has not yet
been composed with the monitor/session package. Continue T306 with the NetAPI
cohort before admitting a provider or copying any additional source body.
