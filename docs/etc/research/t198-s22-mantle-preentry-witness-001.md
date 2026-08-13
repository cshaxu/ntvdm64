# T198 S22 Mantle Pre-Entry Witness 001

## Question

Can the minimal CPU5 Bochs machine publish an opaque copied image while
restoring an opaque reset-time range, then apply only a supplied real-mode
CS:IP entry, without importing VDM or guest-service meaning?

## Inputs

- `src/bx-mantle/bx_ntvdm_finite_run.h` request version 2: fixed-width fields
  and an embedded 65,536-byte opaque image; no host pointer crosses into the
  mantle.
- `src/bx-mantle/bx_ntvdm_finite_run.cc`: one-MiB range preflight, capture,
  image publication, restoration, then CS:IP entry.
- `tests/bx-mantle/t198_s22_preentry_preserve_fixture.cc`: an image at
  `0x700` deliberately overwrites `0x714..0x717`; its code reads `0x714` and
  halts only if the captured reset bytes were restored.  A second invocation
  supplies an out-of-aperture image range and requires rejection.

## Procedure

1. Run `powershell.exe -ExecutionPolicy Bypass -File
   tests/bx-ntvdm-finite-run-boundary.ps1`.
2. Run `powershell.exe -ExecutionPolicy Bypass -File
   tools/Invoke-T198S3FiniteNativeRunProbe.ps1 -RepositoryRoot
   O:\repos.hobby\ntdos64 -BuildRoot
   O:\repos.hobby\ntdos64\artifacts\build\t198-s22-preentry-r4
   -HostArchitecture x64 -UdStopFixture -ExternalFixtureSource
   O:\repos.hobby\ntdos64\tests\bx-mantle\t198_s22_preentry_preserve_fixture.cc`.

## Observations

The boundary script passed.  The second command rebuilt the complete admitted
CPU5/Pentium-MMX closure under MSVC x64 `/MT`, produced PE32+ x64 output, and
the fixture exited zero.  The fixture’s success path proves the overlap is
restored before guest entry; its out-of-aperture request is rejected before
machine initialization or RAM publication.  The run recipe records no
adapter, BOP, OpenNT, CLI, Bochs product target, GUI, or device archive input.

## Interpretation And Confidence

S22 closes the generic mechanical pre-entry primitive.  The finite mantle
still has no NTIO, BOP, DOS, BIOS, host-path, callback, or provider semantic.
The pre-entry image/preserved range are opaque machine inputs.  Confidence is
high for the declared one-image/one-preserved-range finite-machine contract.

## Follow-Up

S23 may have `bx-vdm` prepare this v2 request from the already identity-locked
NTIO image and source-locked addresses, then prove original NTIO reaches the
existing `50:11` boundary.  It must not add guest semantics to mantle.
