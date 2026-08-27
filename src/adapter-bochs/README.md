# adapter-bochs

The sole production caller of `bochs-core`, owning only Bochs construction and
opaque mechanical lifecycle. Existing project code is recovery evidence until
audited in the Bochs foundation task.

## M0 T273 S1 recovery register

The quarantined 11-file adapter candidate set is project-authored Bochs-only
assembly.  It has no imported Bochs body after the historical port-space
extraction, but must still be reviewed per file before any copy.  The audit
records that each future adapter input may depend only on `bochs-core` and
must not introduce OpenNT, DOS, VDM, WOW, BOP, SoftPC/CCPU or Win32 service
semantics.

## M0 T273 S2 mechanical lifecycle

`bounded_run.{cc,h}` is a project-owned copied-record operation. It uses the
recovered lifecycle facade to place opaque bytes in ordinary RAM, arm a
real-mode entry, stop at a timer budget and release the machine. It is not a
BOP, SoftPC/CCPU, session or service ABI.

`machine_lifecycle.{cc,h}` is the reusable lower-level form of that same
mechanical boundary. It owns one opaque native machine at a time and exposes
only create, copied real-mode load, finite budget run, active query and
destroy. A finite stop clears only the adapter-owned Bochs stop latch before
the next run; it neither resets nor recreates CPU/RAM state.

`machine_facade_resolve_protected_range` is a selector-blind mechanical
preflight. It verifies one current active segment through Bochs' native
access/limit checks and returns only the copied numeric linear address. It
does not return a guest pointer, a Bochs object, a VDM selector meaning or a
host service result; the caller remains responsible for the bounded
session-owned lease.

`machine_facade_resolve_protected_range` is a selector-blind mechanical
preflight. It verifies one current active segment through Bochs' native
access/limit checks and returns only the copied numeric linear address. It
does not return a guest pointer, a Bochs object, a VDM selector meaning or a
host service result; the caller remains responsible for the bounded
session-owned lease.

## M0 T289 S3 protected-machine foundation

`machine_facade` additionally owns a selector-blind protected-machine record:
an inspection-only copied frame, an active segment-descriptor snapshot, and a
bounded protected-span transfer.  A candidate frame may change only copied
GPRs, EIP and the native-supported FLAGS bits; it is accepted only after an
exact comparison with the returned CPU-loop snapshot.  CR0, execution mode,
LDT/IDT state and segment selectors are rejected.  This is a mechanical
adapter contract, not a DPMI route or provider API.

## M0 T289 S3 protected-machine foundation

`machine_facade` additionally owns a selector-blind protected-machine record:
an inspection-only copied frame, an active segment-descriptor snapshot, and a
bounded protected-span transfer.  A candidate frame may change only copied
GPRs, EIP and the native-supported FLAGS bits; it is accepted only after an
exact comparison with the returned CPU-loop snapshot.  CR0, execution mode,
LDT/IDT state and segment selectors are rejected.  This is a mechanical
adapter contract, not a DPMI route or provider API.

`machine_lifecycle.{cc,h}` is the reusable lower-level form of that same
mechanical boundary. It owns one opaque native machine at a time and exposes
only create, copied real-mode load, finite budget run, active query and
destroy. A finite stop clears only the adapter-owned Bochs stop latch before
the next run; it neither resets nor recreates CPU/RAM state.
