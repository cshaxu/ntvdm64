param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$package = (Resolve-Path -LiteralPath $PackageRoot).Path.TrimEnd('\')
$build = [IO.Path]::GetFullPath($BuildRoot)
if(Test-Path -LiteralPath $build) {throw 'Use a fresh build-owned verification output'}
New-Item -ItemType Directory -Path $build | Out-Null

foreach($item in @(
    @{ compressed='KRNL386.EX_'; candidate='PATCH\KRNL386-ADAPTED.EXE'; output='KRNL386.EXE' },
    @{ compressed='WIN386.EX_'; candidate='PATCH\WIN386-ADAPTED.EXE'; output='WIN386.EXE' }
)) {
    $compressed = Join-Path $package $item.compressed
    $candidate = Join-Path $package $item.candidate
    $expanded = Join-Path $build $item.output
    if(!(Test-Path -LiteralPath $compressed -PathType Leaf) -or
       !(Test-Path -LiteralPath $candidate -PathType Leaf)) {
        throw "Missing derived-media input: $($item.compressed)"
    }
    & expand.exe $compressed $expanded | Out-Null
    if($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $expanded) -or
       (Get-FileHash -LiteralPath $expanded).Hash -ne (Get-FileHash -LiteralPath $candidate).Hash) {
        throw "Derived $($item.compressed) does not expand to its checked candidate"
    }
    if(Test-Path -LiteralPath (Join-Path $package ($item.output))) {
        throw "Derived media must not add uncompressed $($item.output) beside Setup input"
    }
}
'PASS derived KRNL386.EX_/WIN386.EX_ expand exactly to the approved candidates'
