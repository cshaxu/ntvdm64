# S13 per-file checkpoint r1114

The ten changed mirror files are sas.h, suplib.h, trace.h, vgaports.h,
video.h, xt.h, timer.c, ega_prts.c, error.h and host_inc.h. The per-file
inventory links original comparisons and selection evidence. The original
CPU30 conditional framework in xt.h was restored with its unselected body
excluded; selected CCPU and ANSI alternatives were retained. Exclusions and
original line anchors in the other files follow their worker contexts. The
unchanged original umb.h was reviewed too. Guest source and media were not
changed.

Cached x86, x86-extra and x64 builds succeeded. The
[ten-image comparison](s13-r1114-build-image-compare.json) shows six exact
matches and four PE timestamp-only differences against the previous release,
with no substantive differences. Prior Full evidence
`s13-r682c-full-evidence.json` is reused under the owner's no-repeat rule;
no new runtime tests are claimed. All ten images were published and hashes
verified at `assets/release` and `O:/winnt/system32`.

The [census](s13-r1114-counts.json) lists 1,058 present C/H files: 171
reviewed identical to their original, 756 reviewed with differences and 131
still incomplete. The rename-aware net 578 deleted C/H paths relative to
c97d4ee2c are separate. Existing classifications require final context
verification. Several shared guest headers and product adaptations remain
pending; a lack of observed hits alone is not proof of exclusion.
