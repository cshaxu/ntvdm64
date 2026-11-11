[CmdletBinding()]
param([Parameter(Mandatory)][string]$Fixture,[Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
$build=[IO.Path]::GetFullPath((Join-Path $repo 'build'))+'\'
if(!$root.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)) {
    throw 'Use a fresh build-owned fixture root'
}
$source=(Resolve-Path -LiteralPath $Fixture).Path
$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe',
    'WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll')
$ids=@([guid]::NewGuid(),[guid]::NewGuid())
$results=@()
for($i=0;$i -lt 2;$i++) {
    $dir=Join-Path $root "package-$i"
    New-Item -ItemType Directory -Path $dir -Force | Out-Null
    $paths=@()
    foreach($name in @('identity-test.exe')+$names) {
        $path=Join-Path $dir $name
        Copy-Item -LiteralPath $source -Destination $path
        $paths+=$path
    }
    & "$repo/tools/build/Set-PackageIdentity.ps1" -Image $paths -PackageId $ids[$i]
    $text=& (Join-Path $dir 'identity-test.exe') accept
    if($LASTEXITCODE){throw "Package $i rejected: $text"}
    $results+=($text -join '')
}
if($results[0] -eq $results[1]){throw 'Distinct package identities produced identical endpoints'}
$dir=Join-Path $root 'package-0'
Copy-Item -LiteralPath (Join-Path $root 'package-1/ntvdm.exe') -Destination (Join-Path $dir 'ntvdm.exe') -Force
$text=& (Join-Path $dir 'identity-test.exe') reject
if($LASTEXITCODE){throw "Mixed package not rejected: $text"}
Copy-Item -LiteralPath $source -Destination (Join-Path $dir 'ntvdm.exe') -Force
$text=& (Join-Path $dir 'identity-test.exe') reject
if($LASTEXITCODE){throw "Missing resource not rejected: $text"}
$results
'PASS separate package IDs, mixed-component rejection, missing-resource rejection'
