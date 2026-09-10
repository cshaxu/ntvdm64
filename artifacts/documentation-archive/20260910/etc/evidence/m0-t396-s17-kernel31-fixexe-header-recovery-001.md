# M0 T396 S17 — Kernel31 `fixexe` header recovery 001

## Question

Why did the selected original `KRNL386.EXE` reach `KERNSTUB`'s failure branch
after its successful open, and can the stage reconstruct only a source-proven
original build artifact without changing the retained guest media?

## Inputs

- Immutable selected image:
  `src/mvdm-guest/wow16/kernel31/retail3/krnl386.exe`.
- Original post-link owner: `src/mvdm-guest/wow16/kernel31/fixexe.c`.
- Original linker `STUB` input named by `kernel.def`:
  `src/mvdm-guest/wow16/kernel31/retail3/kernstub.exe`.
- Original stub source: `src/mvdm-guest/wow16/kernel31/kernstub.asm`.
- Read-only corroboration only:
  `O:\repos.external\ntvdmx64\ntvdmpatch\doc\patches.txt` and its WOW16
  patch, which identify the same host-width `fixexe` defect.  No comparator
  code was imported.

## Source and binary audit

`fixexe.c` seeks to byte offset 2 and, under its original 16-bit C ABI,
writes three two-byte values: `e_cblp`, `e_cp`, then (after seeking to 12)
`e_maxalloc`.  The retained output instead bears the exact footprint of a
32-bit host compilation: the three writes start at offsets 2, 6, and 12 and
are each four bytes wide.  Consequently it overwrites these linked fields:

| MZ field | Output after the bad host-width write | Original recovery source |
| --- | ---: | --- |
| `e_cblp` / `e_cp` | `0140` / `0000` | original `fixexe` size calculation |
| `e_crlc` | `00A8` | `KERNSTUB.EXE` MZ header (`0000`) |
| `e_cparhdr` | `0000` | `KERNSTUB.EXE` MZ header (`0020`) |
| `e_maxalloc` | `0000` | original `fixexe` write (`0000`) |
| `e_ss` | `0000` | `KERNSTUB.EXE` MZ header (`000E`) |

The original `kernstub.asm` positions `stacktop` at `0160` and aligns the
expected NE header to `0200`.  The original `KERNSTUB.EXE` has a 32-paragraph
MZ header, so its code begins at file offset `0200`; the combined KRNL image
puts its NE header at file offset `0400`.  Restoring `e_cparhdr=0020` makes
the DOS image begin with the stub code and makes the NE header visible at the
stub's expected image offset `0200`.  The pre-recovery zero header instead
loads the MZ header at image offset zero and leaves the stub's own code at
`0200`, which is exactly the earlier `KERNSTUB: Error during boot` condition.

## Recovered derived artifact

`Apply-Wow16Fixexe.mjs` now requires both immutable inputs.  It writes only
the original `fixexe` words plus the three adjacent values demonstrably
destroyed by the widened writes; `Stage-OriginalSoftpcRuntime.mjs` records
both source hashes.  It does not write either source file.

The derived `KRNL386.EXE` header is:

```text
e_cblp=0140  e_cp=00A8  e_crlc=0000  e_cparhdr=0020
e_minalloc=0000  e_maxalloc=0000  e_ss=000E  e_sp=0080
e_ip=0000  e_cs=0000  e_lfanew=00000400
```

The selected source SHA-256 remains
`bced7650a5b5d7ac25955ab2a21bc53ff06de1c35b25fad0690390135b093a03`; the
derived image SHA-256 is
`d404576899cc11d23f368f2b30554e6108657cd37809837b341fd37e6b1a7af4`.

## Runtime observation and limitation

The recovery removes the observed `KERNSTUB: Error during boot` header cause,
but does not establish a WOW bootstrap advance.  A direct declared
`system32\KRNL386.EXE` run of the v4 current-binding image reaches the existing
real-mode `ARPL`/INT6 route at `05D8:1F8F` and times out after 15 seconds with
28 modules and no WOW32 load.  This is a distinct direct-kernel launch shape,
not the product's `WOWEXEC.EXE` target path.

S18 re-ran the actual `system32\WOWEXEC.EXE` product command from the same
derived-media stage.  The v4 image exits `0xc0000409` after 7156 ms with 15
loaded modules and no WOW32 load, matching the frozen S16 control's terminal
class.  Thus the direct-KRNL observation cannot be used as evidence that the
header recovery or the v4 composition advances the WOWEXEC path.  The
observer's manifest validator was extended solely to verify the declared
dual-input derived-media contract before launching.

## Disposition

High confidence: the earlier header stage recovery was incomplete because it
could not restore fields already overwritten in the retained output. The new
derivation is source-shaped, fixed to the original `KERNSTUB.EXE` link input,
and preserves source-media identity.  It proves no WOW32 reachability by
itself.  S18 owns the exact current-binding control; the next packet owns the
native fast-fail provenance on the actual WOWEXEC product route.
