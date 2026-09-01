# M0 T353 S2 — NTDOS `ProcessConfig` continuation boundary audit

## Original continuation

The `54:0C` caller is original `ProcessConfig` in `sysinit1.asm`.  Once the
original BOP return has consumed its service byte, it restores its saved DS and
calls original near `doconf` in `sysconf.asm`.

`doconf` first asks DOS for the switch character, then opens the filename just
written by `cmdGetConfigSys`.  It uses original DOS INT 21h open, seek, read
and close contracts to copy the temporary expanded CONFIG bytes into the
`confbot` allocation.  It normalizes the terminal bytes and begins the first
parser pass.  An open failure sets `multi_pass_id` to 11 and returns; a read or
close failure takes the original `badfil` route.  No new host-side fallback is
selected.

## Boundary selection

The source-defined S3 cohort is deliberately bounded to:

```text
ProcessConfig return from 54:0C
  -> restore DS -> doconf
  -> DOS INT21 open/seek/read/close of the expanded temporary CONFIG
  -> organize/getchr first parser handoff
```

It includes the existing original DOS/DEM file boundary and CPU40/SAS
mechanics, because they are the direct implementation of these guest INT 21h
operations.  It excludes later `multi_pass` device, INSTALL/EXEC, UMB and
configuration-done work; those are separate source owners and cannot be
smuggled in simply because they occur later in `sysinit1.asm`.

`ECHO`-selected `54:09` console setup is a conditional parser branch, not an
always-reached prerequisite, and is excluded from the first observation
cohort.

## Result

No production source, mapping, media, or machine behavior changes in S2.
The edge-by-edge source/disposition record is
[`m0-t353-s2-processconfig-continuation-ledger.tsv`](../operations/m0-t353-s2-processconfig-continuation-ledger.tsv).
S3 may freeze the current product and use one durable, fixed-container
observation to attribute this already-selected original cohort; it may not
repair a reached leaf service.
