# M0 T420 S13 — DEM and residual-diff recovery

S13 completed the selected `dos/dem` package and then performed the required
whole-S1--S12 residual sweep. It removed default-off observations, historical
debug hooks, fixture-only observers and compiler-only source substitutions;
no new mirror file, guest-media change or independent adapter policy was
introduced.

The ABI sweep was deliberately wider than `nt_inthk`. The following original
OpenNT forms are restored: `intapi.h`/`nt_inthk.{h,c}` `PVOID` callback ABI,
the K&R callback table in `nt_cprgs.c`, C-VID `ev_glue.c` byte/word callback
arguments, the unconditional original video handler table in `gfx_updt.c`,
the original implicit `popf.c` declaration, and the original `ntthread.c`
format conversion. This is a source-shape recovery, not a change to CPU or
guest semantics.

The adjacent sweep retained only demonstrated semantic boundaries: CCPU40
generated-vector selection (`DIV-151`), the missing generated C-VID binder,
the worker guest-memory/lease boundary, and `stdio` declarations used by real
formatting calls. Generated C-VID function-pointer expressions and other
unchanged upstream old-style callback declarations are not local deviations.

The formal x86 root is `build/M0-T420/S13/formal-x86-001`. Because the local
Ninja runner did not reliably reap its compiler children, the exact generated
commands were run in the VS x86 environment. A first partial relink exposed
an important build invariant: both `run16` and `ntvdm` compile their own
`base_rpc_client.c`; after `APP_VERSION` was advanced to `0.0.420`, the stale
objects still reported `0.0.419` and correctly received
`ERROR_REVISION_MISMATCH` (1306). Rebuilding both generated object commands
and relinking restored the four-program protocol agreement.

Verification passed:

- `Verify-ProductVersions.mjs` positive and negative protocol/application
  checks;
- `Verify-CcpuEventConsumption.mjs` (HALT/RESET/timer/SAD/INTR and IRQ
  callback consumers);
- `Verify-VdmTibStorage.mjs` (the 4,208-byte VDM_TIB has one owner);
- documentation governance and `git diff --check`;
- deployed Console product regression: fresh 80x25, 45x34, 60x50 and 120x30
  COMMAND/MEM/EDIT runs all returned the established `exit=1`. The prior full
  matrix from this same source build passed mouse, resize and five short-window
  EDIT→MEM runs; a repeat mouse invocation was discarded after a failed run
  left observer processes sharing the broker session.

Raw observations remain under `O:\winnt\logs`. T420 package recovery is
complete; later CPU, C-VID, WOW or lifecycle behavior work requires a new
admitted packet rather than reopening this cleanup sweep.
