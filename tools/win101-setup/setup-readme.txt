Windows 1.01 / NTVDM INT33 mouse installer

1. Put run16 in PATH. This package never fixes its executable location.
2. Do not run another run16 session during installation (global BaseSrv).
3. Put the package at a short physical DOS-compatible path without spaces
   or shell metacharacters. Do not create drive substitutions or mappings.
   Run PATCH\SETUP.CMD in an existing CMD window, without arguments.
4. In original Setup choose your installation destination interactively.
   Mouse: 2, Microsoft Mouse (Bus/Serial), supplied by our INT33 driver.
   Display: 6, EGA (more than 64K) with Enhanced Color Display.
   Printer: no printer unless you intend to configure one.
   The working source is the package's actual PATCH\WORK directory.
5. After Setup returns, the script asks for the actual installation directory,
   verifies its files, then generates WIN101.PIF, CONFIG.NT,
   AUTOEXEC.NT, SETVER.EXE and win.cmd under that destination's PATCH.
   Win1.01 uses SETVER; no Win3.1 /S switch or NT/WOW DOSX is loaded.
6. Run the installed PATCH\win.cmd.
   Installed PATCH also contains the same independent MOUSE.DRV; actual
   Win1.01 execution uses the mouse module embedded by original Setup.
   Root WIN.PIF and SETUP.PIF are also generated using installed PATCH profiles.
   Copied root CONFIG.NT/AUTOEXEC.NT are rewritten to installed paths.
   You can remove the installation package afterwards: no installed launch
   profile depends on the package's temporary WORK directory.

PATCH\setup-result.txt records the actual run16 exit code. Even after a nonzero
return, enter the actual directory if you confirm installation completed;
otherwise press Enter to skip. The real nonzero exit code is retained and
profile generation is not a claim of verified installation or guest startup.
If original Setup completed but profiles were not generated, run:
powershell -NoProfile -ExecutionPolicy Bypass -File "<package>\PATCH\configure-launch.ps1" -Mode Installed -InstallRoot "<installation-directory>"
This requires installed WIN.COM, WIN100.BIN and WIN100.OVL. It generates only
the launch profile, not missing Windows files. Startup still needs verification.
PIF/config store the supplied paths; regenerate after relocating installation.
PIF paths must fit their original fixed-size fields; use shorter real paths
if rejected, never a temporary mapped drive.

SETUP.CMD generates a private Setup profile before invoking original Setup.
It copies original root media into PATCH\WORK, replaces MOUSE.DRV there,
and runs that working copy directly at its physical path. No mapping is used.
No guest Windows core is patched by this installation script. This directory
contains unchanged original loose media files at root. All additions, including
the independent MOUSE.DRV/source and configuration/launch files, are in PATCH.
The root MOUSE.DRV is the unchanged original; the new one is PATCH\MOUSE.DRV.
Other Windows installations and
global NTVDM configuration are not overwritten by profile generation.
No drive letter or machine-specific package, installation, tool or run16 path
is hardcoded. WORK is created temporarily and removed when the script finishes,
including failure paths; it is not shipped or retained. Forced termination may
leave WORK behind: move it aside before retrying. Never use WORK as the install
destination. Original media and the installed directory are not removed.
