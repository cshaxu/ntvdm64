# T98 S1 current-adapter engine derivative 001

## Purpose

This record closes the static part of T98 S1. It turns the retained r5 runtime island into a generator-only derivative whose only buildable objects are the current adapter and CLI closure. It is not an engine rebuild or runtime evidence.

## Inputs and identity

The retained input is `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r5`. Its `ntdos64-s7-runtime-trace.exe` SHA-256 is `F4416F20F456366CC1AFC83D8B40748BF331D02E336BB9350EE8045329582DC2`.

`tools/New-T98S1CurrentAdapterEngineDerivative.ps1` created the fresh r2 derivative at `artifacts/build/bochs-2.6-t98-current-adapter-msvc-r2`. Its manifest is `t98-s1-current-adapter-engine-derivative.json`; its generated shim hash is `BD4C9E48B6A6C0D35C5D14333ACF7A5823328A2AAD3FF69C3F02E44DAA9A3EE3`. The manifest reports 65 current adapter/CLI objects and zero Bochs replacements.

## Closure result

The shim admits the source set compiled by the current adapter-runtime fixture, including the v4 BYOB parser, immutable profile search snapshot, pathname transaction and DEM `50:09/50:0B` service. It does not admit `bx_ntvdm_host_namespace` or a host-directory fixture.

The retained r5 `main.o`, `cpu/exception.o`, CPU archive, device archives, memory archive, GUI archive and simulator inputs are hash-recorded inherited inputs. The link command consumes them without changing their owner or configuration.

The first dry-run exposed an unsafe make graph: declaring retained archives as target prerequisites caused inherited `Makefile` rules to recurse into device, CPU, memory, GUI, disassembler and FPU builds. That recipe was rejected. The admitted r2 shim makes only the 65 adapter/CLI objects prerequisites; retained archives occur only in the final link command. MSVC x86 `nmake /n` then reports only those 65 C compilations and one final link, with no recursive `nmake` command and no Bochs C/C++ compile rule.

## Disposition

| Class | Disposition |
| --- | --- |
| Required | Current adapter/CLI objects; unchanged r5 link inputs; existing MSVC x86 toolchain. |
| Default but not rebuilt | r5 main/exception/CPU/device/memory/GUI/simulator archives and inherited Bochs product configuration. |
| Explicitly rejected | Full Bochs configure/build, archive repack, device enablement, firmware/media change, Bochs source change, host namespace and any new bridge ABI. |

## Next admission

T98 S1 is statically closed. T98 S2 may execute only the manifest's named target, record its compile/link outcome and compare retained-engine hashes. It must stop before runtime interpretation, new BOP semantics or Bochs changes.
