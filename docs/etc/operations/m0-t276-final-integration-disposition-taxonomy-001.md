# M0 T276 final expected-integration disposition taxonomy

## Purpose

T276 records the final intended product treatment of every selected MVDM path,
not merely whether a source body is enabled today. Build-selection, source
identity and package-boundary records remain evidence inputs; they are not the
final disposition.

## Exactly-one disposition

Each row in the final tracker has exactly one of these values:

- `direct`: the byte-identical original enters the final host graph unchanged.
- `binding-only`: original algorithm/body remains unchanged; only include,
  import or same-shaped declaration binding is needed.
- `adapter-backed`: original algorithm/body enters the final host graph and
  each unavailable old interface is supplied by its named same-shaped adapter.
- `overlay-required`: the mirrored original remains the owner, but a registered
  private overlay is needed for material modern logic that cannot live in the
  mirror.
- `not-host-runtime`: historical host-side material is retained as source
  evidence but does not belong to the product host runtime.
- `guest-only`: selected input belongs solely to a separately loadable guest
  product, never a host link graph.
- `firmware-only`: immutable machine/ROM input selected only by an
  `adapter-bochs` manifest, never compiled as a host provider.
- `tool-only`: independently buildable historical utility/tool input, never a
  host runtime input.
- `unresolved`: source evidence is insufficient to choose one of the preceding
  final states. This is an explicit blocking conclusion, not a synonym for
  “not currently enabled.”

## Required row evidence

Every non-`unresolved` row names the original build/source evidence, final
owner/link boundary, change class (none, binding-only or overlay), named
adapter where applicable, and x86/x64 mapping implication. A historical
runtime body may legitimately end as `not-host-runtime` when its required
private product shell is not part of the modern program; that conclusion must
say why.

The final tracker is updated package by package. A package audit closes only
when every one of its selected paths has this final conclusion; the separate
audit-state field may say that implementation is not yet enabled.
