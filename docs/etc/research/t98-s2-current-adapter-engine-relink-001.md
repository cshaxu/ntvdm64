# T98 S2 current-adapter engine relink 001

## Result

**Source-built (compile/link only); no runtime claim.**

The T98 r3 derivative was generated from the pinned r5 island and the named
MSVC x86 target completed successfully:

```text
nmake /f ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe
```

The output executable is
`artifacts/build/bochs-2.6-t98-current-adapter-msvc-r3/ntdos64-t98-current-adapter.exe`.
Its SHA-256 is
`D79A29BE3541C61C3BCB4504C38265EFB9265AD5CCC7AFAFA7C8909547901E7B`.
The map SHA-256 is
`A79AC1E2948A2E8C96F7758A3A3E7CCAA087D4905CDB0A1EB1FCA6048FB4B15C`.

## Closure correction

The first r2 link compiled its 65 adapter/CLI objects but failed only on
`_bx_ntvdm_bop_catalog_v1_observe`, referenced by retained
`cpu/exception.o`. The missing object is the existing adapter BOP-observation
catalog, not a Bochs feature or a device. The r3 closure adds its source,
bringing the current closure to 66 compiled objects; the named link then
succeeds.

## Retention verification

The following r3 link inputs were compared against r5 and were byte-identical:

| Retained input | SHA-256 |
| --- | --- |
| `main.o` | `655C7550401A151FB62E3C3FCC44A74698BC468D22A287ECA556A6D1C87AA85A` |
| `cpu/exception.o` | `23AB5DE4A1BD575251270ECDBC3B0937065CB2901B8B7873AF2DF1D16B448FEC` |
| `cpu/libcpu.a` | `E017EE968447D9F8DEDDF6F3277C1FEF9D213D04F097D57C99E45B09281BD4DB` |
| `iodev/libiodev.a` | `2850F6C041B2401B3627EF98D11E19CC6F13F9BD59BD6AA075E9FEC2BE195A1C` |
| `memory/libmemory.a` | `1FD886D3C692913BBABABB6B275A85C7BC7B0273339DB69A6B9CB7F7984C4FD0` |
| `gui/libgui.a` | `A2FE28E22E47549612D7F2E6FB673FEF5506F6B6B3C79F1BE9935F8FFC0C30C2` |

The build log contains 66 `cl.exe` invocations, zero recursive archive
`nmake.exe` invocations, and zero Bochs C/C++ source compile commands. The
full log remains next to the generated artifact as `t98-s2-build.log`.

## Boundary

This proves the first current adapter/profile composition can link into the
retained engine without rebuilding Bochs. It does not prove startup, BOP
arrival, transaction commit, DOS operation or guest stability. Those belong to
the separately admitted T98 S3 runtime checkpoint.
