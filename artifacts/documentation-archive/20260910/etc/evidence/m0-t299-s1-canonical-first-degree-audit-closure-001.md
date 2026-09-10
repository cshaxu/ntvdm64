# M0 T299 S1 — canonical first-degree original-source audit closure

## Scope and invariant

T299 consumes only T298's 801 canonical first-degree external interface
spellings and their immutable raw-call identities.  Definitions, bodies and
outgoing calls were read only from the approved OpenNT and OpenNT-4.5 source
trees.  No current mirror modification, adapter, build output or `src.old`
path is an input.

## Resolution procedure

The original include frontier from each zero-degree caller first limits source
body search to the directly indicated original package roots.  If that scope
contains no body, a discovery-only search of both approved trees enumerates
all source candidates; it does not select a provider.  Byte-identical
cross-tree definitions are one identity with explicit source aliases.  Only
content-distinct candidates remain variant identities.

All-uppercase function-like macro expressions are retained in per-body
summaries but are not emitted as false second-degree function candidates.

## Complete first-degree result

- All 801 canonical first-degree interfaces have exactly one resolution row.
- 362 are original public Win32/CRT leaf interfaces; T299 correctly does not
  descend into their system implementation bodies.
- 374 terminate at already-recorded original public/private/kernel or
  not-host-runtime boundaries; no unsupported product shell was searched past.
- 63 original C/C++ function definition identities are located from 66
  original source files: 48 unique definition resolutions and six explicit
  content-distinct variant families.
- Eleven interface spellings have no C/C++ body in either approved tree even
  after discovery fallback: `AsyncDispositionRoutine`,
  `call_ica_hw_interrupt`, `FastWOWCallbackCall`, `GdpOffsetFromName`,
  `GetFastBopEntryAddress`, `GetFaxDataMapName`, `GetFreeSystemResources`,
  `host_config_init`, `VdmSetPhysRecStructs`, `W32SetExceptionContext`, and
  `WK32ICallProc32MakeCall`.  They remain explicit declaration/assembly or
  historical-boundary items, not fabricated source definitions.
- The 63 located bodies yield 481 direct, non-macro initial second-degree
  candidates.  T299 neither reads nor resolves their bodies.

## Outputs

- `mvdm-host-first-degree-include-frontier-ledger.tsv` is the raw-call
  include-boundary input.
- `mvdm-host-first-degree-original-resolution-ledger.tsv` provides one result
  for each canonical interface.
- `mvdm-host-first-degree-original-definition-ledger.tsv` preserves selected
  source root/path/hash/line, aliases, variants and direct-call summary.
- `mvdm-host-second-degree-initial-candidate-ledger.tsv` is the sole valid
  starting frontier for a later second-degree task.

Run `node tools/governance/export-t299-s1-first-degree-original-ledgers.mjs`
with Node 22 to regenerate the three derived ledgers.
