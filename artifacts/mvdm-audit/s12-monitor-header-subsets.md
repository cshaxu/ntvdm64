# S12 — per-consumer unselected MONITOR header branches

DIV-509 marks three original MONITOR-only bodies in `nt_graph.h` and
`sim32.h`. Valid selected Ninja dependencies were joined to actual product
compiler commands: all observed product consumers of these two headers have
MONITOR undefined. Among the complete observed MVDM header union, 47 headers
have mixed MONITOR-on and MONITOR-off consumers; none were changed in this
batch. Original conditions and active alternatives remain. The
[ledger](s12-monitor-header-subsets.jsonl) records per-branch original body
hash and consumer counts. Six selected outputs lacking C dependency rows are
assembler/resource outputs, not omitted C compilation contexts.

Independent reconstruction verified two files and three replacements
against `454bcddd2` with zero unrelated byte changes. Cached x86/x64 ten-image
build passed; after identity alignment, all ten images matched the published
set exactly or only in PE link timestamps. No runtime suite or republish was
repeated for unchanged code. No guest was changed. Mixed headers, X86GFX and
other conditional roles remain under audit.
