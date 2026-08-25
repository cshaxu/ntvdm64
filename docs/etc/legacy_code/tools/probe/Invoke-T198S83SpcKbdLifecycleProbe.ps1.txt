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
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
foreach ($path in @(
    $vsDevCmd,
    (Join-Path $baseline 'command-r1.exe'),
    (Join-Path $baseline 'link.rsp'),
    (Join-Path $baseline 'native-core\config.h'),
    (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_fixture.cc'),
    (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c')
)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Required S83 input missing: $path" }
}

New-Item -ItemType Directory -Path $build | Out-Null
$config = Join-Path $baseline 'native-core\config.h'
$includes = @('src', 'refs\bochs', 'refs\bochs\instrument\stubs', 'src\bx-core',
    'src\bx-core\cpu', 'src\bx-mantle', 'src\bx-vdm', 'src\cli') |
    ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
function Invoke-Compile([string]$language, [string]$source, [string]$object, [string]$defines) {
    $common = 'call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
        ' -host_arch=x64 >nul && cl.exe /nologo /c /MT /DWIN32 ' +
        ($includes -join ' ') + ' /FI "' + $config + '" ' + $defines +
        ' /Fo"' + $object + '" "' + $source + '"'
    $command = if ($language -eq 'C') { $common.Replace('/c /MT', '/TC /c /std:c11 /W4 /WX /MT') } else { $common.Replace('/c /MT', '/c /std:c++14 /EHsc /MT /Gy') }
    & cmd.exe /d /s /c $command 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log') -Append
    if ($LASTEXITCODE -ne 0) { throw "S83 $language compile failed: $source" }
}

Invoke-Compile 'C++' (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_fixture.cc') (Join-Path $build 'fixture.obj') '/DBX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1'
Invoke-Compile 'C' (Join-Path $repository 'tests\bx-vdm\t198_s23_native_ntio_boundary_bridge.c') (Join-Path $build 'bridge.obj') ''

$response = Join-Path $build 'link.rsp'
$fixture = Join-Path $build 'fixture.obj'
$bridge = Join-Path $build 'bridge.obj'
$exe = Join-Path $build 'command-r1.exe'
Get-Content -LiteralPath (Join-Path $baseline 'link.rsp') | ForEach-Object {
    if ($_ -match '\\fixture\.obj"$') { '"' + $fixture + '"' }
    elseif ($_ -match '\\bridge\.obj"$') { '"' + $bridge + '"' }
    elseif ($_ -match '^/OUT:') { '/OUT:"' + $exe + '"' }
    else { $_ }
} | Set-Content -LiteralPath $response -Encoding ascii

& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
    ' -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'link.log')
if ($LASTEXITCODE -ne 0) { throw "S83 link failed: $LASTEXITCODE" }

$stdout = Join-Path $build 'run.stdout.log'
$stderr = Join-Path $build 'run.stderr.log'
& cmd.exe /d /s /c ('"' + $exe + '" 1>"' + $stdout + '" 2>"' + $stderr + '"')
$runExit = $LASTEXITCODE
Get-Content -LiteralPath $stdout, $stderr | Tee-Object -FilePath (Join-Path $build 'run.log')

[ordered]@{
    schema = 'ntdos64.t198.s83.spckbd-lifecycle-probe.v1'
    architecture = $HostArchitecture
    runtimeLibrary = '/MT'
    cpuProfile = 'CPU5/Pentium-MMX, non-x86-64'
    inheritedClosure = 'T198 S74 verified source-built CPU5/MSVC x64 closure'
    rebuiltObjects = @('fixture.obj', 'bridge.obj')
    runExitCode = $runExit
    logs = @{ compile = 'compile.log'; link = 'link.log'; run = 'run.log' }
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't198-s83-spckbd-lifecycle-probe.json') -Encoding utf8
if ($runExit -ne 2) { throw "S83 fixture returned unexpected status: $runExit" }
exit 0
