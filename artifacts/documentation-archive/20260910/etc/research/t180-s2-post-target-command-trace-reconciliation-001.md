# T180 S2 post-target COMMAND trace reconciliation 001

## Exact retained trace

The immutable T179 log has SHA-256
`565DED1A18826A0AF7F7A8C8CDBD5D0248E492F09D3718C5823FDE43601BE1BF`.
It records two semantically distinct occurrences of the exact
`C4 C4 54 01` window at `95AB:03C1`:

| Occurrence | Trace result | State-machine correlation |
| --- | --- | --- |
| First, before target execution | Gather reads one COMMAND buffer, then a 12-write transaction commits and resumes at `03C5`. | `delivered == 0`: the declared target launch path is accepted and commits `delivered = 1`. |
| Second and later, after SHARE termination/teardown | The adapter records boundary pass-through; #UD vectors to source-defined INT 06; the machine island reports the faulting `54:01` bytes and handles it. The trace repeats this post-target request and records zero `54:11`. | `delivered == 1`: `54:01` prepare declines exactly as the source declares. The machine component only preserves original invalid-opcode continuation; it does not provide COMMAND semantics. |

There are 88 machine reports/outcomes for selector `06h` and zero stack-prefetch
faults. This proves the machine closure is active, but not a successful second
COMMAND request or a normal DOS result.

## First unclosed semantic edge

The first unclosed edge is the post-target **second `54:01` command-acquisition
request**. It needs a coherent disposition for one of two product-level
policies:

1. an immutable, explicitly declared next target, with the full launch buffer
   contract again; or
2. an independently source-proven CLI no-next terminal contract.

Neither is supplied by the historical `cmdGetNextCmd` implementation, which
instead enters its NT command broker/environment/console composition. Returning
CF clear with empty or synthetic buffers would invent behavior; returning the
captured target DX code here would conflate `54:01` with `54:11`.

## Exclusions

This is not a Bochs exception defect, machine BOP dispatch request, DEM
provider failure, `54:11` result, or process-exit result. The adapter's
one-target gate is therefore correct as a safety boundary, although it makes
the broader post-target lifecycle incomplete.
