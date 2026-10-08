# Win31 launch preparation

`win31-launch` prepares an **already installed** Windows 3.1 tree for the
project's NTVDM path.  It does not run Windows Setup and does not repair moved
application or Program Manager paths.

The completed tool accepts a chosen installed root, preserves the two approved
retail system binaries before replacement, and writes only these runtime files
under `<root>\PATCH`:

- `MOUSE31.DRV`, `KRNL386.ORIG`, and `WIN386.ORIG`;
- `CONFIG.NT` and `AUTOEXEC.NT`;
- `WINSTD.CMD` / `WINSTD.PIF` for `WIN.COM /S`;
- `WIN386.CMD` / `WIN386.PIF` for `WIN.COM /3`.

The command files resolve `run16` through `PATH`; they contain no fixed drive,
repository, package, or media path.  There is deliberately no generic
`WIN.CMD`, `START.CMD`, or `WIN31.PIF` alias.

The corresponding source belongs in `src/addon/win31-launch/`.  Build-only
candidate derivation may use build-owned files, but neither templates nor
PowerShell sources are installed below `PATCH`.

Open `APPLY.CMD`, enter the installed root once when prompted, and leave the
window open to read the final exit code.  The command file itself has no
fixed-drive dependency.

The self-contained tool directory contains `APPLY.CMD`, `WIN31LAUNCH.EXE`,
and the released `MOUSE31.DRV`.  Do not copy only the command file: the
native finalizer and driver must remain beside it.

`/3` means Windows 3.1's 386 enhanced mode.  The tool can prepare that profile
and its approved candidate image; it does not claim that enhanced mode is
reliable.  The deferred original-Setup transition remains outside this tool.
