# T484 S4 OEM guest preparation

## Retention and change

P22 retains the existing OEM regression: no equivalent replacement was found
for its real CP437 filename, volume, FCB/computer-name and COMMAND environment
assertions. The historical task name alone is not retirement evidence.
The independent oem_file_contract.asm source is unchanged and now selected by
the existing CMD guest-api producer as O37.COM. This is not original guest media.

The runner uses the existing CMD input alias and physical staging owners,
then the existing isolated process scope. It no longer copies a probe into
the supplied package. Reports and the verified stage manifest stay under
build/oem-<prefix>; the owned runtime is cleaned after the run. The volume
oracle checks the physical staging drive, not the source package drive.
Test-created directories and file assertions remain with the test runner.
Standalone assertions may use PowerShell; mandatory binary preparation does not.

## Verification

Owned Userenv process7484 ran build/t484-s4-oem/run.ps1 from
2026-11-24T18:50:13Z to18:51:43Z, exit0. Cached full build passes. Actual
direct and twice-nested COMMAND guest cases both pass the existing text,
volume bytes, Unicode path/content, file-count and normal-exit checks.
Probe SHA256 is DD643A9D94ACC08E8D1548FEE4B933E8F3747FCDAD20BF9466CAD75B14A87F78.
Both cases produce the same checked file hash. The isolated runtime was removed.
The test remains explicitly restricted to the OEM437 host profile.

All11 raw host images and22 dual-published hashes are unchanged; publication
identity was verified, not replaced by an unnecessary recopy. No production,
mirror, original guest or winnt.zip change and no new Full pass is claimed.
Governance, script parsing and diff checks gate this delivery.

## Progress

Frozen census remains343/355 disposed,12 pending. Supplemental preparation
chains decrease11 to10. DPMI fatal preparation remains open: its exact dialog,
Ignore/new-dialog and Terminate contracts also have no demonstrated replacement.
S4 remains active; S5 clean interpreter-free acceptance has not begun.
