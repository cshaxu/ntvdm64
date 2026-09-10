# M0 T309 S2 P3 — original MSSCS/VDD source-boundary audit

## Question

May the original `softpc.new/host/src/nt_msscs.c` be added to the current
Bochs-only DEM/COMMAND provider graph merely to satisfy `HostTerminatePDB` and
`VDDCreateUserHook` residuals?

## Observations

- The unchanged source is a host-composition root, not a VDD-hook helper.  Its
  `scs_init` / `InitialiseDosEmulation` sequence initializes COMMAND, DEM, XMS
  and debugger state, resets the machine, loads NTIO bytes into guest memory,
  and selects the guest entry CS:IP.
- Its VDD block contains registry discovery/loading, memory and I/O hooks,
  UMB allocation, IRQ/DMA state, PDB lifecycle hooks and synchronous
  `VDDSimulate16` re-entry.  These forms reach original SAS, UMB, CPU and VDD
  contracts rather than a single BOP provider callback.
- `HostTerminatePDB` and `VDDCreateUserHook` are therefore parts of the same
  user-hook/PDB lifecycle; satisfying them alone would violate their original
  ordering and VDD state ownership.
- The original `AddSystemFiles` path also creates boot-name placeholders on
  host `C:`.  That is incompatible with the project's explicit rule not to
  create `IO.SYS`/`MSDOS.SYS` or similar files on the host merely to run the
  VDM.

## Disposition

T309 does not select any fragment of `nt_msscs.c` for the current provider
graph.  The complete source remains a mirror input for the queued original-
SoftPC backend and selectable machine-host composition task.  That task must
preserve the original function shapes while routing guest-memory operations
through the session mapping manager and selecting exactly one machine backend
(original SoftPC or Bochs) per session.

The later task must separately decide the VDD package profile and replace
the unsafe `AddSystemFiles` host mutation with the already-approved guest
media/admission model.  It may not use the present residual as authority to
enable registry-loaded VDDs, private kernel VDM services, or a second executor.

## Confidence and follow-up

High confidence: all described paths occur within the literal original source
and its immediately declared MVDM interfaces.  This completes P3's original
source-candidate grouping.  The next T309 action is a whole-cohort summary and
transfer record, not another trace-driven service patch.
