# M0 T319 S2 P1 — Original ROM-loader selection

Date: 2026-08-31

Status: source-shaped ROM-residency code composed and linked; later runtime
failure not yet attributed

## Question

Can the selected CPU40 product recover ROM residency by reusing the existing
original SoftPC loader rather than creating a firmware loader, synthetic
vector, or FDC result shim?

## Recovery decision

The direct original route is available.  `rom.c::rom_init()` already owns the
required sequence:

```text
host_find_file -> host_read_resource -> SAS RAM span -> sas_stores -> SAS ROM span
```

The historical `NTVDM && X86GFX` preprocessor branch skipped that source body
because the retired kernel-VDM product shell pre-populated the ROM address
space.  `MVDM_SOFTPC_RECOVER_ROM_RESIDENCY` selects the otherwise unchanged
original body for `rom.c` only.  It uses the existing session-selected
`host_find_file` firmware root and original staged inputs (`bios1.rom`,
`bios4.rom`, and selected `v7vga.rom`).

No mapping-manager identity is involved: `sas_scratch_address()` is an
internal synchronous CCPU buffer, never an MVDM/guest ABI value.  The original
SAS APIs own the guest-address write and protection transition.

## Verification

1. Generated fresh disposable formal CPU40 Ninja graphs:
   `build/M0-T319/S2/rom-residency-{x86,x64}`.
2. Both complete product targets linked successfully (`419` actions each):
   `original-softpc-process.exe` exists in both roots.
3. Object-symbol inspection verified that each built `obj/system/rom.obj`
   defines `rom_init`/`read_video_rom` and imports both original
   `host_read_resource` and `c_sas_stores`.  The selected empty
   `host_rom_init` hook is not the residency implementation.
4. Staged the x86 product with the existing runtime-stage script.  The staged
   firmware SHA-256 values were:

   - `bios1.rom`: `1ce33544249fef59865b91308faf60f354563cf854f80eb196d47c968899a574`
   - `bios4.rom`: `b09fdff1a500971d54233378ce9c36f1b6a58bf577e4a544da03a0d78d7e0d6f0`
   - `v7vga.rom`: `970f105cd9e42ee56f07aae695bac89786d3455ab9d4c1ea9a1d1643b1e8f6f0`
5. One fixed console-owning, non-debug eight-second x86 observation exited
   `0xC0000005`; it retained the original `50:11`, `50:3B`, `50:0F`, `50:1B`
   and `54:05` markers.  Unlike P76, this run did not produce an
   `MVDM-ORIGINAL-EXCEPTION` report, so it cannot attribute the current exit
   to a specific FDC/PIC/CCPU source line.

## Result and follow-up

The kernel-VDM pre-resident-ROM assumption is removed through the smallest
available original source body, on both required host widths.  This is code
and link closure of the ROM residency seam, not proof that the ROM bytes have
yet been observed in a live recursive FDC wait.

The next S must preserve this source selection and restore a valid,
non-perturbing attribution for the new post-ROM runtime exit before changing
FDC, PIC, CCPU, BIOS, BOP, or guest logic.  No source-shaped evidence supports
those repairs yet.
