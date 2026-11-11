[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildCache,[Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cache=(Resolve-Path $BuildCache).Path;$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
 (Test-Path $output)){throw 'Fresh build-owned diagnostic output required'}
$null=New-Item -ItemType Directory -Path $output
$graph=Get-Content (Join-Path $cache 'build.ninja') -Raw
$flags=[regex]::Match($graph,'(?m)^cflags = (.*)$').Groups[1].Value.Replace('$:',':')
$inputs=[regex]::Match($graph,'(?m)^build ntsrv.exe: link (.*)$').Groups[1].Value
if(!$flags -or !$inputs -or !$inputs.Contains('obj/basesrv/entry.obj')){throw 'Expected selected native service graph'}
$source=Get-Content (Join-Path $repo 'src/ntsrv-exe/main.c') -Raw
$needle='if(!error)error=OpenNtBaseServiceObserveNativeCreation(service,reporter,child,flags,&identity);'
if([regex]::Matches($source,[regex]::Escape($needle)).Count -ne 1){throw 'Fault boundary changed'}
$sdk='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$results=@()
foreach($fault in @('deny','slow')){
 $replacement=if($fault -eq 'deny'){'if(!error)error=ERROR_ACCESS_DENIED;'}else{'if(!error){Sleep(2000);error=OpenNtBaseServiceObserveNativeCreation(service,reporter,child,flags,&identity);}'}
 # Generated test-only translation unit; production source is never edited.
 $file=Join-Path $output ($fault+'.c');$object=Join-Path $output ($fault+'.obj');$image=Join-Path $output ($fault+'.exe')
 [IO.File]::WriteAllText($file,$source.Replace($needle,$replacement),[Text.UTF8Encoding]::new($false))
 $linkInputs=$inputs.Replace('obj/basesrv/entry.obj','"'+$object+'"')
 $command="@echo off`r`ncall `"$sdk`" -arch=x64 -host_arch=x64 >nul`r`nif errorlevel 1 exit /b %errorlevel%`r`ncd /d `"$cache`"`r`ncl.exe $flags /Fo`"$object`" `"$file`"`r`nif errorlevel 1 exit /b %errorlevel%`r`nlink.exe /nologo /machine:x64 /subsystem:windows /entry:mainCRTStartup /opt:ref /out:`"$image`" $linkInputs rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib`r`n"
 $runner=Join-Path $output ($fault+'.cmd');[IO.File]::WriteAllText($runner,$command,[Text.Encoding]::ASCII)
 & $runner *> (Join-Path $output ($fault+'.log'))
 if($LASTEXITCODE){throw "Diagnostic service build failed: $fault"}
 $results+=,[pscustomobject]@{Fault=$fault;SourceHash=(Get-FileHash $file).Hash;ImageHash=(Get-FileHash $image).Hash}
}
$results|ConvertTo-Json|Set-Content (Join-Path $output 'manifest.json')
'PASS two test-only service variants; never publish these binaries'
