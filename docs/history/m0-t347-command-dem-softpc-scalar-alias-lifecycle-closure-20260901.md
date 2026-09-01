# M0 T347 closure — COMMAND/DEM/SoftPC scalar-alias lifecycle

T347 closes the complete source-defined registration cohort initiated by
original NTDOS SVC_CMDSETINFO.

- S1 established the original SCSINFO, binary-byte and FD-access-word
  registrations, consumers, widths, ordering and lifetime.
- S2 extended the existing session mapping-manager/guest-location seam with
  atomic scalar-location registration and short-lease byte/word operations;
  it added no second mapper or guest-pointer carrier.
- S3 migrated every selected COMMAND, DEM, fixed-disk and floppy source use,
  removed live `pIsDosBinary` and `pFDAccess` aliases, and registered the
  minimal mirror difference as MVDM-HOST-DIV-111.
- S4 used one unchanged fixed CPU40/x86 observation. It again reached only
  original `54:05` ingress and the ordinary eight-second controlled timeout.
  It does not prove cmdSetInfo completion and does not attribute a subsequent
  failure to the scalar cohort.

Focused scalar lifecycle proof and a forced selected CPU40/x86 formal product
rebuild pass. T347 does not claim disk/floppy runtime exercise, NTIO/NTDOS
continuity, COMMAND.COM start, DOS child execution, or graphics workload.
Those outcomes remain package-owned integration work; trace does not create a
new leaf repair here.

Evidence:

- `docs/etc/evidence/m0-t347-s1-scalar-alias-registration-lifetime-ledger-001.md`
- `docs/etc/evidence/m0-t347-s2-scalar-location-adapter-closure-001.md`
- `docs/etc/evidence/m0-t347-s3-original-scalar-alias-migration-closure-001.md`
- `docs/etc/evidence/m0-t347-s4-fixed-scalar-observation-001.md`
