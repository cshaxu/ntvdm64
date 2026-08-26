# M0 T271 S4 P3 — XMS source-unit admission audit

## Question

Which files in the pinned OpenNT `xms.486` package are active product source
for this x64 Bochs composition, and which must remain same-shaped seams?

## Findings

- `xms.c`, `xmsdisp.c`, `xmsa20.c`, `xmsblock.c`, `xmsmisc.c`, `xmsumb.c`,
  `suballoc.c`, and `suballcp.h` are the admitted XMS provider package.  The
  original 12-slot `apfnXMSSvc` table remains the only active service ordering.
- `xmsmemr.c` is the RISC implementation.  It calls `sas_manage_xms`, derives
  a host pointer from `GetVDMAddr(0,0)`, and performs raw `RtlMoveMemory`.
  It is not selected by the original i386 `sources` file and is not admissible
  in an x64 composition.
- `i386/xmsmem86.c` is the original selected x86 implementation.  It calls
  private `NtAllocateVirtualMemory`/`NtFreeVirtualMemory` against a host VDM
  address and moves raw host pointers.  It cannot be directly linked without
  reintroducing the removed NT4 product-memory identity.
- The active `adapter-softpc` facade retains its three source-shaped entries
  `xmsCommitBlock`, `xmsDecommitBlock`, and `xmsMoveMemory`, while performing
  bounded Bochs checked-RAM operations.  It is the required recovery-ladder
  seam, not a project-defined XMS allocator or dispatcher.
- `xms.h` is represented by the active compatibility/header closure rather
  than copied wholesale because its historical build-mode and NT4 include
  branches do not form an independently composable header unit.  Its service
  constants, provider names, `XMSUMB`, `PFNSVC`, and reached allocator ABI are
  retained.  The header's remaining non-mirror placement is an S4/S5 layout
  follow-up; it does not change the active source table or provider ordering.

## Disposition

No new XMS provider body is needed for these two memory callback files.  A
future physical extended-memory aperture may improve the adapter's bounded
mechanics, but must be an `adapter-softpc`/machine capability task; it is not
permission to import raw NT4 virtual-memory calls or introduce an XMS-local
memory manager.
