# M0 T436 S3 — Windows 3.1 Setup/PATCH package

## Question

Can a Windows 3.1 package preserve its supplied media, install only an owned
PATCH payload, and leave an installation with two unambiguous `run16` entry
points and no dependency on the package, repository, or temporary work tree?

## Result

Yes for the packaging/profile contract. `tools/win31-setup` replaces the
displaced launcher home. A clean package supplies `PATCH\SETUP.CMD`, the
released `MOUSE31.DRV`, a checked S1 enhanced candidate/provenance, and the
scripts/templates required by Setup. Postconfiguration produces exactly
`WINSTD.CMD/.PIF` and `WIN386.CMD/.PIF` plus one shared `CONFIG.NT` and
`AUTOEXEC.NT`. There is no generic `win.cmd` or `start.cmd` alias.

The shared profiles are justified by the pre-existing same DOSONLY/HIMEM and
local-DOSX semantics. The only mode-specific values are the two PIF titles and
the `/S` versus `/3` values in both main and 386-extension argument fields.

The enhanced path accepts only the reviewed retail input hash, preserves the
first original as `PATCH\WIN386.ORIG`, installs candidate
`C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8`, and keeps
its adaptation JSON. It is still an instability-limited S1 experiment, not a
claim of reliable enhanced startup or normal exit.

## Inputs and procedure

All test writes used fresh owned product-test paths or repository `build`; the
current supplied Win3.1 installation was read-only input. No drive substitution, default NT
profile edit, guest hot patch, or host-product image change occurred.

1. Parser-checked every new/moved PowerShell entrypoint.
2. Ran `win31_dosmgr_adaptation_test.ps1` against the exact retail WIN386 and
   the selected current NT DOS image: single ten-byte range, candidate identity,
   wrong-input rejection and recovery metadata passed.
3. Built a clean media-copy fixture, ran `package-win31-setup.ps1`, and proved
   package PATCH contains `SETUP.CMD`, no WORK, MOUSE31 and the checked
   adaptation payload.
4. Postconfigured a second clean installation copy from that package; the
   two PIFs, both command launchers, checksums, extension arguments, profile
   paths, original backup and candidate identity passed.
5. Re-ran postconfiguration to prove the already-adapted candidate is
   idempotent. `win31_setup_package_test.ps1` scanned installed PIF/CMD/NT
   content for package/repository/WORK dependencies and passed.
6. `win31_setup_runner_test.ps1` substituted only PATH `run16` with a bounded
   test command. It proves `run-setup.ps1` first generates package-local
   `SETUP.PIF`, `CONFIG.NT`, and `AUTOEXEC.NT`, invokes that PIF through PATH
   `run16`, preserves the returned exit code, and creates no WORK directory.
7. A failed first hand launch exposed that a bare `SETUP.EXE` is a Win16 NE
   GUI program, not a synchronous DOS task. The repaired runner now creates
   the profile-bound Setup PIF at the actual package path. A short live
   probe reached a live `run16` plus its new WOW/NTVDM session; it was then
   explicitly terminated and all generated probe files were removed. This
   proves the corrected entry boundary, not original Setup completion.

## Limitations and follow-up

The final hand-test package is rebuilt from the subsequently supplied original
compressed Setup media; its 467 original media files are byte-identical and
`PATCH` is the only added root entry. It contains no WORK tree. The generated
Setup PIF/profile is intentionally not retained in the clean package: every
`SETUP.CMD` execution recreates it for that package's actual location. Actual
original Setup, physical installation selection, and owner hand-test remain
the final T436 acceptance step. Existing S1 enhanced instability remains
unchanged.
