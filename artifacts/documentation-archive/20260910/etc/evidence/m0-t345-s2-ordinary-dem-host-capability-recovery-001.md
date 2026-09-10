# M0 T345 S2 — Original ordinary DEM host-capability recovery

## Recovery result

The selected ordinary DEM capability is one original source-shaped package,
not a collection of BOP leaf replacements.  No new production code is needed
for S2: the current source already retains the original provider bodies and
binds their unavoidable host/guest seams through the existing same-shaped
adapters.  This review verifies that those bodies are the unique live route.

## Included original providers

- `demfile.c`: `demOpen`, `demCreate`, `demCreateNew`, `demDelete`,
  `demRename`, and `demCreateCommon` retain original DOS path, sharing,
  `CreateFileOem`, error and result-register flow. A successful host HANDLE is
  published only through the session's existing opaque mapping boundary.
- `demhndl.c`: `demClose`, `demRead`, `demWrite`, `demChgFilePtr`,
  `demFileTimes`, and `demCommit` retain original count/error results. In
  particular, the source broken-pipe and disk-full result branches are not
  converted into a product-local policy.
- `demdir.c`: `demCreateDir`, `demDeleteDir`, `demQueryCurrentDir`, and
  `demSetCurrentDir` retain the original current-directory calls and carry/AX
  result ownership.
- `demgset.c`: `demSetDefaultDrive`, `demGetDriveFreeSpace`, and
  `demGetDrives` retain original directory, volume and bitmask flow. The
  current root binding is a product media-root selection, not a synthetic
  guest C: provider.
- `demsrch.c` and `demfcb.c`: normal and FCB `FindFirst`/`FindNext`,
  close/delete/rename/create/open FCB forms retain original DTA/FCB layout,
  DOS search progression and result mapping. Guest numeric locations use the
  one session mapping manager rather than a host pointer.
- `demdisp.c`, `demdata.c`, `demmsg.c` and `demerror.c` retain the original
  dispatch/data/error ownership that joins these calls into the DEM service
  package.

## Explicit boundary

- The source's named-pipe branch in `demfile.c` continues to transfer to the
  separately owned original Redirector package; it is not included merely
  because it shares a file-open entrypoint.
- `demdasd.c`, `demioctl.c`, raw disk/floppy, media locking/formatting and
  related VDD forms remain the physical-media owner package. S2 does not
  create an ordinary-file fallback for them.
- No virtual-drive, readonly, overlay-mode or duplicate host-handle manager is
  introduced. All private host handles stay in the session mapping manager;
  only the original DOS-width values cross the DEM register/DTA/PDB/SFT
  contracts.

## Source and build proof

- Definition scan identifies the selected file/handle/dir/drive/search
  providers only in the original `src/mvdm-host/dos/dem` package, with the
  expected original Redirector and SoftPC callers outside that package. No
  self-written second provider is selected in `src`.
- `build/M0-T343/S3/record-x86/build.ninja` builds all 16 original DEM
  translation units into `original-mvdm-dem.lib` and includes that library in
  `original-softpc-candidate`, `original-softpc-process.exe` and the forced
  closure target.
- On 2026-09-01, both existing paired formal graphs reported `ninja: no work
  to do` for `original-softpc-candidate`. Their complete product links were
  then run under the matching installed `VsDevCmd` environment because a
  direct Ninja shell has no MSVC/Windows SDK `rc.exe` on `PATH`. Both x86 and
  x64 rebuilt the original `resource.rc` input and linked
  `original-softpc-process.exe`; the accompanying forced-closure targets were
  also selected. This is a tool-environment correction, not a source or ABI
  change.

## Claim boundary

This is a complete source-shaped and paired-formal-build closure for the
ordinary DEM host-capability package. It does not claim raw-media support,
Redirector completion, DOS child/PSP lifecycle, or a continuous native guest
execution result.
