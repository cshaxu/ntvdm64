# Win31 launch preparation

`win31-launch` prepares an **already installed** Windows 3.1 tree for the
project's NTVDM path.  It does not run Windows Setup and does not repair moved
application or Program Manager paths.

`APPLY.CMD` accepts a chosen installed root, preserves the two approved retail
system binaries before replacement, and replaces the installed
`SYSTEM\MOUSE.DRV` from the released add-on. The original files remain beside
their replacements as:

- `SYSTEM\KRNL386.EXE.BAK`, `SYSTEM\WIN386.EXE.BAK`, and
  `SYSTEM\MOUSE.DRV.BAK`.

The tool writes only these profile files under `<root>\PATCH`:
- `CONFIG.NT` and `AUTOEXEC.NT`;
- `WINSTD.CMD` / `WINSTD.PIF` for `WIN.COM /S`;
- `WIN386.CMD` / `WIN386.PIF` for `WIN.COM /3`.

Both generated launch PIFs explicitly set the original CloseOnExit flag: each
`.CMD` synchronously waits for its own independent PIF session, so it must
return after `WIN.COM` terminates rather than park that session inactive.

The command files resolve `run16` through `PATH`; they contain no fixed drive,
repository, package, or media path.  There is deliberately no generic
`WIN.CMD`, `START.CMD`, or `WIN31.PIF` alias.

`PATCH386.EXE` verifies and transforms the supported
`KRNL386.EXE`/`WIN386.EXE` identities.  The kernel adaptation also clears its
MZ `e_lfanew` field so DOS-only-off launches use its DOS startup stub. This
changes how host image-inspection tools classify the kernel. Both generated
profiles use DOSONLY=0. `PATCH386.EXE` also provides the bounded SYSTEM.INI
transformation described below; `APPLY.CMD` owns backups, driver replacement,
profiles, and launch commands. The corresponding source belongs in
`src/addon/win31-launch/`.  Build-only candidate derivation may use
build-owned files, but neither templates nor PowerShell sources are installed
below `PATCH`.

The tool also prepares the installed `SYSTEM.INI` `[386Enh]` section: it
removes permanent-swap settings, disables paging and direct disk access, and
disables COM1–COM4 IRQs for the NTVDM host. Other INI entries are preserved.
`PATCH\SYSTEM.INI.BAK` holds the prior configuration and
`PATCH\SYSTEM.INI.SHA256` records the prepared file's identity. Repeat apply
and unapply refuse to overwrite later edits to `SYSTEM.INI`; unapply restores
the saved configuration when its current identity still matches.

Open `APPLY.CMD`, enter the installed root once when prompted, and leave the
window open to read the final exit code.  The command file itself has no
fixed-drive dependency.

The tool directory contains only `APPLY.CMD`, `UNAPPLY.CMD`, `PATCH386.EXE`, and this
documentation.  It resolves `PIF.EXE`, `HASH.EXE`, and `MOUSE31.DRV` from the
repository's `assets\release` directory; they are not duplicated here.

The first `APPLY.CMD` preserves the installed mouse driver as its adjacent
backup. Repeated `APPLY.CMD` accepts only the exact currently released driver;
it refuses an unrecognised replacement instead of overwriting it. Restore first
when intentionally changing driver versions.

`UNAPPLY.CMD` is the recovery entry point. It verifies the two patched system
binary identities and requires the adjacent `.BAK` files before moving each
backup back into place. Mouse restoration does not depend on the current or
released driver version. It deletes only the six
tool-owned launch-profile files under `PATCH`; any other `PATCH` content is
preserved.

`/3` means Windows 3.1's 386 enhanced mode.  The tool can prepare that profile
and its approved candidate image; it does not claim that enhanced mode is
reliable.  The deferred original-Setup transition remains outside this tool.
