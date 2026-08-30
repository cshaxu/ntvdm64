# M0 T310 S16 P1 — BIOS/firmware owner audit

## Selected scope

The firmware *input* mirror is byte-exact under
`mvdm-softpc-firmware/softpc.new/{bios,roms,data}`. It contains immutable
BIOS/ROM/CMOS inputs and is neither a guest image nor a second machine.
Executable original firmware/control sources remain in `mvdm-host` at their
original-relative paths:

- `softpc.new/base/bios`: the 14 members of its original `sources` manifest,
  including `bios.c`, `reset.c`, `cmos_bis.c`, `rtc_bios.c`, `rom_basc.c` and
  BOP table/control members;
- `softpc.new/base/system/rom.c` and `cmos.c`: ROM/CMOS byte loading and
  physical-SAS publication;
- `softpc.new/host/src/nt_rez.c` and `nt_unix.c`: original resource read and
  host file-location route.

## Exact source route

`rom.c` and `cmos.c` call original `host_read_resource`. The selected original
`nt_rez.c` body obtains the caller-owned path through original `host_find_file`
and reads it using the public CRT file functions. The selected original
`nt_unix.c` still owns `host_find_file`; its one registered divergence calls
`mvdm_softpc_firmware_find_file` rather than assuming the NT system directory.

The binding is intentionally split by owner:

- `app/package_layout.c` determines the executable-relative product layout and
  stores the selected firmware root in the active session;
- `adapter-mvdm-host-out/softpc/mvdm_softpc_firmware.c` performs only bounded,
  caller-buffer path lookup below that session root;
- the original `nt_unix.c`, `nt_rez.c`, `rom.c`, `cmos.c` and BIOS bodies
  retain file-open/read order, checksum/ROM publication and BIOS control flow.

This uses no mapping-manager token for a path string: it crosses no guest or
fixed-width identity boundary. Actual ROM bytes become guest physical memory
only through the original SAS calls, which S16 must re-read with their selected
CCPU40 binding.

## Initial dispositions

- Immutable ROM/BIOS/CMOS input files: `direct` mirror input.
- Original `host_find_file` body: `adapter-backed` at its existing minimal
  location-binding divergence.
- Original `host_read_resource` and `read_rom` path: `binding-only` pending
  direct x86 CCPU40 formal assembly and SAS/ROM caller audit.
- Original `host_write_resource`: not a firmware-load success path. Its
  writable historical profile/CMOS behavior must receive an explicit policy
  disposition; S16 must not silently enable writes to immutable package media.
- Original BIOS reset/BOP table: `binding-only` pending the later controller
  packets it calls. No controller is enabled simply because its table compiles.

## Next P

Generate a fresh selected x86 CCPU40 original-machine graph and verify that
the complete original BIOS manifest, system ROM/CMOS route and current minimal
location binding compose without introducing a replacement BIOS provider.
