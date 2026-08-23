# M0 T259 S1 — OpenNT Host and SoftPC/CCPU Interface Catalog

Status: active source/ABI catalog  
Scope: OpenNT host-composition and SoftPC/CCPU interfaces reached by the
tracked BOP providers and their dependencies.  This is evidence and a routing
catalog, not a license to recreate the historical NT4 product shell.

The active-source reconciliation that closes S1 is recorded in
[m0-t259-s1-current-facade-reconciliation-001.md](m0-t259-s1-current-facade-reconciliation-001.md).

## Purpose

`bop-list.md` tracks every BOP and dependency at the work-item level.  This
catalog gives those rows one stable answer for each historical host or machine
interface: its OpenNT source family, current owner, allowed modern treatment,
and the boundary beyond which it must remain deferred. The objective is to
recover those interfaces as completely as their source evidence permits. The
same-shaped `bx-vdm` facade is the method that keeps imported
`bop`/dependency bodies and adopted Bochs code minimally modified while that
recovery proceeds. A later provider must cite both its BOP/dependency row and
the applicable catalog ID.

## Interpretation rules

- OpenNT source remains the semantic reference.  Preserve its call ordering,
  data layout, success/failure contract and ownership wherever a public API or
  narrow compatibility facade permits it. This preservation is the preferred
  way to avoid invasive edits to the imported OpenNT body and to Bochs.
- “Non-intrusive CLI” permits ordinary public Win32 APIs, host files, devices,
  registry and console integration.  It forbids requiring users to rebuild or
  patch Windows/kernel components merely to run this program.
- `bx-core` and `bx-mantle` expose selector-blind machine mechanics only.
  They must not learn BOP, DOS, VDM, DEM, COMMAND, SoftPC or CCPU semantics.
- A private NT4 service with no direct public equivalent is not discarded for
  that reason. Keep its source-visible interface shape and reconstruct its
  observable contract with public Win32 or selector-blind Bochs mechanics
  where the original source gives sufficient evidence. Defer only a remainder
  for which no safe, testable equivalent contract has yet been established.
- Historical raw host pointers never cross the x86/x64 boundary.  T258's
  session-owned mapping-manager instances provide the only admitted opaque
  guest-memory, host-handle and session-data crossings.

## Canonical host interface families

| ID | OpenNT source/interface family | Current disposition and owner | Required compatibility boundary |
| --- | --- | --- | --- |
| HOST-01 | File, directory, volume, attribute and time APIs used by DEM | Reusable through public Win32; `bx-vdm` source-shaped provider | Preserve DOS error conversion and use opaque host-handle IDs. |
| HOST-02 | Raw media, printer, serial and block-I/O host services | Per-provider admission only | Direct public API only when source and access contract are proven; otherwise source failure/deferred. |
| HOST-03 | `CreateProcess`, startup, waits, jobs and local pipes in COMMAND | Reusable single-session lifecycle; `bx-vdm` | Child-owned stdio endpoints only; never replace the parent process global std handles. |
| HOST-04 | BaseSrv/CSR command/event broker, `GetNextVDMCommand` | Source-shaped session facade over public Win32 events/waits | Preserve original command/event order and failure; multi-VDM/remote broker branches await a separately proven contract. |
| HOST-05 | Console, keyboard and standard-handle notification | Public Win32 or session-local state; `bx-vdm` | Preserve redirection status; graphical/fullscreen and injected console input require their own owner package. |
| HOST-06 | Registry, system directory, PIF and environment host calls | Public Win32 under COMMAND's original error contract | No host installation mutation is required; per-call access failures remain observable. |
| HOST-07 | NTDLL/RTL string, OEM conversion, environment and `NTSTATUS` helpers | OpenNT-shaped compat facade | Preserve allocation pairing, widths and layouts; do not use a process-global/TEB static substitute. |
| HOST-08 | Dynamic `WOW32`/`VDMREDIR` loading and `GetProcAddress` | Source-shaped loader facade in the WOW/Redirector owner packages | Preserve probing/order/failure with modern module loading only when the receiving provider contract is specified. |
| HOST-09 | `NtVdmControl`, VDM install/emulator calls and delayed interrupt control | Source-shaped typed facade over Bochs/session mechanics | Bochs owns CPU/interrupt mechanics; reconstruct only the observable request/result contract, not the private-NT service. |
| HOST-10 | Pointer-width host `HANDLE` transport | T258 mapping manager; `BX-VDM-001` exception | Host handle ↔ opaque 32-bit guest ID only; no pointer truncation or register splitting. |
| HOST-11 | `GetVDMAddr`, `Sim32*`, SAS guest-pointer access | T258 guest-memory mapping instance | Synchronous checked bounce leases are admitted; durable/protected-mode leases await their owner. |
| HOST-12 | CCPU register macros and host CPU context access | Typed copied CPU-frame records | No raw CCPU frame or host pointer crosses a BOP/worker boundary. |

