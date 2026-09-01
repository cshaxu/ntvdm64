# M0 T355 S6 — CONFIG command first-store observation

Date: 2026-09-01

## Implementation

S6 added a default-off hook after the original CPU40 `c_sas_store` byte write.
It receives the already-calculated linear guest address and byte, has no CPU
or guest-state capability, and only matches the selected fixed-image address
`0x914e6` (`CS=8E08`, map offset `3466`).  The adapter observer does not take
a guest pointer, use a mapping lease, translate an address, or write memory.

The focused x86 fixture proved the disabled path creates no report and the
enabled path copies only the configured address/value.  The formal CPU40/x86
Ninja target rebuilt and linked successfully.

## One admitted observation

The fixed non-debug `O:\\ntvdm` container produced:

```text
MVDM-CONFIG-COMMAND-STORE linear=914E6 value=00 state=copied
MVDM-CONFIG-DONE al=00 cs=8E08 pass=02 command=\COMMAND.COM command-state=copied
```

The unchanged subsequent original DEM terminal remained `C:\\COMMAND.COM` /
`ERROR_FILE_NOT_FOUND`.  The watchdog timeout is not interpreted as a repair
target.

## Result and bounded correction

The first matching store is an original zero write.  It proves the selected
address binding is live, but a first-write-only observer cannot distinguish
that initialization from a later `trys` write of `S`.  It therefore does not
classify `trys` reachability and cannot justify a parser or CPU repair.

The sole successor is a bounded observation correction: record at most eight
post-store bytes at the same already-proven address.  It remains default-off,
does not generalize tracing, and requires one new fixed-container observation.
