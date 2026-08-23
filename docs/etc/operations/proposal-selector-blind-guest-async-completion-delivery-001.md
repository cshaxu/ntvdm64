# Proposal: Selector-blind guest asynchronous completion delivery

## Purpose

Recover the mechanical delivery half of the original Redirector asynchronous
completion path.  It must allow bx-vdm to request delivery of an already
completed, copied guest continuation while keeping bx-core and bx-mantle
blind to Redirector, BOP, DOS, ANR and host-process semantics.

## Original source basis

`src/opennt/base/mvdm/dos/v86/redir/int5c.asm:260+` handles the original
network interrupt.  After `SVC_NETBIOS5CINTERRUPT`, it receives a disposition
and copied `ANR`, buffer and optional semaphore 16:16 values; it constructs a
guest far-call frame and returns by IRET. `vrnmpipe.h` defines the retained
`DOS_ASYNC_NAMED_PIPE_INFO` record and `vdmredir.h` serializes completion as
`VR_ASYNC_DISPOSITION`.

## Boundary

- bx-vdm owns copied Redirector completion records and public overlapped-I/O
  waits after admission.
- Original guest `int5c.asm` owns ANR dispatch, guest stack layout and return.
- bx-core/bx-mantle own only a generic mechanical external-interrupt request,
  pending/run/resume and native PIC/IVT behavior. They may not identify vector
  purpose, BOP selector, Redirector or guest callback.

## Admission plan

1. **S1 — source/mechanics audit (closed):** `int5c.inc` and `vdmredir.h`
   prove physical IRQ14 / slave line 6, and the existing native PIC produces
   vector `76h` through ordinary acknowledgement. See
   [S1 mechanics map](../evidence/m0-t253-s1-selector-blind-async-delivery-mechanics-map-001.md).
2. **S2 — minimal mechanical seam:** add a fixed no-payload physical-IRQ
   request/result ABI around the existing mantle PIC lifecycle, with positive
   and negative tests. No BOP/DOS/OpenNT naming enters bx-core/bx-mantle; no
   Bochs exception is selected.
3. **S3 — Redirector completion composition:** recover the retained
   `MAXIMUM_ASYNC_PIPES` queue, descriptor validation, public overlapped I/O,
   serialized completion and `int5c` delivery, then test original failure and
   completion paths as one group.

## Stop rule

If correct delivery needs a Bochs CPU/device semantic change beyond an
existing mechanical owner seam, stop before code and register/seek the
required external-intrusion decision.  bx-vdm may never synthesize an ANR
far-call frame or inject an interrupt itself.
