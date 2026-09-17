# M0 T420 S19 Guest DPMI/DOSX Package Recovery

## Package identity and selection

`src/mvdm/dpmi` is a **load-only 16-bit guest package**, not a host library.
Its original `makefile` selects the `486` DOSX build, twenty-two object inputs
and `dosxi.lnk`; the result is `486/dosx.exe`, an EXEPACKed 16-bit Windows
DOS extender. None of its source, object, map, symbol or executable is an
input to the x86 host link graph.

The primary OpenNT tree contains 58 files under this path. All 58 have exact
current counterparts with no content differences. The current tree has 23
additional `486` build products: `dosx.exe` and 22 `.OBJ` inputs. The DOSX
executable is a one-sided OpenNT-4.5 member; all 23 products are byte-identical
to their recorded OpenNT-4.5/OpenNT-src-2 provenance. This is the permitted
guest/intermediate retention rule, not a locally rebuilt or patched DOSX.

The selected runtime file is also exact: `src/mvdm/dpmi/486/dosx.exe` and
`O:\winnt\system32\DOSX.EXE` both hash to
`c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`.

## Activation and return contract

The complete guest-to-host boundary is source-shaped and finite.

| Guest source | Original guest action | Host receiver | Disposition |
| --- | --- | --- | --- |
| `486/dxboot.asm` | Builds DOSX GDT/LDT source tables and emits `DPMIBOP PassTableAddress` (15). | `dpmi32.c::DpmiPassTableAddress` | S18 owns descriptor carrier; S19 changes no guest code. |
| `486/dxstrt.asm` | Emits `FBOP BOP_DPMI,InitDosx` (4), then installs fault/interrupt callbacks and PM-stack data. | `nt_bop.c::MS_bop_3` → `DpmiDispatch` | Original `C4 C4 53` BOP number and following minor byte preserved. |
| `486/dxmain.asm` | Emits `XlatInt21Call` (6) for protected-to-real DOS entry/exit translation. | `DpmiDispatchTable[6]` | Original guest DOS-call translation remains guest-owned. |
| `486/dxstrt.asm` | On final child return: `TerminateApp` (16), `FreeAllXmem` (19), `DpmiNoLongerInUse` (13), then real-mode cleanup and DOS `4Ch`. | `DpmiDispatchTable` | Guest frees its own XMS/selector/interrupt state; worker/broker lifecycle is not substituted. |

`src/mvdm/inc/bop.inc` defines `BOP_DPMI = 53h` and its original three-byte
`C4 C4 <number>` encoding. `MS_bop_3` invokes only `DpmiDispatch`; the latter
reads the unchanged guest minor byte at CS:IP, advances IP by one and calls its
original dispatch table. There is no second parser, launcher, guest-command
replacement or adapter-owned DPMI policy.

## Footprint and disposition

- Mirror guest source/build/media changes: **0 added, 0 removed**.
- Overlay changes: **0 added, 0 removed**.
- Adapter/autonomous production changes: **0 added, 0 removed**.
- The historical `.OBJ`, `.MAP`, `.SYM` and `DOSX.EXE` are immutable guest
  build evidence/media. They remain outside host compilation and product EXE
  linkage; deleting them would reduce auditability without reducing runtime
  code.

The direct host-DPMI descriptor and active CPU40 transition boundary was
already recovered in S18. S19 neither reopens it nor claims that an x86
kernel-VDM monitor could be restored by compiling guest assembly.

## Verification

- A fresh independent x86 formal graph at
  `build/M0-T420/S19/formal-x86-001` compiled and explicitly linked
  `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe` and `VDMREDIR.dll`.
  The `cpu40-descriptor-domain-fixture.exe` returned zero.
- The newly deployed program hashes are
  `b0e980e754838982e6c56f0a4abb6d60b81366f5fcfa5721f75332494753f022`
  (`run16`), `538f9e8b514616665238b66a5e0fccfc07aaa3839579df30cc6d0f4d1fe82b4c`
  (`basesrv`), `757c68f8b995626747cb5487fe5205d272d5d7cc2e4e780f66c09d6dee2b1f42`
  (`ntvdm`) and `9374d6b18c01793baaa9b983cae76b06a907bead8c94725767a95588176280c6`
  (`dtmgr`).
- In a real Console, deployed `run16.exe MEM.EXE` and
  `run16.exe COMMAND.COM /C MEM.EXE` both emitted normal DOS MEM output and
  returned. The established owner-accepted interactive COMMAND/MEM/EDIT route
  is unchanged; S19 contains no Console, COMMAND, worker, broker or guest
  executable mutation.

S20 owns the original `vdmredir` DLL as the next package. It must separately
prove its handle, guest-copy, stream and redirection lifecycle; it cannot use
S19's exact DOSX identity as redirector evidence.
