# M0 T301 S1 P9 — Original MVDM Candidate Binding Gate

## Scope

This record applies the existing original caller include-frontier evidence to
the 473 physical declaration-shaped first-degree calls.  It is a static source
gate only: no product provider, adapter, compilation, linking or runtime route
changes.

## Gate

A body is selectable only when all of the following hold:

1. its original-source edition root equals the physical caller’s selected
   original root;
2. its source identity is externally linkable rather than translation-unit
   local; and
3. its MVDM package root is included in that caller's recorded original
   recursive include frontier.

The generated `mvdm-first-degree-rebaselined-mvdm-binding-gate-ledger.tsv`
keeps the caller identity, root, allowed package roots and every selected body
identity.  It does not choose a body by spelling alone.

## Result

- 217 physical calls have exactly one original-MVDM body that passes all three
  gates;
- zero calls have more than one passing body; and
- 256 calls have no passing original-MVDM body.

The result corrects the earlier parser blind spot: for example,
`ExpandConfigFiles → cmdCreateTempFile` has one compatible original body in
`dos/command/cmdredir.c`, within the caller's `base/mvdm/dos` include frontier.

## Next gate

The 217 passing rows are ready for the final per-call prototype/signature and
conditional-form confirmation before they can return to zero-degree MVDM
closure.  They remain explicitly first-degree until that confirmation is
emitted.  The 256 non-passing rows continue through the normal T301 external
source-union classification; none is silently discarded.
