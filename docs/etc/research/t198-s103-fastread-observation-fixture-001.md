# T198 S103 fast-read observation fixture 001

## Question

Can the existing default-off `50:42` observation retain the missing `BP` and
`EFLAGS` copied fields without changing its event path or running the guest?

## Inputs

- Existing native bridge: `tests/bx-vdm/t198_s23_native_ntio_boundary_bridge.c`.
- Existing fixture formatter:
  `tests/bx-vdm/t198_s23_native_ntio_boundary_fixture.cc`.
- Current MSVC x64 `/MT` projection:
  `artifacts/build/t198-s74-dem-pdb-termination-r1/native-core/config.h`.
- Reproducible source-only recipe:
  `tools/Invoke-T198S103FastReadObservationFixturePrepare.ps1`.

## Procedure

The bridge's pre-existing `C4 C4 50 42` recorder now copies `event->ebp` and
`event->eflags` along with its prior copied state.  It exports two test-local
getters.  The fixture declares those getters and appends both fixed-width
values to its already existing `fast-read` line.

The recipe prepared the locked S93 inputs, source-compiled the fixture and
source-compiled the bridge under MSVC x64 `/MT` at
`artifacts/build/t198-s103-fastread-observation-prepare-r1`.  It intentionally
does not link the native fixture or execute guest code.

## Observations

- `bridge-compile.log` contains a successful compilation of
  `t198_s23_native_ntio_boundary_bridge.c`.
- `prepared/fixture-compile.log` contains a successful compilation of
  `t198_s23_native_ntio_boundary_fixture.cc`.
- `outcome.json` declares `linked: false` and `guestExecution: false`.
- SHA-256 values:
  - `native-ntio-boundary-bridge.obj`:
    `87b660e0e21771d7a9d9fff0dc75dac61cf42b0bdabeda84bbc10b6128b59922`.
  - `prepared/source-built-normal-return-fixture.obj`:
    `12cdafed09be0fbd6d1d4f96de94fb66c12e30c6e428508519983c57d958d1aea`.
  - `outcome.json`:
    `71f57cc22221f1870d8e3e4f5f89c40c35615dac05ec10994355cf89d4d5cc14`.

## Interpretation

The expanded marker is source-buildable and remains strictly observational.
It is still reached only on its pre-existing declined-event path: it neither
reads further guest state nor proves that a provider accepted or rejected a
transaction.  S104 may perform one exact-input native run to observe the
added state; that result must not be inferred from this source-only fixture.
