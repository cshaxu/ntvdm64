# S12 — line-sensitive non-PROD host subsets

The owner-directed DIV-501 marks the 29 original non-PROD branch bodies held
from DIV-500 because their six source files contain line-sensitive diagnostics.
The same selected PROD command contexts and original-source checks apply.
[Ledger](s12-nonprod-line-sensitive.jsonl) records per-branch paths, original
ranges and hashes. Independent reconstruction from commit `526b108d3` verified
all six files and 29 branch replacements, including the four documented
line restorations, with zero unrelated byte changes.

The cached NTVDM build passed. Initial image comparison detected changed
line-number constants in `nt_com.c`, `nt_event.c`, and `nt_timer.c`; four
registered `#line` directives after the affected conditional boundaries
restore the original selected diagnostic lines. After rebuilding, the ten
images matched the published set exactly or only in PE link timestamps.
Runtime regression and republishing were not repeated for unchanged code.
No guest source or binary was changed; other conditional paths remain open.
