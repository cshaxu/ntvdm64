# M0 T316 — Monitor user-mode contract closure

## Scope

Recover the selected x86 CCPU40 user-mode monitor ABI without admitting the
retired NT4 kernel VDM/V86 product.

## Result

- Audited all reached `NtVdmControl` calls and `VDM_TIB` consumers.
- Retained DEM `VdmQueryDir` as the only direct user-mode binding.
- Removed the adapter-owned subset `monitor/include/vdm.h`; imported code now
  uses the complete original `opennt-abi` declaration mirror.
- Added a narrow adapter-owned `monitor_context.h` only for session TLS
  binding and the non-kernel `pNtVDMState` carrier.
- Preserved explicit unavailable or named-later-owner disposition for every
  other monitor service class; no generic dispatcher, kernel VDM, CPU30 or
  V86 code was selected.
- Corrected the one original ICA preprocessor coupling exposed by CPU30
  retirement: CCPU retains its original event enum rather than receiving the
  non-CCPU ICA macro.

## Verification

- External x86 CCPU40 formal graph rebuilt all 239 invalidated edges and then
  reported no remaining work.
- Focused monitor fixture passed `VdmQueryDir` bounds and the thread-local
  `VDM_TIB` carrier.
- No live production/source/tool reference remains to the deleted adapter ABI
  copy.
- Documentation governance and `git diff --check` passed at delivery.

## Deferred boundaries

`VdmDelayInterrupt`, `VdmInitialize`, DPMI service classes and
VDD/debugger-related TIB consumers remain owned by their respective later
packages or are explicitly unavailable in the selected product. This closure
does not claim DOS/WOW execution or a kernel-VDM substitute.

## Evidence

- [S1 reconciliation](../etc/evidence/m0-t316-s1-monitor-contract-reconciliation-001.md)
- [S2 ABI import](../etc/evidence/m0-t316-s2-original-vdm-abi-import-001.md)
