# M0 T355 S7 — CONFIG command write-sequence observation

Date: 2026-09-01

## Correction and verification

The S6 helper was corrected to append at most eight post-store values for the
same selected linear address; it remains absent unless both child-only report
path and selected address environment values are supplied.  It still runs
after the unchanged original `c_sas_store` write and owns no guest mapping,
pointer, CPU-control or write capability.

The focused x86 fixture verifies a disabled no-report path and an enabled,
ordered two-write record.  The selected CPU40/x86 formal product rebuilt and
linked successfully.

## One admitted observation

The single fixed `O:\\ntvdm` observation produced only:

```text
MVDM-CONFIG-COMMAND-STORE ordinal=1 linear=914E6 value=00 state=copied
MVDM-CONFIG-DONE al=00 cs=8E08 pass=02 command=\COMMAND.COM command-state=copied
```

No later write at that same address occurred before ConfigDone.  In original
`sysconf.asm::trys`, the first filename-store instruction writes the `S`
indicator at this exact `commnd` address.  Therefore `trys` did not execute
in the selected run.  This is an execution fact; it does not by itself assign
fault to CPU40, `organize`, or the input read.

## Next disposition

The immediate unresolved source predecessor is `doconf`'s original full-read
test: it enters `getcom` only when its DOS read returns `AX == CX`.  The next
whole cohort is a default-off original DEM read-result observation that copies
only its existing scalar request/result state.  It must establish whether the
configuration temporary file reaches that original equality branch before any
parser or CPU repair is considered.
