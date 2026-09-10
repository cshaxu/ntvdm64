# M0 T194 S26: Current-Adapter DPB Derivative

Packet: M0 T194 S26  
Disposition: source-built derivative complete; engine not started.

## Procedure

`tools/build/New-T98S1CurrentAdapterEngineDerivative.ps1` generated `artifacts/build/current/t194-s26-current-adapter-v6-r4` from the retained r5 machine tree and current adapter/CLI source. Then the documented MSVC x86 command built only the generated target:

```text
cmd.exe /d /c call VsDevCmd.bat -arch=x86 -host_arch=x86 &&
  nmake /f ntdos64-t98-current-adapter.mak ntdos64-t98-current-adapter.exe
```

The command completed successfully. It compiled 68 adapter and five CLI objects, then linked `ntdos64-t98-current-adapter.exe`.

## Identity And Closure

The derivative engine SHA-256 is `4A562EBDCD50E3FD16062013B1A9891FBEE84AB602B120925D46C756D7B01531`.

The generated manifest records `bochsReplacementCount: 0`; the generated shim has neither `main.o:` nor `cpu\\exception.o:` target. Its retained inputs are unchanged from S21:

| Input | SHA-256 |
| --- | --- |
| `main.o` | `655C7550401A151FB62E3C3FCC44A74698BC468D22A287ECA556A6D1C87AA85A` |
| `cpu\\exception.o` | `23AB5DE4A1BD575251270ECDBC3B0937065CB2901B8B7873AF2DF1D16B448FEC` |
| `cpu\\libcpu.a` | `E017EE968447D9F8DEDDF6F3277C1FEF9D213D04F097D57C99E45B09281BD4DB` |
| `iodev\\libiodev.a` | `2850F6C041B2401B3627EF98D11E19CC6F13F9BD59BD6AA075E9FEC2BE195A1C` |
| `memory\\libmemory.a` | `1FD886D3C692913BBABABB6B275A85C7BC7B0273339DB69A6B9CB7F7984C4FD0` |
| `gui\\libgui.a` | `A2FE28E22E47549612D7F2E6FB673FEF5506F6B6B3C79F1BE9935F8FFC0C30C2` |

No Bochs source, object, archive, configuration, device, ROM, or execution behavior changed. The derivative's only new source meaning is the already tested adapter runtime DPB snapshot route from S25.

## Follow-up

A later packet may assemble an immutable bundle around this exact executable; a separate packet must admit any bounded engine observation.
