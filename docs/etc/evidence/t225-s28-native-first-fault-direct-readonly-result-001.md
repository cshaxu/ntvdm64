# T225 S28 P3 Native First-Fault Direct/Readonly Result

## Question

Does the explicitly armed, default-off S28 first-fault diagnostic convert the
S27 repeated segment-limit symptom into one typed finite terminal on the
immutable primary source-built guest inputs, without interpreting BOP or guest
service state?

## Inputs

- Formal manifest-verified Ninja root `build/M0-T225-S28/first-fault-r4`;
  MSVC x64 `/MT`, CPU5/Pentium-MMX.  Its fixture closure is recorded in the
  P2 result.  P3 rebuilt the affected formal target incrementally from the
  same manifest after the machine-stage/CLI diagnostic wiring changed.
- Immutable S10 primary input root
  `build/M0-T225-S10/primary-bootstrap-inputs-r2`, including its profile and
  source-built NTIO, NTDOS, COMMAND, and TARGET images.
- The existing paired startup-source environment:
  `NTDOS64_STARTUP_CONFIG_SOURCE=fixture-config.nt` and
  `NTDOS64_STARTUP_AUTOEXEC_SOURCE=fixture-autoexec.nt` below that input root.
- `--instruction-tick-budget 1000000 --observe-first-fault`, once in Direct
  and once in Readonly.  No BOP or generic-UD observation option was selected.

## Procedure

1. Rebuilt `bin/ntdos64-native.exe` from the existing r4 formal Ninja graph.
   A common machine-stage header changed, so Ninja rebuilt its dependent
   bx-vdm library and relinked the target; no product-shell/configuration
   feature was enabled.
2. Verified CLI admission with `--validate-only --observe-first-fault`; it
   reported the new option as selected without arming a run.
3. Re-ran the P2 fixture; its test-owned vector-0 capture and controlled stop
   still exited zero.
4. Ran each immutable primary mode once with the paired startup-source
   environment, captured only disposable build-root stdout/stderr, and
   restored the process environment after each run.

## Observations

| Fact | Direct | Readonly |
| --- | --- | --- |
| Process exit | `1` | `1` |
| First fault | vector `13`, error `0` | vector `13`, error `0` |
| Pre-delivery CS:EIP | `0C41:00002357` | `0C41:00002357` |
| EAX / EFLAGS / mode | `00000000` / `00000093` / `1` | `00000000` / `00000093` / `1` |
| Engine terminal | `3` (machine failure), detail `2` (first-fault stop) | identical |
| Stdout SHA-256 | `693d1f381bc86f210ac6e598c6d93151399f2ea0e1d8256ee9cd7cfeb447fecf` | identical |
| Stderr SHA-256 | `cb8526dc115a44ab0b3511eba6d1c0892fe0534836e1db9aa61bab0198021bc6` | identical |

Both stderr records end with the original Bochs
`read_virtual_word_32(): segment limit violation` line.  Each stdout record
contains exactly the first-fault line and the typed terminal line.  Neither
run emitted a BOP/service identity, copied guest bytes through the diagnostic,
or reached the S27 parent-return marker.

A preliminary P3 invocation without the paired startup-source environment
returned pre-machine composition detail `53`.  That published code denotes
`STARTUP_CONFIGURATION_BIND`, not CLI-standard-stream admission.  It was
therefore rejected as an observation and was not used in the comparison.

## Interpretation And Confidence

High confidence that S28 has established the intended mechanical boundary:
the observer accepts exactly the first non-UD fault, records a fixed typed fact
before normal delivery, and terminates via the existing finite machine path.
Direct and Readonly are byte-identical at this point, so no host-mutation
profile distinction explains the failure.

The typed record does **not** identify whether the missing prerequisite is
incorrect guest/startup state, Bochs segment-access mechanics, or original
exception delivery.  It therefore does not authorize a descriptor, BIOS, PIC,
FPU, timer, guest image, BOP, or provider repair.  The next owner analysis must
attribute `0C41:2357` from source/layout evidence before any implementation.

## Follow-up

S28 P3 completes the diagnostic evidence gate.  Retain its default-off
mechanical observer only as a diagnostic aid.  The next admitted work must be
a source/layout attribution of the typed first failure, not a trace-led BOP
implementation or a guessed machine fix.
