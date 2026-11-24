param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned evidence root required'}
$null=New-Item -ItemType Directory $out
& "$repo/tools/build-fixtures.cmd" x86 mouse-callback-reentry *> "$out/build.log"
if($LASTEXITCODE){throw 'Mouse callback fixture build failed'}
& "$repo/build/.ninja/fixtures-x86/mouse-callback-reentry.exe" *> "$out/result.txt"
if($LASTEXITCODE){throw 'Mouse callback frame regression'}
Get-FileHash "$repo/src/mvdm-host/softpc.new/base/keymouse/mouse_io.c","$PSScriptRoot/fixtures/mouse_callback_reentry.c","$repo/build/.ninja/fixtures-x86/mouse-callback-return.inc","$repo/build/.ninja/fixtures-x86/mouse-callback-enter.inc"|Export-Csv -NoTypeInformation "$out/inputs.csv"
Get-Content "$out/result.txt"
