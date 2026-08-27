# M0 T292 S1 — stage-one MVDM egress inventory closure

## Question

Has every outward dependency expression in the selected `mvdm-host` union been
recorded with a first-hop disposition, before any non-MVDM OpenNT package is
selected or expanded?

## Inputs

- The 1,689-path selected MVDM host union and its historical source/build
  records.
- The complete local OpenNT source tree under `O:\repos.external\opennt`.
- MVDM include, original build-control, selected-definition and declaration
  ledgers.

## Procedure

1. Extract every MVDM include occurrence, normalize it to a unique first-hop
   include edge, and classify it as selected MVDM input, public declaration,
   device contract, named non-MVDM package candidate, or source-layout input.
2. Extract all original build/link and include-path records, preserving their
   source line, and map every library to selected MVDM, CRT, public Win32 or a
   named external package candidate.
3. Replace the incomplete historical call-correlation input with a raw scan of
   every C/C++ `identifier(` occurrence. Correlate names against selected
   MVDM/platform declarations and then against non-MVDM OpenNT headers.
4. Record all non-function `extern` data/function-pointer declarations. Their
   first hop is always the selected MVDM declaration package; resolving the
   definition is explicitly an obligation of that package's later BFS
   expansion, not a reason to invent an external package.
5. Add every discovered non-MVDM package root to the breadth-first node ledger
   and external-boundary ledger. Do not select a package for import.

## Observations

- 5,171 include occurrences reduce to 514 unique include edges. Their first
  hop states are fully assigned: 413 selected-MVDM/platform/firmware inputs,
  30 external package/declaration carriers, 21 public-or-private Win32 client
  candidates, 20 CRT/public leaves, 9 native-NT contracts, 8 DDK contracts,
  6 local source-layout records, 4 MVDM VDD/debug contracts and 3 direct
  non-MVDM source-package candidates.
- 246 original build edges are fully assigned: 196 selected-MVDM/CRT, 36
  public API leaves and 14 named external-package candidates. The latter
  establish `base/ntdll`, USER/GDI private clients and the NetAPI/RAP/Rx/
  XactSrv package family as real graph seeds rather than speculative imports.
- The raw invocation inventory contains 92,459 source sites and 10,956 unique
  spellings. Its deduplicated 5,988 non-local symbol candidates classify as
  3,146 selected-MVDM/platform declarations, 950 non-MVDM upstream declaration
  candidates and 1,892 macro/CRT/public-import/assembly leaf candidates.
- All 1,069 declared data/function-pointer records now have a first hop:
  their declaring selected MVDM package. No raw pointer or HANDLE identity is
  inferred from the lexical declaration.
- The first frontier contains the original Base VDM, BaseClient/NTDLL/CSRSS,
  kernel VDM, Console, USER/GDI, NetAPI/RAP/Rx/XactSrv, OLE, Winsock, Spooler,
  Shell, RTL, declaration-carrier and device-contract roots. The node ledger
  preserves parent edges and depth.

## Interpretation

Stage one is complete as an **egress inventory**: every extracted MVDM outward
expression has a durable first-hop destination category, and every non-MVDM
source-package candidate has a node in the BFS frontier. It does not claim
that a same-named upstream header is the exact callable definition, nor that
any candidate package is importable. Those are precisely the second-stage
package/file/function expansion questions.

The raw invocation scan intentionally retains macro and false-positive lexical
forms. It is the conservative source inventory; stage two resolves reached
declarations/definitions in package context rather than deleting ambiguous
source evidence.

## Follow-up

Begin stage two breadth-first with the direct, build-proven packages: Base VDM,
`base/ntdll`, `windows/core/ntcon`, `windows/core/ntuser`,
`windows/core/ntgdi`, `ds/netapi/netlib`, `ds/netapi/rap`,
`ds/netapi/rpcxlate/{rxcommon,rxapi}` and `ds/netapi/xactsrv`. Then expand
their source/build edges before moving to their children. No package import,
adapter body or selector enablement is authorized by this record.
