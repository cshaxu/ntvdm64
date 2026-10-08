# Win31 path repair

`win31-path` makes an already installed Windows 3.1 tree relocatable.  It is
independent from `win31-launch`: it never installs guest binaries, mouse
drivers, PIF profiles, or NTVDM configuration.

The tool discovers an old root only from a proven, internally consistent set
of absolute references.  It rewrites a reference only when the suffix names a
file or directory in the selected installed tree.  Each changed file and value
is reported.  Ambiguous roots, malformed configuration, and unsupported binary
formats are no-write failures.

Supported records are the Windows INI files, `PROGMAN.INI` group paths, and
Windows 3.1 `PMCC` Program Manager groups.  For a group, the tool validates
the item-offset table and its executable-path fields, appends replacement
strings, updates only those item pointers, and recomputes the format's
16-bit checksum.  Raw binary replacement is forbidden; an unsupported group
is a no-write failure.

Open `APPLY.CMD`, enter the installed root once when prompted, and leave the
window open to read the final exit code.  The command file itself has no
fixed-drive dependency.

The self-contained tool directory contains `APPLY.CMD` and `WIN31PATH.EXE`.
They must remain beside one another.
