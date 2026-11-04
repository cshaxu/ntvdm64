# S13 per-file checkpoint r1136

The ten changed mirror files are insignia.h, sim32.h, nt_error.c, nt_mem.c,
stubs.c, wow32.h, xms.h, memalloc.c, ntddbeep.h and ntddkbd.h. The per-file
inventory links original comparisons and observed-context evidence. Excluded
branches include unsupported debug, platform and C++ linkage alternatives;
both observed alternatives were kept where a product context selected each.
Additional files were reviewed without source changes. No guest source or
guest media was changed.

Cached x86, x86-extra and x64 builds succeeded. The
[ten-image comparison](s13-r1136-build-image-compare.json) shows two exact
matches and eight PE timestamp-only differences relative to the preceding
release, with no substantive differences. Prior Full evidence
`s13-r682c-full-evidence.json` is reused under the owner's no-repeat rule;
no new runtime tests are claimed. All ten images were published and hashes
verified at `assets/release` and `O:/winnt/system32`.

The [census](s13-r1136-counts.json) lists 1,058 present C/H files: 174
reviewed identical to their original, 775 reviewed with differences, and 109
still incomplete. The rename-aware net 578 deleted C/H paths relative to
c97d4ee2c are separate. Existing classifications require final
supported-context verification. Shared guest headers and product adaptations
remain pending; unobserved branches are not by themselves proof of exclusion.
