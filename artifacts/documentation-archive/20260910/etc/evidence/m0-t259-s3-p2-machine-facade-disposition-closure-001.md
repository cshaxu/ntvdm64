# M0 T259 S3 P2 — Reached Machine-Facade Disposition Closure

Status: S3 closure evidence  
Question: do the reached `MACH-*` interfaces have an OpenNT-shaped recovery
or a truthful, source-proven non-success disposition, without introducing a
CCPU product shell or modifying Bochs?

## Inputs

- The current [T259 interface catalog](m0-t259-s1-host-machine-interface-catalog-001.md).
- The current [S3 mechanical contract](../operations/m0-t259-s3-softpc-ccpu-bochs-mechanical-contract-001.md).
- OpenNT `base/mvdm/softpc.new/host/src/nt_cpu.c` and `nt_aorc.c`.
- Imported OpenNT DEM/SoftPC callers: `bop/opennt/dem/demdasd.c`,
  `bop/opennt/softpc/{mem_size.c,update_kbd_int15.c,spckbd_handoff.c}`.
- Current `bx-vdm` shims, the T258 mapping manager, and the S3 P1 direct
  DASD/IOCTL focused fixture.

## Procedure

1. Read each catalogued `MACH-*` source family and each currently reachable
   source mirror call site.
2. Compare the original function spelling, state width and success/failure
   behavior with its current `bx-vdm` boundary.
3. Reject any disposition that would teach `bx-core` or `bx-mantle` a BOP,
   DOS, VDM, DEM, COMMAND, SoftPC or CCPU policy.
4. Confirm that an unavailable recursive execution path remains an explicit
   failure, rather than a false success or an invented nested CPU loop.

## Disposition

| Catalog ID | Original source contract | Current treatment | S3 result |
| --- | --- | --- | --- |
| MACH-01 | `nt_cpu.c:host_simulate` recursively invokes `cpu_simulate`; the CCPU function table in `nt_cprgs.c` binds `host_simulate_func` to `c_cpu_simulate`. | `demdasd_ioctl_shim` retains the source spelling and returns `ERROR_CALL_NOT_IMPLEMENTED`, BIOS invalid-function AH and CF set. It does not recurse into the active Bochs loop. | Source-proven decline. A later selector-blind continuation/scheduler contract must preserve the call shape before this becomes runnable. |
| MACH-01 | `nt_cpu.c:host_cpu_reset` and `host_cpu_interrupt` have empty CCPU bodies. | No fabricated reset or interrupt delivery is added at those historical calls. Separate typed machine effects remain owned by native Bochs when a caller proves one is required. | Direct source parity: no-op retained. |
| MACH-02 | `sas_loadw` and analogous real-mode SAS access read finite addresses in imported DEM/SoftPC bodies. | `demdasd_ioctl_shim`, `softpc_memory_size_shim`, `xms_shim` and `spckbd_handoff_shim` use checked fixed-width RAM requests or a bounded T258 mapping lease. | Recovered facade; no raw SAS pointer crosses an ABI. |
| MACH-03 | SoftPC memory-size/startup fragments read a fixed conventional-memory word and return AX. | `mem_size.c` retains its source body; `softpc_memory_size_shim` supplies only the named `sas_loadw`/`setAX` seam. | Recovered facade. |
| MACH-04 | Historical providers need PIC/IRQ/port effects but do not own their implementation. | Native Bochs/core-mantle own those mechanics; `bx-vdm` can request typed effects only. | Owner boundary retained; no provider-side replacement. |
| MACH-05 | `update_kbd_int15.c` watches IVT 15h and `spckbd_handoff.c` stores narrow BDA/IVT facts. | The imported source fragments remain intact behind checked IVT and RAM shims. Public-input policy is not implied. | Recovered finite facade; broader console/input remains separately owned. |
| MACH-06 | VDD hooks/events depend on historical VDD composition. | No route is falsely enabled and no VDD callback ABI is recreated. | Explicit deferred product-shell remainder. |
| MACH-07 | `nt_bop.c` mixes selector dispatch with CCPU/SAS/CSR host composition. | It remains routing/semantic evidence only; current ingress is in `bx-vdm`. | Explicitly not directly composable. |
| MACH-08 | Selectors `02/06/12` make SoftPC/BIOS requests. | Their routing remains in `bx-vdm`; only opaque typed machine requests enter the machine boundary. | Recovered ownership boundary. |
| MACH-09 | Device facilities are supplied by the historical product shell or native machine. | Device enablement remains source-proven and Bochs-owned; no device is activated merely to turn a BOP into success. | Explicitly deferred/admitted per device. |

## Observations

- The OpenNT CCPU source itself establishes that `host_cpu_reset` and
  `host_cpu_interrupt` are no-ops.  Treating either as a request to add a new
  Bochs action would change source behavior.
- `host_simulate` is different: its observable operation is recursive CPU
  execution.  Current Bochs BOP escape/control flow cannot safely nest that
  loop while an imported C call is active.  The retained failure therefore
  expresses an unavailable mechanism; it is not a completed recovery.
- All current finite memory/IVT interfaces use copied values or checked spans.
  None exports a Bochs object, CCPU frame, SAS object or host pointer.
- The S3 P1 formal Ninja proof remains applicable to the changed copied-state
  CCPU facade: its 286-object graph links and the direct DASD/IOCTL fixture
  passes.  This P2 review adds no executable behavior.

## Interpretation

S3 is closed.  Every reached machine call site now has exactly one honest
disposition: direct source parity, an OpenNT-shaped bounded facade, native
machine ownership, or explicit unavailable/deferred behavior.  The record
also confirms the two simultaneous recovery gates: preserve the original
interface contract wherever possible, and use that preservation to avoid
intruding into the imported OpenNT body or the Bochs core/mantle sides.

The remaining `host_simulate` recursion is not a reason to create a CCPU or
to insert SoftPC knowledge into Bochs.  It transfers as a future
selector-blind machine-continuation package, contingent on a source-proven
resume contract.

## Follow-up

T259 S4 reconciles remaining host-facing source-shaped facades and build
closure.  A future machine-continuation package owns any attempt to recover
the recursive `host_simulate` branch; it must not be enabled by a leaf DEM
change.
