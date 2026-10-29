# Win95 launch tools

`APPLY.CMD <installed-Windows-95-directory>` prepares one selected installed
Windows 95 tree. `UNAPPLY.CMD <same-directory>` reverses only a successful
preparation. Both entry points are CMD files; this directory contains no
PowerShell implementation or wrapper.

The apply command consumes released `patch95.exe`, `compat95.sys`, `PIF.EXE`,
and `MOUSE31.DRV` from `assets/release`. It creates the private NTVDM launch
profile under `PATCH`, prepares identity-checked `SYSTEM\VMM32.VXD` and
`SYSTEM\KRNL386.EXE` replacements with adjacent `.W95LAUNCH.BAK` recovery
copies, and creates a patched `PATCH\SETVER.EXE` from the runtime copy. It creates
`PATCH\\WIN.CMD` / `PATCH\\WIN.PIF` with the existing PIF tool.

Mouse setup deliberately does not edit `SYSTEM.INI`. The tool reads only the
`[boot]` `mouse.drv` value, resolves its active driver, and replaces that file
in place with the released driver. The original is retained beside it as
`<active-driver>.W95LAUNCH.BAK`. Missing, ambiguous, or non-existent active
driver settings are refusals rather than a fallback to an unused `MOUSE.DRV`.

The schema 3 journal records the patched VMM, kernel, mouse and SETVER hashes.
Unapply checks the
installed mouse and release-owned helper before restoring adjacent backups; it
does not remove unrelated `PATCH` content or rewrite `SYSTEM.INI`.
