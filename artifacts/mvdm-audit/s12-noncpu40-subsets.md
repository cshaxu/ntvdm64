# S12 — unselected non-CPU40 host branches

DIV-513 marks 70 original simple non-CPU40 alternatives in 24 selected MVDM
host C files. All observed ten-product compiler commands for these files
define CPU_40_STYLE; current available dependency headers contain no local
define/undef override. The original conditionals and active alternatives
remain. CPU40/CCPU, software V86, Video7 and the actual 486 execution paths
are not modified. One candidate file is under `xms.486`; its selected CPU40
XMS code remains unchanged and this is not a 486 CPU repair. Earlier
project-modified alternatives and compound expressions are held. The
[ledger](s12-noncpu40-subsets.jsonl) records original branch hashes and graphs.

Independent reconstruction checked all 24 files and 70 replacements against
`1e5374097`, with no unrelated byte changes. Cached NTVDM and WOW32 builds
passed; after applying released identity all ten outputs matched published
images exactly or only in PE link timestamps. The Video7/DOS source contract
and software text-cursor fixture passed (mocked memory/geometry, not physical
mouse acceptance). No guest changed and no full runtime suite or republish
was required for unchanged code. Header contexts, compound selectors and
other profiles remain under audit.
