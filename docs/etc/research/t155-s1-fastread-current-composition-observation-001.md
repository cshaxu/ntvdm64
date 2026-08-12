# T155 S1 fast-read current-composition observation 001

## Question

Does the T154 source-derived `50:42` provider execute in one current,
source-built x86 composition while retaining the r5 Bochs machine inputs?

## Build provenance

`tools/New-T98S1CurrentAdapterEngineDerivative.ps1` generated
`artifacts/build/current/t155-fastread-r1` from the r5 root.  Its manifest
lists 66 adapter and four CLI sources, zero Bochs replacements and six
retained engine inputs.  It adds only
`bx_ntvdm_dem_fastio_provider_v1.c` to the generated adapter list.

The permitted link was run once in the MSVC BuildTools x86 environment:

```text
nmake /nologo /f ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe
```

The output binary SHA-256 is
`894A1B7476BECF9A5B5E09298D45E8093B59BF89C695DBE07B230FC701C25DA6`.
The copied `main.o`, `cpu/exception.o`, `cpu/libcpu.a` and `iodev/libiodev.a`
remain respectively
`655C7550401A151FB62E3C3FCC44A74698BC468D22A287ECA556A6D1C87AA85A`,
`23AB5DE4A1BD575251270ECDBC3B0937065CB2901B8B7873AF2DF1D16B448FEC`,
`E017EE968447D9F8DEDDF6F3277C1FEF9D213D04F097D57C99E45B09281BD4DB`, and
`2850F6C041B2401B3627EF98D11E19CC6F13F9BD59BD6AA075E9FEC2BE195A1C`.

An initial accidental x64 link failed with `LNK1112` before an executable was
produced.  The regenerated directory was then built once with the required
x86 toolchain; this is a toolchain correction, not a Bochs or source change.

## One bounded observation

The existing runtime observer was invoked once with the new binary, its fixed
v4 BYOB profile, a one-million-tick benchmark and a fifteen-second watchdog.
Its log is retained at
`artifacts/analysis/t155-s1-fastread-observation-001/bochs.log`.

The watchdog child-tree cleanup received local `Access denied`, so the helper
did not write `observation.json`.  No `ntdos64-t98-current-adapter` or
`bochs` process remained afterward.  This is therefore log-backed bounded
execution evidence, not a clean observer-completion record.

## Observed result

The log records four `50:42` calls and four committed bulk transactions:

| Tick | Bulk bytes | Interpretation |
| --- | ---: | --- |
| 49,349 | `0d` | finite CONFIG payload |
| 109,961 | `3e` | finite boot-namespace file payload |
| 110,767 | `c4d0` | full 50,384-byte COMMAND.COM image |
| 320,848 | `00` | successful EOF read |

Every call resumes at the four-byte BOP continuation.  The log contains zero
`50:43` observations.  Thus the provider is neither a normal-read alias nor
a write capability, and it resolves the T152 command-image population gap.

At tick 326,544, after these reads, Bochs reports three real-mode
`stackPrefetch(2)` limits at `0000:ffff` followed by
`exception(): 3rd (12) exception with no resolution`.  This is a newly
reached execution stop; T155 does not attribute it to the adapter, alter
machine mechanics, or claim continuous guest execution.

## Disposition

T155 closes current adapter/CLI recomposition and the single bounded
fast-read observation.  T156 must first audit the reached stack/exception
state using OpenNT and retained machine source evidence before any change is
considered.
