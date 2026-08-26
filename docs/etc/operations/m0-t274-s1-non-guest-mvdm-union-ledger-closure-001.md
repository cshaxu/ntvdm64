# M0 T274 S1: non-guest MVDM union-ledger closure

## Inputs and method

The reproducible generator
`tools/governance/Export-T274S1MvdmUnionLedger.ps1` recursively enumerates:

- `O:\repos.external\OpenNT\base\mvdm`;
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm`; and
- the owner-designated carried guest inventory `src.old/opennt-guest`.

It writes the complete path-level ledger
[`m0-t274-s1-non-guest-mvdm-union-ledger.tsv`](m0-t274-s1-non-guest-mvdm-union-ledger.tsv).
Each row holds both source paths/hashes, selection, owner root, transfer rule
and local guest identity when applicable.

## Result

The union has 3,671 target-relative paths:

- 3,513 dual-identical paths;
- 151 OpenNT-4.5-only paths;
- one OpenNT-only path; and
- six differing paths, all below external `wow16/` and all retained only as
  rejected external evidence in this S.

Owner/transfer cardinalities are:

- 1,689 `opennt-mvdm-host` rows, all scheduled for later host-supply import;
- 33 `tools/opennt` rows, scheduled for the tools supply subtask;
- 676 `dos/v86` rows that exactly match the directly carried local guest
  mirror;
- 12 existing WOW16/bin86 rows that exactly match the directly carried local
  guest inventory; and
- 1,261 external WOW16 source rows outside that current carried inventory.

The latter 1,261 rows are **not** a permission to copy them now. The owner
direction is to recover the established local DOS/WOW/bin86 inputs directly,
without a second external import. They remain ledger evidence only. A later
guest-source closure must either prove the established selected inventory is
sufficient for its declared artifact or explicitly admit a source-supply
change; it may not silently consume these external paths.

For any same-path conflict, the generator selects the OpenNT primary NT4 source
consistently and retains the OpenNT-4.5 path/hash as rejected evidence. Since
all six conflicts are outside current non-guest host supply, no host package
mixes variants in this S.

## S1 disposition

S1 is closed: every non-guest host path has a unique selected provenance and
owner, and guest/tool paths have explicit non-host dispositions. No source was
imported or compiled. S2 may now import only exact required declarations from
outside MVDM; it cannot change this selection ledger.
