# M0 T305 S1 — Original DEM/COMMAND dispatcher boundary audit

## Question

Can the first DEM/COMMAND selectors enter their original MVDM owner bodies
through the already selected Base VDM, SoftPC-shaped and machine-event seams,
without replacing the original dispatcher with app-owned behavior?

## Inputs

- `src/mvdm-host/softpc.new/host/src/nt_bop.c`
- `src/mvdm-host/dos/dem/demdisp.c`
- `src/mvdm-host/dos/command/cmddisp.c`
- `src/adapter-mvdm-host-out/basesrv/source/base_vdm_client.c`
- `src/adapter-mvdm-host-out/softpc/mvdm_sas.c`
- `src/adapter-mvdm-host-out/softpc/mvdm_command_registers.c`
- The completed copied machine-event ABI from M0 T304.

## Original owner map

- `MS_bop_0` in `nt_bop.c` reads the selector byte through
  `Sim32GetVDMPointer(SEGOFF(getCS(), getIP()), 1, FALSE)`, calls the original
  `DemDispatch`, then advances IP by one.  It is the owner of `50:xx` entry
  sequencing, not a DEM provider.
- `MS_bop_4` in the same source reads the selector through `sas_load`, calls
  the original `CmdDispatch`, then advances IP by one.  It is the owner of
  `54:xx` entry sequencing, not a COMMAND provider.
- `DemDispatch` and `CmdDispatch` are respectively the original DEM and
  COMMAND tables.  The latter invokes its selected original handler via
  `apfnSVCCmd[iSvc]`; neither dispatcher is reimplemented by this packet.

## Existing same-shaped boundaries

- `GetNextVDMCommand(PVDMINFO)` is already exposed by the bounded Base VDM
  local profile.  Its caller-owned request/result copy preserves the original
  public form without recreating CSR transport.
- `sas_load` is already provided by the SoftPC-shaped adapter and resolves one
  bounded byte through the machine facade.  `setIP` similarly writes only the
  copied 16-bit IP form.
- M0 T304 supplies a selector-blind copied machine-event ingress only.  It
  does not decide whether an event is `50:xx`, `54:xx`, or another BOP family.

## Procedure

1. Compile the exact original `demdisp.c` and `cmddisp.c` as independent C11
   translation units for MSVC `/MT` x86 and x64, using only the current
   declaration carriers and named adapter headers.
2. Compile the exact original full `nt_bop.c` under its historical monitor
   defines (`WIN_32`, `DEVL`, `MONITOR`, `PROD`) on both architectures.
3. Make no source-body, handler-table, selector, or machine-provider change.

The disposable probe root is `build/M0-T305/S1/dispatcher-probe/`.

## Observations

- `demdisp.c` compiled on x86 and x64.  Both emitted only the existing MSVC
  `sprintf` deprecation warning at line 233.
- `cmddisp.c` compiled on x86 and x64 without a diagnostic.
- Full `nt_bop.c` reached its original source body on both architectures but
  stopped at `softpc.new/host/inc/nt_eoi.h:31`: the historical `VDMVIRTUALICA`
  declaration is absent from the selected modern monitor form.
- Forcing the historical `softpc.new/base/inc/ica.h` is not a valid repair:
  that header requires the obsolete full Insignia type/macro universe
  (`IPT0`, `IU8`, `IS32`, and related declarations), then conflicts with the
  host-facing `insignia.h`.  This is the beginning of the excluded second
  SoftPC executor/PIC closure, not a missing DEM or COMMAND interface.
- The full translation unit also contains unrelated XMS, DPMI, debugger,
  reset, EOI and other BOP bodies.  Linking it merely to obtain `MS_bop_0` and
  `MS_bop_4` would therefore force unrelated owner families into this slice.

## Interpretation and confidence

The original DEM and COMMAND dispatchers are directly source-composable at
the translation-unit level on both supported host widths.  Their immediate
original selector-entry functions are present, but the historical aggregate
`nt_bop.c` cannot be adopted as a whole without importing a second SoftPC/PIC
executor.  That route violates the sole-Bochs-executor rule and cannot be
treated as a harmless declaration fix.

This is high-confidence static/compile evidence, not runtime evidence.  It
does not prove guest execution, individual service behavior, or a linked
selector route.

## Disposition

T305/S1 has established the original owner map and exact compile boundary.
It does not authorize a handwritten selector provider or a PIC/Bochs semantic
addition.  The first new queue package, external-boundary package selection,
must classify the historical monitor/PIC aggregate and determine the smallest
source-shaped route for the two reached entry functions before a later
DEM/COMMAND package performs link/runtime recovery.

The current `mvdm-platform-abi` to `opennt-abi` component-root transition is
separate pending worktree migration material.  It was needed only to make the
current declaration carrier visible to this probe; it is not claimed here as
a T305 product change.

## Follow-up

1. Close this bounded packet with the compile evidence above.
2. Admit the queue's external-boundary package-selection task.
3. Use its selected monitor/PIC disposition to define the next original
   source-shaped DEM/COMMAND ingress closure; do not enable a trace-selected
   service meanwhile.
