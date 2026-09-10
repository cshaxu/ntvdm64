# M0 T198 S68: `BOP 5E/AL=0` Config-Done Notification Audit

## Observed input

The clean source-built run at
`artifacts/build/t198-s68-config-done-observation-r1` reaches `C4 C4 5E` at
`8dc8:08af` with `AL=00`, `BX=1225`, `DX=a8c0`, `DS=8dc8`, and
`FLAGS=00000046`.

## Original source map

`sysinit1.asm` explicitly sets `AL=0` after CONFIG.SYS processing and issues
`BOP_NOTIFICATION`; after return it calls guest `AllocUMB` and continues DOS
initialization.  `nt_bop.c` maps code zero to `UMBNotify(0)` and `demDasdInit`.
Those two calls are historical NT host composition: UMB coordination and DASD
host-drive integration, respectively, rather than guest CPU/BIOS semantics.

## CLI profile disposition

The active finite profile supplies neither host UMB coordination nor DASD/raw
drive capability; no such backend is linked into the explicit composition
manifest.  The existing `config_done` provider accepts only this exact
real-mode `5E/AL=0` form and resumes without state changes.  It deliberately
does not claim to emulate `UMBNotify` or `demDasdInit`; it records that those
host capabilities are absent while allowing the original guest continuation
(`AllocUMB`) to run.

S69 may compose this exact capability-absence disposition.  It may not add
host drive, DASD, UMB, GUI, VDD, or console semantics.

