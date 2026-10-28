# Win31 path repair

`win31-path` makes an already installed Windows 3.1 tree relocatable.  It is
independent from `win31-setup`: it never installs guest binaries, mouse
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

The tool directory contains `APPLY.CMD` and `UNAPPLY.CMD`. `path31.exe`,
`PIF.EXE` and `GRP.EXE` are consumed from `assets\release`, without duplicate
copies here. path31 only discovers the unique old root; it does not perform
the repair. Its source is `src/addon/path31/path31.c`; build it with
`src\addon\path31\BUILD.CMD output-exe` in a build-owned directory and publish
through `tools/build/Stage-AddonUtilityRelease.ps1 -Path31Build`.
