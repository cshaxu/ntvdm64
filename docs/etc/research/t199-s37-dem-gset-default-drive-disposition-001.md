# T199 S37 DEM GSET Default Drive Disposition 001

## Question

How should `50:1A demSetDefaultDrive` behave in the non-invasive CLI profile
when OpenNT's historical implementation changes the NT host thread's current
directory?

## Source and ABI

OpenNT `src/opennt/base/mvdm/dos/dem/demdisp.c` assigns `50:1A` to
`demSetDefaultDrive`.  `src/opennt/base/mvdm/dos/dem/demgset.c` documents
`DL` as the zero-based target drive and `DS:SI` as its current-directory
string.  It first compares the string's first byte with `DL + 'A'`; mismatch
returns `AX=1` and `CF=1`.  On a match it calls `SetCurrentDirectoryOem` and,
if that fails, delegates to `demClientError`; otherwise it clears CF.

## CLI disposition

The CLI admission contract has no writable or ambient current-directory
capability. It does, however, project one stable root (`X:\`) for each
explicitly admitted drive. The shared provider performs checked guest-RAM
reads and never consults or mutates the host process directory:

- a mismatch retains OpenNT's direct `AX=1`, `CF=1` result;
- a matching, admitted root path (`DL+'A'`, `:`, `\`, NUL) succeeds with CF
  clear; no mutable directory state is created because that root is already
  the whole declared namespace;
- another matching path reaches the contained missing-capability terminal
  result `AX=5`, `CF=1`, in place of unavailable
  `SetCurrentDirectoryOem`/`demClientError` composition;
- neither branch retains a guest pointer or invokes a host directory API.

This is a source-derived rehost of the observable guard plus a documented
CLI-profile replacement for the historical NT-host operation.  It is not a
claim that current-drive/session state is implemented.

## Evidence

`artifacts/build/t199-s37-dem-package-r52/` is the MSVC x64 `/MT` DEM family
regression. Its focused cases put `DS:SI` at physical `0x120` with `DL=2`:
`'D'` proves the original mismatch result, while `C:\` proves the admitted
root success. The same run exercises every canonical
`50:00..48` identity through the DEM session.  No native trace was run.

## Follow-up

A future non-root successful path needs a separately admitted, reproducible
guest current-directory/session capability with its own package-level source
and ABI review. It must not project the host process current directory.
