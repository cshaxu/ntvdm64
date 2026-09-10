# T95 S7 DEMFASTREAD Contained Contract 001

> Superseded by `t97-s2-immutable-namespace-token-contract-001.md`. Direct
> `DemDispatch` table evidence shows `SVC_DEMFASTREAD` is
> `demNotYetImplemented`; this experiment's helper and test assertion were
> withdrawn and must not be re-enabled.

## Question

Can the reached `50:42` BOP be restored as a distinct, bounded adapter service
without changing the DOS-kernel fast/slow branch or treating it as a Bochs
feature?

## Inputs

- The retained trace 009 BOP event (`50:42`, `AX=4001`, `BP=4e53`).
- `dos/v86/doskrnl/dos/handle.asm:447-579`.
- `dos/v86/inc/dosmac.inc:1167-1201`.
- `dos/dem/demhndl.c:93-224`.
- The existing first-profile namespace and generic bulk-result contracts:
  `bx_ntvdm_dem_readonly_file_service.[ch]` and
  `bx_ntvdm_bulk_result_transaction.[ch]`.

## Source Comparison

Both call sites receive the same prepared state from `handle.asm`:

| Fact | `SVC_DEMFASTREAD` (`42h`) | `SVC_DEMREAD` (`16h`) |
| --- | --- | --- |
| File token | `AX:BP` from the SFT NT-handle fields | unchanged |
| Requested bytes | `CX` | unchanged |
| Guest destination | `DS:DX` | unchanged |
| Seek position | `BX:SI`, with ZF set when seeking is unnecessary | unchanged |
| Guest success branch | `AX` is added to the SFT position; CF clear continues | same |
| RAM effect | requested bytes are copied to `DS:DX` | `demRead` explicitly does that |

The only visible caller-side difference is that slow read uses `HRDSVC`, whose
macro invokes `TestHrdErr` after CF set, while fast read uses raw `SVC`.
The contained namespace cannot represent pipes, redirectors, host handles,
asynchronous I/O, media changes or a DOS hard-error condition; its rejected
inputs already return a normal direct DOS-style error. Consequently no
hard-error/retry path is reachable for the narrowly admitted profile.

## Decision

Admit a distinct `50:42` adapter endpoint for the installed private read-only
namespace only. It uses the already approved generic bulk-result transport,
but is not an alias in the dispatcher: it validates the exact `42h` instruction
window and has its own source-derived test cases. Its result is the proven
ordinary file-read success contract: one bounded write to `DS:DX`, actual byte
count in AX, and CF clear. EOF remains the existing zero-payload success.

It must pass through if any precondition is outside the current namespace
(wrong selector/mode, stale token, invalid physical range, oversize request,
or unavailable session). It must not synthesize CF merely to enter `DEMREAD`,
change the MIPS bit, implement fast write, add a host filesystem, or modify
Bochs. This is a rehosted missing host-composition endpoint, not recovery of
an unavailable historical C implementation.

## Verification Plan

Extend the existing focused service test with a `50:42` read of the contained
COMMAND payload, verifying copied payload, single generic bulk transaction,
AX count, CF clear and an exact-selector negative case. Then rebuild the
focused C11 target and the adapter runtime test. A later fresh Bochs root and
one watchdog trace are required before any guest-runtime claim.
