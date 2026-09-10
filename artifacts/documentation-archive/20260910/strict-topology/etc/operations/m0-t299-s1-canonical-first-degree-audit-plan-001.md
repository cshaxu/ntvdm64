# M0 T299 S1 — canonical first-degree original-source audit plan

## Objective

Resolve the 801 canonical external interface spellings produced by T298 against
the approved OpenNT and OpenNT-4.5 source trees. This is the first BFS
expansion from the complete `mvdm-host` zero-degree definition set.

## Inputs and boundary

- The T298 canonical external-interface ledger and its complete raw-call
  resolution ledger are immutable inputs.
- Source definitions, declarations, include candidates and outgoing calls are
  read only from the two approved original source trees.
- Current `src/mvdm-host` divergences, all adapters, build outputs and
  `src.old` are prohibited inputs. Their binding edges are a later, separate
  divergence-ledger concern.

## Required output

- Every one of 801 interface spellings receives exactly one resolution:
  public Win32/CRT leaf, original-source definition identity or explicit
  unresolved original-source absence.
- Every located original definition retains source root, path, hash, line,
  linkage and variant identity; same spellings are never merged merely by
  name.
- Each located body receives a leaf classification. Its direct unresolved
  original calls are emitted as prospective second-degree candidates only;
  this packet does not inspect their bodies.
- The source search records include-frontier pruning and both-tree search
  evidence so later BFS passes can reproduce the boundary.  If the constrained
  original package has no body, a discovery-only fallback enumerates every
  approved-tree definition candidate without selecting a provider.

## Non-goals

No source import, mirror modification, adapter implementation, binding,
compile/link work, provider route, second-degree body inspection or runtime
claim is admitted.

## Exit criteria

All 801 canonical interfaces, every selected original definition and every
outgoing second-degree candidate are accounted for with reproducible original
source evidence. Any ambiguous variant remains explicit rather than being
silently selected.
