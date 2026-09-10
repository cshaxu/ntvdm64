# T225 S20 COMMAND Native Package Validation Result 001

## Question

After the complete COMMAND `54:00..10` source/route reconciliation,
Direct/Readonly package regression and live-route audit, does the current
source-built native CLI retain its bounded immutable-input behavior in both
supported modes?

This is package validation only.  It does not make observed services a work
queue and does not claim guest program completion.

## Inputs

- fresh formal closure:
  `build/M0-T225-S20/native-package-validation-r1/`;
- formal graph: `tools/build/t225-s7-full-module-manifest.json`, MSVC x64
  `/MT`, CPU5/P-MMX projection SHA-256
  `DDB0D13E35C2FD9E7EC29896E6296F83ECE7F6BF5D2AD474E8937D389F97872F`;
- immutable source-built primary input:
  `build/M0-T225-S10/primary-bootstrap-inputs-r2/`; and
- staged `fixture-config.nt` and `fixture-autoexec.nt`, supplied only through
  the existing child-process startup-configuration environment contract.

## Procedure

The fresh graph compiled and linked all 302 native edges to
`bin/ntdos64-native.exe`.  Direct and Readonly each used the same admitted
profile/root/target, a 100000 instruction-tick budget, and both BOP/#UD
observers.  The process environment was restored after each run.

An initial attempt omitted the required paired startup-source environment
variables and returned controlled composition rejection `detail=53` before
any BOP or #UD record.  It is a validation-recipe correction, not a guest or
COMMAND failure.  The successful runs supplied those already staged sources.

## Observation

| Mode | Exit | BOP / #UD records | First / last BOP | Terminal |
| --- | ---: | ---: | --- | --- |
| Direct | 4 | 33 / 33, no overflow | `0070:0475 50:11` / `0041:5a70 50:36` | budget (`terminal=4`) |
| Readonly | 4 | 33 / 33, no overflow | identical | identical budget terminal |

The Direct and Readonly stdout logs are byte-identical with SHA-256
`4512F3D7C428B3C919CA37D8A83998FE49796511D1AEC81FC7BE72FFD44D68D0`.
Both report `detail=0`, `lifecycle=5`, `presentation=5`,
`cancellation=0`, and `budget=100000`.

A subsequent `ninja -n bin/ntdos64-native.exe` reported `no work to do`.

## Interpretation

The completed COMMAND package route, its Direct terminal-child boundary and
Readonly unavailable boundary do not disturb the currently reachable bounded
native composition.  The observation does not reach a new COMMAND endpoint;
under T225 policy it therefore creates no implementation work.

## Follow-up

S20 is closed.  T225 next audits guest DOS `EXEC`/PSP/parent-return ownership
as one package before attempting guest multi-program implementation.