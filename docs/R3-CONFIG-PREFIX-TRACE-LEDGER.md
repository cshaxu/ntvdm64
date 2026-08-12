# R3 Original Config Prefix Trace Ledger

Status: executed default-disabled fixture, revalidated 2026-08-08. This is a narrow
historical-source trace, not a DOS runtime or a bootstrap admission.

## 2026-08-09 Profile Recheck

`config.c:580-587` invokes `InitNtCpuInfo()` only under `CPU_40_STYLE`, after
the original `sas_init()` call. The fixed NT4 x86 product profile must not use
that branch as a startup requirement. The `InitNtCpuInfo` stop is therefore a
non-x86 generated-debugger ABI diagnostic, not an x86 SoftPC recovery task and
not authorization for a host shim. x86 work must instead follow its own
post-`sas_init` original owner sequence.

## Question

Can original `host/src/config.c` traverse its direct-CLI configuration/PIF
prefix and reach its first original SAS lifecycle call without reproducing the
historical CSR/BaseSrv, console product shell, physical-drive policy, or a DOS
machine?

## Selected Historical And Local Inputs

The target `ntdos64-opennt-config-prefix-trace-fixture` links only these
original execution owners:

```text
original config.c -> original nt_pif.c -> original GetPIFData
                         -> original sas_term
```

The fixture supplies a one-shot `GetNextVDMCommand` transport record and a
minimal structural PIF created at runtime. It deletes that file before exit.
`config.c` remains the sole definition of historical `pfdata`; the fixture's
PIF guard deliberately supplies only `bPifFastPaste`,
`ExpandEnvironmentStringsOem`, and `RcMessageBox`. The latter two stop the
trace immediately if reached, rather than return a replacement PIF result.

The fixture sets source-derived DOS-profile context values (`VDMForWOW`,
`fSeparateWow`, `DosSessionId`, and `CntrlHandlerState`) to zero/false. It
saves and restores the temporary console title used by the original code. No
SAS initialization, CPU initialization, ROM setup, guest memory write, BIOS,
BOP, DEM, NTDOS, or guest execution is linked or called.

## Stop Rule

The original `sas_term()` call is replaced solely by a non-returning fixture
stop. The jump environment is established in the still-active fixture `main`
frame; it is not stored in a returned helper frame. A successful run therefore
means exactly:

```text
original config() returned through the original parser/default path
-> original sas_term()
-> fixture stop reason 1
```

It does not mean that `sas_init`, `rom_init`, `cpu_init`, or the later
`InitialiseDosEmulation` sequence is usable.

## Reproduction And Result

Build:

```text
cmake --build artifacts/build/current/opennt-r5-clang-x86 --target \
  ntdos64-opennt-config-prefix-trace-fixture
```

Run:

```text
artifacts/build/current/opennt-r5-clang-x86\ntdos64-opennt-config-prefix-trace-fixture.exe
```

On 2026-08-07 and again on 2026-08-08, the build succeeded with the normal historical-header warnings
and the executable returned `0`. The fixture also asserts successful PIF
deletion and absence after deletion, and title restoration before reporting
success.

The 2026-08-08 revalidation corrected a build-only boundary: the original
`config.c` starts with `<nt.h>`, so its isolated object targets retain the
historical NT-first declaration order and `_X86_` profile instead of accepting
the generic Win32-first staging preamble. This changes no `config()` control
flow and supplies no host, SAS, CPU, or DOS behavior.

## Config To SAS/ROM Extension

The separately default-disabled
`ntdos64-opennt-config-sas-prefix-trace-fixture` uses the same original
`config.c` and PIF path, but does not replace `sas_term` or `sas_init`. It
links the original CCPU SAS owner and original `rom.c` provider, then stops at
the next original call, `InitNtCpuInfo`, with stop reason `5`:

```text
original config()
-> original sas_term()             (safe pre-initialization cleanup)
-> original sas_init(vdm size)
   -> original ccpusas4.c
   -> host_sas_init (bounded fixture aperture)
   -> original rom_init/copyROM
-> original InitNtCpuInfo()
-> fixture stop reason 5
-> original sas_term()             (fixture cleanup)
```

For the selected direct-CCPU compile profile (`NTVDM` without `MONITOR`), the
original `rom_init` code establishes ROM write classification and its original
`copyROM` body is empty. No ROM image, BIOS byte sequence, IVT/BDA scaffold,
or guest media is supplied. The run returned `0` on 2026-08-07 and verifies
that `sas_term` has released the bounded SAS aperture before it reports
success.

## Boundary Result

This admits no general PIF or console policy. It establishes that the next
R3.C trace can begin from a real original `config` caller boundary and has now
crossed the original SAS/ROM initialization owner. The next lower boundary is
the original `InitNtCpuInfo` bridge, followed by the remaining original
configuration owners (`InitUMBList`, runtime initialization, and drive
activation); none may be bypassed by a synthetic success response.

## `InitNtCpuInfo` Admission Result

The next original call cannot currently be admitted into this direct-CCPU
fixture. Its original owner, `host/src/nt_cpu.c`, compiles against the
historical monitor-side generated ABI rather than CCPU-local state. A direct
compile on 2026-08-07 fails on the absent `GDP_PTR` and the generated
`ConstraintR*` layout names. The available CCPU archive instead exports its
own `Gdp` form and does not recover that monitor declaration set.

A source-only recheck of the fixed OpenNT input plus the local OpenNT 4.5,
Windows 2000, and XP reference trees found those declarations only under the
MIPS/PPC monitor header variants and the C/VIDC declaration family. It found
no matching NT4 x86 direct-CCPU generated host input.

This is neither a missing ordinary host API nor one of the three narrowly
approved CCPU completion interfaces. No local definition or `GDP_PTR` alias
is admitted. The config-to-SAS trace therefore remains stopped before this
debugger register-view bridge. The next useful R3 work is to isolate the
independent original configuration owners after that bridge, or recover a
provenanced matching generator/header input; it is not to substitute the
bridge or enter registry-backed `InitUMBList`.
