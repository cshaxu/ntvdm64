# M0 T420 S7 — SoftPC support package recovery

## Selected original manifest

The original `softpc.new/base/support/sources` manifest selects six units:
`terminat`, `time_day`, `ios`, `main`, `get_env`, and `xt`.

## Source-first disposition

`terminat.c`, `time_day.c`, `ios.c`, and `get_env.c` are byte-identical to the
pinned OpenNT source.  This S removes 13 standalone phase-observation lines
from `main.c`; they neither ordered nor checked a startup action.

Only two finite selected-profile boundaries remain:

- `main.c` preserves the original `gfi_init → config`, `cpu_init`, and DOS
  startup order.  It declares three selected provider contracts missing from
  the recomposed header profile; it binds the original C-VID table between
  `gfi_init` and `config`, where EGA can first consume it; and it installs the
  selected original CCPU40 access vector immediately after `cpu_init`.  These
  are calls to original providers, not replacement startup policy.
- `xt.c` excludes only the old non-CPU40 monitor register/descriptor store and
  parity table.  The selected CCPU40 owns both in `c_main.c`; retaining the
  original duplicate definitions would create a second provider rather than
  restore behavior.

The package difference therefore falls from `main.c +40/-2` plus `xt.c +17/-3`
to `main.c +27/-2` plus `xt.c +17/-3`: **13 mirror lines removed**, no adapter
or autonomous implementation added.

## Verification

Fresh x86 graph: `build/M0-T420/S7/formal-x86-001`.

- `original-softpc-support.lib` and all four product programs link.  The final
  VdmTib audit passes with `mvdm_vdm_tib.obj` as the unique 4,208-byte owner.
- The focused CCPU fixture reaches original HALT→RESET behavior (`AX=beef`,
  producer `0`).  The C-VID contract verifies 154 public and 55 private slots,
  direct CPU40 effective-address selection, early video binding before
  `config`, and `cvidc/ev_glue.c` as the sole `Gdp` owner in the final map.
- Deployed `O:\winnt` SHA-256 identities are
  `run16=48943E8C598BE2D76E24570EEB49E3E5555DD437763D0CB65E01BE963900D397`,
  `basesrv=7BE98ACF8D17AE0A8CC1775ECCED7B3A1BD2191D30B8698DC2A7BBA64A8367E7`,
  `ntvdm=E49516544E4709E0EF53927E56191915589DE76706B44EE1C822B674F7495664`, and
  `dtmgr=AD2BEDA26891017E567C5654EA89B2D21197B8ECC67463C2E83F67CEF7063036`.
- Direct MEM, repeated MEM, nested COMMAND→MEM, `COMMAND /C`, and EDIT return
  pass.  Console integration passes 80x25, 45x34, 60x50, 120x30, resize and
  mouse; five short-window EDIT→MEM passes preserve expected geometry and MEM
  output.  The mouse trace records movement, press and release at guest
  coordinate `152,72`.

## Next owner

S8 owns the selected `softpc.new/base/disks` package.  It may consume the
support contracts above but may not reintroduce phase-only startup observers.
