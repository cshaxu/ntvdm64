# M0 T279 S3 — dual-architecture support-library build evidence

## Inputs

The generated Ninja graphs select exactly the original historical inputs:

- `oemuni/file.c` and `oemuni/process.c` for `oemuni.lib`;
- `suballoc/suballoc.c` for `suballoc.lib`; and
- the separately linked `adapter-win32` source-shaped binding.

x86 retains both OEM original definitions. x64 compiles those original bodies
with exactly four definition renames and supplies the public names from
`opennt-mvdm-support-overlay`.

## Formal runs — 2026-08-26

```text
VsDevCmd -arch=x64 -host_arch=x64
ninja -C build/M0-T279/x64 all
ninja -C build/M0-T279/x64 test
PASS: x64 OEM pointer-width overlay contracts

VsDevCmd -arch=x86 -host_arch=x64
ninja -C build/M0-T279/x86 all
```

Both generated graphs then reported `ninja: no work to do` on an unchanged
`all` target. The x64 `dumpbin /symbols oemuni.lib` inspection reported one
public external definition each for `FindFirstFileOem`, `FindNextFileOem`,
`GetFullPathNameOem`, and `SearchPathOem`; the original renamed symbols remain
non-public implementation evidence.

## Warning disposition

`/W4` remains enabled without `/WX` or warning suppression. The original
mirror bodies report their historical uninitialised-local, 16-bit length and
x64 pointer-truncation diagnostics. The latter instructions are present only
in renamed x64 evidence definitions; the four public exports are overlay
implementations. Adapter and overlay sources compile warning-free.

`suballoc.c` deliberately remains an original static-library body. Its
caller-provided commit/decommit/move callbacks are not invoked by this packet,
and no guest-memory mapping behavior is claimed.
