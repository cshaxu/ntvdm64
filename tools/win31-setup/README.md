# Windows 3.1 original-Setup PATCH tooling

`package-win31-setup.ps1` copies owner-supplied Windows 3.1 media to a separate,
build-owned derived package. It never writes to its input media.  The derived
root retains the original layout and replaces only `KRNL386.EX_` and
`WIN386.EX_`: each is regenerated from its identity-checked approved candidate
and expanded again with Windows `EXPAND.EXE` before publication. This is
necessary because Setup loads Windows before any post-Setup PATCHSET can run.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/win31-setup/package-win31-setup.ps1 `
  -BuildRoot build/M0-T436/S3/rNNN-package `
  -OriginalMedia <original-media-directory> `
  -Destination <separate-package-directory> `
  -NtDos <checked-NTDOS.SYS> `
  -PifTemplate <checked-WIN31.PIF> `
  -OriginalWin386 <checked-installed-WIN386.EXE>
```

The checked retail `KRNL386.EXE` and `WIN386.EXE` sources and NT DOS ABI
identities are mandatory. The PATCH carries the owner-approved hash-gated
candidates, matching checked retail recovery images, and the released
`MOUSE31.DRV`; it never invents an alternative mode or falls back from `/3`
to `/S`.

Run `<package>\PATCH\SETUP.CMD`, choose the destination in original Setup,
then supply that actual destination when prompted. The script runs Setup
through DOS `COMMAND.COM /c` from the derived media root, selecting the
original text installer and preserving synchronous completion; it does not
create a `WORK` copy. The
installed destination receives its own self-contained `PATCH` directory:

- `WINSTD.CMD` / `WINSTD.PIF`: standard mode (`/S`)
- `WIN386.CMD` / `WIN386.PIF`: forced enhanced mode (`/3`)
- shared `CONFIG.NT` and `AUTOEXEC.NT`: same checked DOS/DOSX semantics
- `MOUSE31.DRV`, plus the first original `KRNL386.ORIG` and `WIN386.ORIG`

Both command files resolve plain `run16` through `PATH`; no launcher, PIF,
profile, or driver refers to the package directory, repository, or a
hard-coded drive. Default NT profiles are never edited. The enhanced path is
still explicitly instability-limited: successful package generation is not a
claim of reliable `/3` startup or normal exit.
