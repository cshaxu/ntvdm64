# M0 T318 S2 P18 — CCPU/C-VID vector assembly closure

## Question

Can the selected CPU40 product compose the retained original CCPU state
carrier with the complete generated C-VID vector, without a dummy display
provider or a replacement guest route?

## Source finding

`softpc.new/base/cvidc/vglfunc.c` already defines the complete original
`C_Video` table.  The selected `ccpu386/localfm.c` carrier owns `Cpu` and
`Video`, but its generic include search selected the CCPU-local five-member
generated `evidgen.h`.  It consequently created a truncated `Video` object;
the first original VGA setup call through `Video.SetVideov7_bank_vid_copy_off`
was null.

The original CCPU `CpuVector` is still the correct CPU state carrier, while
the C-VID generated `VideoVector` is the correct video table layout.  The
omitted connection is a historical product-generated assembly action, not a
missing C-VID algorithm.

## Change

The private `mvdm-host-overlay` now:

- gives the selected `localfm` carrier the original CCPU `CpuVector` and the
  complete C-VID `VideoVector` declarations;
- copies original `C_Video` into the selected original-name `Video` state;
- publishes that state through `Cpu.Video` exactly once from original
  `setup_vga_globals`.

The mirror invocation and both overlay files are registered as
`MVDM-HOST-DIV-156`.  No video callback, controller behavior, guest memory
mapping, BOP implementation, or app-owned loader was introduced.

## Verification

Both formal product rows rebuilt and linked normally using the parallel Ninja
runner:

```text
build/M0-T318/S2/cpu40-v7vga-r4/run-ninja-parallel.cmd original-softpc-process.exe
build/M0-T318/S2/cpu40-v7vga-r4-x64/run-ninja-parallel.cmd original-softpc-process.exe
```

The rebuild touched only the affected CCPU carrier, C-VID glue/binder and
their archives before relinking.

A console-owning, debugger-attached x86 observation of the staged package no
longer exits with the prior `0xC0000005` null transfer.  Its bounded report
instead reaches the original `16 bit MS-DOS Subsystem` error dialog:

```text
This function is not supported on this system.
Terminate / Retry / Ignore
```

## Disposition

The CCPU/C-VID table assembly prerequisite is closed for compile/link and its
former null-transfer symptom is removed in this debugger-attached x86
observation.  The newly
reached original host-capability error is not attributed here and does not
prove NTIO, NTDOS, child `EXEC`, or parent return.
