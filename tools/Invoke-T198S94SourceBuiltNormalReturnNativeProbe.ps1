param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
$baseline = Join-Path $repository 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$manifestPath = Join-Path $repository 'tools\t198-s50-bx-vdm-composition-manifest.json'
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
foreach ($path in @($vsDevCmd, $manifestPath, (Join-Path $baseline 'link.rsp'),
    (Join-Path $baseline 'native-core\config.h'),
    (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c'))) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Required S94 input missing: $path" }
}

New-Item -ItemType Directory -Path $build | Out-Null
$prepared = Join-Path $build 'prepared'
& (Join-Path $repository 'tools\Invoke-T198S93SourceBuiltNormalReturnFixturePrepare.ps1') -RepositoryRoot $repository -BuildRoot $prepared
if ($LASTEXITCODE -ne 0) { throw 'S94 input preparation failed.' }

$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$compileOnly = @($manifest.compileOnlySources)
$sources = @($manifest.compileSources | Where-Object { $_ -notin $compileOnly })
$objects = Join-Path $build 'current-objects'
New-Item -ItemType Directory -Path $objects | Out-Null
$config = Join-Path $baseline 'native-core\config.h'
$includes = @('src', 'src\bochs', 'src\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') | ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
function Invoke-Compile([string]$Language, [string]$Source, [string]$Object, [string]$Defines) {
    $common = 'call "' + $vsDevCmd + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /c /MT /DWIN32 ' +
        ($includes -join ' ') + ' /FI "' + $config + '" ' + $Defines + ' /Fo"' + $Object + '" "' + $Source + '"'
    $command = if ($Language -eq 'C') { $common.Replace('/c /MT', '/TC /c /std:c11 /W4 /WX /D_CRT_SECURE_NO_WARNINGS /MT') } else { $common.Replace('/c /MT', '/c /std:c++14 /EHsc /MT /Gy') }
    & cmd.exe /d /s /c $command 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log') -Append
    if ($LASTEXITCODE -ne 0) { throw "S94 $Language compile failed: $Source" }
}

$current = @{}
foreach ($relative in $sources) {
    $base = [IO.Path]::GetFileNameWithoutExtension($relative)
    if ($current.ContainsKey($base)) { throw "Duplicate current object base name: $base" }
    $object = Join-Path $objects ($base + '.obj')
    Invoke-Compile 'C' (Join-Path $repository $relative) $object ''
    $current[$base] = $object
}
$bridge = Join-Path $build 'bridge.obj'
Invoke-Compile 'C' (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c') $bridge ''
$fixture = Join-Path $prepared 'source-built-normal-return-fixture.obj'
$commandBytes = Join-Path $prepared 'command_bytes.obj'
$shareBytes = Join-Path $prepared 'share_bytes.obj'
foreach ($path in @($fixture, $commandBytes, $shareBytes)) { if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Prepared S93 object missing: $path" } }

$response = Join-Path $build 'link.rsp'
$exe = Join-Path $build 'source-built-normal-return-r1.exe'
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
foreach ($base in ($current.Keys | Sort-Object)) { if (-not $emitted.ContainsKey($base)) { $responseLines += '"' + $current[$base] + '"' } }
$responseLines += '"' + $commandBytes + '"'
$responseLines += '"' + $shareBytes + '"'
$responseLines | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 | Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S94 link failed: $LASTEXITCODE" }

$stdout = Join-Path $build 'run.stdout.log'; $stderr = Join-Path $build 'run.stderr.log'
& cmd.exe /d /s /c ('"' + $exe + '" 1>"' + $stdout + '" 2>"' + $stderr + '"')
$runExit = $LASTEXITCODE
Get-Content -LiteralPath $stdout, $stderr | Tee-Object -FilePath (Join-Path $build 'run.log')
@{
    schema = 'ntdos64.t198.s94.source-built-normal-return-native-probe.v1'
    architecture = 'x64'; runtimeLibrary = '/MT'; cpuProfile = 'CPU5/Pentium-MMX, non-x86-64'
    preparedInputs = 'S93 exact source-built COMMAND.COM/SHARE.EXE and v7 profile-selection witness'
    inheritedInputs = 'T198 S74 CPU5/mantle/NTIO/NTDOS link response only'
    currentSourceManifest = 'tools/t198-s50-bx-vdm-composition-manifest.json'
    rebuiltObjects = @('S93 prepared fixture/input objects', 'current adapter/CLI objects', 'bridge.obj')
    runExitCode = $runExit; runs = 1
    logs = @{ preparation = 'prepared'; compile = 'compile.log'; link = 'link.log'; run = 'run.log' }
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't198-s94-source-built-normal-return-native-probe.json') -Encoding utf8
Write-Host "S94 one bounded native fixture finished with exit code ${runExit}: $exe"
exit 0
