# M0 T198 S85: BOP-`5F` Host-Contract Delta Map

## Question

Which original host-side effects of `MS_bop_F -> kb_setup_vectors()` are
absent from the current narrow `BOP 5F` continuation, and who must own each
effect in a non-invasive CLI composition?

## Original Contract

`softpc.new/host/src/nt_bop.c` calls `kb_setup_vectors()` for `MS_bop_F`, then
restarts the historical ICA interrupt path and, in the MONITOR form, sets CF.
`base/keymouse/keybd_io.c` consumes the NTIO `kio_table` through guest-memory
access and establishes the following state.

| Historical effect | Evidence | Correct current owner | Current disposition |
| --- | --- | --- | --- |
| CF x86 continuation | `nt_bop.c` MONITOR branch; `spckbd.asm: jc isk_int9` | `bx-vdm` source-derived host-service plane | Present: exact `AX=BEEF`, `C4 C4 5F`, real-mode, `RIP+3`, CF=1. |
| Saved guest INT10/INT42 far pointers | `spckbd.asm: isk_int9` | Guest SpcKbd | Present by normal resumed guest execution; the C000 target is the supplied IVT value. |
| Keyboard table and timer/INT callout addresses | `keybd_io.c: kb_setup_vectors` | Bochs keyboard/PIC/timer machine capability, wired by mantle | Not present. It cannot be reimplemented as a BOP semantic. |
| `useHostInt10` address and state write | `keybd_io.c` obtains the offset from `kio_table`, then writes `getNtScreenState()` | CLI display profile / `bx-vdm` capability boundary, with an explicitly admitted checked guest write | Not present. |
| Windowed/fullscreen/stream selection | `nt_graph.h`: `WINDOWED=0`, `FULLSCREEN=1`, `STREAM_IO=2`; `nt_det.c`, `nt_fulsc.c`, `nt_event.c` write the K.SYS byte | Non-invasive CLI profile policy | Not selected. Historical implementation depends on console registration, window state, graphics and event threads. |
| Re-enable hardware interrupts | `nt_bop.c` calls historical ICA lock/restart | Bochs PIC/lifecycle owner | Not present. No adapter IRQ/PIC implementation is allowed. |

## Interpretation

The present `bx_ntvdm_spckbd_init_service` intentionally carries only the
first row. It is neither a replacement for `kb_setup_vectors()` nor a display
provider. Its result makes the guest execute `isk_int9`, which saves the
fixture's IVT vector; because the resident byte `use_host_int10` remains its
static `FULLSCREEN` value (`1`), the next text output selects the historical
native INT10 far jump to `C000:014A`.

Original NT behavior selected that byte from invasive host-console state:
windowed writes `0`, fullscreen writes `1`, and stream-I/O writes `2`. Those
paths use console/window handles, VDD registration, graphics/regen memory,
event threads and historical PIC control. They are not reusable directly in a
CLI process.

## Architectural Disposition

The required next decision is a **CLI profile contract**, not a Bochs patch:

- A native/fullscreen profile requires an admitted mantle-owned machine image
  with a real executable INT10 target plus the associated native device/PIC
  lifecycle. It is currently unavailable.
- A windowed profile requires historical SoftPC/VGA BOP composition and GUI
  behavior. It is outside the non-invasive CLI objective.
- A stream-I/O/headless CLI profile is structurally compatible with the
  original `STREAM_IO=2` branch, but needs an explicit capability contract
  for the checked K.SYS byte publication and text output sink. It must not be
  silently forced by the BOP continuation.

The Bochs/mantle keyboard, PIC and timer table bindings remain a separate
minimal-machine closure question. No such capability is implied merely by
choosing a CLI display profile.

## Result

S85 closes the source contract map. It does not select or implement a display
profile, publish a guest byte, import a ROM, add a device, or alter BOP-`5F`.
S86 may define the one explicit non-invasive CLI display contract and its
negative/default behavior before any guest-memory write is admitted.

## Confidence

High for the source/current delta and owner classification. Medium for any
future stream profile until its CLI output and checked-write ABI are separately
designed and tested.
