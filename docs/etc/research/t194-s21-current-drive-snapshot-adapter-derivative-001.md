# T194 S21 current drive-snapshot adapter derivative 001

Date: 2026-08-12  
Packet: M0 T194 S21  
Disposition: source-built derivative complete; no engine process started.

## Build

`New-T98S1CurrentAdapterEngineDerivative.ps1` generated
`artifacts/build/current/t194-s21-current-adapter-v6-r3`, then MSVC x86
compiled 68 adapter objects and five CLI objects and linked the derivative.
The new provider object
`adapter/bx_ntvdm_dem_drive_provider_v1.obj` is both an explicit make
prerequisite and a final-link input.

The resulting engine is
`ntdos64-t98-current-adapter.exe`, SHA-256
`3F2187B9153E5DB181880BB78376B17F197774D4C73CA0A2E7B086A7088DF2F4`.

## Bochs preservation

The generated manifest records `bochsReplacementCount: 0`. Its default make
shim has no `main.o:` or `cpu\\exception.o:` rule. Retained-input hashes are:

| Input | SHA-256 |
| --- | --- |
| `main.o` | `655C7550401A151FB62E3C3FCC44A74698BC468D22A287ECA556A6D1C87AA85A` |
| `cpu\\exception.o` | `23AB5DE4A1BD575251270ECDBC3B0937065CB2901B8B7873AF2DF1D16B448FEC` |
| `cpu\\libcpu.a` | `E017EE968447D9F8DEDDF6F3277C1FEF9D213D04F097D57C99E45B09281BD4DB` |
| `iodev\\libiodev.a` | `2850F6C041B2401B3627EF98D11E19CC6F13F9BD59BD6AA075E9FEC2BE195A1C` |
| `memory\\libmemory.a` | `1FD886D3C692913BBABABB6B275A85C7BC7B0273339DB69A6B9CB7F7984C4FD0` |
| `gui\\libgui.a` | `A2FE28E22E47549612D7F2E6FB673FEF5506F6B6B3C79F1BE9935F8FFC0C30C2` |

No Bochs archive, device, configuration or source was rebuilt or changed.

## Follow-up

T194 S22 may assemble a new immutable fixed bundle around this engine and
verify its manifest. A runtime observation remains separately admitted.
