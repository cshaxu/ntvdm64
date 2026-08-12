# Open Watcom 1.9 16-bit Toolchain Probe

## Purpose

This record evaluates Open Watcom 1.9 as a local, reproducible candidate for
the historical **assembler and OMF linker** portion of the OpenNT guest-build
toolchain. It is a toolchain probe only. It does not admit an OpenNT guest
binary to the project, default build, runtime, or release.

The input snapshot remains read-only under `upstream/opennt`. The generated
objects and the self-authored DOS smoke executable are confined to
`artifacts/build/legacy/referenced/build-openwatcom-probe/`, which is not a product input.

## Fixed Tool Input

| Item | Value |
| --- | --- |
| Package | `open-watcom-c-win32-1.9.exe` |
| Publisher index | `https://openwatcom.org/ftp/install/` |
| Official checksum file | `open-watcom-c-win32-1.9.exe.md5` |
| Verified MD5 | `6316F454F732B0705EBFE2A278DC1E59` |
| Installation root | `toolchain-probe/openwatcom-1.9/watcom` |
| System integration | Disabled: installer flags `-i -np -ns` |

The installation location has no spaces, as required by the upstream 1.9
README. It is not added to the machine PATH. Every probe sets `WATCOM`,
`EDPATH`, and a process-local PATH explicitly.

## Confirmed Tools

The isolated Win32 host installation provides:

- `binnt/wasm.exe` -- Open Watcom Assembler 1.9;
- `binnt/wlink.exe` -- Open Watcom Linker 1.9;
- `binnt/wmake.exe` -- Open Watcom Make 1.9.

## Reproduced Evidence

### 1. 16-bit OMF producer and linker

A self-authored 10-line real-mode DOS termination sample was assembled with
`wasm -0` and linked through a response file containing `system dos`. The
result is a 37-byte `MZ` executable with a two-paragraph DOS header. WLink
warns that this deliberately minimal sample has no stack segment; the output
is still a valid link-path smoke result. It is not executed on the 64-bit host.

### 2. Original OpenNT NTIO object inputs

Two read-only OpenNT inputs were assembled directly into temporary OMF objects:

| Input | Profile | Result |
| --- | --- | --- |
| `doskrnl/bios/msend.asm` | `wasm -0` | 0 warnings, 0 errors; expected `USE16` segments verified with `wdis` |
| `doskrnl/bios/msint13.asm` | `wasm -3 -i=. -i=../../inc -i=../../../../inc -i=../../messages` | 0 warnings, 0 errors; 1,778-byte OMF object |

The `msint13` probe uses exactly the include roots stated by the historical
`dos/v86/make.inc`; it does not alter the OpenNT source or introduce a source
overlay.

## What This Establishes

- A current Windows host can run an isolated Open Watcom 1.9 `wasm`/`wlink`
  chain without a system installation.
- That assembler accepts a simple original NTIO source and a nontrivial
  original NTIO BIOS/BOP source with the historical include graph.
- WLink can create a 16-bit DOS MZ executable from Open Watcom OMF input.

## Full NTIO Assembly Audit

The candidate was then tested against all 12 modules in the historical NTIO
BIOS object order, not merely the two representative inputs above. Both audits
create a fresh stage and record every command result in JSON; neither attempts
to link or emit `NTIO.SYS`.

| Stage | Source treatment | Result |
| --- | --- | --- |
| `toolchain-probe/ntio-openwatcom-stage-v1` | copy-only original snapshot | 7/12 modules assembled |
| `toolchain-probe/ntio-openwatcom-stage-v2-jwasm-overlay` | existing, explicitly recorded JWasm staging overlay | 7/12 modules assembled |

The reports are respectively
`ntio-openwatcom-assembly-audit.json` in each stage. The five failures are
`msbio1`, `msinit`, `sysinit1`, `sysconf`, and `sysinit2`.

The original-snapshot failures concentrate in historical MASM macro and
declaration forms in `dossym.inc` and `dosmac.inc`; the overlay removes some
independent parser errors but cannot change that macro-semantics limit.
`msbio1` improves from 15 diagnostics to three, while the four modules that
include the shared macro closure remain blocked. This is evidence that the
issue is neither a missing include root nor an input-file omission.

### Decision

Open Watcom 1.9 is **not selected as the complete NTIO assembler**. It remains
an isolated OMF linker and limited-source assembler candidate. The current
full-set path continues to use the established JWasm/MASM object production
where required, followed by Open Watcom WLink and the separately recorded
post-link conversion. No source transformation is justified solely to make
Open Watcom accept these modules.

## What It Does Not Establish

- Whole-artifact compatibility for `NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`, or
  any other historical guest file.
- Compatibility with the complete MASM macro dialect, all module-level case
  rules, or every original `link16` option.
- Replacement of historical message generation (`dosmsg`/`nosrvbld`),
  relocation, symbol-map processing, packing, or strip tools.
- Runtime boot, NTIO-to-NTDOS handoff, BOP dispatch, DEM behavior, or retail
  byte equivalence.

The existing JWasm/UASM evidence remains the only whole-module-set evidence.
A partial object set must never be linked as a claim of guest capability.

## Next Gate

Use Open Watcom only where its demonstrated scope is sufficient: WLink is an
admitted linker candidate, while the historical assembler path remains selected
per artifact. Any future assembler reconsideration requires a fresh full-list
audit that materially improves on the recorded 7/12 result. Keep every output
BYOB-only and outside the default CMake graph.
