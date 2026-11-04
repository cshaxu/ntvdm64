# S13 condition batch r1302

Ten mirror files received scoped unselected-branch exclusions and original diagnostic continuation corrections. Selected source paths remain unchanged. [Per-file evidence](s13-r1302-condition-batch.json). The non-NTVDM LIM arm subsumes its formerly separately recorded nested GISP_SVGA guards.

Cached x86, x86-extra and x64 product builds passed. The ten-image [comparison](s13-r1302-build-image-compare.json) shows six identical images and four PE timestamp-only differences. The PIF addon also built successfully; its [comparison](s13-r1302-pif-compare.json) shows timestamp-only differences, so the existing addon release was retained. Under the owner rule, tests were not repeated; [r1287 Full](s13-r1287-full-evidence.json) remains runtime evidence. All ten product images were published and verified in `assets/release` and `O:/winnt/system32`. No guest source or binary was changed.

Census remains 1,056 current files: 182 byte-identical and 874 differing; the latter are recorded reviewed files, with supported-context limits documented in individual evidence.
