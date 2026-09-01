# M0 T337 — Packaged MVDM system-root composition closure

## Delivered

- One session-local, executable-relative `mvdm_system_root` replaces the
  provisional split DOS and Win16 media-root state.
- The package contract is `mvdm/`, `mvdm/system32/`, and `mvdm/softpc/`.
  SoftPC ROM/CMOS/profile files remain external original resource files; they
  are not compiled into the executable and no invented `roms/` layer remains.
- The selected original COMMAND, DEM, PIF and NTIO-loading bodies retain their
  original control flow. Their registered, same-shaped lookup divergence now
  selects the MVDM system root only.
- Fresh formal CCPU40 x86 and x64 graphs compile and link the product; one
  fixed x86 observation uses the staged package without guest modification.

## Limits transferred

The package currently stages the selected DOS system cohort. The exact final
Win16 media subset and each WOW `GetSystemDirectory`/`GetWindowsDirectory`
consumer remain owner-package work; they must be audited rather than globally
redirecting modern host APIs. The fixed observation remained a bounded timeout
and creates no new runtime-owner conclusion.

## Evidence

- [S1 audit](../etc/evidence/m0-t337-s1-packaged-system-file-root-contract-audit-001.md)
- [S2 rebaseline](../etc/evidence/m0-t337-s2-original-system-root-binding-rebaseline-001.md)
- [S3 composition](../etc/evidence/m0-t337-s3-unified-mvdm-system-root-composition-001.md)
- [S4 observation](../etc/evidence/m0-t337-s4-fixed-mvdm-system-root-observation-001.md)
