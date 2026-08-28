# M0 T301 S1 P10 — Original MVDM Call / Declaration / Definition Shape Gate

## Scope

P10 completes the final static shape check for the 217 calls that passed P9's
source-root, external-linkage and include-package gate.  The check reads the
original physical caller, the declaration header already recorded in the
caller frontier, and the selected original MVDM definition.  It does not
inspect a second-degree callee body or enable any runtime code.

## Method

For each physical call, the generator records:

- argument count at the original call site;
- argument count of every reachable original declaration header form;
- argument count at the selected original definition; and
- preprocessor nesting context at caller and definition lines.

Historical `VOID` and `void` zero-argument forms are treated equivalently.
Nested argument expressions are counted structurally rather than by a
comma-only split.

## Result

- 160 physical calls have matching original call, declaration and definition
  arity and are admitted to the zero-degree closure rebase;
- 57 calls do not yet have a fully confirmed shape and remain explicit
  first-degree records; and
- every row retains caller and selected-definition physical identity plus both
  conditional contexts.

For example, `ExpandConfigFiles → cmdCreateTempFile` is confirmed as `2 / 2 /
2`; `ExpandConfigFiles → cmdInitConsole` is correctly normalized from the
historical uppercase `VOID` forms to `0 / 0 / 0`.

## Limit

Arity confirmation is source-language evidence, not a build or product-route
claim.  The next rebase consumes only the 160 admitted rows, expands their
original MVDM bodies for direct calls, and retains conditional variants rather
than merging them by spelling.
