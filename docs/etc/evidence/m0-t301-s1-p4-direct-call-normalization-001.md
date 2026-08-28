# M0 T301 S1 P4 — Direct-Call Normalization

The earlier lexical boundary treated historical declaration macros and visible
function-like macros as ordinary external functions. This correction keeps
every textual original function definition, including the OpenNT/Insignia
`name IFNn(...)` form, while excluding a call token only when the caller's
uniquely resolved original include closure proves that token is a macro.

The generator also resolves a basename-only historical include only when it
is unique inside the caller's original MVDM top-level package. It therefore
does not select a WOW header for a SoftPC caller merely because both have the
same filename.

Current generated evidence:

- `mvdm-zero-degree-call-closure-ledger.tsv`: 12,436 physical zero-degree
  function identities.
- `mvdm-zero-degree-variant-family-ledger.tsv`: 4,366 original variant-family
  edges, retained without name-only selection.
- `mvdm-first-degree-rebaselined-boundary-ledger.tsv`: 13,050 direct calls
  leaving the selected original MVDM closure.
- `mvdm-first-degree-function-candidate-ledger.tsv`: 1,442 spelling groups;
  groups are an index only and do not merge physical definitions.

Examples removed from the one-degree function frontier are `assert0`,
`assert1`, `note_entrance0`, `dprintf4`, `dprintf5`, `trace_wave`, and
`trace_midi`. `CrulesRuntimeError` is now correctly recognized through its
original `IFN1` definition and belongs to the MVDM-local zero closure.

No provider was selected, no source was imported, no first-degree body was
read for disposition, and no second-degree body was inspected. The next
T301 step is definition-identity resolution for each retained boundary call.
