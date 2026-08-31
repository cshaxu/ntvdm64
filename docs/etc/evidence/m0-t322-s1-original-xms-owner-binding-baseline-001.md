# M0 T322 S1 — Original XMS owner and binding baseline

## Original package

The selected source package is the complete mirror at `mvdm-host/xms.486`:
`xms.c`, `xmsdisp.c`, `xmsblock.c`, `xmsumb.c`, `xmsa20.c`, `xmsmisc.c`, and
the historical memory backends.  `xmsdisp.c` retains the original single
service table for A20, move/allocate/free/reallocate, query, UMB and INT15.

## Boundary disposition

- **Direct original:** dispatch, allocation bookkeeping, handle/lock rules,
  UMB list ordering/coalescing and register failure results remain in their
  original translation units.
- **Same-shaped binding:** `xms.c` selects `SAInitialize` with the original
  `xmsCommitBlock`, `xmsDecommitBlock`, `xmsMoveMemory` callback shape when
  `MVDM_XMS_SESSION_BACKEND` is selected.  The product graph selects that
  configuration on both x86 and x64.
- **Why direct historical backend is excluded:** `i386/xmsmem86.c` treats
  `ULONG` XMS addresses as host `PVOID`, commits/decommits the process with
  `NtAllocateVirtualMemory`/`NtFreeVirtualMemory`, and calls `RtlMoveMemory`
  on those addresses.  This conflicts with the selected SoftPC guest backing
  and is invalid for x64.
- **Mapped replacement:** `adapter-mvdm-host-out/softpc/mvdm_xms_memory.c`
  exports those original callback names/signatures.  It obtains only bounded,
  synchronous `session_guest_memory_acquire` leases, preserves address/span
  overflow checks and move order, and retains no native pointer after release.
- **A20:** original `xmsa20.c` retains its SAS calls and result order; the
  existing bounded A20 state write is the only guest-memory divergence.
- **UMB:** original `xmsumb.c` retains allocation/list semantics.  Its
  original `PVOID` interface is decoded as a numeric guest physical address
  by the existing UMB address binding, not passed as a host pointer.
- **INT15:** original `xmsNotifyHookI15` remains an XMS owner entry.  Its
  machine interrupt delivery is a later same-shaped SoftPC/CPU40 contract;
  S1 does not fabricate it.

## S1 conclusion

The package has a single, shared guest-memory implementation boundary and no
second XMS-local mapping-manager instance.  The next S can test the complete
dispatcher and its Direct/negative callback behavior, then separately decide
whether original UMB/INT15 machine prerequisites are presently executable.
