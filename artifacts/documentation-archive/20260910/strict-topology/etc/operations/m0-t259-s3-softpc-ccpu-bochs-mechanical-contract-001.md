# M0 T259 S3 — SoftPC/CCPU To Bochs Mechanical Contract

## Objective

Recover the reached historical SoftPC/CCPU interface contracts without
importing their CPU/product shell.  The same source spelling remains at the
OpenNT mirror boundary; `bx-vdm` translates it to typed, selector-blind Bochs
mechanics.  This both restores interface semantics and minimizes edits to
OpenNT mirrors and `bx-core`/`bx-mantle`.  These are concurrent gates: a
facade is acceptable only when it retains the reached original interface
shape, ordering, layout and failure result *and* avoids an otherwise
unnecessary edit to either adopted source side.  Interface recovery is not a
permission to substitute a convenient new ABI; low intrusion is not a reason
to leave an available original interface unrecovered.

## Source map and disposition

| Historical family | Original evidence | Required modern contract | Owner |
| --- | --- | --- | --- |
| `host_simulate` | `softpc.new/host/src/nt_cpu.c`, `nt_cprgs.c` | bounded run request → typed stop/resume result; no recursive CCPU loop | `bx-vdm` scheduler over `bx-mantle` run mechanics |
| CCPU `get*`/`set*` register function table | `nt_cprgs.c`, `nt_aorc.c` | copied fixed-width CPU frame, commit only via typed delta | `bx-vdm`; Bochs owns actual register state |
| `sas_load*` / `sas_store*`, `GetVDMAddr` / `Sim32*` | `nt_aorc.c`, `nt_bop.c`, active SoftPC mirrors | checked physical/guest-memory spans and T258 mapping lease | core RAM/mantle mechanics; `bx-vdm` policy/lifetime |
| A20 wrapping | `nt_aorc.c`, XMS `xmsa20.c` | native A20 mechanical state with original query/set results | core/mantle mechanical owner, source-shaped XMS facade |
| `host_cpu_interrupt`, clear/reset | `nt_cpu.c` | typed interrupt/reset request, PIC/CPU delivery in Bochs | Bochs/core-mantle |
| keyboard INT15 / IVT watch | `keybd_io.c`, retained `update_kbd_int15.c` | typed IVT read/write only; public input arrives through later input owner | `bx-vdm` bridge over mechanical memory |
| BIOS selectors `02/06/12` | `nt_bop.c` and retained SoftPC fragments | route in `bx-vdm`; opaque typed request into machine layer | `bx-vdm` route; core/mantle blind |

## Non-negotiable boundaries

1. No CCPU frame, SAS object, BOP selector, DOS term, COMMAND term or SoftPC
   selector enters `bx-core` or `bx-mantle`.
2. Bochs owns instruction execution, exception/IRET, CPU registers, RAM/ROM,
   PIC/IRQ, ports and device mechanics. It receives only typed mechanical
   requests, not historical product-shell callbacks.
3. `bx-vdm` may retain historical function/macro spelling only inside a shim
   adjacent to an imported mirror. It copies state at entry/exit and uses the
   mapping-manager lease for any native pointer.
4. `host_simulate` is not evidence to add a CCPU worker. A bx-vdm session
   worker requests a bounded Bochs slice; BOP/stop return is the modern form
   of the reached scheduling contract.
5. A historical no-op (`host_cpu_reset`, `host_cpu_interrupt` in CCPU builds)
   is not silently upgraded. The caller must prove the Bochs mechanical effect
   it needs, or retain the original no-op/failure disposition.

## Existing source-shaped seams to reconcile in S4

- `softpc_memory_size_shim`: `sas_loadw` and AX result.
- `softpc_int15_watch_shim` and `spckbd_handoff_shim`: IVT and checked store.
- `xms_shim`: A20, checked moves and future opaque UMB physical span.
- `demdasd_ioctl_shim`: historical `host_simulate` call site must become a
  typed bounded-run request or retain source-proven decline.
- current CPU result, machine composition and startup interrupt ABIs: must be
  shown selector-blind and must not become a replacement CCPU frame ABI.

## Verification plan

Each changed seam needs an original call-site map, fixed-width ABI proof,
negative boundary test (no selector/DOS terms crossing into mantle/core), and
a fresh formal Ninja fixture. Native trace is integration evidence only.
