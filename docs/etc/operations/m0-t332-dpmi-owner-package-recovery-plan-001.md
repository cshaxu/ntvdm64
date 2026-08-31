# M0 T332 — Original DPMI/DPMI32 owner-package recovery

## Objective

Recover the original OpenNT `mvdm-host/dpmi32` provider package and its
separate DOSX guest contract in source dependency order. The host provider
must retain original source ownership, table ordering and failure paths;
DOSX remains guest media/source and is never linked into the host product.
The selected SoftPC CCPU40 machine is the only CPU executor.

## Package boundary

- Host provider: the original `dpmi32` selected C sources, its 25-entry
  `DpmiDispatchTable`, original DPMI data/stack/descriptor/XMEM algorithms,
  and their source-owned service results.
- Guest carrier: original `mvdm-host/dpmi` DOSX assembly, binaries, maps and
  build inputs. It is loaded guest material, not a host static library.
- Mechanical boundary: `adapter-mvdm-host-out/softpc` exposes only the
  existing source-shaped CCPU40/SAS/descriptors/finite-run forms.
- Host/monitor boundary: `adapter-mvdm-host-out/{win32,monitor,basesrv}` and
  `session` provide only same-shaped public-platform and bounded mapping/TIB
  contracts. They do not implement a second DPMI dispatcher, V86 monitor,
  kernel VDM or CSRSS product shell.

## Sequential subtasks

### S1 — Current source, ABI and failure admission

Reconcile every selected `dpmi32` file, every `DpmiDispatchTable` entry,
every DOSX ownership edge, current mirror divergence and direct external
import against the current CPU40-only graph. Classify each current shim or
overlay as retain, replace with original source, migrate, delete or named
later-owner transfer. The deliverable is one auditable ledger; no DPMI
behavior is changed.

### S2 — Shared guest-span, identity and monitor/TIB closure

Recover the common `Sim32GetVDMPointer` users through the session mapping
manager's bounded leases/copies, and reconcile source-shaped `VDM_TIB`/
`NtVdmControl` calls with the monitor adapter. Preserve original numeric
guest values and failure directions. No raw host pointer, family-private
mapper, kernel-VDM recreation or fabricated control success is admitted.

### S3 — Original initialization, descriptor and XMEM source cohorts

Compose eligible original initialization, selector, descriptor, XMEM and
data bodies through S2's interfaces. Preserve original algorithm/order and
use the selected SoftPC CCPU40 mechanics. Local tests exercise complete
source cohorts, not trace-selected `53:xx` leaves.

### S4 — Stack, interrupt, fault, DOSX and package closure

Compose original stack/interrupt/fault and eligible DOS translation cohorts;
record exact unavailable monitor branches and guest DOSX transfer boundaries.
Run the paired x86/x64 formal package matrix and at most one fixed-container
observation only after the full package source closure. Close T332 with every
dispatcher row and original dependency assigned a source-shaped result or a
named later owner.

## Exit

Every selected original DPMI/DPMI32 declaration, definition, table entry and
external boundary has one recorded disposition. No Bochs route, CPU30 path,
second executor, raw host pointer, source-derived DPMI policy or trace-driven
leaf-service implementation remains. Both x86 and x64 product graphs compile
and link; runtime claims are limited to directly observed complete cohorts.
