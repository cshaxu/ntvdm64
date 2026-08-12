# ntvdm64 Core-Machine Probe

Status: default-disabled research probe, 2026-08-07.

## Purpose

This probe answers one narrow question: can the current `ntvdm64`
`core-machine` source target execute an owned instruction stream when consumed
by `ntdos64`, without changing `ntvdm64` and without making historical source
trees part of the same link graph?

It is not an NTDOS bootstrap, a CCPU replacement, a historical BOP dispatcher,
or a `core.dll` ABI proposal.

## Configuration Boundary

The target exists only when both settings are explicit:

```text
NTDOS64_ENABLE_NTVDM64_CORE_EXPERIMENT=ON
NTDOS64_NTVDM64_SOURCE_DIR=<local checkout>
```

The external checkout is configured and built independently using the same
MinGW-w64 GCC configuration. The probe receives the three static archives
required by the current public link interface: `core-machine`,
`core-machine-executor`, and `type-facade`. No file in the external checkout is
written or patched by `ntdos64` itself.

This separation is required because the current `ntvdm64` top-level CMake uses
`CMAKE_SOURCE_DIR` for source include roots and cannot safely be nested through
`add_subdirectory`. The experiment therefore establishes source-interface and
same-toolchain archive compatibility only; it establishes no binary, versioned,
or cross-toolchain contract.

## Probe Contract

The owned probe configures an 8086-profile machine, registers a generic
one-byte undefined-instruction pattern, freezes topology, resets, applies a
real-mode entry plan with owned RAM bytes, and runs to the constrained `STOP`
outcome. It uses no DOS byte sequence, BIOS image, BOP selector, historical
object, host file access, device response, or synthetic DOS service.

Passing proves the following current capabilities compose across the two source
trees:

- core-machine creation and reset;
- frozen generic transition registration;
- checked prepared entry with ordinary-RAM preload;
- CPU instruction execution and controlled stop.

It does not prove compatibility with OpenNT's CCPU private ABI, original BIOS
tables, `MS_bop_0`, DEM, NTIO, NTDOS, protected-mode guest code, or a future
external `core.dll`.

## Verification Record

On 2026-08-07, the external checkout was configured independently with
MinGW-w64 GCC 16.1.0 and built through its existing `core-machine` target.
The three resulting archives linked into `ntdos64-core-machine-probe`; the
probe printed its success result after one generic transition and a controlled
stop. No source file in the external checkout was modified.

A separate clean default `ntdos64` configuration with the experiment disabled
built `ntdos64-run` from `src/runner/ntdos64_run.c` alone. Its no-argument usage
smoke retained the expected exit status 2. This confirms the experiment is not
a default build or normal-CLI dependency.

## Next Evidence Gate

Any extension of this probe must remain an owned, bounded trace. A historical
profile may register a byte pattern only after the original historical consumer
is linkable and the test records the original call boundary. It may not add a
self-authored BOP, DEM, BIOS, IVT/BDA, DOS, or device substitute.
