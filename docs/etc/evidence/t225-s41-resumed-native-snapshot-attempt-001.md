# T225 S41 Resumed Native Snapshot Attempt 001

## Question

After S40 repaired the normal-loop watchdog path, can the one admitted paired
v2 BOP snapshot observation complete under the existing 1,000,000-tick budget?

## Inputs and procedure

The formal S40 native executable and immutable S10 inputs were invoked once
in Direct mode with `--observe-bop-sequence` and budget 1,000,000. The
sequential Readonly invocation was configured to start only after Direct
returned.

## Observations

- Direct repeatedly emitted `read_virtual_checks(): read beyond limit` and
  exceeded the expected native budget terminal. Its log counter reached
  `00182227518`; it was stopped to prevent unbounded disposable-log growth.
- Direct log: 116,323,065 bytes, SHA-256
  `CF2FB24FF6C736D81CD6D1D6807F8DA374E0CF5766D9D3BB0898514DDC6C5698`.
- Stopping Direct allowed the outer sequential wrapper to start Readonly. It
  displayed the same non-convergent behavior and was immediately stopped.
- Readonly log: 37,731,183 bytes. It is an interrupted follow-on, not a
  completed paired result. Neither run reached BOP presentation or a terminal
  result, so no snapshot, `50:36`, parity or COMMAND conclusion exists.

## Interpretation

S40's normal instruction-loop fixture and one-tick CLI pair passed. This
attempt proves a distinct limitation: exception handling returns through
`longjmp`; its time-accounting/async-stop path does not yet deliver the
existing kill latch to the CPU-loop exit gate. This is a mantle/core mechanical
lifecycle diagnosis, not BOP/provider/guest behavior and not permission for a
trace-led repair.

## Follow-up

Keep S41 active only as the recorded failed observation. Before another native
attempt, admit a separate minimal exception-loop watchdog delivery audit and
correction. Do not rerun S41 or modify BOP semantics.