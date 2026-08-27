# M0 T299 S5 — parser control-construct correction plan

## Scope

Correct the original-source scanner so C/C++ and MSVC control constructs are
not emitted as function definitions or outgoing function calls. Regenerate the
T299 degree ledgers and retain the S4 physical-file closure invariant.

## Exit

- `__except`, `__finally`, `try`, `catch` and equivalent control constructs
  are excluded from definition and call candidates.
- All direct missing-definition rows remain per-call.
- No second-degree row resolves in the caller file or any selected
  first-degree file.

No source import, product build or runtime change is in scope.
