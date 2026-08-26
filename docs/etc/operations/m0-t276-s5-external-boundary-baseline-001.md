# M0 T276 S5 selected-definition and external-boundary evidence

## Method

S5 scans selected C/C++ source for a lexical function-definition form, then disposes every S4 unique-declaration-name call candidate. A name observed in any selected source definition is retained as a selected-definition-name observation; all other unique declaration calls are external-boundary candidates. Neither outcome proves linkage, calling convention, selected provider, reachability after preprocessing, or adapter ownership.

## Counts

- Selected C/C++ lexical definition occurrences: 12855
- S4 unique-declaration-name call candidates disposed: 3825
- `external-boundary-candidate`: 2355
- `selected-definition-name-observed`: 1470

## Limits

- Static/conditional/macro forms and assembly symbols require source-level package review.
- A selected definition with the same spelling can be a different linkage scope; it is not a provider decision.
- `external-boundary-candidate` is an audit queue, not an adapter authorization.
