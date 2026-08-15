[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [ValidateSet('x64')][string]$HostArchitecture = 'x64'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
$baseline = Join-Path $repository 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$manifestPath = Join-Path $repository 'tools\build\t198-s50-bx-vdm-composition-manifest.json'
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
foreach ($path in @($vsDevCmd, $manifestPath, (Join-Path $baseline 'link.rsp'),
    (Join-Path $baseline 'native-core\config.h'),
    (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_fixture.cc'),
    (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c'),
    (Join-Path $repository 'tests\bx-vdm\t198_s23_fastread_attempt_ledger.c'),
    (Join-Path $repository 'tests\bx-vdm\t198_s121_dem_lifecycle_ledger.c'))) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Required S89 input missing: $path" }
}

$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$compileOnly = @($manifest.compileOnlySources)
$sourceRelatives = @($manifest.compileSources | Where-Object { $_ -notin $compileOnly })
if ($sourceRelatives.Count -eq 0) { throw 'Current composition manifest has no compile sources.' }
foreach ($relative in $sourceRelatives) {
    if (-not (Test-Path -LiteralPath (Join-Path $repository $relative) -PathType Leaf)) {
        throw "Manifest source missing: $relative"
    }
}

New-Item -ItemType Directory -Path $build | Out-Null
$objects = Join-Path $build 'current-objects'
New-Item -ItemType Directory -Path $objects | Out-Null
$config = Join-Path $baseline 'native-core\config.h'
$includes = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs', 'src\bx-core',
    'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') |
    ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
function Invoke-Compile([string]$language, [string]$source, [string]$object,
    [string]$defines) {
    $common = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
        ' -host_arch=x64 >nul && cl.exe /nologo /c /MT /DWIN32 ' +
        ($includes -join ' ') + ' /FI "' + $config + '" ' + $defines +
        ' /Fo"' + $object + '" "' + $source + '"'
    $command = if ($language -eq 'C') {
        $common.Replace('/c /MT', '/TC /c /std:c11 /W4 /WX /D_CRT_SECURE_NO_WARNINGS /MT')
    } else {
        $common.Replace('/c /MT', '/c /std:c++14 /EHsc /MT /Gy')
    }
    & cmd.exe /d /s /c $command 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log') -Append
    if ($LASTEXITCODE -ne 0) { throw "S89 $language compile failed: $source" }
}

$current = @{}
foreach ($relative in $sourceRelatives) {
    $base = [IO.Path]::GetFileNameWithoutExtension($relative)
    if ($current.ContainsKey($base)) { throw "Duplicate current object base name: $base" }
    $object = Join-Path $objects ($base + '.obj')
    $language = if ([IO.Path]::GetExtension($relative).ToLowerInvariant() -eq '.cc') { 'C++' } else { 'C' }
    Invoke-Compile $language (Join-Path $repository $relative) $object ''
    $current[$base] = $object
}
$fixture = Join-Path $build 'fixture.obj'
$bridge = Join-Path $build 'bridge.obj'
$fastReadLedger = Join-Path $build 'fastread-ledger.obj'
$demLifecycleLedger = Join-Path $build 'dem-lifecycle-ledger.obj'
Invoke-Compile 'C++' (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_fixture.cc') $fixture '/DBX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1'
Invoke-Compile 'C' (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c') $bridge ''
Invoke-Compile 'C' (Join-Path $repository 'tests\bx-vdm\t198_s23_fastread_attempt_ledger.c') $fastReadLedger ''
Invoke-Compile 'C' (Join-Path $repository 'tests\bx-vdm\t198_s121_dem_lifecycle_ledger.c') $demLifecycleLedger ''

$response = Join-Path $build 'link.rsp'
$exe = Join-Path $build 'command-r1.exe'
$emitted = @{}
$responseLines = foreach ($line in Get-Content -LiteralPath (Join-Path $baseline 'link.rsp')) {
    if ($line -match '\\fixture\.obj"$') { '"' + $fixture + '"'; continue }
    if ($line -match '\\bridge\.obj"$') { '"' + $bridge + '"'; continue }
    if ($line -match '^/OUT:') { '/OUT:"' + $exe + '"'; continue }
    if ($line -match '\\([^\\"]+)\.obj"$') {
        $base = [IO.Path]::GetFileNameWithoutExtension($matches[1])
        if ($current.ContainsKey($base)) { $emitted[$base] = $true; '"' + $current[$base] + '"'; continue }
    }
    $line
}
foreach ($base in ($current.Keys | Sort-Object)) {
    if (-not $emitted.ContainsKey($base)) { $responseLines += '"' + $current[$base] + '"' }
}
$responseLines += '"' + $fastReadLedger + '"'
$responseLines += '"' + $demLifecycleLedger + '"'
# bx_ntvdm_host_namespace uses the retained, adapter-private NT directory
# query ABI. Keep its import library explicit in the source-built closure.
$responseLines += 'ntdll.lib'
$responseLines | Set-Content -LiteralPath $response -Encoding ascii

& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
    ' -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S89 link failed: $LASTEXITCODE" }

$stdout = Join-Path $build 'run.stdout.log'
$stderr = Join-Path $build 'run.stderr.log'
& cmd.exe /d /s /c ('"' + $exe + '" 1>"' + $stdout + '" 2>"' + $stderr + '"')
$runExit = $LASTEXITCODE
Get-Content -LiteralPath $stdout, $stderr | Tee-Object -FilePath (Join-Path $build 'run.log')
[ordered]@{
    schema = 'ntdos64.t198.s89.current-composition-native-probe.v1'
    architecture = $HostArchitecture
    runtimeLibrary = '/MT'
    cpuProfile = 'CPU5/Pentium-MMX, non-x86-64'
    inheritedInputs = 'T198 S74 CPU5/mantle/guest-image link response only'
    currentSourceManifest = 'tools/build/t198-s50-bx-vdm-composition-manifest.json'
    currentObjectCount = $current.Count
    rebuiltObjects = @('current adapter/CLI objects', 'fixture.obj', 'bridge.obj')
    runExitCode = $runExit
    logs = @{ compile = 'compile.log'; link = 'link.log'; run = 'run.log' }
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't198-s89-current-composition-native-probe.json') -Encoding utf8
Write-Host "S89 native fixture finished with exit code ${runExit}: $exe"
exit 0
