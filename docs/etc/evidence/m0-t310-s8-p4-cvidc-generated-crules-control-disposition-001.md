# M0 T310 S8 P4 generated C-video `crules` control disposition

## Scope

This review covers all 42 selected C4033 records in the generated original
C-video files `SINIT012.c` and `SINIT013.c`.

## Original contract

Both translation units expose many `IUH(IUH, IUH, IUH, IUH)` generated rule
entrypoints. Each delegates to its file-local `crules(ID, IUH, IUH, IUH,
IUH)` interpreter. The C4033 records occur only in generated interpreter
branches that execute `return;` after applying a side-effect rule, even though
the common function has an `IUH` return type. This is a source-generated
control-result condition, not a function-pointer mismatch, host pointer
conversion, call-convention change, or x64-only result representation.

`IUH` is already the reviewed native C-video word carrier on both hosts. The
same records occur under x86 and x64. Adding invented return values would
change generated-rule behavior and would not correct a cross-architecture
ABI boundary.

## Disposition

All 42 records are retained visibly as original generated control-flow
diagnostics and are classified
`not-x86-x64-generated-cvideo-control-result-contract`. Their semantic
validity belongs to later C-video execution coverage; this P4 ABI sweep does
not hide them or add per-rule wrappers.
