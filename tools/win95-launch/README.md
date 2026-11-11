# Win95 launch tools

`APPLY.CMD` prompts for the installed Windows 95 directory and prepares it.
It always copies `INSIGNIA.386`, `NTVDM64.386`, `PIF.EXE` and one `WIN.CMD`
into `PATCH`. INSIGNIA.386 is enabled as base VM/mouse support; NTVDM64.386 starts disabled.

Run the installed `PATCH\WIN.CMD` to choose DOSONLY (`0` host commands,
`1` guest DOS), then whether to enable NTVDM64.386 multitask payloads (`0` no, `1` yes).
These choices are independent. Each launch generates `CONFIG.NT`,
`AUTOEXEC.NT` and one `WIN.PIF`, using the same staging and comment-toggle
approach as win31-setup. Windows 95 has no Standard/Enhanced choice.
Unrelated SYSTEM.INI entries are preserved. Invalid choices or configuration
errors stop before launch. Do not reconfigure a running installation.

Package runtime uses only CMD and released addon tools. Materials come from
`assets/release`: `patch95.exe`, `compat95.sys`, `PIF.EXE`, `MOUSE31.DRV`,
`SETVER.EXE` and the two drivers. The existing identity-checked preparation of
`SYSTEM\VMM32.VXD` and `SYSTEM\KRNL386.EXE`, their adjacent
`.W95LAUNCH.BAK` recovery copies, and patched `PATCH\SETVER.EXE` are retained.
No new guest binary patch is introduced by the dynamic launcher.

Mouse setup itself does not edit `SYSTEM.INI`. The tool reads only the
`[boot]` `mouse.drv` value, resolves its active driver, and replaces that file
in place with the released driver. The original is retained beside it as
`<active-driver>.BAK`. Missing, ambiguous, or non-existent active
driver settings are refusals rather than a fallback to an unused `MOUSE.DRV`.

If `PATCH\LOGO.SYS` exists, WIN.CMD enables it as startup artwork. The image
is supplied by the user and is not part of the release. The tool does not
modify the system DOS Prompt PIF or the Start menu PIF.

UNAPPLY removes the dynamic launcher's two device entries, or restores the
original SYSTEM.INI backup from the retired standalone multitask installer when present,
then reads the active mouse path from `SYSTEM.INI` and restores each backup
that exists, without hash checks. It also accepts legacy `.W95LAUNCH.BAK`
mouse backups. It deletes the entire `PATCH` directory, including user files
and LOGO.SYS; missing backups are skipped. No journal is created or required.

Acceptance is the four DOSONLY/multitask combinations, not merely generation
of four files. DOSONLY=0 with both drivers must support independent host
payloads in two DOS VMs. With NTVDM64.386 disabled and DOSONLY=0, native CUI
shell-out suspends guest Windows and uses fullscreen text. DOS programs instead
reenter the existing worker and retain windowed guest VM multitasking.
INSIGNIA.386 remains loaded in both cases. Verify scheduling by child type,
not by treating the entire profile as blocking.
DOSONLY=1 keeps guest execution regardless of multitask selection; an unsupported
native executable must not silently become a host payload. Each applicable
path is checked for mouse movement/clicks, DOS-window/fullscreen transitions,
editor interaction/save/exit and return. Script fixtures use a run16 shim and
cannot establish these runtime results. Current evidence is in the active S.

The standalone win-multitask package is retired. Driver deployment is owned by
APPLY.CMD and configuration by the installed WIN.CMD --install/--remove,
matching win31-setup's dynamic launcher model. No new MULTITASK.INI backup,
hash or staging records are generated. UNAPPLY accepts existing original INI
backups only for removal of old installations; it does not call another tool.
