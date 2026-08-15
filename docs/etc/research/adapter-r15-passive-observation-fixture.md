# Adapter r15 Passive Observation Fixture

## Exact Claim

r15 proves one profile-owned passive observation transaction can cross the
existing generic #UD seam, read a predeclared ordinary-RAM range through the
generic Bochs read primitive, resume by the profile-declared two-byte neutral
instruction length, and reach the native bounded halt/benchmark stop.

It does not prove BIOS compatibility, a stock firmware observation, NTIO
preload, NTDOS load, COMMAND execution, a BOP service, DEM behavior, or a DOS
runtime.

## Declared Target

The pinned Bochs 2.6 `Makefile.in` now declares
`ntdos64-observation-fixture.exe`, separate from `all`. It links the same
native mechanics closure needed by `bochs.exe`, but does not request
`bximage.exe`, `bxcommit.exe`, an added device, a service implementation, or
guest policy.

The r15 configuration was created by
`tools/build/Configure-Bochs26NativeContainer.ps1`. The sole built target was:

```text
make -j1 ntdos64-observation-fixture.exe
```

The command exited zero. Existing upstream compiler warnings are retained in
the build transcript; no warning was repaired by changing Bochs configuration,
devices, adapter policy, or OpenNT source.

## Fixture Input

`Invoke-Bochs26NativeResetFixture.ps1 -AdapterObservationFixture` generates a
private 128 KiB ROM whose reset vector is only:

```text
0F 0B F4      UD2 ; HLT
```

It also creates a locally owned BYOB profile containing:

- resource identities for the three required small fixture inputs;
- observation ID `7`, physical range `0..3`; and
- an exact `#UD`, `0F 0B`, resume-by-`2` trigger declaration.

The adapter matches that declaration, queues a one-time transaction with its
own fixed output storage, and returns a checked resume. Bochs does not inspect
the declaration. Its registered `BX-MEM-011` block takes the pending
transaction and calls only `copy_from_ordinary_ram`.

## Runtime Evidence

The following command completed successfully:

```text
powershell -ExecutionPolicy Bypass -File tools/probe/Invoke-Bochs26NativeResetFixture.ps1 \
  -BuildRoot artifacts/build/bochs-2.6-native-nogui-ucrt-r15 \
  -AdapterObservationFixture -BenchmarkMillions 1
```

`artifacts/build/bochs-2.6-native-nogui-ucrt-r15/ntdos64-reset-fixture/bochs.log`
contains all three required predicates:

```text
Bochs benchmark mode is ON (~1 millions of ticks)
ntdos64 adapter observation copied
cpu loop quit, shutting down simulator
```

The first failed run is also useful evidence: it reached `HLT`, proving the
trigger/resume path, but did not log a copy because a wrong-kind write take
cleared the pending observation. The corrected lifecycle rule is now:
wrong operation kind does not consume another kind; a matching kind with a
boundary/preflight failure still cancels itself. `bx-ntvdm-host-session-test`
now covers this cross-kind case.

## Boundary Verification

The r15 run was followed by passing:

- `Test-BochsUdInterceptBoundary.ps1`;
- `Test-BochsOrdinaryRamBoundary.ps1`; and
- `Verify-DocumentationGovernance.ps1`.

No adapter range becomes an arbitrary memory-read API: it must still originate
from the profile, pass exact trigger validation, be bound to the copied
exception state, and survive one generic all-or-nothing RAM preflight.
