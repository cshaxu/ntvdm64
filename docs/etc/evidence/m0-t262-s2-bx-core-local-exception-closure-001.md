# M0 T262 S2 — bx-core local exception closure

## Result

- Deleted `src/bx-core/cpu/bx_ntvdm_exception_intercept.h`: source and test
  scans proved it had zero consumers and it was not an adopted Bochs file.
- The 18 non-identical adopted Bochs files now each contain a local
  `DIVERGENCE(<ID>)` marker. `bx-core/README.md` adds a file-granular index
  covering the segment profile, default-off observations, generic `#UD`,
  typed entry and memory exceptions.
- No executable expression, feature predicate, ABI or include dependency was
  changed. The focused PowerShell boundary test now verifies the current
  `exception.cc` generic-UD seam directly, rather than the removed dead header.

## Verification

- `Test-BochsUdInterceptBoundary.ps1` passed.
- A source scan found a marker in all 18 audited modified files and found no
  remaining obsolete header.
- Formal Ninja rebuilt 130 affected edges and linked
  `t237-s5-bop-nosupport-fixture.exe`; its runtime exit was `0`.
