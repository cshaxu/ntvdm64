# T95 S6 MSVC Native-Container Build Shim 001

## Result

The original imported `src/bochs/Makefile` is already a generated MSVC/nmake
makefile: it names `cl`, `/MT`, `lib`, `link`, original recursive archive
targets and the full native lifecycle link order.  It is therefore the
appropriate retained build owner.  The historical file's Win32/RFB linker
inputs are preserved as original compiled support; they are not a runtime
feature admission.

The fresh build-root generator
`tools/New-T95S6MSVCNativeContainerBuildRoot.ps1` now creates the first
non-product container root:

`artifacts/build/bochs-2.6-native-nogui-msvc-cpu3-r1/`

It hashes the manifest inputs before copying the imported tree, replaces only
the copied-root `config.h` with the accepted v2 CPU3 projection
`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`, copies
the two declared ROM inputs and profile, and writes a root-local
`ntdos64-native-container.mak`.

## Target construction

The shim includes the copied original `Makefile` unchanged.  Its sole target,
`ntdos64-native-container.exe`, is a mechanical line-for-line transform of
the original `bochs.exe` target: only the target name is replaced.  Thus it
retains original lifecycle objects, recursively requested original archives,
link order and link flags while neither building an `all` target nor producing
a `bochs.exe` artifact.

This is intentionally not source-object minimization.  CPU, memory, SIM,
device manager, logging and PC-system ownership remain Bochs-owned and the
adapter/CLI/OpenNT are absent from the copied root and target.

## Dry-run evidence

One admitted command was executed after static target comparison:

```text
nmake /n /f ntdos64-native-container.mak ntdos64-native-container.exe
```

The retained `ntdos64-native-container-dry-run.log` has 47 lines.  It names
the original `libiodev.a`, `libcpu.a`, `libmemory.a`, and `libgui.a` archive
subtargets; root lifecycle compilation commands; and exactly one final link
line with `/out:ntdos64-native-container.exe`.  It contains no `all`,
`bochs.exe`, adapter or OpenNT reference.  The dry run created zero `.obj`
files and no target executable.

NMAKE does not recursively expand its printed child `nmake` calls under this
top-level `/n` observation.  Therefore the record proves only the target
command surface and original dependency direction, not source compilation,
archive production, link success, reset, or runtime behavior.

## Next gate

The build shim is now stable enough for a separate single-invocation admission:
`nmake /f ntdos64-native-container.mak ntdos64-native-container.exe` in this
fresh root, with the original MSVC x86 environment.  That future observation
must stop at its first compiler/archive/link boundary, retain logs and map
inputs, and make no executable or guest-runtime claim unless a later dedicated
reset/controlled-stop admission succeeds.
