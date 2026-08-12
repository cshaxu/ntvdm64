# M0 T95 S6: MSVC/x86 Non-Building Platform Probe

## Scope And Commands

No Bochs configuration, source, artifact, fixture, compile, link, configure,
project, executable, or run was created or invoked.  The only tool command
established the VS environment and queried identity:

```text
call VsDevCmd.bat -arch=x86 -host_arch=x64
cl.exe /Bv
link.exe /?
```

`cl /Bv` printed identity then returned its normal missing-source diagnostic;
it compiled nothing.

## Directly Observed Facts

- VS developer environment: `VSCMD_VER=17.13.5`, host `x64`, target `x86`.
- Compiler: MSVC `19.43.34809` at
  `...MSVC\14.43.34808\bin\Hostx64\x86\cl.exe`.
- Linker binary in the same x86 directory: `14.43.34809`.
- Windows SDK/UCRT: `10.0.22621.0`.
- Installed headers observed: `ucrt/stdio.h`, `stdlib.h`, `string.h`, `io.h`,
  and `um/windows.h` under
  `C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0`.

These prove an available MSVC/x86 host toolchain and header set only.  They do
not prove a Bochs configuration, CRT choice, function semantics, link import,
or generated typedef branch.

## Ledger Platform Rows

| Ledger macro family | Current proof classification |
| --- | --- |
| `_MSC_VER`, `_WIN32` | Directly observed compiler-fact candidates: the established x86 MSVC environment is recorded above. Exact macro values still require the next non-product predefined-macro observation. |
| `WIN32` | Source/platform fact but not enough to select a Bochs macro. `bochs.h` uses it to select Windows headers/osdep behavior; its mapping must be evidenced separately from `_WIN32`. |
| `BX_WITH_WIN32` | Not platform-probed: it is a Bochs GUI backend choice and remains source-forced `0`, irrespective of any compiler `WIN32` mapping. |
| `SIZEOF_*`, `WORDS_BIGENDIAN`, typedef branches, `BX_CONST64`, `CDECL`, `BOCHSAPI`, inline/attribute macros | Still unresolved: installed x86 tools suggest a likely ABI but only a dedicated non-product compile/preprocessor probe may establish the effective values. |
| `BX_HAVE_UNISTD_H`, `BX_HAVE_SNPRINTF`, `BX_HAVE_VSNPRINTF`, `BX_HAVE_STRTOULL`, `BX_HAVE_STRTOUQ`, `BX_HAVE_STRDUP`, `BX_HAVE_STRREV`, `BX_HAVE_STRICMP`, `BX_HAVE_STRCASECMP`, `BX_HAVE_ABORT` | Header paths are directly observed, but that is not enough to select any Bochs `BX_HAVE_*` value. Each requires a dedicated non-product declaration/behavior probe. A similarly named CRT export is not proof. |
| `BX_HAVE_REALTIME_USEC`, time/sleep, filesystem, socket, getenv/select, mmap/XPM, locale/readline, builtin-bswap families | Still unresolved requiring dedicated non-product probes. Header installation neither proves declaration availability nor the exact Bochs fallback contract. |
| `BX_USE_*_SMF`, `BX_SUPPORT_A20`, `BX_FAST_FUNC_CALL`, `BX_ASSERT_ENABLE`, `BX_USE_IDLE_HACK`, `BX_EXIT`, frontend defaults, ATA/default-size families | Retained-symbol-dependent, not platform facts; this probe cannot select them. |

## Non-Circular Next Gate

The next permitted work is a non-product, non-Bochs predefined-macro and
header-declaration probe with commands/output recorded per row.  It must first
prove the compiler-to-`WIN32` mapping used by `bochs.h`; only then may it
propose values for platform-probed ledger rows.  No historical `config.h`, VS
property, similarly named CRT function, or rejected attempt-001 value may fill
a row.  Retained-symbol-dependent rows remain closed until the later approved
COMDAT analysis.

## Non-Claims

This is not a compile/link result and does not establish any Bochs macro,
generated configuration, object, import, CRT model, COMDAT retention, fixture
closure, device capability, or runtime behavior.
