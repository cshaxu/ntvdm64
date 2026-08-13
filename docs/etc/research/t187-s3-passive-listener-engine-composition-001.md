# T187 S3 passive-listener engine composition 001

Date: 2026-08-12  
Packet: M0 T187 S3  
Disposition: source/build closed; no guest observation.

## Inputs and closure

The generator `tools/New-T187S3PassiveListenerEngineDerivative.ps1` copied the
retained current v5 engine closure into
`artifacts/build/current/t187-passive-listener-engine-r5`. Its manifest pins
the base engine SHA-256
`DE750579769772AFF1F5ECCB8AD1C4F8215A0FC4A8D21C438CDECFF8B9AE528A`, 72
retained adapter/CLI object paths, and retained Bochs archive/machine hashes.
The generated makefile declares exactly one Bochs object rule:
`cpu/exception.o: cpu/exception.cc`, with the previously pinned listener,
deferred-startup and machine-composition macro set.  No archive, device,
configuration, main, adapter or CLI object is a make target.

Earlier generator roots r1--r4 are rejected: r1--r3 stopped during
input/structural checks before any compiler or linker invocation; r4 exposed
a generator argument-binding side effect that created an untracked duplicate
source file outside `artifacts/`. That duplicate was hash-verified, removed,
and the generator changed to named parameters. r5 is the sole formal build
root and regenerating it confirms no such file is created.

## Build

The installed Visual Studio 2022 Build Tools x86 environment ran:

```text
nmake /f ntdos64-t187-passive-listener.mak ntdos64-t187-passive-listener.exe
```

It compiled only `cpu/exception.cc` and linked the final executable. The
resulting hashes are:

| Artifact | SHA-256 |
| --- | --- |
| `cpu/exception.o` | `C90E52F001307F469A974BF21238CA558FD15C17D8F9C293AD0EFC2E501E8AD3` |
| `ntdos64-t187-passive-listener.exe` | `EB55EFA8CBBB4E9B640D3AC2BFAF054AE551EB0EB793A597733E660F329CCA57` |
| `ntdos64-t187-passive-listener.map` | `9E861B4A1442004CF0E5378D0F2208213893CF4D061C6D2461129AFF7623DAB4` |

The build exited zero. It retained pre-existing warnings: duplicate `main.o`,
MSVCRT/UCRT model conflict, one UCRT import warning from `illegalp.c.obj`, and
the existing conversion warning in `exception.cc`. No warning is interpreted
as a runtime claim.

## Limitation

This result makes the passive `dx=%04x` field available in a fixed engine but
does not execute it. A future guest observation requires its own admission,
fresh runner/bundle composition and one-shot watchdog contract; link success
does not classify the T186 `54:0C` pass-through.
