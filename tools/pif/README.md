# PIF.EXE

`PIF.EXE` is a format-only tool. It does not launch programs or make
installation decisions.

```text
PIF.EXE show <file>
PIF.EXE create <file> --title T --program P --directory D --arguments A --config C --autoexec E [--close-on-exit] [--w386-max-xms N] [--w386-flags N] [--nt31-flags N]
PIF.EXE update <file> [--title T] [--program P] [--directory D] [--arguments A] [--config C] [--autoexec E] [--close-on-exit] [--w386-max-xms N] [--w386-flags N] [--nt31-flags N]
```

`show` writes the stored fields as `KEY=value` lines, and validates every
linked extension header plus a fixed-record checksum when the legacy file
supplies one. A zero checksum byte is the established no-checksum form and is
accepted; malformed records and bad nonzero checksums are rejected.
`create` writes the documented standard, Windows 386 and NT 3.1 records;
`update` preserves unknown extension records while editing only supplied fields.
It also preserves a legacy zero checksum byte; a pre-existing nonzero checksum
is recomputed after the edit.

`--close-on-exit` explicitly sets the original PIF `MSflags` CloseOnExit bit.
Use it only for a generated PIF whose calling script waits for the launched
session to end.  It is intentionally not the default for generic PIF files.

The three numeric options edit existing documented extension fields without
inventing a new PIF record: `--w386-max-xms` is a KiB value from 0 to 65535;
`--w386-flags` and `--nt31-flags` accept decimal or `0x`-prefixed DWORDs.
They are optional and leave the corresponding fields unchanged when omitted.
