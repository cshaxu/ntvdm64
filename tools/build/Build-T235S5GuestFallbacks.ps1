[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) { (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw "BuildRoot must remain below ${buildBase}: $build" }
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }

& (Join-Path $repository 'tools\build\Build-T235S5HimemFallback.ps1') -RepositoryRoot $repository -BuildRoot (Join-Path $build 'himem')
$source = Join-Path $repository 'src\opennt\base\mvdm'
$tools = Join-Path $repository 'tools\historical\opennt-4.5'
$mvdm = Join-Path $build 'redir\base\mvdm'
New-Item -ItemType Directory -Path $mvdm | Out-Null
Copy-Item -LiteralPath (Join-Path $source 'dos'), (Join-Path $source 'inc') -Destination $mvdm -Recurse
$redir = Join-Path $mvdm 'dos\v86\redir'
Copy-Item -LiteralPath (Join-Path $tools 'masm.exe'), (Join-Path $tools 'link16.exe') -Destination $redir
Copy-Item -LiteralPath (Join-Path $redir 'usa\redirmsg.inc') -Destination (Join-Path $redir 'redirmsg.inc')
New-Item -ItemType Directory -Path (Join-Path $redir 'obj') | Out-Null
Push-Location $redir
try {
    foreach ($module in @('redir','resident','namepipe','mailslot','netapis','int2a','int5c','neterror','msgapi')) {
        & .\masm.exe '/I..\inc' '/I..\..\..\inc' '/DDEBUG=0' '/DCALL_DOS' '/Mx' ("$module.asm,obj\$module.obj;")
        if ($LASTEXITCODE -ne 0) { throw "MASM failed for ${module}: $LASTEXITCODE" }
    }
    [IO.File]::WriteAllText((Join-Path $redir 'redir.lnk'), "obj\redir.obj+obj\resident.obj+obj\namepipe.obj+obj\mailslot.obj+obj\netapis.obj+obj\int2a.obj+obj\int5c.obj+obj\neterror.obj+obj\msgapi.obj`r`nobj\redir.exe`r`nobj\redir.map`r`n`r`n`r`n", [Text.ASCIIEncoding]::new())
    & .\link16.exe '/CP:1' '@redir.lnk'
    if ($LASTEXITCODE -ne 0) { throw "LINK16 failed for REDIR: $LASTEXITCODE" }
} finally { Pop-Location }

$result = Join-Path $build 'fallback-artifacts'
New-Item -ItemType Directory -Path $result | Out-Null
Copy-Item -LiteralPath (Join-Path $build 'himem\base\mvdm\dos\v86\dev\himem\HIMEM.SYS'), (Join-Path $redir 'obj\redir.exe') -Destination $result
$expected = [ordered]@{ 'HIMEM.SYS' = '08aa2c47d835460ed3067fa7d6f8a3b37edeca524ad102b0588fdd1bf389ce08'; 'REDIR.EXE' = 'b6e9fad30a5423ead9ecb45c8e28197ea62a39187d36241f9018db82facac3a7' }
foreach ($name in $expected.Keys) {
    $file = Join-Path $result $name
    if (!(Test-Path -LiteralPath $file) -or (Get-Sha256 $file) -ne $expected[$name]) { throw "Source-built identity mismatch: $file" }
}
[ordered]@{ schema='runner.t235.s5.guest-fallbacks.v1'; origin='source-built'; outputs=@($expected.Keys | Sort-Object | ForEach-Object { [ordered]@{ file=$_; sha256=$expected[$_] } }) } | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $result 'manifest.json') -Encoding utf8
Write-Host "OpenNT source-built fallbacks completed: $result"
