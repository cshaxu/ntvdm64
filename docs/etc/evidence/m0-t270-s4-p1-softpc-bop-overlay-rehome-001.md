# M0 T270 S4 P1 — SoftPC BOP overlay rehome

## Scope

The initial BOP mirror scan identified three adopted SoftPC BIOS fragments
whose retained source body differs from the selected complete original by
more than the mirror threshold: `mem_size.c`, `tape_io.c`, and `emm_fncs.c`.
Each has one admitted source-shaped entry and no independent product-shell
consumer.

## Repair

The retained source fragments moved with `git mv` to private matching paths:

- `opennt-bop-overlay/softpc/mem_size.c`
- `opennt-bop-overlay/softpc/tape_io.c`
- `opennt-bop-overlay/softpc/emm_fncs.c`

The same-named `opennt-bop/softpc/` files now contain only a registered
same-signature entry boundary.  Each macro-renames and includes its matching
private source fragment, then exports the original entry name.  No external
source includes the overlay, and it is not an independent manifest module.

## Verification

The external incremental formal graph passed after the rehome:

```text
ninja -C build/M0-T270-S1/r002 -j 8
[1/82] CC ... softpc/mem_size.c
[2/82] CC ... softpc/emm_fncs.c
[3/82] CC ... softpc/tape_io.c
...
[82/82] LINK ... headless-8042-lifecycle-fixture.exe
exit 0
```

Focused fixture results:

```text
t240-s2-softpc-memory-size-fixture.exe
T240 S2 source-shaped SoftPC memory_size reads mutable BDA RAM

t242-s3-softpc-tape-io-fixture.exe
T242 S3 source-shaped SoftPC INT15 AH=88h non-PM route passes

t245-s3-softpc-emm-unavailable-fixture.exe
exit 0
```

This is only a layout/ownership repair.  It neither admits additional EMS,
PM, CMOS, DMA, timer nor BOP behavior.
