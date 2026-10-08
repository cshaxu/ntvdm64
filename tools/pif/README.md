# PIF.EXE

`PIF.EXE` is a format-only tool. It does not launch programs or make
installation decisions.

```text
PIF.EXE inspect <file>
PIF.EXE verify <file>
PIF.EXE create <file> --title T --program P --directory D --arguments A --config C --autoexec E
PIF.EXE set <file> [--title T] [--program P] [--directory D] [--arguments A] [--config C] [--autoexec E]
```

`inspect` writes the stored fields as `KEY=value` lines. `verify` validates
the fixed-record checksum and every linked extension header before succeeding.
`create` writes the documented standard, Windows 386 and NT 3.1 records;
`set` preserves unknown extension records while editing only supplied fields.
