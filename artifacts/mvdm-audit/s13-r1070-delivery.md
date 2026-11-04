# S13 complete-file audit checkpoint r1070

The file-by-file audit completed the full retained conditional trees of
`wow.h`, `reset.c`, `tape_io.c`, `c_main.c`, `c_main.h`, `c_xcptn.c`,
`ccpusas4.h`, CCPU `cpu4gen.h`, `printer.c`, `printer_.c`, C-VID `cpu4gen.h`,
`cpu_c.h`, `evidgen.h`, `j_c_lang.h`, `ega_dump.c`, and `trace.c`. The
per-file ledger links their evidence. Two shared host/guest headers were
retained and marked unresolved rather than removing guest-used definitions.

Ten C/H files changed. `tape_io.c` restored one original CPU30/CPU40
conditional group, with the unselected CPU30 body replaced by #error. The
existing product paths remained. Previously unsupported host branches were
excluded in `reset.c`, `c_main.c`, `printer.c`, and `printer_.c`, with recorded
original positions and evidence. C-VID `cpu4gen.h` retains both CCPU and
non-CCPU branches: 209 nested PROD arms are excluded only for the non-CCPU
contexts, supported by existing branch observations and 33 actual dependency
contexts in `s13-r1065-cvidcpu4gen-contexts.json`. Existing error and diagnostic
line anchors were corrected in the other changed files. Excluded original
nested groups were not restored; selected local diagnostic code remains.

Cached x86/x64 builds succeeded. Ten-image comparison against the prior release
shows six byte-identical images and four PE timestamp-only differences; see
`s13-r1070-build-image-compare.json`. Under the owner no-repeat rule the prior
Full evidence `s13-r682c-full-evidence.json` is reused; no fresh runtime tests
were run. The complete ten-image package was installed and hash-verified in
both `assets/release` and `O:/winnt/system32`.

The census records 1,058 existing C/H files: 169 reviewed identical, 715
reviewed different, and 174 incomplete. Historical net deletion is 578 C/H
paths versus rename-aware baseline c97d4ee2c and is separate from 1,058.
These completed classifications remain subject to the final evidence audit;
not every supported inclusion context is necessarily captured by the older
ledger. No guest source or image was modified or rebuilt.
