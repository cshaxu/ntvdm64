param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
New-Item -ItemType Directory -Path $build | Out-Null
$exe = Join-Path $build 't210-s3-host-context-family.exe'
$sources = @('bx_ntvdm_mutation_profile_v1.c','bx_ntvdm_mutation_overlay_v1.c',
 'bx_ntvdm_dem_profile_consumer_v1.c','bx_ntvdm_command_host_context_v1.c',
 'bx_ntvdm_host_drive_policy.c','bx_ntvdm_host_namespace.c',
 'bx_ntvdm_session_host_context_v1.c','bx_ntvdm_dem_cwd_context_v1.c',
 'bx_ntvdm_cpu_delta_abi.c','bx_ntvdm_cpu_result_v2.c','bx_ntvdm_cpu_state_abi.c',
 'bx_ntvdm_exception_abi.c','bx_ntvdm_instruction_window_abi.c','bx_ntvdm_guest_range.c',
 'bx_ntvdm_guest_gather_read_action_v1.c','bx_ntvdm_multi_write_abi.c',
 'bx_ntvdm_multi_write_transaction.c','bx_ntvdm_guest_write_abi.c',
 'bx_ntvdm_dem_cwd_service_v2.c','bx_ntvdm_cmd_current_dir_service.c',
 'bx_ntvdm_cmd_comspec_bootstrap_service.c') | ForEach-Object { Join-Path $root ('src\bx-vdm\' + $_) }
$sources += Join-Path $root 'tests\bx-vdm\bx_ntvdm_host_context_family_v1_test.c'
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\bx-vdm') + '" /Fe:"' + $exe + '" ' + ($sources -join ' ') + ' /link ntdll.lib'
& cmd.exe /d /s /c $compile 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T210 S3 host-context family compilation failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -FilePath (Join-Path $build 'run.log')
$runExit = $LASTEXITCODE
[ordered]@{ schema='ntdos64.t210.s3.host-context-family.v1'; architecture='x64'; runtimeLibrary='/MT'; hostIo=$true; guestExecution=$false; runExitCode=$runExit; passed=($runExit -eq 0) } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't210-s3-host-context-family.json') -Encoding utf8
if ($runExit -ne 0) { throw "T210 S3 host-context family fixture failed: $runExit" }
Write-Host "T210 S3 host-context family probe passed: $build"
