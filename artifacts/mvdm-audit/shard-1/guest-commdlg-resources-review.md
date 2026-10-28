# S10 shard 1: Win16 COMMDLG resources

Read-only scope: pending `wow16/commdlg/COMMDLG.RES`, `cddrv.bmp`, `smfldr1.bmp`, `smnet.bmp`, `smram.bmp`, `xlib.h`. No guest rebuild or runtime test.

The current copies and both reference trees (`O:/repos.external/OpenNT/base/mvdm` and `O:/repos.external/OpenNT-4.5/nt/private/mvdm`) share the following SHA-256 prefixes, independently obtained with Python hashlib: RES `9910c9a21575e68f`, cddrv `724f05d141b4f7f8`, smfldr1 `64081942cf7356a1`, smnet `6cb1f6feddd7c8de`, smram `01bc50e05c9bb218`, xlib `5c328c4449b040ad`. Re-run Get-FileHash for full hashes before treating identity as independently verified.

The original makefile lines 112-155 builds COMMDLG.RES from RC/dialog inputs and attaches it to COMMDLG.DLL after linking; host WOW32 `wcommdlg.c` contains its own 16/32 conversion and callbacks. The source DLL, ZIP `WINNT/SYSTEM32/COMMDLG.DLL`, and installed `O:/winnt/system32/COMMDLG.DLL` all hash `54a7981c5667bde19f0d703580d40a448a0bdec010feffa153004ef4870dc0d4`. Equality proves identity of these files, not source-to-binary provenance or restored dialog behavior.

`commdlg.rc:16` selects filebmps.bmp as its directory/drive bitmap; the four specifically audited bitmaps are not named in a case-insensitive textual search over the current wow16 tree. That is not proof no generated/binary reference exists. `xlib.h` is explicitly listed as a makefile prerequisite for dlgs.obj and font.obj, but those C files do not directly include it; font.c includes `privcomd.h`, which includes the different `_xlib.h`. Do not infer live compilation of xlib.h merely from the makefile dependency.

Reproduce via `rg -n -i 'cddrv\\.bmp|smfldr1\\.bmp|smnet\\.bmp|smram\\.bmp|\\bxlib\\.h' src/mvdm/wow16`, inspect recipe and includes, and compare hashes and ZIP entry. No Ninja graph inspection, decoding resource binary, dialog functionality, resource lifetime or callback/teardown testing was performed; none is implied by this classification.
