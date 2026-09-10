# R5 Original Chain Rebuild - 2026-08-07

Status: current R5 build evidence. This record supersedes no historical
conclusion; it repeats the original-chain closure in a fresh current build
directory using the fixed imported inputs.

## Scope

Build the default-disabled original-chain closure fixture from the isolated
OpenNT source island. The fixture contains no normal-runner target and does
not execute a guest, BIOS selector, BOP, DEM service, or DOS program.

The intended historical ownership chain remains:

```text
CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM
```

The purpose is to distinguish an ordinary source/object closure from the
first remaining host-lifecycle and product-shell link boundary. It is not an
admission to replace unresolved services with local handlers.

## Inputs And Command

- Source root: `src/opennt`, fixed by its `SOURCE-MANIFEST.json`.
- Build directory: `artifacts/build/current/opennt-r5-clang-x86`.
- Compiler: LLVM `clang-cl` 22.1.8 in `vcvarsall.bat x86`.
- Generator: `NMake Makefiles`.
- Compile target: `i686-pc-windows-msvc` through
  `/clang:--target=i686-pc-windows-msvc`.
- Target: `ntdos64-opennt-original-chain-closure-fixture`.

The exact configure and build forms were:

```text
call vcvarsall.bat x86
cmake -S src/opennt -B artifacts/build/current/opennt-r5-clang-x86 \
  -G "NMake Makefiles" \
  -DCMAKE_C_COMPILER="C:\Program Files\LLVM\bin\clang-cl.exe" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCMAKE_C_FLAGS=/clang:--target=i686-pc-windows-msvc
cmake --build artifacts/build/current/opennt-r5-clang-x86 \
  --target ntdos64-opennt-original-chain-closure-fixture
```

## Observed Result

The fresh build compiled all selected original object groups, including the
full CCPU archive, original BIOS table, `nt_bop`, DEM, and the selected BIOS,
system, keyboard/mouse, disk, serial/parallel, video, XMS, UMB, EMM, support,
and register-provider object gates.

Final `lld-link` correctly fails rather than emitting an executable. The
captured build log is:

```text
artifacts/build/current/opennt-r5-clang-x86/original-chain-closure-link.log
SHA-256: D5F245FBEEA68B45C6D1184BAC0E482DFDF226C1243582CAEA21E8BD4297FCC4
bytes: 116393
unique undefined-symbol diagnostics: 248
```

The first unresolved family is historical host lifecycle and old product-shell
integration, for example:

```text
ActivityCheckAfterTimeSlice
VDMForWOW
DisplayErrorTerm
TerminateVDM
host_clear_hw_int
dwWNTPifFlags
host_com_heart_beat
host_lpt_heart_beat
host_flpy_heart_beat
GetConsoleInputWaitHandle
ReadConsoleInputExW
```

The log also retains optional console/mouse/VDD, full-screen, native path,
file-system, DPMI, command, debugger, and interrupt/timer owner references.
These are expected consequences of retaining the original complete BIOS and
host graph. They are not a selector-by-selector DOS work list.

## Decision

No shim is admitted by this build result alone. R5 may add a modern-host shim
only after an original caller on the bounded initialization path reaches the
specific dependency and a source-derived caller/return contract is recorded.
In particular, `ActivityCheckAfterTimeSlice`, `host_clear_hw_int`, old console
input, PIF globals, and `DisplayErrorTerm` must not be filled merely to obtain
a link-success executable.

The next R5 step is to isolate the original initialization prefix through the
first live missing dependency, using the existing R3 startup-prefix evidence.
That target must retain the complete original BIOS table and report a redacted
stop; it must not reuse the frozen CCPU lifecycle adapter, per-BOP handlers,
or scaffold IVT/BDA state.

## Initialization-Prefix Revalidation

The same fresh R5 build directory also rebuilt and executed the existing
default-disabled `ntdos64-opennt-config-sas-prefix-trace-fixture`. It exited
with status zero on 2026-08-07.

```text
fixture: ntdos64-opennt-config-sas-prefix-trace-fixture.exe
SHA-256: 0E9F032EF9FA7ACC05312F74EF9A55F7838FA0C3E94F90184F92BC3F9DDB921C
bytes: 327168
```

This is a revalidation of the original, bounded prefix recorded in
`R3-CONFIG-PREFIX-TRACE-LEDGER.md`:

```text
config -> sas_term -> sas_init -> ccpusas4 -> host_sas_init
       -> rom_init/copyROM -> InitNtCpuInfo -> bounded stop
```

It has no guest execution, IVT/BDA construction, BOP/SVC dispatch, or DOS
result. Its significance for R5 is narrow but decisive: the first live
initialization boundary remains `InitNtCpuInfo`. That original `nt_cpu.c`
caller requires an unrecovered direct-CCPU-compatible generated monitor ABI;
the static old console/PIF and full-table link imports are not the next shim
candidate.
