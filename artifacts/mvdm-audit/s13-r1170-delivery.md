# S13 per-file checkpoint r1170

The ten changed files are vga_prts.c, timelib.h, xstypes.h, pif.h, winconp.h,
winddi.h, wingdip.h, dlcapi.h, ntlpcapi.h and vdmdbg.h. The per-file records
link original structure, selection evidence, and local bindings. Excluded
branches include obsolete non-NTVDM VGA timing and masks, inactive host
profiles and C++ alternatives. Product-selected branches remain. The
independent `src/addon/pif/BUILD.CMD` is also a product consumer of pif.h;
its actual C configuration was checked rather than relying solely on Ninja
dependency records. Guest source and media were not changed.

Cached x86, x86-extra and x64 builds succeeded. Independent PIF compilation
also succeeded; its [image comparison](s13-r1170-pif-image-compare.json)
shows only PE timestamp differences against the published PIF.EXE. The
[ten-image comparison](s13-r1170-build-image-compare.json) shows two exact
matches and eight timestamp-only differences relative to the preceding
release, with no substantive differences. Prior Full evidence
`s13-r682c-full-evidence.json` is reused under the owner's no-repeat rule;
no new runtime suite is claimed. All ten images were published and hashes
verified at `assets/release` and `O:/winnt/system32`.

The [census](s13-r1170-counts.json) lists 1,058 present C/H files: 181
reviewed identical to their original, 801 reviewed with differences, and 76
still incomplete. The rename-aware net 578 deleted C/H paths relative to
c97d4ee2c are separate. Existing classifications require final
supported-context verification; actual consumers outside the Ninja inventory,
including addon build scripts, must also be included in that audit.
