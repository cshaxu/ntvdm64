# M0 T331 — Original SoftPC XMS owner package

## Purpose

Recover the selected original `mvdm-host/xms.486` package as one CPU40/SoftPC
owner package.  The task keeps the original XMS allocator, dispatch, A20,
move, UMB and INT15 policy in its original files.  It replaces only the
historical direct-host-address backend with the existing same-shaped
`adapter-mvdm-host-out/softpc` mapping-manager binding.

This is not a trace-selected `52:xx` patch task.  A guest trace may only
verify a completed XMS package.

## Package boundary

The selected original source set is:

```text
mvdm-host/xms.486/xms.c
mvdm-host/xms.486/xmsa20.c
mvdm-host/xms.486/xmsblock.c
mvdm-host/xms.486/xmsdisp.c
mvdm-host/xms.486/xmsmisc.c
mvdm-host/xms.486/xmsumb.c
mvdm-host/suballoc/*
```

`xms.486/i386/xmsmem86.c` is retained as original source evidence but cannot
be the active backend: it converts guest-sized `ULONG` values directly to host
pointers.  Its three original exported contracts (`xmsCommitBlock`,
`xmsDecommitBlock`, `xmsMoveMemory`) must instead be supplied with the same
names, parameters, ordering and failure direction through the session
guest-memory mapping manager and the CPU40 SoftPC mechanical boundary.

The task owns neither a new allocator, a guest loader, an invented XMS
provider, an FDC/ROM/CPU workaround, kernel VDM, MONITOR, Bochs, nor a raw
host-pointer ABI.  A20, UMB and INT15 changes remain original XMS policy over
original SoftPC machine interfaces; a missing machine effect transfers to its
named original machine owner rather than becoming a service-success shortcut.

## S plan

### S1 — Current original XMS ownership and ABI rebaseline

Re-read every selected declaration, definition, dispatch-table entry, startup
caller and machine callback against the current CPU40 graph.  Reconcile the
prior 25-row XMS ledger with the retired-Bochs removal, classify every old
shim/overlay as retain, replace with original source, migrate, or delete, and
name the exact machine prerequisite for A20, UMB and INT15.

### S2 — Original-body binding recovery

Use the S1 result to connect all composable original XMS bodies and original
SubAlloc support through the smallest same-shaped adapter/mapping seams.  Do
not move allocator or XMS policy into an adapter.  Remove a stale project shim
where an original body now supplies the same contract.

### S3 — XMS package contract matrix

Exercise the original initialization, dispatch, allocation/free/reallocation,
move, A20, UMB and INT15 success/failure contracts against the selected
CPU40/session machine boundaries.  A source-defined unavailable result is
acceptable only where S1 identified a missing original machine prerequisite.

### S4 — Formal product closure

Rebuild the selected original SoftPC product independently for Win32/x86 and
x64.  Run at most one fixed-container x86 observation only if S2 changed a
reached package path; it confirms a package result and does not select the
next repair.

## Exit

Every reached XMS declaration, body, table entry and callback has one
source-shaped disposition; selected original bodies link through a minimal
same-shaped seam; no Bochs-era route remains in the live XMS graph; and both
formal x86 and x64 links pass.  Any non-runnable A20/UMB/INT15 behavior is
recorded as an exact original machine-owner transfer, not as an XMS success
claim.
