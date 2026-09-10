# M0 T355 S4 — loaded-NTIO ConfigDone state observation

Date: 2026-09-01

## Question

S3 proved the first ConfigDone recorder used file-layout map segments as live
guest segments.  S4 corrects only that recorder binding: the original
`MS_bop_E` supplies its already-live `CS`, while the selected map continues to
supply only the `multi_pass_id` and `commnd` offsets.

## Implementation and focused verification

`MS_bop_E` still performs its original sequence:

```text
getAL() == 0 -> UMBNotify(0) -> demDasdInit() -> return
```

The registered `MVDM-HOST-DIV-183` call follows those original operations.  It
receives only `getCS()`, takes one one-byte and one 64-byte read lease, copies
into host-local storage, releases both leases, and returns.  No mapper,
pointer retention, BOP dispatch, CPU/guest write, or provider result changes.

The isolated fixture passed after the correction:

- default-off execution: zero guest reads, zero writes, no report;
- enabled execution: exactly two reads, zero writes, copied `CS`, pass and
  command record;
- no lease survives the helper call.

The CPU40/x86 formal Ninja graph incrementally rebuilt the changed host and
adapter units and linked `original-softpc-process.exe` successfully.

## One admitted observation

The sole S4 non-debug, console-owning observation used the unchanged staged
media at `O:\ntvdm` and the formal S4 product.  It reached the original
notification and produced:

```text
MVDM-CONFIG-DONE al=00 cs=8E08 pass=02 command=\COMMAND.COM command-state=copied
```

The same durable observation later recorded the unchanged original permanent
open and failure:

```text
MVDM-DEM-OPEN phase=0 ... path=C:\COMMAND.COM
MVDM-DEM-OPEN phase=2 status=00000002 ax=0002 cf=1 path=C:\COMMAND.COM
```

The observer watchdog timed out after eight seconds; this is not interpreted
as a repair target.  It is sufficient for the selected state classification.

## Interpretation

At the source-defined boundary after all CONFIG passes, the loaded NTDOS
`commnd` buffer still holds its original default.  Therefore the generated,
valid `shell=` text did not produce a persistent `trys` assignment before
`dfil`.  This rules out both the path-length hypothesis and later-only
overwriting of a valid configured command.

The remaining owner cohort is the original in-guest
`organize -> org_count/confbot -> multi_pass_id=1 -> ordinary dispatch ->
trys` path.  It must be attributed as NTDOS/CPU execution behavior; no DEM,
path projection, host COMMAND workaround, or BOP return fabrication is
allowed.

## Next disposition

Select a static source/selected-binary execution-cohort audit before adding
another observer.  It must identify the exact source instruction/data
contracts that carry a reorganized `S` record through pass one, determine
whether the selected CPU40/SoftPC mechanical path can preserve them, and bound
one source-shaped next observation or repair owner.  It does not authorize a
guest rebuild or parser edit.
