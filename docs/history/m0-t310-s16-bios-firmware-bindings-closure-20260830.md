# M0 T310 S16 closure — BIOS / firmware bindings

## Result

S16 closes the selected original SoftPC BIOS/firmware package at the
source-owner boundary.  The complete BIOS manifest, original ROM/CMOS resource
route, app-selected firmware location binding, CMOS/profile write-root rule
and first-session system-file policy have one recorded disposition.

## Accepted evidence

- [P1 firmware owner audit](../etc/evidence/m0-t310-s16-p1-bios-firmware-owner-audit-001.md)
- [P2 formal assembly](../etc/evidence/m0-t310-s16-p2-bios-firmware-formal-assembly-001.md)
- [P3 manifest owner ledger](../etc/evidence/m0-t310-s16-p3-bios-manifest-owner-ledger-001.tsv)
- [P3 reset/dispatch boundary](../etc/evidence/m0-t310-s16-p3-bios-reset-dispatch-boundary-001.md)
- [P4 resource-write root](../etc/evidence/m0-t310-s16-p4-firmware-resource-write-root-001.md)
- [P5 startup system-file disposition](../etc/evidence/m0-t310-s16-p5-startup-system-file-disposition-001.md)
- [P6 final disposition](../etc/evidence/m0-t310-s16-p6-bios-firmware-final-disposition-001.md)

## Deferred integration condition

S16 does not enable reset or BIOS dispatch by itself.  The original source
order transfers controller work to S17–S31.  S49 must later validate the
integrated selected SoftPC lifecycle without a Bochs fallback or fabricated
firmware success.
