# S13 per-file checkpoint r1249

Ten mirror files were audited and changed: `vdm.c`, `clmsg.c`, `globals.h`,
`ntsend.h`, `handtabl.c`, `inctlpan.c`, `input.c`, `queue.c`, `server.c`,
and `rtl/wow.c`. Original condition groups and line anchors were reconciled
with the pinned OpenNT files; retained ANSI/Unicode, CCPU and local owner
selectors remain. Original omitted functions remain outside their adopted
subsets; empty frames do not claim those functions are implemented. A further
`kernel/class.c` conditional tree required no source edit. The
[per-file inventory](s13-r489-per-file-observed-inventory.json) records each
review. No guest source or binary changed.

Cached x86, x86-extra and x64 product builds [passed](s13-r1249-full-build-results.json).
The [ten-image comparison](s13-r1249-build-image-compare.json) shows five
identical images and five differing only by parsed PE/debug timestamps; no
substantive image bytes differ. The [r1214 Full](s13-r1214-full-evidence.json)
was therefore reused. Updated [MSVC guard checks](s13-r1249-guard-evidence.json)
passed, including selected/rejected profiles for newly restored nested guards.
The complete ten-image candidate was published and hash-verified at
`assets/release` and `O:/winnt/system32`.

The [census](s13-r1249-counts.json) records 1,056 present C/H files: 181
reviewed identical, 854 reviewed with differences and 21 pending. Rename-aware
net 580 deleted paths against `c97d4ee2c` are separate. Remaining work includes
shared guest-referenced headers, hardware-specific source subsets, and the
`nt_fulsc.c` repeated-condition mapping previously found ambiguous. Complete
supported-context enumeration remains open.
