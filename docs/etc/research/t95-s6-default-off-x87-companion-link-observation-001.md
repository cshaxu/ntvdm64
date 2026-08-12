# T95 S6 Default-Off x87 Companion Link Observation 001

## Result

The default-off `BX-TRACE-035` source was compiled into a newly rebuilt FPU
object/archive and linked into a fresh same-island r4 companion executable.
The child wrapper recorded exit `0`. This proves only the default-off object
and link closure; it does not enable the diagnostic or execute a guest.

## Root and inputs

The fresh root is
`artifacts/build/bochs-2.6-native-adapter-msvc-r4`, copied from the immutable
r2 companion. Its root manifest records r2 executable/map/FPU identities. The
generator replaced only `fpu/fpu.cc` with the current registered source and
removed the copied `fpu.o`, `libfpu.a`, companion executable/map, and inherited
observer artifacts before the build.

The rebuilt source is SHA-256
`E3BFBD1DA401D143D10C3EB4E7C8F111D476EF386C75FFDD57F1F7ADD1F84845`.
The macro has its local default `0`; no compiler opt-in was present.

## One build observation

The sole command was the predeclared same-island target:

```text
nmake /f ntdos64-native-adapter-observation.mak ntdos64-native-adapter-observation.exe
```

The foreground observer transport exceeded its host time budget, but the
already-started child wrapper completed and atomically recorded exit `0`.
There was no second `nmake` invocation. The retained wrapper/log/output/map
provide the terminal evidence; the recovered observation JSON explicitly
preserves the outer transport limitation.

| Artifact | SHA-256 |
| --- | --- |
| rebuilt `fpu/fpu.o` | `2A449E9097879B35F018B235E75BAD6221921FE1F3D1BCD5A210F1FFCC543483` |
| rebuilt `fpu/libfpu.a` | `19B49F6F04F5FA850A5F6C5CDCFBA436E68CDF0361A4F3A0F16B76309D6840B1` |
| link log | `3BD823F177E5A4A71E968D5936B9962D9BC70EBF44BCCD1A90630C9F3CD30121` |
| companion executable | `131174B601B677A685151FBF5ACB14B048247D3AEA58FDDE7F5E3BDD260A1AF4` |
| companion map | `E2B15D4CE81B736EE5DB8016C41B39A891EB4A7E7F63801B847713FDC2ABA2C9` |

The map contains neither `x87-compat-diagnostic` nor a diagnostic-only
external symbol, as expected for a compiled-out local log. The link command
and its verbose-library log retain the pre-existing adapter objects only; no
new adapter, debugger or instrumentation object was introduced.

## Harness failures retained separately

The first r3 generator copied r2's pre-existing observer record, causing the
observer to refuse before `nmake`; the root is retained unchanged. A manual
dry-run wrapper also failed in PowerShell quoting before reaching `nmake`.
The r4 generator removes all inherited observer artifacts and preflighted that
condition before the one real invocation.

## Classification and next gate

`BX-TRACE-035` is default-off source-built/same-island link-closed. It has no
runtime result. The next separately admitted action may enable its one macro
only in a new root and run the immutable source-built NTIO input once, with the
same PC-reset-floor profile and all guest/host inputs unchanged.
