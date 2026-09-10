# T230 S6 — Direct `demgset.c` Import Map

## Source and owner

- Original owner: `src/opennt/base/mvdm/dos/dem/demgset.c`.
- Baseline: `d450bfc1b7711234072eebfdfa23735ea509e7c5`.
- Mirror: `src/bx-vdm/bop/opennt/dem/demgset.c`.
- Services: `50:0D`--`10`, `14`, `15`, `19`, `1A`--`1C`, `25`, `41`, `46`.

The complete original translation unit is the implementation.  `bop-v1` and
the former partial v2 remain comparison material only and are not called by
this package shim or fixture.

## Named divergences

1. The original `dem.h`/SoftPC/CCPU/SAS include closure is replaced by
   `bop/shim/demgset_shim.*`, which reproduces only the required packed 16-bit
   DPB/VOLINFO/DOSWOW layouts, typed register access, checked guest copies and
   OEM Win32 entrypoints.
2. DPB `Next` stays a 32-bit guest far-link on both x86 and x64.  The three
   original host-pointer assignments are adjacent, documented 32-bit guest
   assignments in the mirror.
3. `demSetDTALocation` delegates only its unavailable persistent SAS aliases
   to a typed, thread-local registration seam.  It copies the original DTA
   far pointer and current PDB through checked guest memory; the subsequent
   original `demFindFirst` fixture proves the stored registration is consumed.
4. The Direct default for `demSetDate`/`demSetTime` remains `SetLocalTime`.
   An opt-in fixture writer verifies their original register-to-`SYSTEMTIME`
   and AL-result behavior without changing the developer machine clock.  It
   is not a profile backend and is disabled by default.
5. `demGetBDS` returns null until the original raw-DASD owner is imported in
   S7.  This deliberately selects `demgset.c`'s own documented synthetic-DPB
   fallback; no new drive semantics were added to the shim.

## Focused Direct witness

Fresh formal Ninja graph `build/M0-T230-S6/demgset-direct-r1` source-built
`bx-vdm.lib`, linked and ran
`t230-s6-demgset-direct-import-fixture.exe` with exit zero on 2026-08-19:

```text
T230 S6 direct OpenNT demgset import: DTA registration, drive, clock and search continuation verified
```

| Services | Imported body / focused outcome |
| --- | --- |
| `50:0D`--`0F` | Original boot-drive, disk-free-space and drive enumeration bodies return typed Direct results. |
| `50:10` | Original `demGSetMediaID` fills a checked 25-byte guest `VOLINFO` from the host volume. |
| `50:14`, `15` | Original local date/time queries populate their AX/CX/DX byte fields. |
| `50:19`, `1C` | Original set-date/set-time register and AL paths verified with the explicit fixture-only clock writer; the production default remains Win32 `SetLocalTime`. |
| `50:1A` | Original default-drive body accepts the real process current directory through its OEM pathname seam. |
| `50:1B` | Original registration contract copies DTA/PDB state; a subsequent original `50:09` writes the registered DTA. |
| `50:25`, `46` | Original DPB and DPB-list bodies write packed 32-bit guest layouts. |
| `50:41` | Original computer-name body writes its 16-byte guest result. |

This is local Direct source parity only.  It does not prove whole DEM,
selector routing, raw-DASD/IOCTL ownership, or native guest integration.
