# T225 S28 P2 First-Fault Observation Fixture Result

## Question

Does the S28 default-off mechanical observer capture one non-UD CPU exception
and return through the existing finite controlled-stop path without linking an
adapter/service provider?

## Inputs

- `BX-TRACE-084` in `adapter-external-intrusion-exceptions.md`.
- `src/bx-core/cpu/exception.cc` and new mantle observer record.
- Test-owned `tests/bx-mantle/t225_s28_first_fault_observation_fixture.cc`.
- Fresh formal Ninja root `build/M0-T225-S28/first-fault-r4` generated from
  `tools/build/t225-s7-full-module-manifest.json`, MSVC x64 `/MT`, CPU5 /
  Pentium-MMX configuration.

## Procedure

1. Generated a fresh manifest-verified Ninja graph and built the full
   `bx-core` and `bx-mantle` libraries plus the S28 fixture.
2. The fixture writes only four opaque test-owned entry bytes: `xor ax,ax` /
   `div ax`. The second instruction produces native vector 0 (`#DE`), not
   `#UD`.
3. Before arming, the fixture submits a valid test-owned non-UD record and
   verifies the default-off observer declines it without recording a fact.
4. It arms the private first-fault observer, runs the existing finite-machine
   loop, copies the record after its controlled stop, and verifies magic,
   version, size, vector 0, CS `0e00`, AX zero and fault EIP two.
5. It resets the observer and verifies that the copied record is unavailable.

## Observations

- The fresh 145-edge fixture closure compiled and linked successfully.
- The fixture exited `0`, including an explicit default-off valid-event decline before its armed #DE run.
- Its only runtime output was ordinary CPU/reset/APIC/RAM initialization; it
  emitted no BOP, provider, OpenNT, DOS, path or host-capability trace.
- Two earlier disposable graphs exposed graph-boundary issues only: r1 omitted
  the existing finite-run source; r2 linked its direct closure but lacked the
  externally selected generic-UD bridge provider; r3 proved that importing the
  whole bx-vdm provider merely to satisfy that symbol pulled unrelated COMMAND,
  OpenNT-host and Win32 link requirements. They were removed before r4. The
  final fixture instead owns a decline-only implementation of the already
  external generic-UD bridge contract, so no generic event is accepted.

## Interpretation And Confidence

High confidence that the new path is mechanically isolated: `exception.cc`
recognizes no selector/service and copies no guest memory; the mantle record is
fixed width; the test demonstrates a non-UD event accepts a one-shot stop; and
an existing external #UD bridge can remain present but declining. This is not
native NTIO evidence and does not classify S27's segment-limit failure.

## Follow-up

P3 must expose the same explicitly armed observer through the existing
machine-stage execution owner, classify it separately from generic-UD STOP
and watchdog termination, then run the immutable primary Direct and Readonly
inputs once each. No repair of descriptors, BIOS, FPU, PIC, timing or guest
code is admitted by this result.