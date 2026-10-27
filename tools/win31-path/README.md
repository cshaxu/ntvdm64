# Win31 path repair

`win31-path` makes an already installed Windows 3.1 tree relocatable.  It is
independent from `win31-launch`: it never installs guest binaries, mouse
drivers, PIF profiles, or NTVDM configuration.

The tool first displays the proven old root and the requested new root. CMD
enumerates root-level INI files and changes only those containing the old root;
it makes an adjacent `.BAK` copy immediately before each replacement. The
released `GRP.EXE` owns PMCC Program Manager `.GRP` parsing/pointer/checksum
updates, while released `PIF.EXE` owns PIF root fields. An existing backup is
never overwritten. Ambiguous roots, malformed configuration, and unsupported
binary formats are no-write failures.

Supported records are root-level Windows INI files, PIF program/directory/NT
profile fields, and Windows 3.1 `PMCC` Program Manager groups. `GRP.EXE`
validates the item-offset table and executable-path fields, appends replacement
strings, updates only those pointers, and recomputes the format's 16-bit
checksum. Raw binary replacement is forbidden; an unsupported group is a
no-write failure.

Open `APPLY.CMD`, enter the installed root once when prompted, and leave the
window open to read the final exit code.  It records the names it changed in
`PATCH\PATH-REPAIR.MANIFEST`. `UNAPPLY.CMD` restores only those adjacent
backups, leaving unknown owner content in `PATCH` untouched. Neither command
needs a fixed drive or `HASH.EXE`.

The tool directory contains `APPLY.CMD`, `UNAPPLY.CMD`, and `WIN31PATH.EXE`.
The latter must remain beside the command files. `PIF.EXE` and `GRP.EXE` are
consumed from `assets\release`, not copied into this tool directory.
