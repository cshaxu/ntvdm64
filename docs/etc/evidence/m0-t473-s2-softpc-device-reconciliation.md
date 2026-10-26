# M0 T473 S2 — SoftPC device-boundary reconciliation

## Selected original device owners

The worker product selects the original Keymouse, System, BIOS, Disks, Video
and C-VID packages.  The source/build ledger identifies them as six distinct
original libraries rather than optional adapter implementations.  Their
remaining runtime evidence is intentionally marked partial in the S1 owner
matrix; this S2 source review does not convert that limitation into a host
replacement.

| Device area | Original state/algorithm owner | Project boundary | S2 disposition |
| --- | --- | --- | --- |
| PIC, timer and EOI | original System `ica.c`, timer and IRQ consumers | host event/wake transport | Retain original device algorithm.  The ICW1 change is a narrowly sourced device-side cancellation repair: it cancels a pending device EOI request when PIC reset retires IRR/ISR state; it is not a synthetic timer or a new PIC owner. |
| 8042 keyboard and IRQ1 | original Keymouse `keyba.c`/`keybd_io.c` | host input ingress, optional history metadata and reset hook | Retain original queue, output slot, read and EOI algorithm.  The early-EOI repair separates “byte read” from “IRQ acknowledged”, so Win95's EOI-before-0x60 order preserves the same original scan byte.  History carries origin metadata only; it does not create another scan-code ring. |
| Mouse driver/cursor | original `mouse_io.c` guest position, show count, shape and draw/erase code | bounded host-console input queue | Retain.  The bridge owns only host event capacity/retirement; original code remains the guest-coordinate and painter authority.  A route cannot have two active cursor painters. |
| VGA/C-VID/video | original EGA/VGA/V7 registers, planes, glyphs and C-VID tables | copied presentation font/text frame and generated vector binding | Retain.  Presentation copies a selected original snapshot across the worker boundary; it cannot choose modes, alter registers or maintain a second framebuffer. |
| BIOS, ROM and disks | selected original BIOS/reset/resource readers and disk services | immutable media/resource lookup and bounded host I/O | Retain.  Product policy supplies only admitted immutable resources; original BIOS/disk dispatch and error paths remain selected. |

## No hidden second controller

The reviewed adapters carry one of four finite things only: a host input
record, a copied read-only presentation snapshot, an immutable product resource
identity, or reset-time retirement metadata.  None owns a PIC IRR/ISR queue,
keyboard output byte, mouse position/show count, VGA plane/register state,
disk geometry, BIOS interrupt table or timer cadence.  The only device-state
mutations in this group are source-local repairs to the selected original
controller implementations, and each has a named original call-site contract:

- PIC ICW1 clears source PIC state and now sends the matching existing
  device-side cancellation notification.
- Keyboard consumes an output slot only after both source events (data-port
  read and normal EOI) have occurred; status-port reads do not consume data.
- Cursor/VGA route gates preserve the original guest state while preventing a
  host Console arrow/presenter from becoming a competing painter.

These boundaries cannot be replaced by a same-named original file because all
such original device bodies are already selected.  Removing a bridge would
either lose the modern input/presentation endpoint or incorrectly make the
host own guest device state.  Restoring a CPU30 monitor or guest change is not
an allowed alternative.

## Evidence and next boundary

The retained source contracts include the PIC reset/EOI probe matrix,
keyboard controller/PIC early-EOI fixture, mouse state fixtures, C-VID/vector
fixture and original no-media disk paths.  They establish component semantics
but do not replace later real DOS/WOW/Win3.x lifecycle acceptance; that proof
remains S3 work.  This source review identifies no removable autonomous device
engine and no missing original device file.

The next S2 group reviews WOW32/session carriers, where public USER identity
and recursive callback lifetime—not hardware state—are the critical authority
boundaries.
