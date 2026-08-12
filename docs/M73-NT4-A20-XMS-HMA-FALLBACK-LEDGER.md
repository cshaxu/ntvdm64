# M73 NT4 A20, XMS, and HMA Fallback Ledger

Status: source-derived HMA-path classification, 2026-08-08.

## Scope

This record distinguishes three different concepts that the historical source
places close together: machine A20 address semantics, an XMS service provider,
and the optional policy to relocate DOS/BIOS code into the HMA. It does not
adopt the historical XMS allocator, BOP, or product configuration policy.

## Sources

- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msbio1.asm`
- `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysconf.asm`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_msscs.c`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`
- `src/opennt/base/mvdm/xms.486/xms.c`
- `src/opennt/base/mvdm/xms.486/xmsa20.c`
- `src/opennt/base/mvdm/xms.486/xmsdisp.c`
- `src/opennt/base/mvdm/inc/XMSSVC.INC`

## Guest HMA Decision

`LoadDOSHiOrLo` calls `TryToMovDOSHi`; carry set takes the explicit `LdngLo`
path, emits the historical low-memory message, moves DOS low, reinitializes
its segment, and records `runhigh = 0`. A successful `MovDOSHi` instead moves
BIOS and DOS code to `FFFFh`, saves the remaining HMA pointer, and records a
high-running state.

`AllocHMA` is deliberately failure-tolerant:

1. `INT 2Fh/AX=4300h` must report XMS installed (`AL=80h`), then
   `AX=4310h` obtains its entry point in `ES:BX`.
2. Calling that entry with `AH=1`, `DX=FFFFh` must grant the HMA.
3. `INT 15h/AH=88h` must report at least 64 KiB.
4. Calling the same entry with `AH=5` must enable A20.
5. The guest writes and rereads `FFFF:0010` as a final non-wrapping probe.

Failure at any stage returns carry and therefore selects the low-memory path.
Later CONFIG processing calls `LoadDOSHiOrLo` only while `runhigh == FFh`.
The separate UMB path also checks for XMS first and returns carry when it is
absent. Consequently, a particular selected `CONFIG`/HIMEM policy can make
XMS relevant after the prefix, but the static source does not make successful
HMA placement a prerequisite for reaching the low-memory DOS path.

## Historical Host Composition

The old `MSSCSInit` sequence calls `XMSInit` and terminates its historical VDM
if it fails. That is a composition decision of the NT4 host stack, not proof
that a future x64 backend must reproduce its allocator. The original XMS
module reserves UMBs, creates a suballocator above 1 MiB, and dispatches a
private BOP-selected service table. Its A20 helper calls SAS to enable/disable
20-bit wrapping and mirrors state into optional HIMEM-owned guest storage.

Those are useful behavioral references, but private service byte values,
legacy virtual-memory APIs, UMB ownership, and `XMSInit` process policy remain
outside core and outside the first contained wrapper cut.

## Boundary Classification

| Concern | Correct owner | Reason |
| --- | --- | --- |
| Real-mode address wrap/non-wrap at the 1 MiB boundary | generic core machine | it is CPU/memory semantics independent of an XMS provider |
| Checked mapping of HMA-capable RAM when a profile elects to provide it | generic core machine/topology | mapping capability is neutral; quantity and placement remain configured |
| XMS discovery, entrypoint ABI, allocation, UMB policy, and HIMEM integration | wrapper/mantle profile capability | all are DOS/XMS conventions and guest policy |
| Exact `CONFIG` choice to run DOS high or accept the low fallback | guest/profile policy | the original guest explicitly selects between both outcomes |
| BOP selector and XMS service byte transport | profile adapter only after governed decode support | neither is a generic instruction or core API |

## Admission Consequence

A20 remains required generic machine semantics. XMS/HMA is **trace-gated** for
the first bounded command profile: the profile must declare whether it permits
the documented low-memory fallback or supplies an XMS capability. The first
backend fixture must record the `INT 2Fh` result, `AllocHMA` outcome, A20
probe, and final `runhigh` state without synthesizing a successful result.

This does not add a third core gap. It continues to depend on M70's configured
decode observation and paused ordinary-RAM transaction before any historical
image prefix can execute.
