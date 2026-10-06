[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-only output'}
New-Item -ItemType Directory -Path $build -Force|Out-Null
$source=Join-Path $repo 'tests/component-integration/native_observation_probe.cpp'
$utf8=[Text.UTF8Encoding]::new($false)
foreach($width in @('x86','x64')) {
 $wrapper=Join-Path $build "$width.cmd"
 $command='@echo off'+"`r`n"+'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch='+$width+' -host_arch=x64 >nul'+"`r`n"+'if errorlevel 1 exit /b %errorlevel%'+"`r`n"+'cl.exe /nologo /MT /W4 /EHsc /D_WIN32_WINNT=0x0A00 /Fo"'+(Join-Path $build "$width.obj")+'" /Fe"'+(Join-Path $build "$width.exe")+'" "'+$source+'" /link /subsystem:console /incremental:no'+"`r`n"
 [IO.File]::WriteAllText($wrapper,$command,$utf8)
 & $wrapper *> (Join-Path $build "$width.log")
 if($LASTEXITCODE){throw "Observation probe $width build failed"}
}
@{Source=$source;Sha256=(Get-FileHash $source).Hash;Architectures=@('I386','AMD64')}|ConvertTo-Json|Set-Content (Join-Path $build 'inputs.json')
'PASS independently authored observation probes x86/x64'
