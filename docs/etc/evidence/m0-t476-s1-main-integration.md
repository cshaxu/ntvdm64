# M0 T476 S1 — integration to main

The owner accepted and closed T474 and directed integration of the mirror
conditional worktree. The integration retains sibling commits `08d353ac7` and
`0403e0d9b` in Git history, with main's T474 S6 test reorganization and its
closure retained. The two documentation conflicts were reconciled without
discarding the accepted queue decision or the T474 closure.

The independent [S1 audit record](m0-t476-s1-mirror-conditional.md) describes
the original-source inventory and the 30 selected outermost exclusions in 15
host files. Its marker and preprocessing observations are not a claim of
complete per-file or diagnostic-line proof. The specified guest and non-C/H
mirror exclusions remain in force; no guest source or image was changed here.

## Main integration verification

- Cached x86 product, NTVDM and WOW32 builds and x64 native product builds
  passed from the merged source. `tests/audit/verify-t476-s1-exclusions.py`
  passed for 30 recorded exclusions. Documentation governance passed.
- The coherent candidate `9cbf91c3-9d92-4d7c-b007-7bde1d500219` passed the
  full selected product integration gate. This includes native and DOS command
  execution, real Console handoff and repeated transitions, GUI and RPC
  behavior, lifecycle failures and WOW application startup observations. The
  DOS4GW/NESticle check asserts startup with a graphics window, not gameplay
  or pixels; WOW window observations are not gameplay acceptance.
- The identical ten host images and hook license were published and SHA-256
  checked against the candidate at `assets/release` and `O:/winnt/system32`.
  The release manifest SHA-256 is
  `DB39347C2DF8CD28F751334E93DA5F5AFF788770B7CC10A29F0878D1692F547D`.
  Product application version is 0.0.476; RPC protocol remains 65.

The full per-file original-structure and line-number review limitations stated
in the S1 audit record remain open. This delivery integrates the work performed;
it does not present those inherited limitations as resolved.
