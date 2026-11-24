[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
# Compile-only declaration contract: explicit /Gd must not hide missing WINAPI.
# Invoke from the owned background supervisor.
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory $out
try {
 & "$repo/tools/build-fixtures.cmd" x86 wow-public-memory-abi *> "$out/build.log"
 if($LASTEXITCODE){throw 'Native public ABI compile failed'}
 $native="$repo/build/.ninja/msvc-x86-release/wow32"
 $object=(Get-Content "$native/public-memory-abi-object.txt" -Raw).Trim()
 if(!(Test-Path $object)){throw 'Current ABI object missing'}
 Get-FileHash "$repo/test/adapter-mvdm-host-out/wow/wow_public_memory_abi_fixture.c",$object|Export-Csv -NoTypeInformation "$out/inputs.csv"
 'WOW_PUBLIC_MEMORY_ABI_OK'|Set-Content "$out/result.txt"
 '0'|Set-Content "$out/exit.txt"
}catch{$_|Out-String|Set-Content "$out/failure.txt";'1'|Set-Content "$out/exit.txt";throw}
