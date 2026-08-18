# T228 S1 P13 — COMMAND Initial-Environment Size Boundary Result

## Question

Does the current source-derived `54:02`/`54:0F` COMMAND bootstrap provider
misproject a particular host environment entry, or does the demonstrated
failure follow the original guest's environment-size allocation path?

## Fixed Baseline

Every valid run below used the same current-source native executable,
hash-locked source-built NTIO, NTDOS, COMMAND and SHARE-as-`TARGET.EXE`
inputs, Direct profile, one-million instruction budget, BOP sequence observer
and guest lifecycle ledger. No guest input or product source changed.

The existing formal family fixture already proves the provider's relevant
mechanical contract: a copied host environment larger than 4 KiB enters the
same `54:02` gather, `54:0F` paragraph-size/retry and one checked guest-RAM
write path as a small environment. It is not a 4,023-byte implementation.

## Observations

| Host environment form | Second `54:0F` BX | Result |
| --- | ---: | --- |
| Retained minimal host environment | `0013h` | Reaches child entry, normal `INT 21h/4Ch`, `50:3C`, and normal terminal. |
| Ordinary host environment | `010Eh` | Reaches only the parent `50:36`; later transfers into the invalid dynamic transient path and ends at the bounded budget terminal. |
| Controlled environment: required CLI/startup values plus four distinct 1,000-byte values | `0121h` | Reproduces the same parent-only lifecycle and bounded budget terminal. |

The controlled run has no ordinary host-environment entries beyond the CLI
admission/startup values and the four opaque large values. It therefore
rejects a content-specific interpretation. The two rejected child processes
created while discovering the CLI's absolute-target admission requirement are
not evidence and are not included in the table.

## Original Source Map

`src/opennt/base/mvdm/dos/command/cmdenv.c:179..324` builds the filtered OEM
environment, reports `ceil(bytes / 16)` in `BX` when the first buffer is too
small, and writes only after the retry has adequate capacity.

`src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm:537..574` allocates the
initial `EnvSiz` paragraph block, calls `SVC_GETINITENVIRONMENT`, releases and
reallocates when returned `BX` is larger, then requires the retry to return
the same `BX`. Afterward `rdata.asm:623..673` obtains the largest DOS block,
moves the transient to its top, and frees it for the resident's later
allocation. Those operations are original guest COMMAND plus original guest
NTDOS MCB allocation behavior; they are not BOP-provider work.

## Attribution And Next Repair Boundary

The current provider's size/retry transaction is source-shaped and has a
separate greater-than-4-KiB focused regression. The runtime boundary is now
the complete original guest **COMMAND environment-reallocation / NTDOS MCB /
transient placement** domain. No adapter environment truncation, BOP leaf,
synthetic guest write, or Bochs address-specific change is admitted.

The next repair packet must inspect the original guest memory-allocation and
transient-placement state across the first and second `54:0F` calls, then
repair only a demonstrated owner-domain defect. Trace is evidence for that
package; it does not select a new service task.