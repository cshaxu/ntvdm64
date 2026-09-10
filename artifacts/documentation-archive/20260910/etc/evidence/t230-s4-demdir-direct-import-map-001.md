# T230 S4 — Direct `demdir.c` Import Map

## Source and owner

- Original: `src/opennt/base/mvdm/dos/dem/demdir.c`
- Baseline: `d450bfc1b7711234072eebfdfa23735ea509e7c5`
- Mirror: `src/bx-vdm/bop/opennt/dem/demdir.c`
- Services: `50:04 demCreateDir`, `50:06 demDeleteDir`,
  `50:13 demQueryCurrentDir`, `50:18 demSetCurrentDir`.

No partial-v2 `demdir` implementation was present, so there is no replacement
algorithm to retain or repair.

## Minimal composition seam

The imported file keeps its function order, register use, error branches and
OpenNT current-directory environment behavior. `demdir_shim` supplies only
the unavailable historical CCPU/SAS/OEM composition:

- the existing scoped copied-register/result call record;
- checked NUL-terminated OEM path gathering for DS:DX inputs;
- a checked fixed 71-byte copy-in/copy-out for the packed OpenNT `CDS` input
  at DS:SI of `demQueryCurrentDir`;
- OEM-to-ANSI wrappers for the same Win32 directory, current-directory and
  environment APIs called by the original owner.

The fixed CDS copy is necessary because the original file mutates a raw SAS
pointer directly. It transports the historical structure; it does not
reimplement CDS validation. The imported file contains the adjacent comment
identifying the replaced historical include closure.

## Focused Direct witness

Fresh formal Ninja graph `build/M0-T230-S4/direct-demdir-r3` source-built and
linked `bx-vdm.lib` and the focused fixture. The fixture returned zero on
2026-08-19 after exercising all four imported bodies against a temporary host
directory. It restores the process current directory after `demSetCurrentDir`.

This proves local Direct source parity only: no selector routing, Readonly,
Overlay, Virtual, native guest execution or DOS-wide current-directory claim
is made.
