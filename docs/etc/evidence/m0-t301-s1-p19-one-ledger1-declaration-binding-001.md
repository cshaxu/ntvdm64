# M0 T301 S1 P19 — `one-ledger1` Declaration/Linkage Binding Frontier

## Inputs and method

P19 consumes P18's 11,134 physical original definition candidates and the
original include closure of each physical caller.  It resolves only original
headers and records tokens of the same call spelling.  It does not read a
candidate function body, select a package/provider, or enable a route.

## Result

- 1,240 distinct physical caller-function identities had one or more P18
  source candidates.
- All 11,134 candidate rows have a P19 disposition:
  - 310 have one source candidate and a reachable original declaration token;
  - 3,942 have multiple source candidates with reachable declaration tokens;
  - 6,639 have source candidates but no declaration token in the physically
    resolved caller include closure;
  - 243 are translation-unit-local candidates outside the caller translation
    unit and are explicitly non-bindable.

## Interpretation

The 310 rows are the only P19 candidates eligible for a later physical
signature/condition gate before their bodies can be examined for degree two.
They are not automatically admitted: declaration token occurrence does not by
itself prove matching prototype, build condition, or package selection.  All
ambiguous and declaration-absent rows remain explicit, preserving the rule
that identical spellings in different source files are never silently merged.
