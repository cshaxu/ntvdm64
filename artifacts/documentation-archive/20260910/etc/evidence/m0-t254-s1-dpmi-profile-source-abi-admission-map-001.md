# M0 T254 S1 — OpenNT DPMI Profile Source/ABI Admission Map

## Question

Can one bounded OpenNT `53:xx` DPMI profile be recovered now by importing the
original `dpmi32` owner package through only the existing fixed-width
bx-vdm/bx-mantle boundary?

## Inputs

- `src/opennt/base/mvdm/inc/dpmi.h` — the canonical 25-service selector map.
- `src/opennt/base/mvdm/dpmi32/sources` — original `dpmi32` library closure:
  `debug`, `dpmi32`, `dpmiint`, `dpmiselr`, `buffer`, `data`, `int21map`,
  `modesw`, `register`, `savestat`, `stack`, `vxd`, `xmem`, and the i386
  `dpmi386`, `dpmimem`, `dpmimisc` members.
- `src/opennt/base/mvdm/dpmi32/{dpmi32,dpmiint,dpmiselr,int21map,modesw,
  stack,xmem}.c` and `i386/{dpmi386,dpmimem,dpmimisc}.c`.
- Guest DOSX callers in `src/opennt/base/mvdm/dpmi/{dxboot,dxfunc,dxmain,
  dxintr,dxint31}.asm` and the `486/` equivalents.
- Current copied-state/result ABIs: `src/bx-vdm/bx_ntvdm_cpu_state_abi.h`,
  `src/bx-vdm/bx_ntvdm_cpu_result_v2.h`, and
  `src/bx-mantle/bx_ntvdm_generic_ud_bridge.h`.

## Procedure

1. Compared the original dispatch table with all retained DOSX call sites.
2. Classified every original body by its machine-state and historical-host
   dependencies.
3. Compared those requirements with the current boundary rather than
   inferring support from Bochs's internally complete protected-mode CPU.
4. Tested whether any `53:xx` leaf can be admitted independently without
   silently replacing its initialization, exception, memory, or return
   lifecycle.

## Observations

### Original package is a lifecycle, not 25 independent leaves

`dpmi32.c:44..69` dispatches all 25 services.  The DOSX startup first obtains
the fast-BOP address, passes the descriptor-table address, initializes DOSX,
installs interrupt/fault handlers and PM-stack state, then switches mode.
The same guest package later invokes `53:06` for DOS INT 21 translation and
`53:14..17` from its interrupt/fault IRET stubs.  Consequently a convenient
memory or query leaf cannot be called as a standalone DPMI implementation.

| Contract group | Original sources / services | Required machine or host contract |
| --- | --- | --- |
| Dispatch and PM entry | `dpmi32.c`, `i386/dpmi386.c`; `00,01,03,04,05,0F` | protected selector translation, descriptor installation, checked selector:offset reads, atomic selector/segment/CR0 transition, and the fast-BOP context contract |
| Interrupt/fault and return | `dpmiint.c`, `stack.c`, `savestat.c`; `02,0A,11,14..18` | protected IDT/fault delivery, checked PM frame reads/writes, selector-cache-safe CS/SS changes and 16/32-bit IRET restoration |
| INT 21 translation | `int21map.c`, `buffer.c`, `register.c`; `06` | nested PM→real transition, original DOS service routing, saved segment/stack lifecycle and re-entry into PM |
| Extended memory | `xmem.c`, `i386/dpmimem.c`; `07..09,0B,10,13` | guest-visible 32-bit linear allocation/mapping, allocation identity and cleanup; original x86 body exposes host-process virtual addresses and host pointers, so it is not x64-safe unchanged |
| historical product hooks | `i386/dpmi386.c`, `dpmiint.c`, `vxd.c`, `i386/dpmimisc.c`; `03,0C,0D,0E,12` | `VdmTib`, `NtVdmControl`, per-thread debug context, VCD/VxD and fixed NTVDM-state assumptions |

