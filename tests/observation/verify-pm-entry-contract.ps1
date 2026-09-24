param([string]$BuildRoot = 'build/M0-T422/S2/pm-entry-contract')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$out.StartsWith((Join-Path $repo 'build/'),[StringComparison]::OrdinalIgnoreCase)) { throw 'Build root must be under build' }
New-Item -ItemType Directory -Path $out -Force | Out-Null
$source=Join-Path $repo 'src/mvdm/dpmi32/modesw.c'
$text=[IO.File]::ReadAllText($source)
$body=[regex]::Match($text,'(?ms)^VOID\r?\nDpmiCpu40SwitchToProtectedMode\(.*?^\}')
if (!$body.Success) { throw 'Production PM entry body not found' }
# Generated test input is the complete current production function, not a
# copied sanitization expression. CPU/descriptor dependencies are test doubles.
[IO.File]::WriteAllText((Join-Path $out 'pm_entry_body.inc'),$body.Value)
$fixture=Join-Path $repo 'tests/mvdm-host/dpmi/pm_entry_contract_fixture.c'
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x86 >nul && cl /nologo /W4 /MT /TC /I"'+$out+'" /Fo"'+$out+'\fixture.obj" /Fe"'+$out+'\fixture.exe" "'+$fixture+'"')
if ($LASTEXITCODE) { throw 'PM entry fixture build failed' }
& (Join-Path $out 'fixture.exe')
if ($LASTEXITCODE) { throw 'PM entry contract failed' }
Get-FileHash $source,$fixture,(Join-Path $out 'fixture.exe') | Format-List
