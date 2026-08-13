# M0 T195 S1: Bochs Intrusion Repair Audit

## Method

The audit treated the 58-row external-intrusion register as a ledger, not a
live-patch count. It read each row's status and named source surface, searched
current `src/bochs`, and inspected the S56/S57 generated makefile and final
map. No source, build root, executable, or runtime process was changed.

S56/S57 explicitly compiles `main.o` and `cpu/exception.o` with deferred
startup, BOP catalogue listener, and machine-composition macros. Its map
contains `bx_ntvdm_bop_catalog_v1_observe`, the listener in `exception.o`,
the machine-composition probe in `exception.o`, and the original
`unexpected_int`/`illegal_op_int` island. This proves two live repair targets;
it does not make the other registered items live.

## Per-entry Disposition

| Disposition | Register entries | Evidence and T195 action |
| --- | --- | --- |
| Retain as selector-blind mechanics or embedding | BX-UD-001, BX-BUILD-002, BX-UD-003, BX-ABI-004, BX-ABI-005, BX-MEM-007, BX-MEM-010, BX-ABI-008, BX-MEM-020, BX-ABI-021, BX-MACH-023, BX-MEM-024, BX-IO-025, BX-MACH-026, BX-ABI-027, BX-EXEC-032, BX-BUILD-033, BX-ABI-052, BX-ABI-057 | These are generic event/state transfer, checked RAM, typed result/stop, machine lifecycle, or non-semantic build compatibility. Recheck their exact live subset in S6; do not migrate guest meaning into them. |
| Retain default-off pending or diagnostic-only | BX-ABI-006, BX-EXEC-014, BX-EXEC-016, BX-TRACE-017, BX-ABI-018, BX-ABI-019, BX-ABI-022, BX-ABI-015, BX-ABI-028, BX-IO-029, BX-IO-030, BX-EXEC-034, BX-TRACE-035, BX-ABI-036, BX-ABI-037, BX-TRACE-037, BX-TRACE-038, BX-TRACE-039, BX-ABI-041, BX-ABI-042, BX-TRACE-048, BX-TRACE-049, BX-TRACE-050, BX-TRACE-051, BX-TRACE-056 | Register rows are approved/pending or non-semantic diagnostics. They are not selected by the S56 makefile except its generic vector diagnostic. S5 decides whether a still-needed default-off diagnostic remains justified. |
| Move to adapter composition | BX-ABI-040, BX-TRACE-043, BX-TRACE-044, BX-TRACE-045, BX-TRACE-046, BX-TRACE-059, BX-TRACE-060, BX-TRACE-061 | These are the BOP-catalogue/listener family. Current `exception.cc` includes adapter catalogue code, obtains identity from copied instruction bytes, and formats selector/family/service output. The equivalent observer belongs in adapter after a generic typed event is delivered. |
| Move to adapter machine-composition plane | BX-ABI-053, BX-ABI-054, BX-TRACE-055, BX-ABI-058 | The current Bochs probe supplies mechanics then calls an external component which selects historical handlers, including 02h/06h. Selection and outcome interpretation must move under adapter ownership; Bochs retains only opaque request/result mechanics. |
| Remove / non-live historical record | BX-MACH-009, BX-TRACE-047 | The register itself records rejection/removal and no remaining source patch. Preserve their evidence rows; do not treat them as source work. |

Every one of the 58 register identifiers is present exactly once in the five
groups above. The 17 entries whose text says implemented are not a separate
live count: several are generic infrastructure, while S56/S57's map proves
only the selected closure's actual object surface.

## Repair Groups

1. S2 defines the adapter-owned event observer and the opaque machine request/
   result contract, preserving current fixed-width state and checked RAM
   mechanics.
2. S3 removes catalogue/listener identity and selector/service formatting from
   Bochs, placing it in the adapter observer.
3. S4 moves 02h/06h selection and historical-handler arrangement under the
   adapter machine-composition plane, leaving Bochs selector-blind.
4. S5 removes the superseded Bochs hooks and decides the remaining default-off
   diagnostics individually; S6 rebuilds and proves the resulting surface.

## Limits

This is a disposition audit, not a claim that all retained mechanics are
currently required by one runtime profile. No BOP provider behavior, guest
semantics, firmware/device feature, or runtime result was tested. Any source
move requires a separately admitted implementation S.
