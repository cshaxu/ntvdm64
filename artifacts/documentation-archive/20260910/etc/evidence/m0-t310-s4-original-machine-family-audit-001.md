# M0 T310 S4: original SoftPC machine-family workset audit

## Method

This audit starts from the original `softpc.new/host/src/sources` manifest and
the existing exact SoftPC firmware mirror. It follows the original startup
roots rather than compiler-error order:

1. `InitialiseDosEmulation` calls `reset` before the package initialization and
   guest-image load.
2. `nt_reset.c` owns host reset/startup/teardown ordering.
3. The selected host manifest then supplies timer, input, display, disk,
   serial and parallel bridge translation units.
4. Each base device family is paired with its original host bridge or
   immutable firmware input before an adapter is considered.

The resulting workset is
[m0-t310-s4-machine-family-workset-001.tsv](../operations/m0-t310-s4-machine-family-workset-001.tsv).

## Decisions

- The workset has nine complete machine families. None is omitted because it
  happened not to be in the S2 ten-root compiler cohort.
- Firmware is immutable input, not a library secretly linked into a host
  service. Its selected input is supplied only by app composition.
- UMB/EMM/VDD remains a named XMS/VDD prerequisite, not a reason to crop or
  replace `nt_umb.c`/`nt_msscs.c`.
- Memory/SAS, timers, host I/O and endpoint bridges are source-shaped adapter
  candidates. The original state/order stays in `mvdm-host`; public Win32,
  session and mapping mechanics go only through their named adapters.
- No original source body is authorized to include a Bochs type/object/global,
  and no device family is allowed to fall back to the Bochs backend.

## Provenance checks

The following roots were read directly in the current mirror:

- `softpc.new/host/src/{sources,nt_reset.c,nt_msscs.c,nt_cpu.c,nt_cprgs.c}`;
- `softpc.new/base/system/{rom.c,cmosnt.c,ica.c,timer.c,at_dma.c}`;
- `softpc.new/base/{keymouse,video,disks}` and their source manifests;
- `mvdm-softpc-firmware/softpc.new/{bios,roms,data}`.

They are retained from the selected original MVDM union. The earlier T276
interface ledgers are supporting evidence only; this workset supersedes their
old “Bochs sole executor” planning conclusion for the selected SoftPC branch.

## Next action

S4 proceeds in the listed order. It must first bind firmware/memory/SAS and
the original reset/startup sequence into a bounded original-SoftPC target;
each remaining family is added only through its recorded source-shaped
adapter boundary and x86/x64 build evidence.
