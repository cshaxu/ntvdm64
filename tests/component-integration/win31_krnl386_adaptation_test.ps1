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
$generator=Join-Path $repo 'tools/win31-setup/adapt-retail-krnl386.ps1'
& $generator -OriginalKrnl386 $OriginalKrnl386 -OutputRoot "$root/positive"
$output=Join-Path $root 'positive/KRNL386.EXE'
if((Get-FileHash -LiteralPath $output).Hash -ne '88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181') {
    throw 'Changed candidate identity'
}
$original=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $OriginalKrnl386).Path)
$changed=[IO.File]::ReadAllBytes($output)
if($original.Length -ne $changed.Length){throw 'File extent changed'}
$offset=0xCFED;$size=40;$differences=0
for($i=0;$i -lt $original.Length;$i++) {
    if($original[$i] -eq $changed[$i]){continue}
    if($i -lt $offset -or $i -ge $offset+$size){throw "Unexpected changed byte $i"}
    $differences++
}
if(!$differences){throw 'No substitution made'}
$manifest=Get-Content -LiteralPath "$root/positive/adaptation.json" -Raw|ConvertFrom-Json
if(!$manifest.allOtherBytesUnchanged -or $manifest.offset -ne '0xCFED' -or $manifest.length -ne $size){
    throw 'Wrong adaptation contract'
}
$rejected=$false
try { & $generator -OriginalKrnl386 $output -OutputRoot "$root/rejected" }
catch { if($_.Exception.Message -ne 'Unsupported retail KRNL386 identity'){throw};$rejected=$true }
if(!$rejected -or (Test-Path -LiteralPath "$root/rejected")){throw 'Non-original input not safely rejected'}
'PASS KRNL386 exact candidate, one changed range, all-other-byte preservation and wrong-input rejection'
