# M0 T199 S17 Top-Level Grouped Disposition

## Decision

The remaining top-level selectors are split by owner, not by trace order.
Every group enters through common ingress before a provider decision. Existing
direct helpers are migration inputs, not permanent bypasses.

| Group | Selectors | Selected owner and disposition |
| --- | --- | --- |
| CLI host terminal | `51,59,5B,FE` | One `bx-vdm` top-level facade returns typed controlled stop. WOW/UI/debug host components are absent; `FE` is CLI terminal lifecycle. No UI, WOW or debugger emulation. |
| CLI idle | `5A` | Same facade returns `RIP+3` with no register/flag mutation. The current profile does not schedule a host idle wait; this is a bounded source-shaped no-scheduler continuation. Existing `wait_if_idle` becomes facade implementation input. |
| Adapter composition notification | `5E` | Same facade routes only the existing `AL=0` config-complete continuation. It remains a partial CLI composition boundary: no UMB allocator or DASD initialization claim. Existing config helper moves behind common ingress. |
| Mantle machine lifecycle | `5F` | Retained `spckbd` bridge remains a source-defined NTIO/machine handoff. It may use checked RAM action composition but does not put keyboard/PIC/BIOS semantics into bx-vdm. Its direct recognizer must be wrapped by package routing, not duplicated. |
| Monitor/DPMI machine transition | `FD` | Deferred behind C4/C5/C6 in the shared XMS/DPMI capability plan. No adapter transition implementation. |
| Variant-ambiguous monitor forms | `5C,5D` | Controlled stop/deferred, pending a source-identified active product variant. Header labels conflict with NTVDM BIOS binding; no speculative behavior. |

## Boundary Consequences

`bx-core` remains selector-blind. `bx-mantle` may own the selector-blind
machine action needed by `5F` and later `FD`, but must not recognise BOP
names. `bx-vdm` owns all selector routing, the terminal/no-scheduler results,
and the adapter composition of the existing config/keyboard boundaries.

## Migration Rule

S18 creates one facade for the CLI host-terminal, idle, config and machine
handoff group. It must remove their direct execution bypasses from the
composition handler. `5C`, `5D`, and `FD` are not admitted to that facade;
their ingress disposition remains deferred/controlled stop until the relevant
machine package closes.
