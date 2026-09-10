# M0 T310 S8 P4 — SAS direct-call and native-pointer diagnostic closure

## Scope

This evidence covers only the active whole-tree ABI audit scope:
`src/mvdm-host/**` and `src/mvdm-support/**`.  It does not suppress or modify
any diagnostic in another component.

## Source findings

Under the selected `CPU_40_STYLE` profile, the original `sas.h` delegates most
SAS operations to generated `sas4gen.h` function-vector declarations.
`sas_overwrite_memory` is different: it is a direct CCPU physical-range
invalidation function, defined with `PHY_ADDR` address and length by
`ccpu386/ntstubs.c`, but omitted from that generated declaration set.  Its
original callers in `sim32.c`, `nt_emm.c`, and `nt_mem.c` therefore compiled as
implicit `int` calls.

The original `nt_mem.c` allocation-failure diagnostic also formatted the
private native `IHP commitAddr` value with a 32-bit `%lx` conversion.  It is a
host allocation pointer, not a guest address and not a mapping-manager ID.

## Recovery

- `MVDM-HOST-DIV-092` adds only the exact direct CCPU declaration
  `void sas_overwrite_memory(PHY_ADDR, PHY_ADDR)` next to the CPU_40 generated
  declarations.  No provider, vector slot, page mapping, or behavior changed.
- `MVDM-HOST-DIV-093` retains the original failure diagnostic and changes only
  the `commitAddr` conversion to the CRT-native `%p` form.

`VdmMapDosMemory` and `VdmUnmapDosMemory` are intentionally not claimed by
this closure: no implementation exists in the selected original user-mode
sources, and their EMS physical-page remap semantics require a separately
audited machine binding rather than an implicit declaration or a dummy shim.

## Formal verification

Fresh clean Ninja rebuilds of `original-softpc-host-roots.lib` completed with
exit code zero on both targets:

- `build/M0-T310/S8/p1-machine-source/x64/p4-sas-native-pointer-closure.log`
- `build/M0-T310/S8/p1-machine-source/x86/p4-sas-native-pointer-closure.log`

Neither log contains `sas_overwrite_memory`, `nt_mem.c(1133)`, or the former
`C4477`/`C4313` record for `commitAddr`. Other visible warnings, including the
separate `config.c` and `stubs.c` native-pointer formatting records, remain
visible for their own whole-tree ABI clusters.
