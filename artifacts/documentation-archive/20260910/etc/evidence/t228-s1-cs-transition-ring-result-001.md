# T228 S1 P11 - scalar CS-transition ring result

## Question

Can a default-off, scalar-only mantle diagnostic retain enough ordered CS
transitions to identify the boundary from the intact initial COMMAND transient
image to the later dynamic transient execution area, without adding BOP,
adapter, guest-memory, or CPU semantics?

## Inputs

- P9/P10 immutable source-built COMMAND continuity evidence.
- Current-source MSVC x64 `/MT` CPU5 diagnostic graph at
  `build/M0-T228-S1/cs-transition-ring-20260818a` with instruction history
  enabled and provenance disabled.
- Hash-locked T228 guest input root
  `build/M0-T228-S1/guest-exec-inputs-20260817a`, manifest SHA-256
  `c888239b9de415f5c5f38b8d8dcacf5a30c102ab1465189c1ffc1a97fb1b9b6a`.

## Procedure

The admitted mantle ring stores no bytes and uses only already copied scalar
instruction-history records.  Its fixed capacity is sixteen chronological
previous/current CS-transition pairs.  The existing budget-terminal stage
copies that ring only after its existing watchdog result.  The CLI exposes it
only through `--observe-budget-terminal-cs-transitions`.

The focused CPU5 fixture checked disabled unavailability, watchdog capture,
ring ordering, wrap retention, and reset clearing.  The paired native run used
a 1,000,000-tick budget in Direct and Readonly with only the explicit ring
option enabled.  The two stdout streams had SHA-256
`ae98db5e71c75ccb5e588de52267bef6607ea838b094a94c4cdbbba6422d2139`.

## Observation

The focused fixture exited zero.  The paired native observations each reached
the same execution-budget terminal and retained sixteen transitions.  Their
output was byte-identical.  The relevant ordered entries are:

1. `0041:5422` to `0C41:8048` at sequence 886576--886577, still in the
   initial source-built COMMAND transient placement;
2. later `0041:5422` to `0C41:25A9` at sequence 886807--886808;
3. immediately after, `0C41:25A9` to `2073:7365` at sequence 886808--886809.

The final entries remain in the `2073` segment.  No ring record carries guest
bytes, decoded instruction identity, selector/service identity, host route,
or mutation-mode distinction.

## Interpretation

This is the first ordered scalar evidence connecting the intact initial
COMMAND transient domain to the later dynamic `2073` execution domain.  It
confirms that the candidate repair remains the complete original COMMAND
transient allocation/move/reload lifecycle with the guest NTDOS allocator and
control-transfer state.  It neither identifies a BOP provider defect nor
proves a particular allocation, move, checksum, or reload branch failed.

## Limitation and follow-up

The ring deliberately cannot explain the instructions or guest-memory values
which produced the `2073` target.  It must not be expanded into byte capture,
selector filtering, BOP dispatch, or adapter policy.  Any repair requires a
source/ABI/failure map for the complete COMMAND/NTDOS transient lifecycle
owner domain and an admission independent of this trace.

## Verification status

- Focused explicit-history CPU5 fixture: passed.
- Paired immutable Direct/Readonly observation: passed with byte-identical
  output and identical execution-budget terminal.
- Fresh default formal Ninja graph: passed all 433 declared targets under
  `build/M0-T228-S1/formal-default-20260818c`, with no history diagnostic
  source added to the default graph.
- Focused fixture from that default graph: passed with both history and the
  new terminal transition copy unavailable.