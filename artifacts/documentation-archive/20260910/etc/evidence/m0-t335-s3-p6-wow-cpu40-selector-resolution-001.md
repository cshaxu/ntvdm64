# M0 T335 S3 P6 — WOW CPU40 selector-resolution recovery

## Decision

The WOW32 direct pointer family must resolve an original protected-mode
selector that is not one of the current CCPU segment registers. The prior
`mvdm_softpc_effective_address` adapter declined that valid CPU40 case. It
now preserves the original numerical selector:offset result by calling
CCPU40's `selector_outside_GDT_LDT` and `read_descriptor_linear` routines.

## Boundary

- Real/VM86 conversion remains `selector << 4 + offset`.
- A loaded segment still uses the CPU40 segment cache first.
- Other protected-mode selectors use the original CPU40 GDT/LDT descriptor
  walker and return `CPU_DESCR.base + offset`.
- Invalid selectors still decline. No CPU30 monitor table, raw SoftPC alias,
  new mapping manager, or WOW-specific pointer resolver is introduced.

This is a same-shaped replacement for the CPU30 `DESCR/read_descriptor`
carrier in `sim32.c`. `MVDM-HOST-DIV-177` records the mirror binding change.
The result stays a guest-linear scalar: a caller needing bytes must still take
a bounded synchronous lease through `adapter-mvdm-host-out/softpc`.

## Verification

After adding the already-required `mvdm_softpc_guest_memory.c` edge to the
focused lifecycle graph, its fresh x86 build completed all 97 actions and the
fixture exited zero. Its original CCPU40 run reached `returned-start` and
`returned-recursive`.

This does not enable the full WOW pointer plane or an arbitrary protected-mode
callback; those remain S3's next bounded frame work.
