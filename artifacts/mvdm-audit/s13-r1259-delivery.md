# S13 per-file checkpoint r1259

Ten mirror files were reviewed and changed: `config.c`, `nt_fulsc.c`,
`host_cpu.h`, `video.c`, `egawrtm0.c`, `egwrtm12.c`, `wowfax.h`,
`wowinfo.h`, `ntpsapi.h` and `userrtl.h`. Original condition structures and
line anchors were reconciled with the pinned OpenNT source while selected
product and guest-referenced declarations remain. `xt.c`, `wowcomm.h` and
`wowmmcb.h` were reviewed without edits. Individual evidence is recorded in
`artifacts/mvdm-audit/s13-r1250` through `s13-r1259`. No guest source or
image was changed or rebuilt.

Cached x86, x86-extra and x64 builds [passed](s13-r1259-full-build-results.json).
The [ten-image comparison](s13-r1259-build-image-compare.json) shows one
identical image, eight timestamp-only images and NTVDM with four additional
diagnostic line-number bytes. The mapped instructions retain their original
file reference; the line numbers changed with original `#line` reconciliation.
The [Full suite](s13-r1259-full-evidence.json) passed, including 17/17 Console
and 17/17 Window cases. The updated x86 and x64
[guard checks](s13-r1259-guard-evidence.json) passed; stale guard inventory
physical line offsets were corrected before rerunning. All ten candidate
images were published and hash-verified at `assets/release` and
`O:/winnt/system32`.

The [census](s13-r1259-counts.json) records 1,056 present C/H files: 182
reviewed identical, 866 reviewed with differences and eight pending. Rename-aware
net 580 deleted paths against `c97d4ee2c` are separate. Complete enumeration of
all supported inclusion contexts remains open; guest-shared header conclusions
use static source and existing product build evidence, not a guest rebuild.
