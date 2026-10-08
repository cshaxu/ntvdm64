# PIF.EXE

`PIF.EXE` is a format-only tool. It does not launch programs or make
installation decisions.

```text
PIF.EXE show <file>
PIF.EXE create <file> --title T --program P --directory D --arguments A --config C --autoexec E
PIF.EXE update <file> [--title T] [--program P] [--directory D] [--arguments A] [--config C] [--autoexec E]
```

`show` writes the stored fields as `KEY=value` lines, and validates every
linked extension header plus a fixed-record checksum when the legacy file
supplies one. A zero checksum byte is the established no-checksum form and is
accepted; malformed records and bad nonzero checksums are rejected.
`create` writes the documented standard, Windows 386 and NT 3.1 records;
`update` preserves unknown extension records while editing only supplied fields.
It also preserves a legacy zero checksum byte; a pre-existing nonzero checksum
is recomputed after the edit.
