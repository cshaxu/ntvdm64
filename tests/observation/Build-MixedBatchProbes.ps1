[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned probe outputs'}
$null=New-Item -ItemType Directory -Path $out
$source=Join-Path $PSScriptRoot 'mixed_batch_probe.c'
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$rows=[Collections.Generic.List[object]]::new()
foreach($width in 'x86','x64'){
 $wrapper=Join-Path $out "$width.cmd"
 [IO.File]::WriteAllText($wrapper,"@echo off`r`ncall `"$vs`" -arch=$width -host_arch=x64 >nul`r`nif errorlevel 1 exit /b %errorlevel%`r`n%*`r`n",[Text.ASCIIEncoding]::new())
 foreach($kind in 'cui','gui'){
  $stem="$width-$kind";$link=if($kind -eq 'gui'){@('/DBATCH_GUI','/link','/subsystem:windows','/entry:wWinMainCRTStartup','shell32.lib')}else{@('/link','/subsystem:console','/entry:wmainCRTStartup')}
  Push-Location $out
  try{
   & $wrapper cl.exe /nologo /MT /W4 /we4013 /we4311 /we4302 /DUNICODE /D_UNICODE $source "/Fo$out/$stem.obj" "/Fe$out/$stem.exe" @link *> "$out/$stem.log"
   if($LASTEXITCODE){throw "Probe compile failed $stem"}
   $rows.Add(@{File="$stem.exe";Width=$width;Kind=$kind;Sha256=(Get-FileHash "$out/$stem.exe").Hash;SourceHash=(Get-FileHash $source).Hash})
  }finally{Pop-Location}
 }
}
$rows|ConvertTo-Json|Set-Content "$out/manifest.json"
Push-Location $out
try{
 & (Join-Path $out 'x64.cmd') cl.exe /nologo /MT /W4 /we4013 "/I$repo/src/opennt-abi/source/public/internal/windows/inc" "$PSScriptRoot/mixed_batch_pif.c" "/Fo$out/pif.obj" "/Fe$out/pif-builder.exe" *> "$out/pif-build.log"
 if($LASTEXITCODE){throw 'Authored PIF builder failed'}
}finally{Pop-Location}
'PASS authored mixed-batch CUI/GUI probes built at both widths; no product code linked'
