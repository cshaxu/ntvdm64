# M0 T420 S6 — SoftPC BIOS package recovery

## Selected original manifest

The original `softpc.new/base/bios/sources` manifest selects fourteen units:
`bios`, `cmos_bis`, `cntlbop`, `emm_fncs`, `reset`, `rom_basc`, `rtc_bios`,
`slave_bs`, `tape_io`, `equipmnt`, `ill_bop`, `build_id`, `virtual`, and
`mem_size`.

## Source-first disposition

Ten selected units are byte-identical to pinned OpenNT.  This S removes the
13-line EMS request/result/init observation from `emm_fncs.c`; it supplied no
EMS allocation, mapping, BOP, or guest behavior.

The remaining source-first finite boundaries are:

- `reset.c`: CPU40 enters the DOSX-created direct continuations at their live
  EIP rather than applying the historical BOP `HOST_BOP_IP_FUDGE`.  The two
  targeted entries were already source- and byte-proven in the DOSX
  continuation recovery record; CPU30 retains the original adjustment.
- `tape_io.c`: `xms.h` provides the original A20 interface declaration in the
  recomposed source graph; provider and ordering are unchanged.
- `virtual.c`: the historical header carrier is absent from the recomposed
  graph, so the exact existing `ClearInstanceDataMarking(void)` contract is
  declared at its original call boundary.

## Verification

Fresh x86 graph: `build/M0-T420/S6/formal-x86-001`.

- The selected `original-softpc-bios.lib` builds successfully after the EMS
  observer removal.
- The four formal x86 product targets link, and `Verify-VdmTibStorage.mjs`
  passes: `mvdm_vdm_tib.obj` is the single 4,208-byte VdmTib owner with no
  overlapping symbol.
- The deployed `O:\winnt` product has SHA-256 identities
  `run16=1995BD88F39588F43E8BF7630B48B7B5484B8EDEDB8C62627582EAC61528C2BE`,
  `basesrv=8FA64B1F17DE90FDBB5572A26F75BA493C5AE5DE250000A24E5A60D86ECCCC50`,
  `ntvdm=4DA991AC7B928F94371FBD3F501FCBA9CCD62CDD164DCA1393AC5475BD2B365A`, and
  `dtmgr=656A4473E7CB78CEEB24D1D1385E4CE1D9DAFB32A6DCD0731E8DB7BD22FDBBD8`.
- `Verify-CommandExitStatus.ps1` passes direct MEM, repeated MEM, nested
  COMMAND→MEM, `COMMAND /C`, and EDIT return.  This covers the required
  direct and interactive COMMAND routes.
- `VerifyCvidIntegrated.ps1` passes 80x25, 45x34, 60x50, 120x30, resize and
  mouse Console matrices.  Its five independent short-history EDIT→MEM runs
  all retain the expected post-EDIT MEM output and `80x300`/five-row view
  geometry.  The mouse record observes move, left-button press and release
  at guest coordinate `152,72`.

## Next owner

S7 owns the selected `softpc.new/base/support` package.  It may use BIOS
contracts but may not replace BIOS firmware policy or restore EMS observers.
