# Win95 path migration

`APPLY.CMD <installed-Windows-95-directory>` discovers the unique `SystemRoot`
in `SYSTEM.DAT`, then rewrites only supported CREG hives, INI files, PIFs,
Program Manager groups and writable classic `.LNK` targets below that tree.
It refuses a root or any descended reparse point, malformed/ambiguous input,
stale candidate, or pre-existing recovery copy before replacement.

Every changed file is prepared separately, then recorded with its installed
SHA-256 in `PATCH\\W95PATH.MANIFEST`. Byte-identical candidates are discarded
without replacing the original. `UNAPPLY.CMD` validates every journal path,
backup and installed identity before making any change; it refuses rather than
overwriting a file edited after migration. The original is retained beside the
file as `.W95PATH.BAK`.

`WIN95PATH.EXE` must be beside the command wrappers. Released `PIF.EXE`,
`GRP.EXE` and `LNK.EXE` are read from `assets/release`; none are copied into a
guest tree. This is an offline recovery tool, not a Run16/NTVDM runtime
dependency and not a general Win95 compatibility claim.
