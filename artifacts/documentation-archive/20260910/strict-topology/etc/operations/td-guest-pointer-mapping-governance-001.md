# Td — Guest-Pointer Mapping Governance

## Decision

`src/bx-vdm` owns one session-scoped guest-pointer mapping manager. It is the
only compatibility implementation for historical OpenNT guest-pointer APIs:
`GetVDMAddr`, `Sim32GetVDMPointer`, `Sim32FlushVDMPointer`, and
`Sim32FreeVDMPointer`.

The manager preserves the original pointer-returning source shape when an
admitted, synchronous OpenNT call needs it. It maps a guest `16:16` or
`32-bit` linear address to a bounded mapping lease and may return the native
host pointer to the imported source body. This is an internal `bx-vdm` call
fact, not a guest ABI or a Bochs/OpenNT bridge value.

## Required Contract

Each lease records:

- owning VDM session and active-call epoch;
- guest addressing form and checked address/span;
- read/write access and direct-map or copied/bounce disposition;
- native pointer lifetime and one teardown path; and
- any checked write-back/failure result.

Direct mapping is allowed only while the corresponding BOP/OpenNT call is
synchronous and the machine RAM backing is stable. The returned native pointer
may be used by the imported body and synchronous public Win32 calls, but must
not enter guest memory, a fixed-width adapter result, a Windows `HANDLE` map,
an asynchronous worker, a callback retained after the call, or a Bochs
object. Calls that retain a pointer, are asynchronous, cannot establish their
access span, or depend on unavailable protected-mode mapping semantics use a
copied/bounce lease or fail explicitly.

The manager may share a session resource-table implementation with host-handle
and child/event ownership. Its guest-pointer leases remain a separate resource
kind: a guest address is never a host handle, and a host pointer is never a
guest token.

## Recovery Rule

The historical function/macro name and parameters are preferred over a new
family-specific `bx_ntvdm_*` pointer interface. Existing DEM, COMMAND, XMS,
DPMI, Redirector, Debugger, and machine shims must migrate to this manager when
their owner package is next admitted. A different pointer API needs a recovery
exception that proves why the shared historical-shaped facade cannot represent
the reached call.

## Closure Evidence

A task that composes a pointer-using OpenNT body records the manager's lease
kind and validates:

1. correct mapping for its admitted guest address/span;
2. rejection of an out-of-range or expired mapping;
3. no host pointer/token reaches guest state or the bx↔machine ABI;
4. direct-map teardown before resume; and
5. copied/bounce write-back or explicit refusal where direct mapping is not
   admitted.

This Td changes no active numeric packet and does not itself admit a new BOP
or machine capability.
