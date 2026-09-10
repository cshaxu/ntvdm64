# M0 T310 S30 P1 — host-VDD binding closure

## Original owner and selected surface

The selected Win32/x86 CCPU40 host VDD implementation remains the original
`mvdm-host/softpc.new/host/src/nt_vdd.c` body.  It provides the source-defined
VDD dispatch and resource contracts without a project-written replacement:

- `VDDInstallIOHook` / `VDDDeInstallIOHook` retain the original I/O-hook
  installation and removal order.
- `VDDReserveIrqLine` / `VDDReleaseIrqLine` retain the original IRQ ownership
  result forms.
- `VDDRequestDMA`, `VDDQueryDMA`, and `VDDSetDMA` retain the original DMA
  request/query/update sequence.

The `MONITOR`-conditional printer path is not selected.  No kernel VDM,
Bochs route, or arbitrary VDD sample/product package is introduced by this
closure.

## Fixed-width and SFT/JFT boundary

The selected original VDD caller surface reaches the existing
`mvdm_vdd_sft_shadow` synchronous host-shadow bridge.  Its identity facade
`mvdm_host_identity` is not a second mapping implementation: it delegates
publish, lookup, resolve, and release directly to the current session's sole
`host_resource_mappings` instance.  Thus neither a raw host pointer nor a raw
host handle crosses the source-shaped fixed-width boundary.

The focused x86 fixture rebuilt and completed successfully at
`build/M0-T280/x86-vdd-sft-shadow/mvdm_vdd_sft_shadow_fixture.exe`.  It
compiles `session`, the one mapping-manager implementation, the identity
facade, guest-location facade, and VDD SFT/JFT shadow together.

## Formal selected-graph evidence

`New-T310OriginalSoftpcNinja.ps1` now selects
`mvdm_vdd_sft_shadow.c` with the other adapter-softpc binding units.  Fresh
`build/M0-T310/S30/formal-host-vdd-x86-r1` completed the selected CCPU40
candidate and forced-link closure with 381 graph edges.  The forced-link audit
has no unresolved `mvdm_vdd_sft_shadow` or selected `nt_vdd.c` symbol.

This is source/binding closure only.  Product VDD expansion and integrated
machine behavior remain owned by later S40 and S49 work.
