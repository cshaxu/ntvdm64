# M0 T271 S5 P2 — `opennt-bop-overlay` consumer and boundary audit

## Question

Does every exceptional `opennt-bop-overlay` body remain private to its
matching `opennt-bop` mirror translation unit, without a production consumer
outside that mirror component?

## Inputs

- `src/opennt-bop-overlay/` production body inventory.
- `src/opennt-bop/` include boundaries and its divergence register.
- Production roots `src/app`, `src/session`, `src/adapter-bop`,
  `src/adapter-softpc`, `src/adapter-win32`, and `src/opennt-host`.
- T271/S5/P1 source classification.

## Procedure

1. Enumerated each overlay body and searched every production root for the
   literal `opennt-bop-overlay` include path.
2. Compared every resulting include with the corresponding BOP divergence
   register entry.
3. Separately searched the non-mirror production roots for an overlay include.

## Observations

The 13 private bodies have exactly one matching mirror boundary each:

- COMMAND: `cmdexec32.c` through `command/cmdexec.c`; command, binary,
  configuration, environment, and PIF compositions through their identically
  named COMMAND mirror units.
- DEM: `demdisp.c` through `dem/demdisp.c`.
- XMS: `opennt_xms_composition.c` through its identically named XMS mirror
  unit.
- SoftPC: `emm_fncs.c`, `mem_size.c`, and `tape_io.c` through the three
  identically named SoftPC mirror units.
- VDMREDIR: `vrdisp.c` and `vrmslot.c` through their identically named mirror
  units.

No production include in `app`, `session`, `adapter-bop`, `adapter-softpc`,
`adapter-win32`, or `opennt-host` names `opennt-bop-overlay`.  The remaining
references outside these 13 boundaries are README/register documentation.

## Interpretation and confidence

The present overlay component has no public include surface and no external
consumer.  It therefore meets T271's private-body direction: a mirror unit is
the sole compilation/linkage boundary for each registered exceptional body.
This is structural evidence only; it does not assert that every exception is
minimal or that S5 is closed.  The 22 project-authored ingress inputs recorded
by S5/P1 still require their owner-specific migration, replacement, or
explicit deferred disposition.

Confidence is high for the checked source tree because the procedure searched
all live production component roots and the formal source input roots.

## Follow-up

Use this boundary map while resolving S5/P1's non-mirror ingress inventory.
Re-run the consumer scan after every move or new overlay boundary; an external
overlay consumer is a T271 stop condition.