## Canonical machine interface families

| ID | OpenNT/SoftPC interface family | Current disposition and owner | Required compatibility boundary |
| --- | --- | --- | --- |
| MACH-01 | `host_simulate`, `host_cpu_interrupt`, CCPU execution loops | Bochs execution; `bx-vdm` bounded run/stop/resume composition | A session worker may schedule slices, but Bochs remains BOP/DOS/COMMAND-blind. |
| MACH-02 | SAS memory, conventional memory and UMB spans | `bx-core` RAM plus `bx-mantle` checked mechanics | UMB policy belongs to `bx-vdm`; mantle receives only opaque physical spans. |
| MACH-03 | Reset, BDA, memory-size and machine startup contracts | source-shaped `bx-vdm` provider over core/mantle reset | Do not place VDM startup policy inside machine code. |
| MACH-04 | PIC, IRQ, ports and device callbacks | Bochs/core-mantle native mechanics | BOP providers request typed mechanical effects only; no selector recognition in Bochs. |
| MACH-05 | Keyboard/INT 15 bridge and IVT fragments | Source-shaped typed input bridge | Reconstruct original request/result semantics over public console/input and Bochs IVT mechanics; expand only under its input/console package. |
| MACH-06 | VDD events and user hooks | Source-shaped session event facade where evidence permits | Reconstruct observable hook/event results over public APIs; do not recreate the private VDD shell or callback ABI. |
| MACH-07 | `nt_bop.c` generic dispatcher | Source-shaped routing reference in `bx-vdm` | Preserve selector/service ordering and failure contracts without linking the NT4 CCPU/SAS/CSR product shell. |
| MACH-08 | SoftPC selectors `02`/`06`/`12` and BIOS helpers | `bx-vdm` provider over opaque core/mantle mechanics | No SoftPC terms or BOP logic enter core/mantle. |
| MACH-09 | FDC, DMA, CMOS, VGA, network and other device product facilities | Native Bochs/mantle mechanics, admitted by source-proven need | Preserve source-visible result/failure; enabling a device solely to make a BOP succeed remains forbidden. |

## Reconciliation rule

The catalog is intentionally narrower than the historical source tree. A
future package must first retain the original interface shape and derive its
observable semantics from source. It may then implement that contract through
a direct public API or a narrow source-shaped facade over public Win32/Bochs
mechanics, recording ABI mapping, failure disposition, and any exception in
the BOP tracker. It may not create a bespoke substitute merely to advance a
guest trace or make core/mantle understand VDM semantics.

## Initial finding

The immediate recoverable seam is not a new CPU-frame ABI.  It is a canonical,
source-shaped compatibility surface for the existing OpenNT mirror calls and a
selector-blind typed machine contract.  T259 therefore first inventories and
consolidates current shims; only after that evidence is stable may a package
replace a specific historical interface.
