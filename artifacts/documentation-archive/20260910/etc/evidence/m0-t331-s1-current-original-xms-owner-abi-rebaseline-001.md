# M0 T331 S1 — Current original XMS owner and ABI rebaseline

Date: 2026-08-31

## Source and selection result

The selected XMS package is the original `xms.486/sources` set:
`xms.c`, `xmsa20.c`, `xmsblock.c`, `xmsdisp.c`, `xmsmisc.c`, and `xmsumb.c`.
Its shared original allocation support is `mvdm-host/suballoc`, which remains
in the canonical MVDM host mirror rather than a separate live support
component.

The selected source baseline is the matching OpenNT-4.5
`nt/private/mvdm/xms.486` package. The original `xmsdisp.c` table and
dispatcher, `xmsmisc.c`, and the allocation/free/reallocation/query bodies
remain byte-identical. The complete current entry/callback disposition is in
[the S1 XMS owner ledger](m0-t331-s1-xms-owner-abi-rebaseline-001.tsv).

The only live mirror divergences are registered and bounded:

- `MVDM-HOST-DIV-132`: `xms.c` selects the same original `SAInitialize`
  callback branch on both host architectures rather than treating host `i386`
  spelling as permission to use direct process pointers.
- `MVDM-HOST-DIV-010`: the historical retained `pHimemA20State` pointer is
  represented only as its original AX:BX guest location and written through a
  fresh session lease by a private XMS overlay.
- `MVDM-HOST-DIV-077`: `xmsMoveBlock` retains its original descriptor order,
  forward-copy rule and AX success result while its direct aliases are resolved
  by the same-shaped mapping-manager backend.
- `MVDM-HOST-DIV-078`: the legacy UMB `PVOID` carrier remains an Intel numeric
  physical value; the private codec rejects an unrepresentable value before
  the original paragraph/list logic, without making it a native host pointer.

`i386/xmsmem86.c` remains a complete original evidence file, but it is not a
runtime translation unit. Its `ULONG -> PVOID` implementation would expose
unbounded process addresses even in the x86 build. The selected adapter owns
only its three same-named allocation/move callbacks and uses the existing
session guest-memory mapping-manager lease. It does not own XMS policy or
SubAlloc behavior.

## Ownership and retired-route sweep

`nt_msscs.c -> XMSInit` remains the original startup path and
`nt_bop.c -> XMSDispatch` remains the original BOP path. A20 transitions are
original XMS/SAS calls; UMB allocation is original XMS list logic over the
original `ReserveUMB` provider; INT15 notification remains the original
`UpdateKbdInt15` call. None is moved into the adapter.

The current `New-T310OriginalSoftpcNinja.ps1` source manifest includes the six
original XMS files, original `mvdm-host/suballoc`, the two narrow private
overlays and `mvdm_xms_memory.c`. Its forbidden-input list excludes `src.old`,
`bochs-core` and `adapter-bochs`. The live XMS source/adapter paths contain no
Bochs include or call. Historical Bochs-only fixtures and scripts remain under
indexed legacy material and are not T331 acceptance inputs.

## S2 admission result

S2 may now rebind only the above original/adapter boundary. In particular it
must prove the current callback ABI against the actual CPU40/session memory
provider and remove a duplicate shim only if an original XMS body takes over
the same selected contract. It must not create an XMS result from the prior
startup trace, nor reopen CPU/FDC/ROM work.
