# T484 S3 P196: consolidated WOW host contracts

## Retention and changes

Retain five font cases (glyph rotation, ETM contract/native boundary, tracking
and native search) with unchanged C test bodies. One CMD/CMake target owns
all five source/define profiles; retain the separate public metafile contract.
Remove the redundant glyph wrapper, whose coverage remains in the font runner.
No production source, original guest or mirror change.

Retire inspect-ne-classification: a one-time NE classification investigation,
not a current product assertion. Its historical result remains in
m0-t435-s3-setup-package.md and Git; this does not resolve that historical issue.
Retire verify-wow-gdi-handle-contract and its sole private GdiQueryTable probe:
the old table acquisition is no longer the product path. Historical NULL-table
evidence remains in m0-t422-s2-user-client-lifecycle-ledger.md and subsequent
GDI ownership records. Current wow_gdi_alias_fixture coverage remains intact.
No claim that these retired observations were rerun or are universally true.

## Verification and delivery

Owned background PID21924 ran build/t484-s3-wow-host-contracts/run.ps1,
2026-11-24 12:44:19 through12:45:46 UTC, exit0. Five font cases pass, including
actual native font queries; public metafile roundtrip passes
WOW_METAFILE_PUBLIC_HOST_OK. Cached full host build passes. All11 raw host
hashes and22 dual-publication hashes match the existing package exactly;
no redundant product overwrite. This is focused host verification, not a new
Win16 guest or Full acceptance run. Governance and script parsing are required
before the containing commit/push.

## Tracking

Eight of13 supplemental compiler candidates now have dispositions; five remain.
This batch removes two obsolete diagnostics, consolidates one wrapper and
migrates two retained producer entrypoints. Original355 census remains316
resolved/39 pending;21 supplemental preparation/staging chains remain.
S3 is not closed; S4/S5 obligations and the recorded native-output failure remain.
