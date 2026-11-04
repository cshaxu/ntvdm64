# S13 ordered first-level audit checkpoint r769

The pending per-file inventory was processed in its existing order. This
checkpoint changes exactly ten C/H files; `dpmi32.c`, `dpmimemr.c`, and
`dpmi386.c` were also reviewed without source changes. The changed files are
`cmd.h`, `dem.h`, `dpmi32p.h`, `dpmidata.h`, `dpmiint.c`, `dpmiint.h`,
`dpmiselr.c`, `modesw.c`, `mvdm.h`, and `softpc.h`. The existing original
outer conditions were retained; this batch did not restore any missing
original outer condition. Excluded first-level arms and necessary original
line anchors were reconciled. Selected local adaptations were retained.

Per-file evidence is recorded in the updated observed inventory and census;
`r747` through `r768` witnesses cover actual observed preprocessor contexts.
The 159 and 203 context checks also include test objects and are not claims
that all configurations or runtime semantics have been proved.

Cached x86/x64 build completed successfully; ten-image comparison is in
`s13-r769-build-image-compare.json`. Six binaries match byte for byte and
four differ only in independently parsed PE/debug-directory timestamps.
There are zero other differing bytes. The prior `s13-r682c-full-evidence.json`
Full result is reused under the owner-approved no-repeat rule; no new runtime
test was run. The complete ten-image package was installed to both
`assets/release` and `O:/winnt/system32` and all destination SHA-256 values
were checked against the package manifest.

The `s13-r769-counts.json` census has 1,058 existing C/H files: 168 reviewed
identical to their matched original, 698 reviewed with differences, and 192
pending. It includes one separately identified generator input outside the
observed graph inventory; it is not a claim that every existing file is a
compiled mirror file. Separately, 578 C/H paths are net deleted relative to
`c97d4ee2c` under rename-aware Git comparison; they are not part of the
1,058 existing count. These numbers do not prove complete supported-context
enumeration. Guest source and images were not rebuilt or modified.
