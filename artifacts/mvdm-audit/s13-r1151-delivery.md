# S13 per-file checkpoint r1151

The ten changed original headers are ntddser.h, ntddvdeo.h, align.h,
smbmacro.h, tstr.h, vdm.h, netdebug.h, netlib.h, netlibnt.h and rap.h.
The per-file inventory links the original comparison and observed selection
evidence. Selected alternatives remain; unselected platform, C++ and
configuration branches were replaced with error guards and original diagnostic
line anchors. Other reviewed headers required no source change. Shared guest
headers and unverified include-guard cases remain pending. No guest source or
guest media was changed.

Cached x86, x86-extra and x64 builds succeeded. The
[ten-image comparison](s13-r1151-build-image-compare.json) found one exact
match and nine PE timestamp-only differences against the preceding release;
there were no substantive differences. Prior Full evidence
`s13-r682c-full-evidence.json` is reused under the owner's no-repeat rule;
no new runtime suite is claimed. All ten images were published and hashes
verified at `assets/release` and `O:/winnt/system32`.

The [census](s13-r1151-counts.json) lists 1,058 present C/H files: 178
reviewed identical to their original, 786 reviewed with differences, and 94
still incomplete. The rename-aware net 578 deleted C/H paths relative to
c97d4ee2c are separate. Existing classifications require final
supported-context verification; an unobserved branch alone is not proof of
exclusion.
