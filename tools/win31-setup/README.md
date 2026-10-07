# Windows 3.1 original-Setup PATCH tooling

`package-win31-setup.ps1` is the S3 producer. It copies an owner-supplied,
unchanged Windows 3.1 media/install tree to a separate package directory and
adds only authored files below `PATCH`. It never writes to its input media.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/win31-setup/package-win31-setup.ps1 `
  -BuildRoot build/M0-T436/S3/rNNN-package `
  -OriginalMedia <original-media-directory> `
  -Destination <separate-package-directory> `
  -NtDos <checked-NTDOS.SYS> `
  -PifTemplate <checked-WIN31.PIF> `
  -OriginalWin386 <checked-installed-WIN386.EXE>
```

The checked retail `WIN386.EXE` source and NT DOS ABI identities are mandatory.
The package carries the S1-approved recoverable `/3` candidate, its provenance,
and the released `MOUSE31.DRV`; it never invents an alternative mode or falls
back from `/3` to `/S`.

Run `<package>\PATCH\SETUP.CMD`, choose the destination in original Setup,
then supply that actual destination when prompted. The script runs Setup from a
package's unchanged media root; it does not create a `WORK` copy. The
installed destination receives its own self-contained `PATCH` directory:

- `WINSTD.CMD` / `WINSTD.PIF`: standard mode (`/S`)
- `WIN386.CMD` / `WIN386.PIF`: forced enhanced mode (`/3`)
- shared `CONFIG.NT` and `AUTOEXEC.NT`: same checked DOS/DOSX semantics
- `MOUSE31.DRV`, the first original `WIN386.ORIG`, and adaptation provenance

Both command files resolve plain `run16` through `PATH`; no launcher, PIF,
profile, or driver refers to the package directory, repository, or a
hard-coded drive. Default NT profiles are never edited. The enhanced path is
still explicitly instability-limited: successful package generation is not a
claim of reliable `/3` startup or normal exit.
