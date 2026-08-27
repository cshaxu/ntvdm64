# Proposal: OpenNT platform-ABI canonical union

## Decision sought

Before historical-interface adapter convergence, convert
`opennt-platform-abi` from edition-named imported roots into one canonical
OpenNT ABI declaration tree. OpenNT and OpenNT-4.5 are provenance inputs to
the same project family, not parallel product source roots.

## Target layout

`src/opennt-platform-abi/source/` contains only logical upstream topology,
such as `public/...` and `private/...`. It must not contain `opennt/`,
`opennt-4.5-private/`, an edition name, or a source-repository name.

Every current include/build manifest selects only this canonical tree. The
original absolute source path, selected hash, any rejected alternative and
selection reason remain in the platform-ABI import manifest; they are not
encoded in a product source path.

## Required audit and migration

1. Inventory every current platform-ABI file and all original candidates from
   both source trees.
2. Classify each logical target as identical dual-source, one-sided, or
   conflicting content.
3. For every conflict, choose one source exactly once from caller ABI,
   surrounding package/build lineage and content evidence; record selected and
   rejected hashes and rationale.
4. Re-root selected headers into the single canonical public/private tree,
   preserving their original relative topology beneath that logical root.
5. Update include directories, Ninja manifests and ABI import records; prove
   no live source/build/test input names an edition directory.
6. Compile the selected original declaration-carrier probes on MSVC x86 and
   x64. No BOP, provider, machine or host behavior is enabled by this task.

## Constraints

- This is a declaration-mirror recovery task, not an opportunity to rewrite
  headers or merge incompatible definitions.
- The selected header remains byte-exact unless the mirror rules authorize and
  register a minimal divergence; a conflict is resolved by selection, not by
  synthesizing a hybrid declaration.
- Historical version names remain legal only in provenance/evidence records,
  external input paths and explanatory prose.
- The task changes no active runtime route or component ownership other than
  the platform-ABI component's own canonical source layout.

## Exit criteria

There is one product ABI tree, every imported header has a unique selected
source disposition, every same-path conflict has an explicit choice, all live
selection manifests are edition-root-free, and x86/x64 declaration-carrier
builds pass. The component README and source-policy record the final rule.
