# M0 T367 S2 — Original DOS read-to-provider route

Status: closed static/link route proof.  This is not a guest EXEC or child
completion claim.

## Reached source contract

The unchanged guest `handle.asm:$READ` procedure performs its normal-file
operation by resolving JFN/SFT state, loading `sf_NTHandle`, selecting
`SVC_DEMFASTREAD` only when the original fast predicate permits it, and then
using unchanged `HRDSVC SVC_DEMREAD` at line 466 when that fast request
declines or fails.  Its immediate post-service instructions advance the SFT
position by returned `AX`; its error branch preserves the original DOS system
call error return.

The original visible provider table remains:

```text
guest handle.asm:$READ
  -> SVC_DEMREAD (50:16)
  -> mvdm-host/dos/dem/demdisp.c:demDispatchTable[SVC_DEMREAD]
  -> mvdm-host/dos/dem/demhndl.c:demRead
  -> original ReadFile / error order
  -> one bounded session guest-memory write lease
  -> AX byte count + CF success, or original error + CF failure
```

The `50:42` fast entry remains deliberately non-authoritative for normal
EXEC: it either completes its narrow safe regular-file contract or returns
CF-set, where unchanged `$READ` immediately enters the original `50:16` route.
It cannot replace the original normal read provider.

## Formal graph evidence

The existing CPU40/x86 formal Ninja graph was queried without rebuilding:

```text
original-softpc-process.exe
  -> original-mvdm-dem.lib
       -> obj/dem/demdisp.obj
       -> obj/dem/demhndl.obj
            -> src/mvdm-host/dos/dem/demhndl.c
```

Thus the final product already links the imported original DEM table and
`demRead` body.  No adapter-local or legacy v1 provider is selected as a
replacement for the `50:16` route.

## Result

There is no missing host file-read implementation to invent for the selected
guest `$Exec` cohort.  T367 must next verify the existing formal product link,
then use a single immutable original DOS executable observation to determine
whether COMMAND reaches this already-linked guest `$Exec` chain.  The selected
observation workload must be an existing original `MEM.EXE` artifact, not a
synthetic `.COM` program or mutated guest media.
