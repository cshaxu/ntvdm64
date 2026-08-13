param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [ValidateSet('x64')]
    [string]$HostArchitecture = 'x64'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\t198-s16-bx-vdm-x64-r1'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}

$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vsDevCmd -PathType Leaf)) {
    throw "Required MSVC environment entry point missing: $vsDevCmd"
}

# This is the exact source closure retained by the former i686 composition
# witness.  It deliberately contains only the boot-namespace composition,
# its generic ingress/session bridge, and direct adapter dependencies.
$sourceRelatives = @(
    'src\bx-vdm\bx_ntvdm_boot_namespace_composition_v1.c',
    'src\bx-vdm\bx_ntvdm_boot_namespace_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_boot_namespace_provider_v1.c',
    'src\bx-vdm\bx_ntvdm_bop_ingress_v1.c',
    'src\bx-vdm\bx_ntvdm_bop_provider_registry_v1.c',
    'src\bx-vdm\bx_ntvdm_bulk_result_transaction.c',
    'src\bx-vdm\bx_ntvdm_cmd_boot_file_service.c',
    'src\bx-vdm\bx_ntvdm_command_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_cpu_delta_abi.c',
    'src\bx-vdm\bx_ntvdm_cpu_result_v2.c',
    'src\bx-vdm\bx_ntvdm_cpu_state_abi.c',
    'src\bx-vdm\bx_ntvdm_dem_path_search_service_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_plane_v1.c',
    'src\bx-vdm\bx_ntvdm_dem_readonly_file_service.c',
    'src\bx-vdm\bx_ntvdm_exception_abi.c',
    'src\bx-vdm\bx_ntvdm_guest_gather_read_action_v1.c',
    'src\bx-vdm\bx_ntvdm_guest_range.c',
    'src\bx-vdm\bx_ntvdm_guest_read_action_v1.c',
    'src\bx-vdm\bx_ntvdm_guest_write_abi.c',
    'src\bx-vdm\bx_ntvdm_instruction_window_abi.c',
    'src\bx-vdm\bx_ntvdm_multi_write_abi.c',
    'src\bx-vdm\bx_ntvdm_multi_write_transaction.c',
    'src\bx-vdm\bx_ntvdm_profile_search_snapshot_v1.c',
    'src\bx-vdm\bx_ntvdm_readonly_namespace.c',
    'src\bx-vdm\bx_ntvdm_search_plan_v1.c',
    'src\bx-vdm\bx_ntvdm_search_request_v1.c',
    'src\bx-vdm\bx_ntvdm_search_result_v1.c',
    'src\bx-vdm\bx_ntvdm_search_session.c',
    'src\bx-vdm\bx_ntvdm_search_transaction_v1.c',
    'src\bx-vdm\bx_ntvdm_vdm_generic_ud_bridge_v1.c',
    'src\cli\byob_identity.c',
    'src\cli\byob_image.c',
    'src\cli\byob_profile.c',
    'tests\bx-vdm\bx_ntvdm_boot_namespace_composition_v1_test.c'
)
$sources = @($sourceRelatives | ForEach-Object { Join-Path $repository $_ })
foreach ($source in $sources) {
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required source missing: $source"
    }
}

$forbidden = @('src\bx-core', 'src\bx-mantle', 'src\bochs', 'CLI engine', 'MinGW',
    'OpenNT runtime', 'main.cc', 'bochs.exe', 'device archive')
New-Item -ItemType Directory -Path $build | Out-Null
$objectsDirectory = Join-Path $build 'obj'
New-Item -ItemType Directory -Path $objectsDirectory | Out-Null
$compileLog = Join-Path $build 'compile.log'
$includeRoots = @('src', 'src\cli', 'src\bx-vdm', 'src\bx-mantle') |
    ForEach-Object { '/I "' + (Join-Path $repository $_) + '"' }
$includeArguments = $includeRoots -join ' '
$objects = @()
$compileCommands = @('@echo off', ('call "' + $vsDevCmd + '" -arch=' +
    $HostArchitecture + ' -host_arch=x64 >nul'), 'if errorlevel 1 exit /b %errorlevel%')

foreach ($source in $sources) {
    $object = Join-Path $objectsDirectory (([IO.Path]::GetFileNameWithoutExtension($source)) + '.obj')
    $command = 'cl.exe /nologo /TC /c /std:c11 /W4 /WX /MT /DWIN32 ' +
        $includeArguments + ' /Fo"' + $object + '" "' + $source + '"'
    $compileCommands += $command
    $compileCommands += 'if errorlevel 1 exit /b %errorlevel%'
    $objects += $object
}
$compileBatch = Join-Path $build 'compile.cmd'
$compileCommands | Set-Content -LiteralPath $compileBatch -Encoding ascii
$compileOutput = & cmd.exe /d /s /c $compileBatch 2>&1
$compileExit = $LASTEXITCODE
$compileOutput | Out-File -LiteralPath $compileLog
if ($compileExit -ne 0) { throw "MSVC C source closure compilation failed: $compileExit" }

$exe = Join-Path $build 't198-s16-bx-vdm-boot-namespace.exe'
$map = Join-Path $build 'link.map'
$response = Join-Path $build 'link.rsp'
$linkLog = Join-Path $build 'link.log'
@('/nologo', ('/OUT:"' + $exe + '"'), ('/MAP:"' + $map + '"'), '/OPT:REF') +
    @($objects | ForEach-Object { '"' + $_ + '"' }) + @('bcrypt.lib') |
    Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
    ' -host_arch=x64 >nul && link.exe @"' + $response + '"') 2>&1 |
    Tee-Object -LiteralPath $linkLog
$linkExit = $LASTEXITCODE
if ($linkExit -ne 0) { throw "MSVC x64 boot-namespace link failed: $linkExit" }

$headers = Join-Path $build 'headers.txt'
& cmd.exe /d /s /c ('call "' + $vsDevCmd + '" -arch=' + $HostArchitecture +
    ' -host_arch=x64 >nul && dumpbin.exe /headers "' + $exe + '"') 2>&1 |
    Tee-Object -LiteralPath $headers
$runLog = Join-Path $build 'run.log'
& cmd.exe /d /s /c ('"' + $exe + '" > "' + $runLog + '" 2>&1')
$runExit = $LASTEXITCODE
$record = [ordered]@{
    schema = 'ntdos64.t198.s16.boot-namespace-vdm-x64-probe.v1'
    architecture = $HostArchitecture
    compiler = 'MSVC cl.exe/link.exe via VsDevCmd'
    runtime = '/MT'
    platformLibraries = @('bcrypt.lib')
    sourceClosure = $sourceRelatives
    fixture = 'tests/bx-vdm/bx_ntvdm_boot_namespace_composition_v1_test.c'
    forbiddenInputs = $forbidden
    linkExitCode = $linkExit
    runExitCode = $runExit
    expectedRunExitCode = 0
    passed = ($linkExit -eq 0 -and $runExit -eq 0)
    compileLog = 'compile.log'
    linkLog = 'link.log'
    linkMap = 'link.map'
    headers = 'headers.txt'
    runLog = 'run.log'
}
$record | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 't198-s16-bx-vdm-boot-namespace.json') -Encoding utf8
if ($runExit -ne 0) { throw "MSVC x64 boot-namespace fixture failed: $runExit" }
Write-Host "Built and ran MSVC x64 boot-namespace composition fixture: $exe"
