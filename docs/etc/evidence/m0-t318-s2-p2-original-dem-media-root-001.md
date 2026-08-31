# M0 T318 S2 P2 — Original DEM DOS-media root binding

## Scope

This record closes one source-shaped prerequisite of the existing original
`NTIO -> SVC_DEMLOADDOS -> NTDOS.SYS` path.  It does not claim that BIOS BOP,
`50:11`, NTDOS, or an ordinary child has executed.

## Finding

The selected original `dos/dem/dem.c::DemInit` initialized
`pszDefaultDOSDirectory` with `GetSystemDirectory`.  The selected original
`demmisc.c::demLoadDos` subsequently appends `\\ntdos.sys` to that same
caller-owned buffer.  In the product, `nt_msscs.c` already resolves `ntio.sys`
under the app/session-selected immutable DOS-media root.  Leaving DEM at the
system directory would therefore split the two original loader stages across
different media roots.

## Minimal binding

`mvdm_softpc_dos_copy_root` now copies the already admitted session DOS-media
root into DEM's original caller-owned directory buffer.  `DemInit` retains its
allocation, terminal failure direction, and all later DEM operations;
`demLoadDos` retains its original filename append, `CreateFileOem`, 16 KiB
read loop, guest-address acquisition, and close ordering.  This is registered
as `MVDM-HOST-DIV-154` / `ADAPTER-SOFTPC-031`.

No app loader, guest-memory alias, BOP provider, or synthetic NTDOS image was
introduced.

## Verification

- The formal selected CPU40 graph rebuilt `dem.c`, the media adapter, and
  affected archives successfully: `ninja -j 6 original-softpc-candidate`
  completed its seven incremental actions in
  `build/M0-T318/S2/cpu40-formal-r2`.
- The existing x86 media-resource fixture was extended only to exercise the
  new directory-shaped binding.  It verifies an exact root copy and a bounded
  too-small-buffer failure alongside the existing `NTIO.SYS` lookup and
  missing-file failure.  `ninja -j 6 all-verify` completed 10 actions in
  `build/M0-T310/S8/p1-firmware-resource/x86`.

## Next boundary

The next S2 action remains an observation through the original process
lifecycle into the existing NTIO loader.  This P only ensures that once NTIO
issues `SVC_DEMLOADDOS`, original DEM will resolve `NTDOS.SYS` under the same
session-owned DOS-media root; it does not replace or individually invoke that
service.
