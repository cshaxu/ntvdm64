# M0 T323 S1 — Original DPMI/DPMI32 owner and binding baseline

## Package boundary

The two original packages have different product roles and must not be merged
because they share a name:

- `src/mvdm-host/dpmi32` is the host-side OpenNT provider. Its `sources`
  manifest names thirteen common C units (`debug`, `dpmi32`, `dpmiint`,
  `dpmiselr`, `buffer`, `data`, `int21map`, `modesw`, `register`, `savestat`,
  `stack`, `vxd`, `xmem`), plus the original x86-specific `i386/dpmi386.c`,
  `i386/dpmimem.c`, and `i386/dpmimisc.c`, and the historical RISC fallback
  pair `dpmimemr.c`/`dpmimscr.c`.
- `src/mvdm-host/dpmi` is the DOSX/DPMI guest assembly carrier (`dx*.asm`,
  include files, link definitions and guest build material). It is not a host
  static-library input. Its BOPs and stack layouts are a load-time guest
  contract for a later DPMI integration S, not an excuse to mix guest objects
  into the MSVC host graph.

The current selected product explicitly compiles the complete common provider
set with `dpmimemr.c` and `dpmimscr.c` under `LINKED_INTO_MONITOR`; it does not
select the kernel-VDM `CPU_30_STYLE` path. The three original i386 provider
units remain a required source comparison cohort: they cannot be silently
dropped merely because an x64 build can select the RISC-style fallback.

## Original dispatch and immediate owner map

`dpmi32.c` retains the original 25-entry `DpmiDispatchTable`, in service order:

1. descriptor entry; protected-mode switch; protected interrupt; fast BOP;
   DOSX init; application init; INT21 translation;
2. XMEM allocate, free, realloc; fault handler; memory info; in-use/no-longer-
   in-use; debug registers; table address; free-app-XMEM; PM-stack info;
3. VCD PM service; free-all-XMEM; INT IRET16/32; fault IRET16/32; unhandled
   exception.

The following are the only immediate external owner families reached by that
complete host package:

- **Original CCPU40/SAS mechanical interface:** register and segment macros,
  descriptor queries, mode switches, hardware-stack state, and
  `host_simulate`. They stay original source-shaped calls into the selected
  SoftPC machine; the DPMI package must not implement a second executor.
- **Session guest-memory mapping:** ten provider files call
  `Sim32GetVDMPointer`; there are no reached `Sim32FlushVDMPointer` or
  `Sim32FreeVDMPointer` calls in `dpmi32`. The raw alias is used for immediate
  reads/writes, pointer arithmetic, BOP-byte fetches, PM stack frames, IVT
  entries, DTA/FCB buffers and DOSX shared tables. It therefore cannot be
  replaced by a family-private mapper or a single durable x64 pointer cast.
  Every such reach must use the sole session mapping-manager/lease model, with
  source-shaped scoped copy or a narrowly registered mirror overlay where the
  original body requires a longer live span.
- **Monitor/VDM_TIB:** `dpmiint.c` consumes the original-layout per-thread
  `VdmTib` for PM stack and fault/interrupt records. The existing monitor
  adapter is the correct location for that TLS layout binding. It currently
  proves the layout and session binding, not a replacement kernel VDM.
- **Kernel VDM control contract:** `dpmiint.c` calls
  `NtVdmControl(VdmSetInt21Handler, ...)`. The original function and request
  layout remain evidence; modern Windows has no public equivalent. The current
  monitor adapter only admits `VdmQueryDir`, so `VdmSetInt21Handler` is a
  named source-shaped unavailable/monitor-recovery edge—not an allowed
  fabricated success.
- **DOS/DEM/COMMAND and Win32 capability calls:** `int21map.c`, `buffer.c`
  and XMEM paths preserve their original call order. Their invoked service
  owners remain the existing original DEM/COMMAND package and the named
  adapter-mvdm-host-out Win32 facade; DPMI does not become a duplicate DOS
  provider.

## Current mirror divergences and disposition

- `MVDM-HOST-DIV-018` in `dpmiint.c` replaces only the historical CX:DX host
  pointer publication with a session host-resource mapping ID; the original
  register shape and failure result remain. Its future consumer must resolve
  the ID through the same session, never as a raw address.
- `MVDM-HOST-DIV-012` in `xmem.c` and `MVDM-HOST-DIV-139` in `dpmidata.h` are
  existing mirror/build divergences. S1 records them for the next S; it does
  not validate their runtime semantics by a merely successful product link.
- Direct static composition is already demonstrated by the selected product
  graph on x86 and x64. That establishes source selection only. It does not
  make raw `Sim32GetVDMPointer` arithmetic, `VdmSetInt21Handler`, protected
  exception delivery, or DOSX re-entry runtime-complete.

## S1 conclusion and next recovery order

The first executable recovery seam is not a new DPMI service. It is the shared
source-shaped guest-span contract for the ten `Sim32GetVDMPointer` users,
followed by the original monitor/TIB control edge and only then the original
dispatcher's protected-mode and DOSX transitions. The 25 dispatcher entries
remain one owner package; trace order may verify the package after those
cohorts close but may not select isolated entries.
