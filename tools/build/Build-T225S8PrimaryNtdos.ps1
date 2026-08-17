[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
$build = [IO.Path]::GetFullPath($BuildRoot)
if (-not $build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw "BuildRoot must remain below ${buildBase}: $build" }
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }

$sourceMvdm = Join-Path $repository 'src\opennt\base\mvdm'
$tools = Join-Path $repository 'tools\historical\opennt-4.5'
$runner = Join-Path $repository 'tools\build\Invoke-DosBoxBatchWithCompletion.ps1'
foreach ($path in @($sourceMvdm, $runner)) { if (-not (Test-Path -LiteralPath $path)) { throw "Required primary NTDOS input missing: $path" } }
foreach ($tool in @('buildidx.exe','nosrvbld.exe','masm.exe','link16.exe','reloc.exe','stripz.exe')) {
    if (-not (Test-Path -LiteralPath (Join-Path $tools $tool))) { throw "Required canonical NTDOS tool missing: $tool" }
}

$stageMvdm = Join-Path $build 'base\mvdm'
New-Item -ItemType Directory -Path $stageMvdm | Out-Null
foreach ($relative in @('dos\v86\doskrnl\dos','dos\v86\inc','dos\v86\messages','inc')) {
    $source = Join-Path $sourceMvdm $relative
    $destination = Join-Path $stageMvdm $relative
    Copy-Item -LiteralPath $source -Destination $destination -Recurse
}

$dos = Join-Path $stageMvdm 'dos\v86\doskrnl\dos'
Remove-Item -LiteralPath @("$dos\MSDOS.CL1", "$dos\MSDOS.CL2") -Force -ErrorAction SilentlyContinue
Copy-Item -LiteralPath (Join-Path $stageMvdm 'dos\v86\messages\usa-ms.msg'), (Join-Path $stageMvdm 'dos\v86\messages\usa-ms.idx') -Destination $dos
Copy-Item -LiteralPath (Join-Path $tools 'buildidx.exe'), (Join-Path $tools 'nosrvbld.exe') -Destination $dos
[IO.File]::WriteAllText((Join-Path $dos 'RUN.BAT'), "@echo off`r`nbuildidx usa-ms.msg > B.LOG`r`nnosrvbld msdos.skl usa-ms.msg > N.LOG`r`n", [Text.ASCIIEncoding]::new())
& $runner -RepositoryRoot $repository -WorkingDirectory $dos -BatchFile 'RUN.BAT' -CompletionFiles @('MSDOS.CL1','MSDOS.CL2') -TimeoutSeconds 60
foreach ($name in @('MSDOS.CL1','MSDOS.CL2')) { if (-not (Test-Path -LiteralPath (Join-Path $dos $name))) { throw "NOSRVBLD output missing: $name" } }

$modules = @('origin','msdata','mstable','msdisp','mscode','msdosme','time','getset','parse','misc','misc2','cpmio','cpmio2','fcbio','fcbio2','search','path','ioctl','dup','open','abort','disk','dir2','dev','fcb','msctrlc','msproc','alloc','srvcall','segcheck','util','handle','macro','macro2','file','lock','exepatch','msinit')
Push-Location $dos
try {
    foreach ($module in $modules) {
        & (Join-Path $tools 'masm.exe') '/I.' '/I..\..\inc' '/I..\..\..\..\inc' '/I..\..\messages' '/Mx' '/t' '/W1' ("$module.asm,$module.obj;") 2>&1 | Tee-Object -FilePath (Join-Path $dos "$module-masm.log")
        if ($LASTEXITCODE -ne 0) { throw "MASM failed for ${module}: $LASTEXITCODE" }
    }
    & (Join-Path $tools 'link16.exe') '@msdos.lnk' 2>&1 | Tee-Object -FilePath (Join-Path $dos 'link16.log')
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath (Join-Path $dos 'ntdos.exe'))) { throw 'LINK16 did not produce NTDOS.EXE' }
} finally { Pop-Location }

Copy-Item -LiteralPath (Join-Path $tools 'reloc.exe'), (Join-Path $tools 'stripz.exe') -Destination $dos
[IO.File]::WriteAllText((Join-Path $dos 'PACK.BAT'), "@echo off`r`nreloc ntdos.exe ntdos.bin > R.LOG`r`nstripz ntdos.bin ntdos.sys > S.LOG`r`n", [Text.ASCIIEncoding]::new())
& $runner -RepositoryRoot $repository -WorkingDirectory $dos -BatchFile 'PACK.BAT' -CompletionFiles @('ntdos.bin','ntdos.sys') -TimeoutSeconds 60

$output = Join-Path $dos 'ntdos.sys'
$expectedHash = '957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84'
if (-not (Test-Path -LiteralPath $output) -or (Get-Item -LiteralPath $output).Length -ne 27858 -or (Get-Sha256 $output) -ne $expectedHash) { throw "Primary NTDOS identity mismatch: $output" }

$manifest = [ordered]@{
    schema = 'ntdos64.t225.s8.ntdos-primary-source-build.v1'
    classification = 'primary-original-toolchain-source-built'
    source = [ordered]@{ managedRoot = 'src/opennt'; upstreamRevision = '5c5b979ec08c17d3ca2eb70e8aad62d26515d01c'; guestSourceTransforms = @() }
    tools = [ordered]@{ buildidx = Get-Sha256 (Join-Path $tools 'buildidx.exe'); nosrvbld = Get-Sha256 (Join-Path $tools 'nosrvbld.exe'); masm = Get-Sha256 (Join-Path $tools 'masm.exe'); link16 = Get-Sha256 (Join-Path $tools 'link16.exe'); reloc = Get-Sha256 (Join-Path $tools 'reloc.exe'); stripz = Get-Sha256 (Join-Path $tools 'stripz.exe') }
    modules = $modules
    output = [ordered]@{ relativePath = 'base/mvdm/dos/v86/doskrnl/dos/NTDOS.SYS'; bytes = 27858; sha256 = $expectedHash }
}
[IO.File]::WriteAllText((Join-Path $build 'ntdos-build-manifest.json'), (($manifest | ConvertTo-Json -Depth 6) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Host "Primary NTDOS source build completed: $output"
