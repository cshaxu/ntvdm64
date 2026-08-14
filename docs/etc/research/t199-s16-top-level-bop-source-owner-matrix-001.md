# M0 T199 S16 Top-Level BOP Source And Owner Matrix

## Scope And Inputs

This record covers non-family selectors from `bop.h`/`BOP.INC`:
`51,59,5A,5B,5C,5D,5E,5F,FD,FE`. All are three-byte BOP forms by the original
`BOP_SIZE=3` contract. Inputs include the NTVDM BIOS table, `nt_bop.c`, DPMI
and DOS callers, and retained host/machine sources.

## Owner Map

| Selector | Original path | Owner/disposition basis |
| --- | --- | --- |
| `51` WOW | `MS_bop_1` lazy-loads `WOW32`, resolves multiple exports, initializes and dispatches; any load failure terminates the VDM. | Host WOW composition absent: one CLI deferred-stop package member. |
| `59` no-support | `MS_bop_9` calls host direct-access/error UI using AX. | Host UI unavailable: explicit unavailable/controlled stop, no dialog emulation. |
| `5A` wait-if-idle | `MS_bop_A` calls `WaitIfIdle` or `WakeUpNow` based on AX. | CLI scheduler capability candidate; not CPU, DOS or Bochs semantics. |
| `5B` debugger breakpoint | `MS_bop_B` writes debug output and invokes the host debugger. | Debug host absent: unified debugger/deferred-stop owner. |
| `5C` / `5D` | `bop.h` labels keyboard/video temporary forms, but the active NTVDM BIOS table maps them to `MS_bop_C` illegal BOP and `MS_bop_D` monitor IRET-hook control. | Historical product-variant conflict. No adapter behavior may be inferred; retain machine/monitor deferred ownership. |
| `5E` notification | `MS_bop_E` consumes AL; code zero calls `UMBNotify` and `demDasdInit`. DOS `ConfigDone` is a concrete caller. | Existing adapter config-done service is a partial source-derived host-composition member; full UMB/DEM lifecycle remains package-owned. |
| `5F` unimplemented interrupt | The active BIOS table maps `MS_bop_F`, which calls `kb_setup_vectors`; NTIO passes vector state through this path. Header/scaffold labels differ. | `bx-mantle`/machine lifecycle owns vector setup; no BOP service semantics in `bx-core`. |
| `FD` switch real mode | BIOS table binds `switch_to_real_mode` only under `NTVDM && MONITOR`; DPMI `dxutil` calls it. | DPMI/monitor transition: deferred behind C4/C5/C6 machine capabilities. |
| `FE` unsimulate | BIOS table binds host unsimulate/termination; DPMI/DOS utility callers use it to exit VDM execution. | CLI lifecycle terminal candidate, not guest DOS or CPU semantics. |

## Interpretation

The current top-level ingress labels are identity facts only. They are not a
single provider contract. `5E` and `5F` already have separate, source-derived
composition/mantle paths; `FD` belongs with DPMI machine requirements; and
the remaining host selectors need grouped CLI disposition facades. The `5C`
and `5D` conflict proves that the header comments alone cannot authorize an
implementation.

## Follow-up

T199 S17 must produce a grouped disposition plan: host UI/WOW/debug terminal
forms; idle scheduler form; existing config/machine lifecycle forms; and
DPMI/monitor forms. It must identify which current direct helpers migrate into
the top-level package facade and which remain selector-blind machine calls.
