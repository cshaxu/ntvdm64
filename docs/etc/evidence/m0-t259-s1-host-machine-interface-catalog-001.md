# M0 T259 S1 — OpenNT Host and SoftPC/CCPU Interface Catalog

Status: active source/ABI catalog  
Scope: OpenNT host-composition and SoftPC/CCPU interfaces reached by the
tracked BOP providers and their dependencies.  This is evidence and a routing
catalog, not a license to recreate the historical NT4 product shell.

## Purpose

`bop-list.md` tracks every BOP and dependency at the work-item level.  This
catalog gives those rows one stable answer for each historical host or machine
interface: its OpenNT source family, current owner, allowed modern treatment,
and the boundary beyond which it must remain deferred.  A later provider must
cite both its BOP/dependency row and the applicable catalog ID.

## Interpretation rules

- OpenNT source remains the semantic reference.  Preserve its call ordering,
  data layout, success/failure contract and ownership wherever a public API or
  narrow compatibility facade permits it.
- “Non-intrusive CLI” permits ordinary public Win32 APIs, host files, devices,
  registry and console integration.  It forbids requiring users to rebuild or
  patch Windows/kernel components merely to run this program.
- `bx-core` and `bx-mantle` expose selector-blind machine mechanics only.
  They must not learn BOP, DOS, VDM, DEM, COMMAND, SoftPC or CCPU semantics.
- A private NT4 service with no public equivalent is not silently emulated.
  It is deferred or returns the source-proven unavailable/failure result until
  an owner-approved package defines a replacement contract.
- Historical raw host pointers never cross the x86/x64 boundary.  T258's
  session-owned mapping-manager instances provide the only admitted opaque
  guest-memory, host-handle and session-data crossings.

## Canonical host interface families

| ID | OpenNT source/interface family | Current disposition and owner | Required compatibility boundary |
| --- | --- | --- | --- |
| HOST-01 | File, directory, volume, attribute and time APIs used by DEM | Reusable through public Win32; `bx-vdm` source-shaped provider | Preserve DOS error conversion and use opaque host-handle IDs. |
| HOST-02 | Raw media, printer, serial and block-I/O host services | Per-provider admission only | Direct public API only when source and access contract are proven; otherwise source failure/deferred. |
| HOST-03 | `CreateProcess`, startup, waits, jobs and local pipes in COMMAND | Reusable single-session lifecycle; `bx-vdm` | Child-owned stdio endpoints only; never replace the parent process global std handles. |
| HOST-04 | BaseSrv/CSR command/event broker, `GetNextVDMCommand` | Limited session-local facade only | Multi-VDM broker, remote queues and private CSR protocol remain deferred. |
| HOST-05 | Console, keyboard and standard-handle notification | Public Win32 or session-local state; `bx-vdm` | Preserve redirection status; graphical/fullscreen and injected console input require their own owner package. |
| HOST-06 | Registry, system directory, PIF and environment host calls | Public Win32 under COMMAND's original error contract | No host installation mutation is required; per-call access failures remain observable. |
| HOST-07 | NTDLL/RTL string, OEM conversion, environment and `NTSTATUS` helpers | OpenNT-shaped compat facade | Preserve allocation pairing, widths and layouts; do not use a process-global/TEB static substitute. |
| HOST-08 | Dynamic `WOW32`/`VDMREDIR` loading and `GetProcAddress` | Deferred to WOW/Redirector owner packages | No speculative dynamic-loading shim. |
| HOST-09 | `NtVdmControl`, VDM install/emulator calls and delayed interrupt control | Deferred | Bochs owns CPU/interrupt mechanics; no private-NT API recreation. |
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
| MACH-05 | Keyboard/INT 15 bridge and IVT fragments | Narrow active bridge; remainder deferred | Broader keyboard/console semantics require their own input/console package. |
| MACH-06 | VDD events and user hooks | Deferred | No private VDD shell or callback ABI recreation. |
| MACH-07 | `nt_bop.c` generic dispatcher | Structural source evidence only | Selector routing is implemented in `bx-vdm`; do not link the NT4 CCPU/SAS/CSR product shell. |
| MACH-08 | SoftPC selectors `02`/`06`/`12` and BIOS helpers | `bx-vdm` provider over opaque core/mantle mechanics | No SoftPC terms or BOP logic enter core/mantle. |
| MACH-09 | FDC, DMA, CMOS, VGA, network and other device product facilities | Default-deny/deferred until native machine owner admission | Enabling a device solely to make a BOP succeed is forbidden. |

## Reconciliation rule

The catalog is intentionally narrower than the historical source tree.  A
future package may move an interface only by recording source evidence, ABI
mapping, exact failure disposition, and any registered exception in the BOP
tracker.  It may not turn a deferred private interface into a bespoke facade
merely to advance a guest trace.

## Initial finding

The immediate recoverable seam is not a new CPU-frame ABI.  It is a canonical,
source-shaped compatibility surface for the existing OpenNT mirror calls and a
selector-blind typed machine contract.  T259 therefore first inventories and
consolidates current shims; only after that evidence is stable may a package
replace a specific historical interface.
