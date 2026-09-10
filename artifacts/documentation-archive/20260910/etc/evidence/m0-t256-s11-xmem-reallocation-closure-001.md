# M0 T256 S11 — XMEM Reallocation Closure

## Source-shaped transaction

The bx-vdm XMEM record adapter now retains `i386/dpmimem.c`'s allocation
shape without retaining its host-pointer model:

- equal or smaller 4 KiB page coverage keeps the allocation address and
  updates the logical requested length;
- growth first reserves a new 64 KiB-aligned ordinary-RAM block;
- it copies the former logical length through bounded checked-RAM transfers;
- it releases the former private mantle allocation only after the copy; and
- only then replaces the existing record's address, size and mantle ID. The
  bx-vdm record ID remains unchanged.

If allocation or copy/release fails, the old record remains selected. The
release-failure rollback is a deliberate safety tightening over the historic
unchecked `NtFreeVirtualMemory` result; it is unobservable in the normal
successful source path and prevents an inaccessible reservation leak.

## Verification

Fresh graph: `build/M0-T256-S11/formal-r1`.

The focused XMEM fixture covers growth with copied bytes, stable record ID,
same-page shrink, stale-ID rejection, owner sweep, full reset and minimal
machine cleanup. No `53:xx` ingress, host virtual allocation, raw pointer,
LDT or paging path is enabled.
