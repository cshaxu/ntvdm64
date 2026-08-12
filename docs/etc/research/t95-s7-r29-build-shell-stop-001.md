# T95 S7 r29 Build Shell Stop 001

## Question

Did the one admitted r29 MSVC/x86 runtime-trace build reach its declared
Bochs/adapter compilation boundary?

## Inputs and procedure

The fresh, generator-hashed root is
`artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r29`.  The admitted
build command was the existing `nmake /f ntdos64-s7-runtime-trace.mak
ntdos64-s7-runtime-trace.exe` after `VsDevCmd.bat` configured x86 tools.  Its
PowerShell wrapper was asked to preserve standard output in
`r29-build.stdout.log`.

## Observation

The wrapper failed while PowerShell parsed the quoted Visual Studio path:
`(x86)` was interpreted as a PowerShell expression and produced
`x86 : The term 'x86' is not recognized`.  No `cmd.exe`, `VsDevCmd.bat`,
`nmake`, compiler, archive, linker, or runtime executable was invoked.

## Interpretation and follow-up

This is a host-shell quoting failure before the admitted build action, not a
Bochs, adapter, OpenNT, ABI, profile, or machine result.  r29 remains
immutable and cannot be retried.  A later fresh-root admission may change only
the host invocation to pass the complete `cmd /d /c` argument as one literal
string; it must retain the identical source closure, profile, feature set and
build command.  No runtime trace is authorized from r29.
