# M0 T391 S6 — embedded SoftPC media

## Scope

The owner required the selected SoftPC ROM, CMOS and profile defaults to be
embedded in both `ntvdm32.exe` and `ntvdm64.exe`, rather than deployed as
standalone files. This record covers only that media disposition; it makes no
claim that every SoftPC startup path is runtime-complete.

## Source and boundary

- The eight byte-exact upstream `softpc.new/roms` paths now reside below
  `src/mvdm-host/softpc.new/roms`, preserving the original SoftPC-relative
  package layout.
- `host_read_resource(ROMS_REZ_ID, ...)` and
  `host_read_resource(CMOS_REZ_ID, ...)` remain the original source-shaped
  resource entrypoints. Under `MVDM_EMBED_SOFTPC_MEDIA`, `nt_rez.c` calls the
  same-shaped `mvdm_embedded_resource_read` binding.
- The generated, build-local RC carrier embeds `bios1.rom`, `bios2.rom`,
  `bios4.rom`, `v7vga.rom`, `profile.spc` and `cmos.ram` as named `RCDATA`.
  `bios1.asm` and `bios4.asm` remain source evidence, not runtime resources.
- ROM reads always resolve to the embedded immutable resource. CMOS/profile
  defaults first resolve from the same resource, while a successful original
  `host_write_resource(CMOS_REZ_ID, ...)` persists the changed value under
  `HKCU\\Software\\NTVDM64\\SoftPC\\Media`; neither the EXE nor a peer media
  file is written.

## Divergence

`MVDM-HOST-DIV-222` is registered in `src/mvdm-host/README.md`. It preserves
the original function names, parameters, timer bracket and failure direction.
The sole changed binding replaces package-file lookup/write with PE-resource
lookup and per-user persistence because the selected product is a fixed
container and cannot safely rewrite itself.

## Build evidence

The formal Ninja graph was generated with
`tools/build/New-T310OriginalSoftpcNinja.ps1`, then invoked through its
architecture-specific `run-ninja-parallel.cmd` runner:

- `build/M0-T391/S6/embedded-media/x86/original-softpc-process.exe` linked;
  `obj/host/softpc-media.res` is 96,816 bytes.
- `build/M0-T391/S6/embedded-media/x64/original-softpc-process.exe` linked;
  `obj/host/softpc-media.res` is 96,816 bytes.

Both PE inputs contain the six named resource identifiers. The runtime staging
manifest no longer creates or populates a `softpc/` peer-media directory.

## Limitation

This is resource/link closure only. It does not prove a complete SoftPC boot,
CMOS mutation during a guest run, or runtime display behavior.
