# Win31 path repair

`win31-path` makes an already installed Windows 3.1 tree relocatable.  It is
independent from `win31-launch`: it never installs guest binaries, mouse
drivers, PIF profiles, or NTVDM configuration.

The tool discovers an old root only from a proven, internally consistent set
of absolute references.  It rewrites a reference only when the suffix names a
file or directory in the selected installed tree.  Each changed file and value
is reported.  Before a file is replaced, its preimage is retained beside it as
`<file>.BAK`; an existing backup is never overwritten.  Ambiguous roots,
malformed configuration, and unsupported binary formats are no-write failures.

Supported records are the Windows INI files, `PROGMAN.INI` group paths, and
Windows 3.1 `PMCC` Program Manager groups.  For a group, the tool validates
the item-offset table and its executable-path fields, appends replacement
strings, updates only those item pointers, and recomputes the format's
16-bit checksum.  Raw binary replacement is forbidden; an unsupported group
is a no-write failure.

Open `APPLY.CMD`, enter the installed root once when prompted, and leave the
window open to read the final exit code.  It records the names it changed in
`PATCH\PATH-REPAIR.MANIFEST`. `UNAPPLY.CMD` restores only those adjacent
backups, leaving unknown owner content in `PATCH` untouched. Neither command
needs a fixed drive or `HASH.EXE`.

The tool directory contains `APPLY.CMD`, `UNAPPLY.CMD`, and `WIN31PATH.EXE`.
The executable must remain beside the command files.
