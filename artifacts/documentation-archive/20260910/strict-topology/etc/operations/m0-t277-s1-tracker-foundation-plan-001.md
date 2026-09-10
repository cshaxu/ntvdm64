# M0 T277 S1 — tracker foundation and evidence normalization plan

## Purpose

Create the governed, machine-readable starting point for the MVDM package
dependency and interface-closure tracker. T276 classified every selected file
for its final expected product disposition. That evidence does **not** prove
that a declaration is called, that a caller is linkable, or that a historical
interface has a recoverable x86/x64 binding. S1 preserves that distinction.

## Inputs

- `m0-t276-final-integration-disposition-ledger.tsv`: all 1,689 selected
  paths and their final file treatment.
- T276 S2 literal build/input evidence.
- T276 S3 declaration evidence.
- T276 S4 caller/callee correlation evidence.
- T276 S5 boundary/disposition evidence.
- The selected source-union manifest and the pinned source mirrors.

## Governed ledgers

1. **File recovery ledger**: one final product treatment per selected path.
2. **Interface recovery ledger**: source-located interface-family and child
   candidates. A row remains `lexical-candidate`, `source-reached`, or
   `source-ambiguous`; no state is inferred from its declaration alone.
3. **Package dependency ledger**: package-to-package and package-to-interface
   edges, including cycles and a provisional wave.
4. **Build/profile ledger**: original source/build inputs and proposed x86/x64
   target membership, explicitly separate from enabled build edges.
5. **Divergence/exclusion ledger**: every expected mirror binding difference,
   overlay requirement, Bochs-replacement exclusion and unavailable historical
   product boundary.

## S1 delivery sequence

1. Freeze schemas and generate stable IDs from the T276 file identity.
2. Import existing evidence without rewriting its confidence: textual rows stay
   textual; assembly/macro rows remain non-projected until a later source-aware
   pass.
3. Build package identities and initial dependency edges only from observed
   include/build/caller evidence.
4. Validate every ledger against the 1,689-path final ledger and reject false
   reachability or undeclared adapter ownership.
5. Record counts, known ambiguity and the next source-reachability S boundary.

## Non-goals

S1 does not recover an adapter, compose a provider, modify an original mirror,
compile a host package, change Bochs, run Ninja or execute a guest trace.

## Exit evidence

The generated ledgers and verifier must prove reproducible initial population,
unique row identities, complete final-file linkage, and a non-empty explicit
distinction between lexical candidates and source-reached dependencies.
