[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$ProfilePath = '',
    [string]$InputRoot = '',
    [switch]$CompileOnly,
    [switch]$Resume
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if ((Test-Path -LiteralPath $build) -and !$Resume) {
    throw "Refusing to overwrite existing build root: $build"
}
if ($Resume -and !(Test-Path -LiteralPath $build -PathType Container)) {
    throw "Cannot resume a missing build root: $build"
}
if (!$CompileOnly -and ([string]::IsNullOrWhiteSpace($ProfilePath) -or
        [string]::IsNullOrWhiteSpace($InputRoot))) {
    throw 'ProfilePath and InputRoot are required unless -CompileOnly is used.'
}

$closure = Join-Path $root 'tools\probe\Invoke-T220S5SourceCompositionClosure.ps1'
$fixture = Join-Path $root 'tests\bx-mantle\t225_s6_four_image_profile_fixture.c'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach ($path in @($closure, $fixture, $vs)) {
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required T225 S6 source input missing: $path"
    }
}

$closureRecord = Join-Path $build 't220-s5-source-composition-closure.json'
if (!$Resume) {
    & powershell.exe -ExecutionPolicy Bypass -File $closure -RepositoryRoot $root -BuildRoot $build
    if ($LASTEXITCODE -ne 0) {
        throw "T225 S6 current source-composition closure failed: $LASTEXITCODE"
    }
} elseif (!(Test-Path -LiteralPath $closureRecord -PathType Leaf) -or
    -not ((Get-Content -LiteralPath $closureRecord -Raw | ConvertFrom-Json).linkSucceeded)) {
    throw "Cannot resume without a successful T220 source closure: $build"
}

$nativeCore = Join-Path $build 'native-core'
$config = Join-Path $nativeCore 'config.h'
$fixtureObject = Join-Path $build 't225-s6-four-image-profile-fixture.obj'
$includes = @('src', 'src\cli', 'src\bx-vdm', 'src\bx-mantle', 'refs\bochs',
    'refs\bochs\instrument\stubs', 'src\bx-core', 'src\bx-core\cpu',
    'refs\bochs\iodev') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe ' +
    '/nologo /c /TC /std:c11 /W4 /WX /MT /DWIN32 ' + ($includes -join ' ') +
    ' /FI "' + $config + '" /Fo"' + $fixtureObject + '" "' + $fixture + '"'
& cmd.exe /d /s /c $compile 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'fixture-compile.log')
if ($LASTEXITCODE -ne 0) { throw "T225 S6 fixture compilation failed: $LASTEXITCODE" }

$coreObjects = Get-ChildItem -LiteralPath $nativeCore -Filter '*.obj' -File |
    Where-Object { $_.Name -ne 'minimal_machine_link_probe.obj' } |
    ForEach-Object { $_.FullName }
$compositionObjects = Get-ChildItem -LiteralPath (Join-Path $build 'composition-objects') `
    -Filter '*.obj' -File | ForEach-Object { $_.FullName }
if ($coreObjects.Count -eq 0 -or $compositionObjects.Count -eq 0) {
    throw 'T225 S6 source closure emitted an incomplete object set.'
}
$exe = Join-Path $build 't225-s6-four-image-profile-fixture.exe'
$response = Join-Path $build 'fixture-link.rsp'
@('/nologo', ('/OUT:"' + $exe + '"'), '/OPT:REF') +
    @($coreObjects) + @($compositionObjects) + @(
        (Join-Path $build 'mechanical-action.obj'),
        (Join-Path $build 'generic-ud-bridge.obj'),
        $fixtureObject,
        'libcmt.lib', 'libvcruntime.lib', 'libucrt.lib',
        'legacy_stdio_definitions.lib', 'kernel32.lib', 'bcrypt.lib', 'ntdll.lib',
        'user32.lib') | ForEach-Object {
            if ($_ -match '\.obj$') { '"' + $_ + '"' } else { $_ }
        } | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -FilePath (Join-Path $build 'fixture-link.log')
if ($LASTEXITCODE -ne 0) { throw "T225 S6 fixture link failed: $LASTEXITCODE" }

$runExit = $null
if (!$CompileOnly) {
    & $exe $ProfilePath $InputRoot 2>&1 |
        Tee-Object -FilePath (Join-Path $build 'fixture-run.log')
    $runExit = $LASTEXITCODE
    if ($runExit -ne 0) { throw "T225 S6 fixture execution failed: $runExit" }
}

[ordered]@{
    schema = 'ntdos64.t225.s6.four-image-profile-fixture.v1'
    architecture = 'x64'
    runtimeLibrary = '/MT'
    machineProfile = 'CPU5/Pentium-MMX, non-x86-64'
    sourceClosure = @(
        'tools/probe/Invoke-T220S5SourceCompositionClosure.ps1',
        'tools/build/t198-s50-bx-vdm-composition-manifest.json',
        'tests/bx-mantle/t225_s6_four_image_profile_fixture.c'
    )
    productProfiles = @('direct', 'readonly', 'overlay-deferred')
    virtual = 'retired-unselected'
    compileOnly = [bool]$CompileOnly
    runExitCode = $runExit
    nativeGuestExecution = $false
    forbiddenInputs = @('artifacts/build/t198-s74-dem-pdb-termination-r1',
        'bochs.exe', 'main.cc', 'GUI/plugin/device archives')
} | ConvertTo-Json -Depth 4 |
    Set-Content -LiteralPath (Join-Path $build 't225-s6-four-image-profile-fixture.json') -Encoding utf8
Write-Host "T225 S6 four-image profile fixture completed: $build"
