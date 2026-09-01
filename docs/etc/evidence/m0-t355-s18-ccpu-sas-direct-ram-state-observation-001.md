# M0 T355 S18 — CCPU SAS direct-RAM state observation

## Fixed container and scope

S18 reused the immutable `O:\ntvdm` package and the existing console-owning,
non-debug observer.  The only production addition was a default-off,
child-report scalar record immediately before original CPU40
`ccpusas4.c::c_GetPhyAdd` takes its existing normal `Start_of_M_area + addr`
fallback.  It copied no guest or host pointer, made no mapping decision, and
did not change any CPU, SAS, BOP or DEM result.

Exactly one fixed-container execution was admitted.  Its durable records are:

```text
O:\ntvdm\m0-t355-s18-cpu40-sas.txt
O:\ntvdm\m0-t355-s18-cpu40-sas.txt.exception.txt
O:\ntvdm\m0-t355-s18-direct-ram.txt
```

The formal CPU40/x86 product was rebuilt after correcting the report line
terminator only.  It was not executed again.

## Copied scalar result

The default-off record immediately preceding the fault reported:

```text
requested=00110000
direct=00110000
wrap=FFFFFFFF
length=01000000
base=038D0000
resolver=miss
state=copied
```

The existing original exception reporter then recorded access target
`0x039e0000`.  This is exactly:

```text
0x038d0000 + 0x00110000 = 0x039e0000
```

Thus the access did not come from an external physical-page resolver or EMS
numeric alias.  It is the unchanged original normal RAM calculation.

## Source interpretation

`Length_of_M_area` is `0x01000000`, so the requested physical address is in
the selected 16 MiB span.  Original `nt_mem.c::InitIntelMemory` reserves that
full span but initially commits only the conventional-memory/A20 region; later
XMS and EMS allocation callbacks own additional commits.  `0x00110000` is the
first byte beyond the historical 1 MiB plus 64 KiB initial range.

The selected formal graph defines `MVDM_XMS_SESSION_BACKEND`.  Consequently
original `xms.486/xms.c::XMSInit` selects original `xmsmemr.c` callbacks,
whose `xmsCommitBlock` calls `sas_manage_xms`.  The currently selected
CPU40 `softpc.new/host/src/stubs.c::sas_manage_xms` is only a diagnostic
success stub: it reports success but does not commit the reserved Intel range.

The original caller is retained in `nt_msscs.c::scs_init`, which invokes
`XMSInit` after DEM initialization.  The immediate next owner is therefore
the original XMS allocation/commit lifecycle, not a CCPU dereference repair.

## Disposition

S18 closes with one unambiguous owner transfer.  S19 must audit the original
`XMSInit -> SAInitialize -> xmsCommitBlock -> sas_manage_xms` lifecycle,
including selected compilation conditions and the actual original allocation
callers, before any recovery is admitted.  It must not pre-commit the full
M-area or force `c_GetPhyAdd` to another path.
