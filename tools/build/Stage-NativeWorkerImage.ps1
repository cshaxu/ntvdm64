param([Parameter(Mandatory)][string]$InputFile,[Parameter(Mandatory)][string]$OutputFile)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=(Join-Path $repo 'build')+'\'
$inputPath=(Resolve-Path -LiteralPath $InputFile).Path
$outputPath=[IO.Path]::GetFullPath($OutputFile)
if(!$inputPath.StartsWith($root,[StringComparison]::OrdinalIgnoreCase) -or
   !$outputPath.StartsWith($root,[StringComparison]::OrdinalIgnoreCase)){throw 'Worker import must stay below build/'}
$bytes=[IO.File]::ReadAllBytes($inputPath)
if($bytes.Length -lt 64){throw 'Missing native worker PE'}
$pe=[BitConverter]::ToInt32($bytes,60)
if($pe -lt 0 -or $pe -gt $bytes.Length-94 -or [BitConverter]::ToUInt32($bytes,$pe) -ne 0x4550 -or
   [BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x8664 -or
   ([BitConverter]::ToUInt16($bytes,$pe+22) -band 0x2000) -or
   [BitConverter]::ToUInt16($bytes,$pe+24+68) -ne 3){throw 'Expected AMD64 native worker EXE'}
if($inputPath -eq $outputPath){throw 'Do not overwrite worker input'}
Copy-Item -LiteralPath $inputPath -Destination $outputPath -Force
# The build runner can invoke Windows PowerShell with the caller's module
# path. Hash through the base CLR, without importing another PS edition's DLL.
$sha=[Security.Cryptography.SHA256]::Create()
try {
 $expected=[BitConverter]::ToString($sha.ComputeHash($bytes))
 $actual=[BitConverter]::ToString($sha.ComputeHash([IO.File]::ReadAllBytes($outputPath)))
 if($actual -ne $expected){throw 'Native worker import hash mismatch'}
}finally{$sha.Dispose()}
# The map is producer evidence, never an executable input.
if(Test-Path ($inputPath+'.map')){Copy-Item -LiteralPath ($inputPath+'.map') -Destination ($outputPath+'.map') -Force}
