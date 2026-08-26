# M0 T276 S4 textual caller/callee correlation evidence

## Scope and method

All 612 selected translation units are accounted for. C/C++ files receive a comment-stripped lexical identifier-before-parenthesis scan. Assembly units are recorded but not projected into a false C-call model. Candidate callee names are matched only against the S3 lexical declaration names; this is neither a compiler symbol table nor a proof of an external, reachable call.

## Counts

- Selected translation units: 612
- Lexical call/definition candidates: 89924

### Translation-unit scan states

- `c-like-lexical-scan`: 554
- `not-c-like-lexical-scan`: 58

### Name-correlation outcomes

- `ambiguous-declaration-name-candidate`: 7043
- `no-selected-extern-candidate`: 79056
- `unique-declaration-name-candidate`: 3825

## Limits

- Calls, function definitions, macro expansions and function-pointer forms can share this lexical shape; candidates require S5 source-level review.
- A unique name match does not establish ABI, linkage, direct availability, adapter ownership or a package dependency.
- Assembly exports/imports, macro-generated calls and build-condition reachability remain explicit follow-up evidence.
