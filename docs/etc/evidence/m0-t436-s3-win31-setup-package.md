# M0 T436 S3 — Windows 3.1 Setup/PATCH package

## Question

Can a Windows 3.1 package preserve its supplied media, install only an owned
PATCH payload, and leave an installation with two unambiguous `run16` entry
points and no dependency on the package, repository, or temporary work tree?

## Result

Yes for the packaging/profile contract. `tools/win31-setup` replaces the
displaced launcher home. A clean package supplies `PATCH\SETUP.CMD`, the
released `MOUSE31.DRV`, checked recovery candidates for `KRNL386` and
`WIN386`, and a packaging-only finalizer. It carries no PowerShell,
template, manifest, log or scratch payload. Postconfiguration produces exactly
`WINSTD.CMD/.PIF` and `WIN386.CMD/.PIF` plus one shared `CONFIG.NT` and
`AUTOEXEC.NT`. There is no generic `win.cmd` or `start.cmd` alias.

The shared profiles are justified by the pre-existing same DOSONLY/HIMEM and
local-DOSX semantics. The only mode-specific values are the two PIF titles and
the `/S` versus `/3` values in both main and 386-extension argument fields.

The finalizer accepts only the reviewed retail input hashes. It preserves the
first originals as `PATCH\KRNL386.ORIG` and `PATCH\WIN386.ORIG`, then installs
the checked candidates. A pre-adapted system without a valid recovery copy is
rejected rather than silently accepted. Enhanced mode is still an
instability-limited S1 experiment, not a claim of reliable enhanced startup or
normal exit.

## Inputs and procedure

All test writes used fresh owned product-test paths or repository `build`; the
current supplied Win3.1 installation was read-only input. No drive substitution, default NT
profile edit, guest hot patch, or host-product image change occurred.

1. Parser-checked every new/moved PowerShell entrypoint.
2. Ran `win31_dosmgr_adaptation_test.ps1` against the exact retail WIN386 and
   the selected current NT DOS image, plus
   `win31_krnl386_adaptation_test.ps1` against the exact retail KRNL386:
   both assert their sole approved changed range, candidate identity,
   all-other-byte preservation and wrong-input rejection.
3. Built a clean media-copy fixture, ran `package-win31-setup.ps1`, and proved
   package PATCH contains `SETUP.CMD`, no WORK, MOUSE31 and the checked
   adaptation payload.
4. Postconfigured a second clean installation copy from that package; the
   two PIFs, both command launchers, checksums, extension arguments, profile
   paths, original backup and candidate identity passed.
5. Re-ran postconfiguration to prove the already-adapted candidate is
   idempotent. `win31_setup_package_test.ps1` scanned installed PIF/CMD/NT
   content for package/repository/WORK dependencies and passed.
6. `win31_setup_runner_test.ps1` substitutes only PATH `run16` with a bounded
   test command. It proves `SETUP.CMD` invokes `command /c SETUP.EXE`,
   preserves the returned exit code, and creates no WORK directory or Setup
   PIF/profile.
7. A failed hand launch established that a bare `SETUP.EXE` follows its Win16
   NE GUI entry rather than the intended DOS installer. The package now starts
   the MZ text installer through `COMMAND.COM /c SETUP.EXE`; this gives an
   ordinary synchronous DOS completion and needs no generated Setup PIF.
8. Fresh r071 packaging rebuilt the finalizer from
   `src/addon/win31-setup/patchset.c`, then passed the exact KRNL386/WIN386
   adaptation tests, the short-path PIF finalizer test, the deliberate missing
   recovery-copy rejection, and the batch-runner test. The resource build was
   corrected to pass slash-normalized filenames to `rc.exe`; that is a build
   carrier correction only and does not affect the checked guest bytes.
   The r076 fixture composes the clean media PATCH check with the positive
   installed-PATCH check before its recovery-copy negative case, proving those
   assertions apply to the same finalizer output.

## Limitations and follow-up

The final hand-test package is rebuilt from the subsequently supplied original
compressed Setup media; its 467 original media files are byte-identical and
`PATCH` is the only added root entry. It contains no WORK tree or generated
Setup PIF/profile. Actual original Setup, physical installation selection, and
owner hand-test remain the final T436 acceptance step. Existing S1 enhanced
instability remains unchanged.

The owner hand-test PATCH payload was refreshed from r071 after per-file
SHA-256 comparison. This confirms delivery identity only; it does not
substitute for an owner-run original Setup completion.

## Current repeat verification

The current source was rebuilt into a fresh clean-media package in r103.  Its
batch entrypoint test passed, proving that `PATCH\SETUP.CMD` delegates only to
the user-operated original text Setup and preserves `run16`'s result.  r107
ran the resulting `PATCHSET.EXE` at a short physical O: test path, where
the historical PIF field limit is representable: both installed profiles,
their PIF checksums and extensions, recovery originals, the driver selection,
and the deliberate missing-recovery rejection passed.  The disposable fixture
was deleted after the test.  This remains packaging evidence, not proof of a
completed graphical installation: the current automation context was denied
permission to create its private desktop before visual Setup inspection.

r109 additionally rebuilds the embedded PIF resource after clearing every
path-bearing template field and recomputing its checksum.  The static
finalizer test rejects any drive-qualified string in `PATCHSET.EXE` before it
configures the short-path fixture.  The owner-selected PATCH directory now
matches that six-file r109 payload exactly.  The owner-selected installation
target is intentionally empty before the hand test: it is the direct
installation target, not a copied media root. Original compressed media stay
in the owner-supplied source root; only its owned `PATCH`
directory is authored.
