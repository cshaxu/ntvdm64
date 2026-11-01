# S12 — unselected MONITOR else branches

DIV-508 marks nine original MONITOR-only alternatives behind selected
`#ifndef MONITOR` branches in `nt_umb.c` and `stubs.c`; their actual product
commands do not define MONITOR. The active non-MONITOR UMB path and the five
other product sources that do use MONITOR remain unchanged. Exact original
branch locations and hashes are in the [ledger](s12-monitor-else-subsets.jsonl).

Independent reconstruction verified two source files and nine branch
replacements against `4c23a0ac0` with no unrelated byte changes. Cached NTVDM
build passed; all ten released images matched exactly or only in PE link
timestamps. The actual cached original UMB object contract passed 4,240
checks and six callbacks under controlled host boundaries; this is not a
complete guest lifecycle validation. The guest was not changed, and no repeat
runtime suite or republish was required for unchanged code. Other headers,
combined selectors and source roles remain under audit.
