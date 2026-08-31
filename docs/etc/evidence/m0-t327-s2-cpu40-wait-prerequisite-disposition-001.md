# M0 T327 S2 — CPU40 wait prerequisite disposition

## Scope

This evidence resolves only the proposed earliest CPU40/INT15 wait candidate.
It does not claim that a DOS program has completed, and it does not add a BOP
provider, BIOS result, controller result, or guest-image change.

## Static source result

- The selected non-`MONITOR` CPU40 build leaves `wait_int_seg:wait_int_off`
  at the original `RCPU_WAIT_INT_SEGMENT:RCPU_WAIT_INT_OFFSET` default:
  `FE00:0CE0`.
- `softpc.new/base/bios/rom.c::rom_init` loads the selected original
  `bios4.rom` through the original `read_rom` route at `BIOS2_START`.
  `softpc.new/roms/bios4.asm` places `wait_int` at `ORG 0CE0`; its source
  endpoint is the original `BOP BIOS_CPU_QUIT` return.
- `softpc.new/base/ios/keybd_io.c::kb_setup_vectors` only replaces that
  default through the NTIO KIO table in the original `MONITOR` branch.  The
  formal CPU40 profile does not select that branch, so no table installation
  is missing from this path.
- The CPU40 immediate `BOP FE` route enters the original
  `c_cpu_unsimulate`/`ccpu386Unsimulate` frame return.  No project-owned
  substitute frame or BOP result is involved.

Therefore the previously observed zero-address exception cannot be assigned
to a missing ROM residency, KIO wait-vector, or invented FDC/PIC prerequisite.
Those proposed repairs are rejected.

## Fixed-container repetition

The existing fixed observer launches the product with `-f -o
--ordinary-child`.  `app/launch_declaration.c` publishes the declared
`COMMAND.COM /C VER` record before original entry.  The selected BaseClient
and local BaseSrv preserve the original copied `VDMINFO` request/result and
pending/retry split.

With the newly linked x86 product and unchanged staged media, the bounded
observation recorded:

```
50:11 -> 50:3B -> 50:0F -> 50:1B -> 54:05
result=timeout (8 seconds)
```

It did not reproduce the earlier access violation and emitted no controlled
exception record.  The single reached `54:05` is consistent with the
pre-published first COMMAND request being consumed; the remaining timeout is
inside the declared guest COMMAND execution/return path, not evidence that
the command was never supplied.

Raw fixed-container report, console snapshot, and command metadata are kept
in `artifacts/research/m0-t327-s2-cpu40-wait-diagnostic.txt{,.console.txt,.json}`.

## Disposition

S2 is closed with no production source change: the first proposed machine
prerequisite is present and source-shaped.  S3 may now use the existing
declared-command path to identify the exact original guest execution or
return boundary.  It may not reopen this candidate by adding a trace-selected
device or BOP workaround.
