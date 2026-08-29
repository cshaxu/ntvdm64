# M0 T310 S8 P4 — Original host native-format ABI closure

## Scope

This closure is limited to the active whole-tree ABI scope:
`src/mvdm-host/**` and `src/mvdm-support/**`.

## Findings and recovery

- `config.c` builds a private host console title from two original 32-bit
  process identifiers and the native `ConsoleHandle`.  The handle is not a
  guest value, so it must retain its native width and is now passed to the
  original title formatting as `%p`.
- The retained CPU_40 `sas_manage_xms` diagnostic in `stubs.c` reports its
  native start pointer plus an `ULONG` byte count and `INT` action.  Its
  pointer now uses `%p`; scalar arguments retain scalar conversions.

Neither operation publishes a handle or pointer in a guest ABI.  No mapping
manager, adapter, XMS behavior, console behavior, or stub result changed.

## Verification

Fresh clean formal Ninja rebuilds of `original-softpc-host-roots.lib` completed
with exit code zero on both targets:

- `build/M0-T310/S8/p1-machine-source/x64/p4-host-native-format-closure.log`
- `build/M0-T310/S8/p1-machine-source/x86/p4-host-native-format-closure.log`

Neither output contains the former `config.c:342` or `stubs.c:346` C4313/C4477
pointer-format records. The remaining `sprintf` C4996 and old-style C4131
warnings are intentionally retained because they are not x86/x64 ABI
diagnostics and this task forbids warning suppression or unrelated cleanup.
