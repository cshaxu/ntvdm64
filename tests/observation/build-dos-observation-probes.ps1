[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory -Path $build|Out-Null
$source=Join-Path $PSScriptRoot 'dos_observation_probe.asm'
$nasm=(Get-Command nasm.exe -ErrorAction Stop).Source
foreach($variant in @(
 @{Name='P';Flags=@('-DROOT=1')},@{Name='PN';Flags=@('-DROOT=1','-DEXCLUSIONS=1')},
 @{Name='P2';Flags=@('-DROOT=1','-DSECOND=1')},@{Name='C';Flags=@()},
 @{Name='F';Flags=@('-DFAST=1')},@{Name='TSR';Flags=@('-DTSR=1')})){
 & $nasm -f bin @($variant.Flags) $source -o "$build/$($variant.Name).COM"
 if($LASTEXITCODE){throw "Guest witness assembly failed: $($variant.Name)"}
}
$wrapper=Join-Path $build 'build-pif.cmd'
$command='@echo off'+"`r`n"+'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul'+"`r`n"+'if errorlevel 1 exit /b %errorlevel%'+"`r`n"+'cl.exe /nologo /MT /W4 /I"'+$repo+'\src\opennt-abi\source\public\internal\windows\inc" /Fo"'+$build+'\pif.obj" /Fe"'+$build+'\pif.exe" "'+$PSScriptRoot+'\dos_observation_pif.c"'+"`r`n"
[IO.File]::WriteAllText($wrapper,$command,[Text.UTF8Encoding]::new($false))
& $wrapper *> (Join-Path $build 'pif-build.log')
if($LASTEXITCODE){throw 'Authored PIF builder failed'}
@{GuestSource=$source;GuestSha256=(Get-FileHash $source).Hash;Assembler=$nasm;
  AssemblerVersion=(& $nasm -v);PifSourceSha256=(Get-FileHash "$PSScriptRoot/dos_observation_pif.c").Hash;
  Outputs=@(Get-ChildItem $build -File|Where-Object Extension -in @('.COM','.exe')|ForEach-Object {
   @{Name=$_.Name;Sha256=(Get-FileHash $_.FullName).Hash}
  })}|ConvertTo-Json -Depth 5|Set-Content (Join-Path $build 'inputs.json')
'PASS authored COM witnesses and native PIF builder; not guest replacements'
