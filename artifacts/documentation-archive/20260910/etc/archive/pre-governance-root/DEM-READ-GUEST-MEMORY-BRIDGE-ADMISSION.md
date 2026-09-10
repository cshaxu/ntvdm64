# DEM Read Guest-Memory Bridge Admission

## Historical Shape

The original normal-file read path in `demhndl.c` does this:

```text
DS:DX + CX -> GetVDMAddr -> host ReadFile writes that pointer
                         -> Sim32FlushVDMPointer
                         -> Sim32FreeVDMPointer
```

`Sim32FlushVDMPointer` in `softpc.new/host/src/sim32.c:177-193` converts the
guest address and calls the SAS overwrite notification.  Its matching
`Sim32FreeVDMPointer` at lines 196-200 is explicitly a no-op because the old
host allocated no temporary range.  This is an internal historical SAS
pointer convention, not an acceptable x64 core/mantle ABI.

## Decision

The bridge must never give host file I/O a writable guest RAM pointer.  A
future backend provides one bounded write transaction at a returned BOP/trap
execution boundary:

```text
begin_guest_write(address-reference, requested bytes)
    -> owned staging buffer, capacity, opaque transaction
host read fills only staging buffer
commit_guest_write(transaction, actual bytes)
    -> all actual bytes visible, or none
abort_guest_write(transaction)
    -> no guest-memory change
```

The transaction is only for a single synchronous DEM invocation.  It cannot
survive resume, reset, fault, cancellation, session teardown, another trap,
or another begin call.  Neither the historical module nor an external wrapper
receives a raw RAM pointer, a mapping base, or a provider callback.

## Required Semantics

1. `requested == 0` is valid: begin may return a zero-capacity transaction;
   commit with zero succeeds without guest mutation.  Address translation is
   still validated so that a zero-length operation cannot hide malformed
   segment/mode state.
2. Before the host read begins, begin validates the complete requested span,
   including overflow and every translation/mapping boundary.  It rejects a
   ROM, device-memory provider, hole, inaccessible range, or an address whose
   CPU-mode interpretation is unavailable.
3. The address reference is copied from the trapped CPU state, not inferred
   from an old host pointer.  `demRead` initially supplies `DS:DX`; the
   executor/backend determines whether that is real, V86, or protected-mode
   semantics and applies its normal A20/segment/paging route.
4. Begin must prove that the requested range is ordinary writable RAM and
   stable for the returned paused boundary.  It allocates host-owned staging
   storage after validation, leaving guest memory unchanged if allocation
   fails.
5. The file adapter writes only into the staging storage and reports
   `actual <= requested`.  A host-read error, cancellation, or adapter error
   invokes abort and leaves every guest byte unchanged.
6. Commit accepts only `actual <= capacity`; it validates its transaction
   epoch and writes exactly that prefix through the machine's frozen checked
   route.  The full prefix is prevalidated before the first guest mutation. A
   commit failure leaves all guest bytes unchanged and surfaces a bridge error
   for original `demClientError` handling.
7. Commit then invalidates the transaction.  Double commit, double abort,
   stale epoch, or use after resume/reset is rejected without guest mutation.

This is a write transaction, not an instruction fetch permission, memory map
editor, direct DMA endpoint, or generic raw-memory lease.

## Ownership

| Concern | Owner |
| --- | --- |
| Segment/mode/paging/A20 translation and frozen mapping query | execution backend / core machine |
| Proving ordinary writable RAM and all-or-none commit | core machine |
| Creating/invalidating the trap-scoped transaction | mantle/backend adapter |
| Host-owned staging buffer and file read | contained DEM file adapter |
| `DS:DX`, `CX`, register result and `demClientError` | original DEM bridge |
| DOS SFT/PSP/file position/EXEC | original NTDOS |

The split is deliberate.  The token/file adapter cannot know CPU translation;
the core cannot know DOS SVC selectors, DOS paths, or host file policy.

## Current Core Evidence And Gap

The current `ntvdm64` core contract already has checked physical
`core_machine_memory_read/write`, applies A20, and rejects raw writable RAM
pointers.  It also freezes device-memory providers.  These are necessary
preconditions, not proof of this transaction: the public documentation does
not yet state a trap-scoped, full-span ordinary-RAM preflight plus atomic
multi-byte commit guarantee, nor does it expose the required copied
segment/mode translation result to an external adapter.

Therefore M29 identifies a future neutral core/mantle delta but authorizes no
`ntvdm64` source modification and no local alternate executor.  The M27
fixture remains a host-file proof only; it cannot write guest memory.

## Required Fixtures Before Runtime Admission

1. exact ordinary-RAM span success, including a zero-length request;
2. address-plus-length overflow and range-end rejection with no mutation;
3. range crossing ordinary RAM into ROM, device mapping, or a hole rejection
   with no mutation;
4. host read failure after successful begin produces abort and no mutation;
5. partial host read commits exactly the returned prefix;
6. injected commit failure preserves the entire original range;
7. stale/double commit and reset/resume invalidation preserve guest memory;
8. real-mode and each eventually admitted protected/V86 translation form use
   the backend's normal address route, never a mantle reimplementation.

## Outcome

M29 is complete as a neutral contract admission.  The next ROI item is not to
implement a local CPU or memory subsystem.  It is to reconcile the fixed NT4
first-command path with the already-existing BYOB profile loader and define
the exact manifest-to-NTIO/NTDOS load ownership that an eventual backend will
consume.
