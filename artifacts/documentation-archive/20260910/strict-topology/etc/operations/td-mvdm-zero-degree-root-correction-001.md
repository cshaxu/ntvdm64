# MVDM Source-Graph Zero-Degree Root Correction

## Question

What counts as zero-degree in the OpenNT MVDM source-function BFS after the
project split the original `mvdm` tree into runtime, support, tool, firmware,
and guest mirror components?

## Decision

The zero-degree root is not the entire local `mvdm-*` inventory and it is not
only `mvdm-host`.

1. Every original function definition mirrored in `mvdm-host` is zero-degree.
2. The root expands transitively through a call only while its resolved
   physical definition is selected original OpenNT `mvdm` source already
   mirrored below a project `mvdm-*` component.
3. A support, tool, firmware, or guest definition that exists locally but is
   not reachable through that MVDM-local call closure remains outside zero.
4. A match is by selected physical definition identity: original path,
   edition/provenance, signature, and content identity where applicable. A
   shared spelling alone never joins definitions or calls.
5. This is a source-graph classification only. It cannot make
   `mvdm-tools`, `mvdm-softpc-firmware`, or `mvdm-guest` a host-runtime link
   provider.

## Consequence

T298/T299 retain their raw observations as evidence, but their prior
`mvdm-host`-only root and derived first/second-degree counts are not planning
inputs. The queue now requires a project-MVDM zero-degree rebaseline before a
first-degree implementation package is admitted.

## Procedure For The Successor

The successor must inventory physical definitions under each eligible original
project MVDM mirror, resolve the reachable `mvdm-host` call closure against
that inventory, emit an explicit reclassification ledger, and regenerate first
and second-degree candidates. It must record every ambiguous or missing
identity without name-based fallback. It does not import, link, route, compile,
or enable a provider.
