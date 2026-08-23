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

1. **S1 — source/mechanics audit:** map `int5c.asm` call-frame and native
   Bochs/PIC interrupt injection candidates; determine whether an existing
   selector-blind mantle request can deliver a pending external interrupt.
2. **S2 — minimal mechanical seam:** only if S1 proves a finite existing
   machine operation or a documented minimum Bochs exception.  Define a
   fixed-width opaque interrupt request/result ABI with positive and negative
   tests; no BOP/DOS/OpenNT naming enters bx-core/bx-mantle.
3. **S3 — Redirector completion composition:** recover the retained
   `MAXIMUM_ASYNC_PIPES` queue, descriptor validation, public overlapped I/O,
   serialized completion and `int5c` delivery, then test original failure and
   completion paths as one group.

## Stop rule

If correct delivery needs a Bochs CPU/device semantic change beyond an
existing mechanical owner seam, stop before code and register/seek the
required external-intrusion decision.  bx-vdm may never synthesize an ANR
far-call frame or inject an interrupt itself.
