# S12 — per-source unselected MONITOR alternatives

DIV-507 marks 39 original simple positive MONITOR branches in 14 MVDM host C
files whose observed product command contexts do not define MONITOR. The
conditional shells and selected alternatives remain. MONITOR is still
selected for five other product source files (`nt_lpt.c`, `bios.c`,
`keybd_io.c`, `video.c`, and `printer.c`), which are outside this batch.
Software CCPU/V86, active Video7 and input routes are not removed. Current
compiler commands and available Ninja dependencies were checked; 31 stale
historical dependency paths are not proof about arbitrary external profiles.
The [ledger](s12-monitor-subsets.jsonl) records original branch hashes and
per-source graph, plus required line restorations.

Independent reconstruction checked 14 files and 39 replacements against
`c39a3d5b4` with no unexpected bytes. Initial NTVDM comparison revealed
line-number constants in the selected `nt_graph.c` path; registered `#line`
restorations preserve them. Cached NTVDM rebuild passed; the ten released
images matched exactly or only in PE link timestamps. The focused DOS/Video7
source contract passed; that check alone does not prove interactive rendering.
No guest was changed, and no runtime suite or republish was repeated for
unchanged code. Other MONITOR contexts and compound conditions remain in scope.
