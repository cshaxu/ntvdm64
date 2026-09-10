# Owned Monitor M0 Work Order

Status: implementation work order, 2026-08-07. This packet implements the
first owned execution substrate after the historical CCPU source-recovery
boundary. It does not claim an executable DOS runtime.

## Outcome

Add a default-disabled `owned-monitor-m0-test` target that proves a narrow,
per-session machine-state contract:

```text
finite zeroed physical RAM
  -> one checked read/write/fetch mapping route
  -> resettable real-mode visible register state
  -> typed stop/result vocabulary
```

The target is a conformance fixture, not `ntdos64-run.exe`, `nvtdm.exe`, a DOS
engine, a BOP handler, or an emulated CPU instruction loop.

## New Source Boundary

| File | Responsibility | Explicitly excluded |
| --- | --- | --- |
| `refs/archive/legacy-adapters/owned_monitor_m0.h` | fixed-width M0 public types, result codes, and operations | BOP/DOS/NTIO names, Win32 APIs, execution callbacks |
| `refs/archive/legacy-adapters/owned_monitor_m0.c` | allocation, reset, checked physical range routing, visible real-mode state | decode, host I/O, devices, interrupt delivery, mode changes |
| `refs/archive/legacy-adapters/owned_monitor_m0_test.c` | independent owned fixture and negative tests | guest payloads, historical binaries, frozen instrumentation |

These files must not include or link `bop_rewrite.*`, `transition_registry.*`,
`bootstrap_profile.*`, `whpx_com_run.c`, `nvtdm.c`, OpenNT, NTVDMx64, or
`ntvdm64`. The source-only references remain evidence and cannot become an
implementation shortcut.

## M0 Contract

### Mapping

- Constructor accepts only a nonzero finite physical-RAM byte count.
- RAM is allocated by the owned machine and is all zero before the constructor
  returns and after every successful reset.
- `read`, `write`, and `fetch` use the same checked interval helper.
- `size == 0` is `INVALID_ARGUMENT` for every access.
- `physical + size` overflow or an interval beyond RAM is `INACCESSIBLE_RANGE`.
- M0 has only ordinary RAM. ROM, providers, A20 translation, and guest memory
  topology are later contracts, so no API may imply their behavior now.

### Visible CPU State

M0 stores only real-mode visible registers: sixteen-bit general, segment, IP,
and FLAGS fields. Reset sets all fields to zero and sets FLAGS to `0x0002`.
It does not store descriptor caches, CRx, CPU mode, IDT/GDT, PIC/PIT state, or
interrupt state. It has no instruction decoder or execution backend yet.

### Result Vocabulary

The stable M0 result codes are `OK`, `INVALID_ARGUMENT`, `ALLOCATION_FAILED`,
`INACCESSIBLE_RANGE`, and `UNAVAILABLE`. `UNAVAILABLE` is reserved for an
explicitly requested operation outside M0, rather than fabricating a result.

## Required Fixture Cases

1. Constructor rejects zero RAM and produces no usable machine.
2. Newly created RAM is zero; a write/read round trip succeeds.
3. `fetch` observes the same bytes and range rules as `read`.
4. zero-size and overflowing/out-of-RAM intervals fail without changing RAM.
5. Reset clears RAM and restores the exact visible register baseline.
6. The test calls no historical or frozen substitute source.

## Build And Isolation

Add `owned-monitor-m0-test` with `EXCLUDE_FROM_ALL`, C11 only, and a focused
CTest entry. The top-level default target and `ntdos64-run` link line must
remain unchanged. Build outputs go under `artifacts/build/`.

## Verification

From a MinGW-w64 GCC configure tree:

```powershell
cmake --build artifacts/build/current/<stage> --target owned-monitor-m0-test --parallel 4
ctest --test-dir artifacts/build/current/<stage> --output-on-failure -R owned-monitor-m0
cmake --build artifacts/build/current/<stage> --target ntdos64-run --verbose
```

The final verbose link inspection must show that `ntdos64-run` still contains
only `src/runner/ntdos64_run.c`; passing M0 does not establish DOS execution.

## Follow-Up

M1 may add a generic, frozen-before-reset transition registry only after M0
range/reset tests pass. It will define transition matching and atomic patches
from scratch, using historical traces only as external behavior oracles.

## Implementation Evidence

On 2026-08-07, the M0 module was added as `src/owned_monitor_m0.[ch]` with an
independent `refs/archive/legacy-adapters/owned_monitor_m0_test.c` fixture. It contains no historical
source include or dependency and is built only through the `EXCLUDE_FROM_ALL`
`owned-monitor-m0-test` target.

The recorded MinGW-w64 verification was:

```powershell
cmake -S . -B artifacts/build/current/default-mingw-verify
cmake --build artifacts/build/current/default-mingw-verify --target owned-monitor-m0-test --parallel 4
ctest --test-dir artifacts/build/current/default-mingw-verify --output-on-failure -R '^owned-monitor-m0$'
cmake --build artifacts/build/current/default-mingw-verify --target ntdos64-run --parallel 4
```

The M0 test passed. The generated `ntdos64-run` object response file still
contains only `CMakeFiles/ntdos64-run.dir/src/runner/ntdos64_run.c.obj`, proving that
M0 did not enter the normal CLI link graph.
