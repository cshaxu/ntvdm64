# T225 S27 P5 Native Parent-Return Observation Attempt

## Question

Does the freshly built native CLI, using immutable source-built guest inputs,
reach a natural COMMAND parent-return `54:0B` observation under a bounded
one-million-tick Direct run?

## Inputs

- Fresh formal Ninja/MSVC x64 `/MT`, CPU5/P-MMX graph:
  `build/M0-T225-S27/native-parent-return-r5` (307 graph nodes).
- Its `bin/ntdos64-native.exe` native CLI target.
- Immutable primary source-built input root:
  `build/M0-T225-S10/primary-bootstrap-inputs-r2`.
- Existing paired startup configuration and autoexec files from that input
  root.
- Direct mutation mode, existing BOP and generic-UD observations, and the
  existing `--instruction-tick-budget 1000000` CLI option.

## Procedure

After the formal target linked, one Direct invocation used only the existing
profile/root, startup sources, target identity, observers, and internal
one-million-tick watchdog. Its stdout and stderr were redirected to the
same disposable build root. No guest bytes, profile, provider, Bochs feature,
or source file changed.

The child did not print a CLI terminal. It emitted repeated
`read_virtual_checks(): read beyond limit` diagnostics and continued past the
point at which this observation remained bounded. After 182 seconds, with
9,576,436 stderr bytes and zero stdout bytes, the coordinator terminated only
the exact PID it had started. `direct.attempt.json` in the disposable run root
records start/end time, PID-local termination, sizes, and output hashes.

## Observations

| Item | Result |
| --- | --- |
| Formal native CLI build | linked successfully, 307/307 graph nodes |
| Direct CLI terminal | absent |
| Direct stdout | 0 bytes; SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |
| Direct stderr | 9,576,436 bytes; SHA-256 `ef6f0d1af8f41be0b86e0ffc8bd1ec4f0943b9f1a44796134f0eadfe77bb2024` |
| Reached `54:0B` | not observed or claimed |
| Readonly companion | not run: repeating an unbounded diagnostic stream is not valid parity evidence |

## Interpretation

This is not a COMMAND parent-return result and does not identify a BOP to
implement. The existing one-million-tick native run is not presently a useful
bounded observation on this input: it neither returns through its advertised
CLI terminal nor reaches the required original guest parent-return path.

The first next task is a source-and-mechanics diagnosis of the repeated
`read_virtual_checks` path and the watchdog/lifecycle interaction. It must
classify the actual owner before any change: guest execution state, native
Bochs memory/CPU mechanics, mantle timer/run-control, or an already-admitted
provider result. It must not infer an owner from the text diagnostic, add a
BOP handler, synthesize a guest continuation, or run Readonly as a duplicate
until a bounded Direct execution contract exists.

## Follow-up

S27 P1--P3 remain valid component evidence. P4's required real guest path has
not been reached. S27 stays active, but its native exit criterion is currently
blocked by the newly observed bounded-run failure. A revised owner-scoped S
brief and renewed approval are required before implementation work continues.
