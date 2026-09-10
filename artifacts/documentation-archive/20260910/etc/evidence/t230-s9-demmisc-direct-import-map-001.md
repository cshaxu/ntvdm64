# T230 S9 — OpenNT `demmisc.c` Direct-Import Map

## Question

Can the complete original OpenNT owner of `50:11`, `50:23`, `50:34`--`50:3E`
and `50:45` be composed as Direct code without retaining or repairing the
legacy v1/provider implementations?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demmisc.c`
- `src/opennt/base/mvdm/dos/dem/demdisp.c`
- `src/opennt/base/mvdm/inc/dossvc.h`, `dbgsvc.h`, `softpc.h`
- formal manifest `tools/build/t225-s7-full-module-manifest.json`

The checked-in OpenNT baseline is `d450bfc1b7711234072eebfdfa23735ea509e7c5`.
The original file notice and body order are retained in the mirror.

## Imported shape and minimum seam

The full original translation unit is at
`src/bx-vdm/bop/opennt/dem/demmisc.c`; v1 and the earlier partial v2 remain
comparison-only and are neither compiled nor exercised by this witness.
`src/bx-vdm/bop/shim/demmisc_shim.*` supplies only unavailable historical
CCPU/SAS/product-host bindings:

- checked real-mode guest RAM, typed resume/stop and the non-returning
  `TerminateVDM` effect reuse the neutral `demhndl_shim` call context;
- OpenNT's OEM `CreateFile`/`ReadFile` loader path maps the actual host file
  contents back through checked guest RAM;
- the source's `ULONG` pointer increment is replaced at its exact line by a
  named x86/x64-safe shim.  The original 16 KiB loop, error/termination path
  and file-name construction are unchanged;
- OpenNT `dossvc.h`, `dbgsvc.h` and `softpc.h` constants are reproduced from
  their original definitions, not inferred values;
- the original floppy/fixed-disk reset calls remain explicit shim calls.  The
  current raw-DASD seam opens and closes each transfer (no retained media
  handles), so their Direct current state is empty; `pFDAccess` is explicitly
  bound and cleared exactly by the original `demDiskReset` body.

No shim identifies a BOP selector, implements DOS/BIOS semantics, or imports
Readonly, Overlay, Virtual, or legacy-v1 policy.

## Focused Direct witness

The MSVC x64 `/MT` Ninja target compiled the whole `bx-vdm` library and ran
`tests/bx-vdm/t230_s9_demmisc_direct_import_fixture.c`:

| Entry | Original body exercised | Local observed contract |
| --- | --- | --- |
| `50:11` | `demLoadDos` | reads a real 20,000-byte `ntdos.sys` in the original 16 KiB loop and writes the exact payload to checked guest RAM |
| `50:23` | `demDiskReset` | invokes both source reset calls and clears the bound `pFDAccess` word |
| `50:34`, `50:35` | `demLoadDosAppSym`, `demFreeDosAppSym` | use source `ES:DI` OEM path and retain normal debug-gate behavior |
| `50:36`--`50:3A` | entry, dispatcher and console bodies | retain source lifecycle/diagnostic gates and resume in a non-debug Direct process |
| `50:3B` | `demIsDebug` | derives AL from the original process-debug-port test |
| `50:3D` | `demExitVDM` | reports then reaches typed controlled stop through original `TerminateVDM` order |
| `50:3E` | `demWOWFiles` | preserves AL when not WOW and sets AL to 255 when WOW |
| `50:45` | `demSystemSymbolOp` | retains source symbol/debug gate and ordinary return path |

This is local Direct source parity only.  It does not claim selector ingress,
the debugger/VDD host products, whole DEM dispatch, raw-media lifecycle with
resident handles, or native guest execution.

## Verification

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\build\New-T225S7FullNinjaGraph.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T230-S9\direct-import-r3
ninja -C O:\repos.hobby\ntdos64\build\M0-T230-S9\direct-import-r3 `
  bin/t230-s9-demmisc-direct-import-fixture.exe
& O:\repos.hobby\ntdos64\build\M0-T230-S9\direct-import-r3\bin\t230-s9-demmisc-direct-import-fixture.exe
```

Observed output:

```text
T230 S9 direct OpenNT demmisc import: all lifecycle, loader and diagnostic service bodies verified
```

## Follow-up

S10 alone reconciles the complete `demdisp.c` 73-entry table, removes
superseded comparison code only after that all-family proof, and performs the
bounded native observation.
