# S12 — excluded PIG comparator branches

The owner requires retaining conditional structure and marking bodies not
selected by the project. This bounded batch covers 94 simple positive
`#ifdef PIG` or `#if defined(PIG)` bodies in 25 selected host C sources and
observed dependency headers. It preserves each outer condition and active
alternative. Combined conditions are not altered. Each removed body was
matched to local OpenNT and recorded with original hash, line range, and
conditional boundary in the [ledger](s12-pig-subsets.json); one mismatching
branch was held for individual review. The previous audit describes PIG as
an excluded CPU comparison role, not ordinary software V86/CCPU execution.

The current eight x86/x64 product target command closures contain no `PIG`
definition. Repository source and reachable available Ninja dependency
headers contain no definition; 31 stale dependency paths point to prior
renamed source roots and are not proof about current active headers. This is
an observed current profile, not proof of arbitrary external build profiles.
No guest was altered. Full cached x86/x64 build passed; independent reconstruction checked all 25 files and 94 original bodies with zero unrelated changes. Initial NTVDM comparison found two diagnostic line constants changed in `c_reg.c` and `nt_timer.c`; the selected else paths now restore those original `#line` values. Rebuild and ten-image comparison then matched the published package exactly or only in link timestamps. No repeat runtime suite or republish is required for timestamp-only output. Unselected standalone files and mixed
conditions remain in the audit.
