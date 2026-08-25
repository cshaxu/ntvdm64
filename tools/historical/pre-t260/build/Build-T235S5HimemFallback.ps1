[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) {
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
$build = [IO.Path]::GetFullPath($BuildRoot)
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }

$source = Join-Path $repository 'src\opennt\base\mvdm'
$tools = Join-Path $repository 'tools\historical\opennt-4.5'
$runner = Join-Path $repository 'tools\build\Invoke-DosBoxBatchWithCompletion.ps1'
foreach ($file in @($source, $runner, (Join-Path $tools 'masm.exe'), (Join-Path $tools 'link16.exe'),
        (Join-Path $tools 'reloc.exe'), (Join-Path $tools 'stripdd.exe'))) {
    if (!(Test-Path -LiteralPath $file)) { throw "Required historical build input missing: $file" }
}

$mvdm = Join-Path $build 'base\mvdm'
$work = Join-Path $mvdm 'dos\v86\dev\himem'
New-Item -ItemType Directory -Path (Split-Path -Parent $work), (Join-Path $mvdm 'inc') -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $source 'dos\v86\dev\himem') -Destination (Split-Path -Parent $work) -Recurse
Copy-Item -LiteralPath (Join-Path $source 'inc') -Destination (Join-Path $mvdm 'inc') -Recurse
Copy-Item -Path (Join-Path $source 'inc\*') -Destination (Join-Path $mvdm 'inc') -Recurse
Copy-Item -LiteralPath (Join-Path $tools 'masm.exe'), (Join-Path $tools 'link16.exe'),
    (Join-Path $tools 'reloc.exe'), (Join-Path $tools 'stripdd.exe') -Destination $work

Push-Location $work
try {
    foreach ($module in @('himem','himem1','himem2','messages','himem4','himem5')) {
        & .\masm.exe '/I.' '/I..\..\..\..\inc' '/Mx' '/t' ("$module.asm,$module.obj;")
        if ($LASTEXITCODE -ne 0) { throw "MASM failed for ${module}: $LASTEXITCODE" }
    }
    & .\link16.exe '/MAP' 'himem+himem1+himem2+messages+himem4+himem5/map/li;'
    if ($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath 'himem.exe')) { throw 'LINK16 did not produce HIMEM.EXE' }
} finally { Pop-Location }

[IO.File]::WriteAllText((Join-Path $work 'PACK.BAT'), "@echo off`r`nreloc himem.exe himem.bin 0`r`nstripdd himem.bin himem.sys`r`n", [Text.ASCIIEncoding]::new())
& $runner -RepositoryRoot $repository -WorkingDirectory $work -BatchFile 'PACK.BAT' -CompletionFiles @('HIMEM.SYS') -TimeoutSeconds 60
$output = Join-Path $work 'HIMEM.SYS'
if ((Get-Item -LiteralPath $output).Length -ne 4768 -or
    (Get-Sha256 $output) -ne '08aa2c47d835460ed3067fa7d6f8a3b37edeca524ad102b0588fdd1bf389ce08') {
    throw "HIMEM.SYS source-build identity mismatch: $output"
}

$manifest = [ordered]@{
    schema = 'runner.t235.s5.himem-fallback.v1'
    origin = 'source-built'
    source = 'src/opennt/base/mvdm/dos/v86/dev/himem'
    tools = @('tools/historical/opennt-4.5/masm.exe','tools/historical/opennt-4.5/link16.exe',
        'tools/historical/opennt-4.5/reloc.exe','tools/historical/opennt-4.5/stripdd.exe')
    output = [ordered]@{ file = 'HIMEM.SYS'; bytes = 4768; sha256 = Get-Sha256 $output }
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $build 'himem-fallback-manifest.json') -Encoding utf8
Write-Host "OpenNT source-built fallback completed: $output"
