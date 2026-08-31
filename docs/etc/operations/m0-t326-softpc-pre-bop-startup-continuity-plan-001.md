# M0 T326 — SoftPC pre-BOP startup continuity plan

## Objective

Advance the selected original CPU40 SoftPC product from its current
pre-BOP termination/exception boundary to either a first original BOP ingress
or an exact, source-owned pre-BOP transfer. This package is machine/startup
work only; it cannot enable a BOP family by observing one.

## Ordered S packets

1. **S1 — fixed-container startup cut**: establish one fixed non-debug
   execution container, derive the original call chain from process entry to
   first CPU40 execution, and classify every reachable direct terminal,
   console, timer, worker, firmware and media prerequisite before the first
   BOP. Reproduce the x86 observation with the same selected inputs, and
   compile the same source graph on x86 and x64.
2. **S2 — selected pre-BOP owner recovery**: recover only the complete
   original owner cluster identified by S1, using existing source-shaped
   Win32/session/SoftPC bindings. No synthetic successful device result or
   generic startup shim is permitted.
3. **S3 — raw-worker/lifetime closure**: if S1/S2 prove that the first stop is
   inside an original raw worker, retain the original worker owner and recover
   its creation, session binding, wait/cancel and controlled-stop contract as
   one cluster.
4. **S4 — continuity observation and verdict**: rerun the unchanged fixed
   container once. Record either the first original BOP ingress or the next
   exact source-owned transfer. A BOP observation verifies this package; it
   does not authorize leaf-service work.

## S1 admission boundary

### Included

- Original product entry, `InitialiseDosEmulation`, `scs_init`, DOS-media and
  firmware staging, `host_start_cpu`, CPU40 entry, `nt_timer`, `nt_event`,
  `nt_error`, and each directly created original worker reachable before BOP.
- The existing fixed observer and staging scripts, only as observation tools.
- One formal x86 CCPU40 runtime invocation and paired x86/x64 formal source
  graphs for the selected cut.

### Excluded

- BOP selector/service implementation; DEM, COMMAND, XMS, DPMI, Redirector,
  WOW, VDD, debugger or guest-image changes.
- New CPU executor, Bochs, CPU30/MONITOR/kernel VDM, synthetic FDC/PIC/BIOS
  success, a replacement console product shell, and any raw pointer identity.

## S1 exit

The source cut has one ordered original call graph, one fixed execution
container contract, a classified list of direct terminal/worker prerequisites,
one observed x86 stop/reach result, and paired compile evidence. It names the
single next source-owner cluster, or closes T326 by exact transfer if no
admissible modern recovery exists.

## Completion

S1 reached the first original `50h:11h` crossing, rather than a pre-BOP
terminal. The planned S2--S4 were consequently not admitted: they are
conditional recovery packets for a pre-BOP owner gap and would be trace-driven
work after the achieved boundary. The evidence and successor transfer are in
[the fixed startup-cut record](../evidence/m0-t326-s1-fixed-softpc-pre-bop-startup-cut-001.md).
