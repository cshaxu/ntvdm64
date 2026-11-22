$ErrorActionPreference='Stop'
& test/integration/verify-win16-message-probe.ps1 -PackageRoot "$PSScriptRoot/candidate-final" -Observer build/t483-s13-dpmi-boundary/observers/console-startup-observer.exe -CompilerDriver build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd -BuildRoot "$PSScriptRoot/native-guest-final" -NativeExtension
if($LASTEXITCODE){exit $LASTEXITCODE}
& test/integration/verify-command-native-edit-return.ps1 -Observer build/t483-s13-dpmi-boundary/observers/console-startup-observer.exe -PackageRoot "$PSScriptRoot/candidate-final" -ReportPath "$PSScriptRoot/command-final.txt" -LogonHelper "$PSScriptRoot/native-guest-final/logon-env.dll"
exit $LASTEXITCODE
