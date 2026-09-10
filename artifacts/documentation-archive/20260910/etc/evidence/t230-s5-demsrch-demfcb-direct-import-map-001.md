# T230 S5 — Direct `demsrch.c` / `demfcb.c` Import Map

## Source and owner

- Original: `src/opennt/base/mvdm/dos/dem/demsrch.c`, `demfcb.c`, and the
  supporting `demmsg.h` declaration file.
- Baseline: `d450bfc1b7711234072eebfdfa23735ea509e7c5`.
- Mirrors: `src/bx-vdm/bop/opennt/dem/demsrch.c`, `demfcb.c`, `demmsg.h`.
- Services: `50:07`, `50:09`--`50:0C`, `50:20`, `50:2C`--`50:31`, `50:3C`.

The two owner translation units were copied as whole OpenNT files.  Legacy
`bop-v1` and former partial-v2 files remain comparison material only.  The S5
shim and fixture neither call them nor use them as acceptance input.

## Minimal composition seam

`demsrch_fcb_shim` supplies only unavailable host-composition mechanics:

- the existing copied-register, typed-result, checked guest-memory and session
  token boundary from `demhndl_shim`;
- OEM `FindFirstFile` / `FindNextFile` wrappers over ordinary Win32 APIs;
- modern SDK declarations for the original NT directory-query calls;
- an explicit unavailable `NtVdmControl` response, which retains the original
  slow-reset fallback instead of fabricating a VDM host service;
- default-first-call VDD/PDB lifecycle no-owner seams, while the imported
  `demTerminatePDB` still owns search-state release ordering.

No shim implements wildcard matching, directory enumeration, FCB search state,
FCB metadata, PDB search lists, file operation ordering, or DOS error logic.
Those remain in the imported OpenNT sources.

## Required source-local ABI divergences

Every change is adjacent to the original code:

1. `SRCHDTA` and `DIRENT` retain their original four-byte reserved search
   reference as `FFindRef`; it is a stable `FFindId`-derived validation value,
   never an x64 host pointer.  The original code already uses `FFindId` for
   lookup, so no guest-visible pointer is required.
2. `demsrch.c` host-private directory-buffer arithmetic uses `uintptr_t` in
   place of OpenNT's 32-bit `ULONG` casts.  Guest DTA/FCB layouts remain fixed
   43-byte/52-byte structures.
3. A checked 43-byte DTA or 52-byte FCB search-buffer bounce span replaces a
   raw SAS mapping and is copied back after the imported service body.
4. FCB Open/Create returns an opaque Direct session token through the original
   AX:BP register contract; FCB Close releases that token rather than closing
   a truncated host handle.

These are host/guest ABI adaptations only.  They add no Readonly, Overlay or
Virtual policy.

## Focused Direct witness

Fresh formal Ninja graph `build/M0-T230-S5/direct-search-fcb-r2` source-built
`bx-vdm.lib`, linked the focused fixture with the original package's Win32 and
NT user-mode libraries, and returned zero on 2026-08-19:

```text
T230 S5 direct OpenNT demsrch/demfcb import: search lifecycle, tokenized FCB operations and PDB cleanup verified
```

The fixture exercises all owned identities:

| Services | Imported body / focused outcome |
| --- | --- |
| `50:07` | `demDeleteFCB`: deletes a temporary host file. |
| `50:09`, `50:0B` | `demFindFirst`, `demFindNext`: real host wildcard enumeration and 43-byte guest DTA continuation. |
| `50:0A`, `50:0C` | `demFindFirstFCB`, `demFindNextFCB`: real host enumeration and 52-byte guest FCB search-buffer continuation. |
| `50:20` | `demRenameFCB`: renames a temporary host file. |
| `50:2C`, `50:2D`, `50:2E` | `demCreateFCB`, `demOpenFCB`, `demCloseFCB`: direct file lifecycle with x64-safe token publication and release. |
| `50:2F` | `demFCBIO`: reads `OK` through the token and DTA guest span. |
| `50:30` | `demDate16`: returns DOS date/time through typed AX/DX deltas. |
| `50:31` | `demGetFileInfo`: queries the temporary file's metadata. |
| `50:3C` | `demTerminatePDB`: releases the imported PDB-owned search records. |

This is local Direct source parity only.  It does not prove selector routing,
whole DEM closure, Readonly/Overlay behavior, native guest execution, or the
historical invasive VDD host composition.
