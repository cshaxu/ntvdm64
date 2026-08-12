# T95 S6 MSVC Native-Container Build Observation 001

## Result

The single admitted MSVC/x86 native-container invocation was run in the frozen
fresh root:

```text
nmake /f ntdos64-native-container.mak ntdos64-native-container.exe
```

It exited `1` at the first configuration-to-makefile closure boundary.  This
is not a compiler, CRT, adapter, OpenNT, linker, firmware or runtime failure.
No retry was performed.

## Exact boundary

The copied accepted v2 configuration sets `BX_SUPPORT_GAMEPORT` to `0`.
The copied original generated `iodev/Makefile`, however, unconditionally adds
`gameport.o` to `OBJS_THAT_CAN_BE_PLUGINS`, then to `NONPLUGIN_OBJS` because
the historical file has `PLUGIN_OBJS =` empty.  The target therefore compiled
the denied `gameport.cc` anyway.

That source expects gameport declarations removed by the CPU3 denial overlay:
the first terminal diagnostic is `gameport.h(34): error C2504:
'bx_game_stub_c': base class undefined`, followed by missing
`pluginGameport`, `stubGameport`, and logging-member diagnostics.  NMAKE then
reports `U1077` for `gameport.cc` and the `iodev` archive submake.

The retained log is
`artifacts/build/bochs-2.6-native-nogui-msvc-cpu3-r1/ntdos64-native-container-build-001.log`.
It records BuildTools Developer Prompt 17.13.5 and NMAKE 14.43.34809.0.

## What the observation proves

- 25 `cl /c` commands were reached, all within the original copied `iodev`
  archive owner; the accepted projected configuration was consumable by the
  earlier device units up to the mismatch.
- The generated Makefile's static object membership is an independent product
  configuration output.  It cannot be retained unchanged while only
  `config.h` is projected to deny features.
- No archive was produced, no final link occurred, no target executable or
  link map exists, and the log contains no adapter/OpenNT path.

The copied root contains `.o` evidence from preceding original device units;
it is deliberately retained.  It is not a usable or complete archive.

## Classification and next gate

Classification: **source/configuration build-closure blocked**.  The native
container architecture and MSVC toolchain remain viable candidates, but a
`config.h`-only projection is insufficient because original generated
sub-make object membership encodes the full historical product profile.

Do not cure this by enabling gameport or by manually pruning objects.  The next
allowed analysis is to derive a complete, mechanical configuration-to-Makefile
substitution manifest from original `Makefile.in` and its recorded configure
variables.  Only if that manifest supplies every denied/required member may a
new fresh-root, root-local generated makefile be considered.  It must preserve
the original lifecycle graph and remain independent of adapter/CLI/OpenNT.
