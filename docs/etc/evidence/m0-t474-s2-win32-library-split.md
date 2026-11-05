# M0 T474 S2 — Win32 library ownership split

The former `lib-win32/io` source is split by responsibility: `pipe/` owns
connected pipe pairs and bounded transfers, `console/` owns Console membership
sampling, and `file/` owns native path resolution and file identity. Their
static libraries are `lib-pipe.lib`, `lib-console.lib` and `lib-file.lib`.
Interfaces and runtime policy remain unchanged. The existing common link graph
still requests these libraries together for several shared consumers; archive
members are resolved by the linker. This split does not claim minimal link
inputs per executable.

Cached x86 NTVDM, WOW32, VDMREDIR, nthook32 and x64 native targets built. The
frontend link ownership check passed; focused pipe-pair, Console membership
and file-identity tests passed 13, 66 and 14 checks respectively. Documentation
governance passed. The first Full attempt exposed incorrect quoting of split
archives in two fixture scripts; those links were fixed before the Full rerun.

The Full selected product suite passed on the staged candidate, including
DOS/native nested handoff, frontend and worker loss, version-negative controls,
management, GUI startup and DOS4GW startup. Winmine, SOL and WRITE windows were
observed; this is not gameplay or editing acceptance. The prior link failure was
fixed and Full rerun completed successfully. All ten package images and hook
license were installed and SHA-256 verified against the candidate manifest at
`assets/release` and `O:/winnt/system32`. The release manifest records hashes.
No guest source or guest image was modified. Awaiting owner acceptance.


## P1 — Console membership ownership consolidation

The owner approved moving the Console process-membership snapshot into
`lib-win32/process-observation` and removing `lib-console.lib`. The original
snapshot API and algorithm remain unchanged. Cached x86 and x64 product builds,
frontend link-ownership verification, 66 Console snapshot checks and 19 process
observation lifecycle checks passed. The Full selected product gates passed on
the staged candidate, including DOS/native nesting, frontend loss, Console
return, version negative controls and DOS4GW launch. WOW application checks
observed windows only; they are not gameplay acceptance. The ten candidate
images and hook license were published and verified against the manifest in
both `assets/release` and `O:/winnt/system32`. No guest was changed.