The original `Sim32GetVDMPointer` is not a bounded physical-RAM request.  On
i386 it resolves a selector through `FlatAddress[]` and returns an in-process
host pointer; `dpmiselr.c` additionally uses `NtSetLdtEntries` /
`NtSetInformationProcess(ProcessLdtInformation)`.  `dpmiint.c` then mutates
those pointers and directly changes execution CS/SS/EIP/ESP around synthetic
IRET frames.  `xmem.c` returns both a host allocation address and an
`XMEM_BLOCK *` split into guest registers.  These are all invalid cross-width
or cross-boundary assumptions, not ordinary missing Win32 imports.

### Existing backend versus existing integration boundary

Bochs already owns ordinary protected-mode instruction execution,
descriptors and exceptions.  That does **not** make the current product
boundary DPMI-capable.  Its copied CPU record contains register values and
selector values only; its comment explicitly excludes descriptor caches,
page tables, mappings and host pointers.  The accepted result ABI can resume,
stop, pend, update selected GPR values, CF/ZF and segment selectors; the
existing core interception path applies an accepted selector through Bochs's
native `load_seg_reg`.  It still cannot make the original atomic `setMSW`
mode transition, construct/consume an entire 16/32-bit IRET context, or carry
a checked protected virtual-memory operation.  Current active
DEM/COMMAND/XMS shims require real mode, and no active `53:xx` source mirror
is linked.

## Four-rung disposition

| Rung | Result |
| --- | --- |
| 1. Direct original source | Rejected for the selected x64 runtime: `Sim32GetVDMPointer` raw host pointers, `VdmTib`, old `NtVdmControl`, process LDT and host-address/pointer exposure cannot compose directly. |
| 2. Smallest contract-preserving seam | Not yet available. It must be a selector-blind machine substrate, not a DPMI service implementation: bounded protected selector:offset memory actions; an atomic protected-context/IRET transition; and a guest-linear allocation/mapping identity. A bx-vdm compatibility layer must replace the legacy NT4 product APIs while retaining original package order and failure rules. |
| 3. External-code intrusion | Not admitted. Existing Bochs mechanics must first be evaluated through bx-mantle; no core patch is authorized by this audit. |
| 4. New behavior | Rejected. A hand-written `53:xx` dispatcher, isolated leaf stubs, or adapter-owned DPMI server would violate source-first ownership. |

## Decision

**No bounded DPMI profile is presently admissible for S2.**  This is an
explicit S1 admission rejection, not a claim that DPMI is impossible.  The
minimum original DOSX path itself consumes the descriptor, transition,
interrupt/fault, PM-stack and cleanup groups; `53:03` is also not an isolated
escape hatch because its x86 body publishes a `VdmTib` fast-BOP context.

The next dependency-ordered package is therefore a selector-blind
protected-mode substrate investigation and recovery.  Its success criterion
is not “implement DPMI”: it must prove that existing Bochs mechanics can
expose exactly the bounded operations above, with no BOP/DOS/OpenNT vocabulary
in bx-core or bx-mantle, and identify any genuine external-code exception
before a patch.  Only then can a new DPMI admission select the original DOSX
profile and import its complete source closure.

## Tracker effect

`BOP-DPMI-53-00..18` and `BOP-DEPENDENCY-059..077` remain source-located but
have **no active v1 or v2 provider**.  Their prior “v1/entry shallow” wording
is corrected to “deferred entry classification only”; T239 retired all
selected v1 routes.  New `BOP-DEPENDENCY-117` records the common prerequisite.

## Confidence and follow-up

High confidence: the conclusion follows both original source call edges and
the declared current ABI fields, not an absent trace.  The next task must
audit the protected substrate before proposing a Bochs or mantle modification;
it must reject any interface that exposes raw Bochs objects, host pointers or
DPMI policy across the boundary.
