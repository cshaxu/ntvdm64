# Windows 1.01 Setup additions

Owner-assigned source home for installer/patch scripts and launch templates.
The independent driver source stays in src/addon/win101-mouse-drv; neither directory
is an OpenNT mirror or a host executable component.

Run `APPLY.CMD` in this source directory. It asks for the original
Windows1.01 media directory, reads the compiled MOUSE101.DRV from assets/release,
validates its add-on manifest/hash and creates media/PATCH. PowerShell callers
may supply `-MediaRoot <directory>` to apply-setup.ps1. No assembler or driver
build directory is required. PIF/SETVER support comes from the already accepted
assets/win101-setup.zip; installer scripts come from this directory.
Original media stay untouched. Existing accepted/owned PATCH files can be
updated; unknown modified files are refused before any write, and unrelated
PATCH files are retained. No WORK directory is generated or shipped at this step.

package-win101-setup.ps1 prepares unchanged original flat media at the package
root and puts all additions under PATCH. PATCH/SETUP.CMD resolves run16 from
PATH, creates PATCH/WORK, substitutes the new MOUSE.DRV in that working copy,
generates its PIF/config and invokes original Setup. Original media stay intact.
Run `PATCH\SETUP.CMD` from CMD, with no arguments. Choose your destination
interactively in original Setup. After Setup returns, the script asks
for the actual installation directory, checks its files, and generates
launch/PIF/config/SETVER files under that directory's PATCH subdirectory.
The package location comes from the script's own directory. Run the installed
PATCH's `win.cmd`.
Installed root WIN.PIF and SETUP.PIF point to installed PATCH profiles.
CONFIG.NT/AUTOEXEC.NT copied by original Setup are rewritten with installed
paths too; no installed launcher/profile depends on the package or WORK.
No disk letter, package location, installation destination or run16 location
is hardcoded. Use short physical DOS-compatible directories, without spaces
or shell metacharacters; PIF path fields have fixed size limits. Do not create
drive substitutions or temporary drive mappings. WORK is created only when
Setup starts and removed on normal script completion, success or failure.
It is not shipped or retained. If the script is forcibly terminated, a leftover
WORK is preserved rather than silently deleted on the next run; move it aside
before retrying. Original media and installed destination are never cleaned up.

`PATCH\setup-result.txt` records the actual run16 exit code. Even after a
nonzero return, you may confirm installation completed by entering its actual
directory; press Enter to skip if it did not. File checks permit profile
generation, not a claim of successful guest execution. The original nonzero
exit code is retained, even when profiles are generated.
If Setup genuinely completed but profile generation was skipped, run:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "<package>\PATCH\configure-launch.ps1" -Mode Installed -InstallRoot "<installation-directory>"
```

The generator requires WIN.COM, WIN100.BIN and WIN100.OVL at that destination;
it does not install missing guest files or prove that Windows starts correctly.
Generated PIF/config files contain the actual supplied paths, not a fixed
default. Regenerate them after relocating the installed directory.

For package preparation, explicitly supply `-BuildRoot`, `-OriginalMedia`,
`-Destination`, `-SetverPath` and `-PifTemplate`; `-ReleaseRoot` optionally
selects another explicitly validated release directory.
There are no machine-specific input-path defaults. Existing-package replacement
requires its previous manifest and retains a recoverable directory backup.

install-mouse101.ps1 is the earlier owner-approved binary-copy installer for
research/recovery; SETUP.CMD does not call it or substitute its output for Setup.
Builds, packaging manifests and backups remain under repository build/.
Actual owner-run Setup completion remains an explicit verification boundary.
