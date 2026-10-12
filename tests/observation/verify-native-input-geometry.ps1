param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerWrapper)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Require fresh build-owned evidence'}
$null=New-Item -ItemType Directory $root
$source=Get-Content "$repo/src/ntvwm-exe/presentation.c" -Raw
$begin=$source.IndexOf('DWORD ntvwm_presentation_input(')
$end=$source.IndexOf('DWORD ntvwm_presentation_text(',$begin)
if($begin -lt 0 -or $end -le $begin){throw 'Production input function boundary missing'}
[IO.File]::WriteAllText("$root/native_input_geometry.inc",$source.Substring($begin,$end-$begin),[Text.Encoding]::ASCII)
Get-FileHash "$repo/src/ntvwm-exe/presentation.c","$repo/tests/component-integration/native_input_geometry_test.c" | ConvertTo-Json | Set-Content "$root/inputs.json"
& $CompilerWrapper cl.exe /nologo /MT /O2 /W4 /WX /TC /std:c11 "/I$root" "/I$repo/src" "/Fo$root/test.obj" "/Fe$root/test.exe" "$repo/tests/component-integration/native_input_geometry_test.c" /link /STACK:8388608 *> "$root/build.log"
if($LASTEXITCODE){throw 'Native input geometry build failed'}
& "$root/test.exe" *> "$root/result.txt"
if($LASTEXITCODE){throw 'Native input geometry assertions failed'}
Get-Content "$root/result.txt"
