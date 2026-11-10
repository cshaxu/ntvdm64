# Win95 launch tools

`APPLY.CMD` prompts for the installed Windows 95 directory and prepares it.
It then offers DOSONLY mode `0` (the default, with host multitask payloads)
or `1` (guest-only DOS execution). Mode 0 uses the shared multitask installer
to install `INSIGNIA.386` and `NTVDM64.386` and update `SYSTEM.INI`.
`UNAPPLY.CMD` prompts for the same directory and restores a successful preparation. Both entry points are CMD files; this directory contains no
PowerShell implementation or wrapper.

The apply command consumes released `patch95.exe`, `compat95.sys`, `PIF.EXE`,
`MOUSE31.DRV` and the unchanged `SETVER.EXE` from `assets/release`. It creates the private NTVDM launch
profile under `PATCH`, prepares identity-checked `SYSTEM\VMM32.VXD` and
`SYSTEM\KRNL386.EXE` replacements with adjacent `.W95LAUNCH.BAK` recovery
copies, and creates a patched `PATCH\SETVER.EXE` from that asset. It creates
`PATCH\\WIN.CMD` / `PATCH\\WIN.PIF` with the existing PIF tool.

Mouse setup itself does not edit `SYSTEM.INI`. The tool reads only the
`[boot]` `mouse.drv` value, resolves its active driver, and replaces that file
in place with the released driver. The original is retained beside it as
`<active-driver>.BAK`. Missing, ambiguous, or non-existent active
driver settings are refusals rather than a fallback to an unused `MOUSE.DRV`.

If `PATCH\LOGO.SYS` exists, APPLY enables it as startup artwork. The image
is supplied by the user and is not part of the release. The tool does not
modify the system DOS Prompt PIF or the Start menu PIF.

UNAPPLY first restores the shared multitask configuration when installed,
then reads the active mouse path from `SYSTEM.INI` and restores each backup
that exists, without hash checks. It also accepts legacy `.W95LAUNCH.BAK`
mouse backups. It deletes the entire `PATCH` directory, including user files
and LOGO.SYS; missing backups are skipped. No journal is created or required.
