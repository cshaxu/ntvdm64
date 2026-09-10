# M0 T316 S2 — Original monitor ABI import

## Question

Can the selected x86 CCPU40 monitor binding use the complete original OpenNT
`vdm.h` ABI without reintroducing the retired NT4 kernel VDM monitor product?

## Inputs

- `src/opennt-abi/source/public/internal/base/inc/vdm.h`, the original ABI
  mirror.
- The T316 S1 monitor contract reconciliation.
- `adapter-mvdm-host-out/monitor` production sources and focused fixture.
- The selected x86 CCPU40 formal Ninja graph.

## Procedure

1. Delete the adapter-owned subset `monitor/include/vdm.h`.
2. Import the original header through the normal ABI root and isolate only
   session-bound `VDM_TIB`/`pNtVDMState` storage in `monitor_context.h`.
3. Preserve the existing one-service `VdmQueryDir` route and reject every
   other service class.
4. Run the focused monitor fixture and a fresh external selected CCPU40 graph.

## Observations

- The adapter no longer owns a copied `VDMSERVICECLASS`, record or `VDM_TIB`
  layout. Imported users now see the complete original declaration mirror.
- `monitor_context.h` locally opens only the original `_X86_` declaration gate
  while including `vdm.h`; it does not define `CPU_30_STYLE`, `MONITOR` or
  `i386`.
- The original fixed-address `pNtVDMState` macro is replaced only inside the
  context carrier with session-thread-local storage. This is the minimum
  non-kernel divergence and is registered as `ADAPTER-MONITOR-004`.
- The focused x86 monitor fixture passes both the VDM TIB carrier and the
  bounded `VdmQueryDir` contract. The fresh x86 CCPU40 formal graph completes
  all 239 invalidated edges and then reports no remaining work.
- Removing the historical `CPU_30_STYLE` definition exposed one incidental
  ICA preprocessor dependency: `ica.h` otherwise defines `CPU_HW_INT` as a
  macro and overwrites CCPU's event enum. `MVDM-HOST-DIV-146` keeps that old
  macro out of the original `CCPU` executor while leaving the original
  non-CCPU branch untouched.

## Interpretation

This closes the directly recoverable user-mode monitor ABI work. The sole
enabled service remains DEM's `VdmQueryDir`; kernel VDM, graphics, DPMI and
VDD/debugger service classes retain the exact unavailable or later-owner
dispositions recorded by S1. No kernel VDM or CPU30/V86 mechanism was added.

## Follow-up

Later owner packages may reuse the original ABI declarations, but must not
promote another service class without their own source-shaped admission and
verification.
