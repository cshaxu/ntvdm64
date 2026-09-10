# M0 T245 S4 — Headless mouse-vector owner-package map

## Question

What complete original owner package stands behind the next reached `BOP C8`,
and which subset is required for the first headless source-built profile?

## Original ownership map

| Phase | Original source | Historical behavior | First headless disposition |
| --- | --- | --- | --- |
| Guest setup | `dos/v86/doskrnl/bios/spcmse_4.asm:InstSpcMse` | Builds 84-byte `mio_table`, places its address in `CS:BX`, calls C8, reads the returned INT 33 vector, then installs its guest wrapper. | Guest-owned; do not recreate in bx-vdm. |
| C8 dispatch | `softpc.new/base/bios/bios.c:v_host_mouse_install1` → `softpc.new/host/src/nt_mouse.c:host_mouse_install1` | Sets historical host mouse state, registers EOI hook 9, calls `mouse_install1`. | Direct NT4 host composition is not reusable. |
| First installer | `softpc.new/base/keymouse/mouse_io.c:mouse_install1` | Decodes `mio_table`; writes INT 71/33/possibly INT 10; unmasks PIC lines; resets mouse state; host/UI messages. | Only the four bytes at table offsets 0..3 used to publish INT 33 are presently reached/proven. |
| C9 dispatch / second installer | `bios.c:v_host_mouse_install2` → `nt_mouse.c:host_mouse_install2` → `mouse_io.c:mouse_install2` | Existing-driver notification path. | Preserve explicit no-route until a source-reached caller and failure/continuation contract are mapped. |
| Runtime service plane | `bios.c` B8..BF and `mouse_io.c` | Mouse interrupts, I/O, language/video and callback services. | Separate device/input owner work. No C8 implementation may enable them. |

## Required first-profile contract

The immediate `spcmse_4.asm` continuation reads only the freshly published
INT 33 vector. The minimal source-derived rehost is therefore a single
two-phase transaction:

1. fixed checked read of the first four bytes at guest physical `CS*16+BX`;
2. atomic write of those bytes to IVT INT 33 (`0xCC..0xCF`); and
3. normal `RIP+3` resume.

The first four bytes are the original `mouse_io` offset and original guest DS
segment written by `InstSpcMse`; they are not a host pointer and do not expose
the full table. This preserves the source continuation while avoiding a
mouse implementation.

## Excluded historical behavior

`host_mouse_install1`'s mouse state, EOI hook, PIC mask changes, host input,
GUI/display actions, INT 71, video vector, all B8..BF callbacks, and C9's
existing-driver UI path are not required by the C8 immediate continuation.
They remain explicitly unavailable/deferred until their own source-reached
owner package is admitted. They cannot be placed in bx-core or bx-mantle.

## Dependencies and next implementation package

The current mantle already provides selector-blind checked ordinary RAM and
atomic multi-range write mechanics. The future bx-vdm package needs only a
source-mirror C8 provider plus a named shim for that fixed gather/write
transaction. It must include a C8 positive/negative ABI fixture and a C9
explicit-decline fixture, and must prove that no mouse device/input/IRQ or
host UI becomes active. This is a full **headless mouse-vector** package,
not a trace-selected generic mouse service.

## Confidence

High for C8's required vector handoff: guest source, historical BIOS table,
host source and earlier retained C8 witness agree. Medium for later C9 and
B8..BF routes because they have not been reached in the current source-built
profile; they are deliberately not guessed into this package.
