[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) {
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
$build = [IO.Path]::GetFullPath($BuildRoot)
if (-not $build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }

$sourceMvdm = Join-Path $repository 'src\opennt-guest\base\mvdm'
$toolsRoot = Join-Path $repository 'tools\historical\opennt-4.5'
$runner = Join-Path $repository 'tools\build\Invoke-DosBoxBatchWithCompletion.ps1'
foreach ($path in @($sourceMvdm, (Join-Path $toolsRoot 'masm.exe'), (Join-Path $toolsRoot 'link16.exe'), (Join-Path $toolsRoot 'buildidx.exe'), (Join-Path $toolsRoot 'nosrvbld.exe'), (Join-Path $toolsRoot 'reloc.exe'), $runner)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Required primary NTIO input missing: $path" }
}

$stageMvdm = Join-Path $build 'base\mvdm'
New-Item -ItemType Directory -Path $stageMvdm | Out-Null
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'dos\v86\doskrnl\bios') -Destination (Join-Path $stageMvdm 'dos\v86\doskrnl\bios') -Recurse
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'dos\v86\inc') -Destination (Join-Path $stageMvdm 'dos\v86\inc') -Recurse
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'dos\v86\messages') -Destination (Join-Path $stageMvdm 'dos\v86\messages') -Recurse
Copy-Item -LiteralPath (Join-Path $sourceMvdm 'inc') -Destination (Join-Path $stageMvdm 'inc') -Recurse

$bios = Join-Path $stageMvdm 'dos\v86\doskrnl\bios'
Remove-Item -LiteralPath @("$bios\MSBIO.CL3", "$bios\MSBIO.CL4", "$bios\MSBIO.CL5", "$bios\MSBIO.CL6") -Force
Copy-Item -LiteralPath (Join-Path $stageMvdm 'dos\v86\messages\usa-ms.msg'), (Join-Path $stageMvdm 'dos\v86\messages\usa-ms.idx') -Destination $bios
Copy-Item -LiteralPath (Join-Path $toolsRoot 'nosrvbld.exe'), (Join-Path $toolsRoot 'buildidx.exe') -Destination $bios
[IO.File]::WriteAllText((Join-Path $bios 'RUN.BAT'), "@echo off`r`nbuildidx usa-ms.msg > B.LOG`r`nnosrvbld msbio.skl usa-ms.msg > N.LOG`r`necho done > DONE.OK`r`n", [Text.ASCIIEncoding]::new())
& $runner -RepositoryRoot $repository -WorkingDirectory $bios -BatchFile 'RUN.BAT' -CompletionFiles @('MSBIO.CL3','MSBIO.CL4','MSBIO.CL5','MSBIO.CL6') -TimeoutSeconds 60

$classHashes = [ordered]@{
    'MSBIO.CL3' = '279d5cf516a39248f910a17e02647f37737b2c4607a6cd119dc7236affed78e2'
    'MSBIO.CL4' = '3c753ff790aad0f86a84130c8a40a23a420e8ebe842cb7dde442c55e9a525873'
    'MSBIO.CL5' = 'a929484571f630176541f39d8c411d6e924d8cd6358e6bc395f94b23295015d2'
    'MSBIO.CL6' = '4a23d5d9e82762b4e0fde991c2c0347c1fe33e6097d2f1ed54da9dd6cfbd0440'
}
foreach ($name in $classHashes.Keys) {
    if ((Get-Sha256 (Join-Path $bios $name)) -ne $classHashes[$name]) { throw "OpenNT NOSRVBLD identity mismatch: $name" }
}

$modules = @('msbio1','mschar','msint13','spckbd','spcmse_4','spcemm','msinit','sysinit1','sysconf','sysinit2','sysimes','msend')
Push-Location $bios
try {
    foreach ($module in $modules) {
        & (Join-Path $toolsRoot 'masm.exe') '/I.' '/I..\..\inc' '/I..\..\..\..\inc' '/I..\..\messages' '/Mx' '/t' '/W1' ("$module.asm,$module.obj;") 2>&1 | Tee-Object -FilePath (Join-Path $bios "$module-masm.log")
        if ($LASTEXITCODE -ne 0) { throw "MASM failed for ${module}: $LASTEXITCODE" }
    }
    & (Join-Path $toolsRoot 'link16.exe') '/MAP' '@msbio_4.lnk' 2>&1 | Tee-Object -FilePath (Join-Path $bios 'link16.log')
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath (Join-Path $bios 'ntio.exe'))) { throw 'LINK16 did not produce NTIO.EXE' }
} finally { Pop-Location }

Copy-Item -LiteralPath (Join-Path $toolsRoot 'reloc.exe') -Destination $bios
[IO.File]::WriteAllText((Join-Path $bios 'PACK.BAT'), "@echo off`r`nreloc ntio.exe ntio.sys 70 > R.LOG`r`necho done > PACK.OK`r`n", [Text.ASCIIEncoding]::new())
& $runner -RepositoryRoot $repository -WorkingDirectory $bios -BatchFile 'PACK.BAT' -CompletionFiles @('ntio.sys') -TimeoutSeconds 60

$output = Join-Path $bios 'ntio.sys'
if (-not (Test-Path -LiteralPath $output)) { throw 'RELOC did not produce NTIO.SYS' }
$expectedHash = 'cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937'
if ((Get-Item -LiteralPath $output).Length -ne 33792 -or (Get-Sha256 $output) -ne $expectedHash) { throw "Primary NTIO identity mismatch: $output" }

$manifest = [ordered]@{
    schema = 'ntvdm64.t225.s8.ntio-primary-source-build.v1'
    classification = 'primary-original-toolchain-source-built'
    source = [ordered]@{ managedRoot = 'src/opennt-guest'; upstreamRevision = '5c5b979ec08c17d3ca2eb70e8aad62d26515d01c'; guestSourceTransforms = @() }
    tools = [ordered]@{ masm = Get-Sha256 (Join-Path $toolsRoot 'masm.exe'); link16 = Get-Sha256 (Join-Path $toolsRoot 'link16.exe'); buildidx = Get-Sha256 (Join-Path $toolsRoot 'buildidx.exe'); nosrvbld = Get-Sha256 (Join-Path $toolsRoot 'nosrvbld.exe'); reloc = Get-Sha256 (Join-Path $toolsRoot 'reloc.exe') }
    modules = $modules
    output = [ordered]@{ relativePath = 'base/mvdm/dos/v86/doskrnl/bios/NTIO.SYS'; bytes = 33792; sha256 = $expectedHash }
    mapsym = 'not required for runtime artifact identity; no MAPSYM tool is consumed'
}
[IO.File]::WriteAllText((Join-Path $build 'ntio-build-manifest.json'), (($manifest | ConvertTo-Json -Depth 6) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Primary NTIO source build completed: $output"
