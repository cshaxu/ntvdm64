param(
    [Parameter(Mandatory)][string]$OriginalKrnl386,
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)) {
    throw 'Fresh build-owned test output required'
}
$generator=Join-Path $repo 'tools/build/win31-setup/adapt-retail-krnl386.ps1'
& $generator -OriginalKrnl386 $OriginalKrnl386 -OutputRoot "$root/positive"
$output=Join-Path $root 'positive/KRNL386.EXE'
if((Get-FileHash -LiteralPath $output).Hash -ne '75E931AE000FB7C70CF686DA6F7D3FCB032FDC0E0A4E3187ED68179B85E2CD4A') {
    throw 'Changed candidate identity'
}
$original=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $OriginalKrnl386).Path)
$changed=[IO.File]::ReadAllBytes($output)
if($original.Length -ne $changed.Length){throw 'File extent changed'}
$offset=0xCFED;$size=40;$differences=0
for($i=0;$i -lt $original.Length;$i++) {
    if($original[$i] -eq $changed[$i]){continue}
    if($i -ne 0x3d -and ($i -lt $offset -or $i -ge $offset+$size)){throw "Unexpected changed byte $i"}
    $differences++
}
if(!$differences){throw 'No substitution made'}
$manifest=Get-Content -LiteralPath "$root/positive/adaptation.json" -Raw|ConvertFrom-Json
if(!$manifest.allOtherBytesUnchanged -or $manifest.offset -ne '0xCFED' -or $manifest.length -ne $size -or $manifest.headerOffset -ne '0x3D' -or $original[0x3d] -ne 4 -or $changed[0x3d] -ne 0){
    throw 'Wrong adaptation contract'
}
$rejected=$false
try { & $generator -OriginalKrnl386 $output -OutputRoot "$root/rejected" }
catch { if($_.Exception.Message -ne 'Unsupported retail KRNL386 identity'){throw};$rejected=$true }
if(!$rejected -or (Test-Path -LiteralPath "$root/rejected")){throw 'Non-original input not safely rejected'}
'PASS KRNL386 exact candidate, two bounded changes, all-other-byte preservation and wrong-input rejection'
