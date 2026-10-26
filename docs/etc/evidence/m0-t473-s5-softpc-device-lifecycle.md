# M0 T473 S5 — SoftPC device-owner runtime closure

## Decision

The selected SoftPC PIC, keyboard, mouse, VGA/C-VID, BIOS and disk packages
remain their respective device-state and algorithm owners.  Current project
code is confined to input/output/resource transport and two explicit repairs
inside original device flows.  No replacement controller, timer, keyboard
ring, cursor painter, framebuffer, BIOS dispatcher or disk geometry provider
was found.  This is a zero-product-diff packet.

## Device authority table

| Device | Original authority | Modern boundary | Lifecycle conclusion |
| --- | --- | --- | --- |
| PIC/timer/RTC | System `ica.c`, timer and CMOS source | host wake/EOI callback only | ICW1 reset clears original IRR/ISR/count state, then issues the existing `host_EOI_hook(line,-1)` cancellation for each retired device line. This cancels device-side wait state; it does not inject an IRQ or replace PIC arbitration. |
| 8042/keyboard | Keymouse `keyba.c`/`keybd_io.c` | queued host key ingress and optional origin metadata | output slot is released only after both original data-port consumption and normal EOI acknowledgement. Status-port reads are not consumption. This preserves normal read-then-EOI and early-EOI-then-read orders without a second scan-code queue. |
| Mouse/cursor | selected mouse I/O code | bounded frontend input records | original code owns coordinates, buttons, show count, guest state and drawing; host has no second cursor state/painter. |
| VGA/C-VID | selected EGA/VGA/V7 ports, planes, fonts and C-VID tables | copied presentation snapshot/vector binding | producer state remains original. Transport copies a snapshot and never chooses mode/register/plane values. |
| BIOS/ROM/disks | selected BIOS/reset/resource and disk service bodies | immutable product resource/media resolution | project gives named immutable roots and bounded I/O only; original dispatch/error path stays authoritative. |

## Repair review

The PIC repair is deliberately in original `SWPIC_outb` ICW1 handling.  It
first computes the original controller state to retire, runs the pre-existing
initialization reset, and then uses the pre-existing `host_EOI_hook` cancel
operation.  Its companion `SWPIC_clear_int` already carries the same cancel
meaning.  Thus pending IRR and in-service ISR device work cannot survive a
PIC reset as stale timer/RTC/keyboard EOI state.

The keyboard repair keeps two source events—0x60 data read and IRQ1 EOI—as
separate facts around the original one-slot `output_full` state.  It does not
count a 0x64 status read as data consumption, fabricate a second IRQ, or
alter 8042 direct replies.  The existing order-sensitive state allows both
ordinary DOS read-before-EOI and the VKD/VPICD early-EOI order.

## Evidence and limits

The formal generator wires `pic-reset-eoi-test`,
`keyboard-controller-pic-test`, `relative-mouse-bridge-test` and
`cvidc-vector-binding-fixture` against the selected original package
libraries.  The retained PIC probe matrix records pending, in-service, empty,
master/slave and repeated-reset contracts; the retained keyboard test records
46 assertions for normal and early EOI ordering.  Existing mouse/C-VID and
no-media routes cover the corresponding bounded transport contracts.

No source changed in this packet.  The current formal cache composition issue
recorded by S3 prevents treating a new cached build attempt as evidence; the
retained focused results are unchanged rather than claimed fresh.  Real DOS,
WOW and Win3.x application acceptance remains the later integrated packet,
not a device-owner assertion.

## Result and handoff

There is no source-shaped device recovery to perform.  S6 takes the distinct
WOW32/session package, where public USER identity and recursive callback
lifetime must be reviewed independently of device state.
