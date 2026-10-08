# Win31 launch preparation

`win31-launch` prepares an **already installed** Windows 3.1 tree for the
project's NTVDM path.  It does not run Windows Setup and does not repair moved
application or Program Manager paths.

`APPLY.CMD` accepts a chosen installed root, preserves the two approved retail
system binaries before replacement, replaces the installed `SYSTEM\MOUSE.DRV` from
the released add-on, and writes these owned recovery/profile files under
`<root>\PATCH`:

- `KRNL386.ORIG`, `WIN386.ORIG`, and `MOUSE.DRV.ORIG` (the original
  `SYSTEM\MOUSE.DRV`);
- `CONFIG.NT` and `AUTOEXEC.NT`;
- `WINSTD.CMD` / `WINSTD.PIF` for `WIN.COM /S`;
- `WIN386.CMD` / `WIN386.PIF` for `WIN.COM /3`.

Both generated launch PIFs explicitly set the original CloseOnExit flag: each
`.CMD` synchronously waits for its own independent PIF session, so it must
return after `WIN.COM` terminates rather than park that session inactive.

The command files resolve `run16` through `PATH`; they contain no fixed drive,
repository, package, or media path.  There is deliberately no generic
`WIN.CMD`, `START.CMD`, or `WIN31.PIF` alias.

`PATCH386.EXE` has one purpose: it verifies and transforms the two already
approved `KRNL386.EXE`/`WIN386.EXE` identities.  It neither creates backups
nor touches drivers, PIFs, configuration, or launch commands.  `APPLY.CMD`
owns those ordinary file operations.  The corresponding source belongs in
`src/addon/win31-launch/`.  Build-only candidate derivation may use
build-owned files, but neither templates nor PowerShell sources are installed
below `PATCH`.

Open `APPLY.CMD`, enter the installed root once when prompted, and leave the
window open to read the final exit code.  The command file itself has no
fixed-drive dependency.

The tool directory contains only `APPLY.CMD`, `PATCH386.EXE`, and this
documentation.  It resolves `PIF.EXE`, `HASH.EXE`, and `MOUSE31.DRV` from the
repository's `assets\release` directory; they are not duplicated here.

`/3` means Windows 3.1's 386 enhanced mode.  The tool can prepare that profile
and its approved candidate image; it does not claim that enhanced mode is
reliable.  The deferred original-Setup transition remains outside this tool.
