# M0 T355 S8 — CONFIG temporary-file DEM-read observation

## Scope

This record closes the bounded S8 observation of the original
`sysconf.asm::doconf` temporary-CONFIG read path.  It records only scalar
state at the original `demRead` boundary.  It does not copy CONFIG content,
retain guest pointers, alter a guest/DEM result, or change the immutable
`O:\ntvdm` media container.

## Source contract

The original `doconf` flow opens the temporary CONFIG file, seeks to discover
its length, allocates `confbot`, seeks back to the start, then invokes DOS
read.  Its `cmp cx, ax` / `jz getcom` branch admits parsing only after a full
read.  `demRead` is the reached original DEM implementation: on success it
clears carry and returns the completed byte count in `AX`.

## Implementation and focused proof

- `dos/dem/demhndl.c::demRead` emits default-off, copied scalar entry,
  success and failure records through `mvdm_softpc_record_dem_read`.
- The observer is enabled solely by `MVDM_DEM_READ_REPORT_PATH`; production
  behaviour is unchanged when it is absent.
- The focused S8 fixture verifies the report helper's entry/success record
  formatting and does not need a guest payload.
- Formal CPU40/x86 Ninja rebuilt `demhndl.c`, `original-mvdm-dem.lib` and
  `original-softpc-process.exe` successfully.  Existing C4701/C4703 warnings
  for the original `demhndl.c` `hFile` path remain visible and were not
  suppressed or changed by S8.

## Single fixed-container observation

The one permitted run used the formal CPU40/x86 product in the unchanged
`O:\ntvdm` stage.  The durable records are:

```text
MVDM-DEM-OPEN phase=0 ... path=C:\USERS\NEKO\APPDATA\LOCAL\TEMP\SCSE58C.TMP
MVDM-DEM-OPEN phase=1 ... ax=0000 cf=0 path=C:\USERS\NEKO\APPDATA\LOCAL\TEMP\SCSE58C.TMP
MVDM-CONFIG-DONE al=00 cs=8E08 pass=02 command=\COMMAND.COM command-state=copied
MVDM-DEM-OPEN phase=0 ... path=C:\COMMAND.COM
MVDM-DEM-OPEN phase=2 ... ax=0002 cf=1 path=C:\COMMAND.COM
```

No `MVDM-DEM-READ` record was emitted, although the temporary-file open is
observably successful.  This is a valid negative boundary result: the selected
run does **not** reach original `demRead`, so it cannot be classified as a
short/full CONFIG read.  The established source-owned interval is instead
between successful temporary-file open and the original read call: the
`doconf` seek/allocation/pre-read cohort.

## Conclusion

S8 is closed.  It rejects a DEM `ReadFile` repair and any claim that `CX == AX`
failed.  The next bounded cohort is original `doconf` post-open seek and
pre-read progression, beginning with the original file-pointer service; it
must be separately admitted before another runtime observation.
