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
$exe = Join-Path $build 't217-s2-command-initial-environment.exe'
$sources = @(
    'src\bx-vdm\bop-v1\bx_ntvdm_command_host_context_v1.c',
    'src\bx-vdm\bop-v1\bx_ntvdm_command_initial_environment_v1.c',
    'tests\bx-vdm\bx_ntvdm_command_initial_environment_v1_test.c'
) | ForEach-Object { Join-Path $root $_ }
$compile = 'call "' + $vs + '" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TC /std:c11 /W4 /WX /MT /I "' +
    (Join-Path $root 'src\bx-vdm') + '" /Fe:"' + $exe + '" ' + ($sources -join ' ') + ' /link user32.lib'
& cmd.exe /d /s /c $compile 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if ($LASTEXITCODE -ne 0) { throw "T217 S2 initial-environment compilation failed: $LASTEXITCODE" }
& $exe 2>&1 | Tee-Object -FilePath (Join-Path $build 'run.log')
$exit = $LASTEXITCODE
[ordered]@{ schema='ntdos64.t217.s2.command-initial-environment.v1'; architecture='x64'; runtimeLibrary='/MT'; hostIo=$true; guestExecution=$false; runExitCode=$exit; passed=($exit -eq 0) } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 't217-s2-command-initial-environment.json') -Encoding utf8
if ($exit -ne 0) { throw "T217 S2 initial-environment fixture failed: $exit" }
Write-Host "T217 S2 COMMAND initial-environment probe passed: $build"
