# mvdm-guest

Complete load-only MVDM guest mirror.  `dos/` carries the selected DOS/V86
guest; `win16/` carries the selected WOW16/bin86 guest.  Neither subtree is a
host library, compile input or link input.  App loads only manifest-selected
immutable guest bytes.

## Provenance

- [DOS carry manifest](../../docs/etc/operations/m0-t274-s3-dos-guest-mirror-manifest.tsv)
  records every selected DOS/V86 path and its source hash.
- [WOW16 carry manifest](../../docs/etc/operations/m0-t274-s4-wow16-carry-manifest.tsv)
  and [bin86 carry manifest](../../docs/etc/operations/m0-t274-s4-bin86-carry-manifest.tsv)
  record the selected Win16 paths and hashes.
- `build/output/dos` and `build/output/wow16` remain in-place immutable
  product evidence. They are not copied here or linked into the host.
- Eight selected Win16 driver products (`comm.drv`, `vga.drv`, `keyboard.drv`,
  `mouse.drv`, `sound.drv`, `system.drv`, `TIMER.DRV`, and `WFWNET.DRV`) are
  hash-identical carries from `build/output/wow16`; the S9 evidence records
  their exact source paths.
- Eight selected Win16 driver products (`comm.drv`, `vga.drv`, `keyboard.drv`,
  `mouse.drv`, `sound.drv`, `system.drv`, `TIMER.DRV`, and `WFWNET.DRV`) are
  hash-identical carries from `build/output/wow16`; the S9 evidence records
  their exact source paths.

## Divergence register

None.  Apart from this component README, every carried file below `dos/` and
`win16/` remains byte-identical to its recorded local source path.
