# T95 S6 Intact Native Build Root 001

## Result

`tools/build/New-T95S6IntactNativeBuildRoot.ps1` created:

`artifacts/build/bochs-2.6-native-intact-msvc-r1/`

It copied the intact native tuple without altering `config.h`, `config.status`
or any generated Makefile.  All 41 tuple makefile/template hashes match the
frozen manifest after copying.  The declared runtime profile and ROMs are
copied separately as runtime inputs.

The root-local `ntdos64-native-container.mak` includes the copied original
Makefile unchanged.  Its only target is a mechanical target-name transform of
the original executable recipe; source/archive/link membership is unchanged.

## Dry-run

One permitted command completed:

```text
nmake /n /f ntdos64-native-container.mak ntdos64-native-container.exe
```

The 47-line log names original archive subtargets (`iodev`, CPU, memory, GUI,
disassembly and FPU), root lifecycle objects, and a final link line whose
output is `ntdos64-native-container.exe`.  It contains no `all`, `bochs.exe`,
adapter or OpenNT reference.  It produced zero `.o` files and no target
executable.

This proves the command surface only; child nmake calls remain unexpanded in
the top-level dry-run, and no compile/link/reset/runtime conclusion follows.

## Next gate

One separately admitted actual invocation may now use this exact fresh root
and target.  It must be a single no-retry observation, retain the whole log,
stop at its first boundary, and not run the resulting executable.
