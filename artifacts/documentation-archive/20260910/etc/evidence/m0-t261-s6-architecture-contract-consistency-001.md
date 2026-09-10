# M0 T261 S6 architecture contract-consistency governance

## Question

Can the new dependency-free `session` owner be made a consistent target
architecture without claiming that its implementation or formal Ninja module
already exists?

## Inputs

- Current design and rule authorities under `docs/design/` and `docs/rules/`.
- T261 S5 closure `ab49a2d4` and the current eight-module formal Ninja
  manifest.
- Component boundary registers for `bx-core`, `adapter-bop`, and
  `adapter-softpc`.

## Procedure

1. Reconcile named source-component count, component ownership, dependency
   direction, and mapping-manager ownership.
2. Correct stale adapter terminology, duplicated test-root labels, and the
   obsolete `bx-vdm` guest-pointer policy spelling.
3. Distinguish target source components from present formal link modules.
4. Classify selector-aware observation and fixture-only code still below
   adapter production roots as explicit later migration work, rather than as a
   permitted exception to adapter blindness.
5. Move implementation admission from S6 to S7 and reserve S8 for the
   residual ownership cleanup.

## Observations

- The target has ten source components: four source mirrors and six authored
  composition/adaptation/runtime components.
- The existing formal graph has eight host-side linkable modules;
  `opennt-guest` is an image input and `session` has no implementation yet.
- `session` owns mapping-instance identity and lifecycle; `adapter-softpc`
  owns the one same-shaped mapping-facade implementation.
- Existing adapter-resident BOP/NTDOS/COMMAND observations and fixture hooks
  do not meet the target production-boundary rule. They are not moved in S6.

## Interpretation

The architecture is internally consistent after this governance change. S7 is
the sole admitted implementation packet for `session`; S8 must restore the
remaining production-root boundary before later owner-package recovery. This
record makes no source-build, runtime, or session-lifecycle claim.

## Verification

- `tools/governance/Export-DocumentationInventory.ps1`
- `tools/governance/Verify-DocumentationGovernance.ps1`
- `git diff --check`
