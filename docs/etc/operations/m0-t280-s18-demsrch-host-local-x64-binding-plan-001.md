# M0 T280 S18 — `demsrch.c` host-local x64 binding closure

Recover the S17 groups `SRCH-X64-001` through `SRCH-X64-005` together as one
original source-owner package. Preserve the selected `demsrch.c` algorithm,
call order and failure paths while changing only host-local pointer/sentinel
expressions needed for x64 and relocating the reached `NtVdmControl(VdmQueryDir)`
same-shaped facade from `adapter-win32` to its sole architectural owner,
`adapter-vdm-monitor`.

The work includes focused x86/x64 source syntax and host-local facade tests.
It does not enable a DEM provider or BOP, add a directory backend, handle
guest DTA/FCB access, or implement a `GetVDMAddr` lease. `SRCH-X64-006` stays
the separate `adapter-softpc -> session.guest_memory -> adapter-bochs` future
boundary; `SRCH-X64-007/008` remain unmodified original source-hygiene debt.

Every changed mirror expression requires `DIVERGENCE:` and an
`opennt-mvdm-host` README register entry. `adapter-vdm-monitor` may preserve
only the reached original interface shape and deterministic unavailable/slow
fallback behavior; it must not recreate kernel VDM, CSRSS or BaseSrv.
