# T95 S6 PC Reset-Floor Profile Observation 002

## Question

Does the corrected declarative PC reset-floor profile retain original native
lifecycle while denying the optional PCI/ACPI/IDE/VBE group and all attached
media?

## Result

Yes. The immutable observation root is
`artifacts/analysis/t95-s6-pc-reset-floor-observation-002-20260811-001`.
The r4 binary hash is unchanged; generated profile hash is
`EA58157828EB29CA4988B925B9F07B1B9222C00FD83B6C0E87E001D1DA4C7FDA`.
The process completes in 323 ms with no watchdog termination. Its exit `1`
is the documented native Bochs 2.6 nogui exit path.

The trace proves all required facts:

- `PCI support: yes, enabled=no`; no PCI, 440FX, PIIX3, ACPI or PCI IDE init
  occurs.
- No VBE extension is initialized; core VGA alone loads the unchanged VGA ROM.
- Exactly the original reset-floor devices initialize, plus `harddrv` for one
  empty ATA controller. The trace contains no disk/CD image path, ATA media
  initialization, adapter environment, BYOB, guest payload or host path.
- `Reset(HARDWARE)` and CPU reset occur; at tick `10,000,005` the original
  benchmark timer reaches `cpu loop quit, shutting down simulator`.
- The CPU remains real-mode halted within firmware execution at bounded stop;
  no DOS, NTDOS, BOP, DEM, WOW16 or host service is claimed.

## Conclusion

`pc-reset-floor-v1` is accepted as the current Bochs runtime-minimum profile:
an intact native PC reset floor with no attached guest media and optional
PCI/i440FX, ACPI, PCI IDE and VBE denied declaratively. The lone empty ATA
controller is a required original lifecycle dependency, not a host capability
or guest storage feature.

The next integration gate is not another device reduction. It is a separate
adapter/CLI composition design that supplies a declared guest input without
turning Bochs or the adapter into DOS/filesystem semantics.
