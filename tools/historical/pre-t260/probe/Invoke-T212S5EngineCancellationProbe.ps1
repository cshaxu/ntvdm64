param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
New-Item -ItemType Directory -Path $build | Out-Null
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$sources = @('src\bx-mantle\bx_ntvdm_cancellation_controller_v1.cc', 'src\bx-vdm\bx_ntvdm_engine_contract_v1.c', 'src\bx-vdm\bx_ntvdm_engine_run_v1.c', 'tests\bx-mantle\t212_s5_engine_cancellation_test.c') | ForEach-Object { Join-Path $root $_ }
foreach($source in $sources + $vs){if(-not(Test-Path -LiteralPath $source)){throw "Missing T212 S5 engine input: $source"}}
$exe=Join-Path $build 't212-s5-engine-cancellation.exe'
$command='call "'+$vs+'" -arch=x64 -host_arch=x64 >nul && cl.exe /nologo /TP /std:c++14 /W4 /WX /MT /I "'+(Join-Path $root 'src')+'" /I "'+(Join-Path $root 'src\bx-mantle')+'" /I "'+(Join-Path $root 'src\bx-vdm')+'" /Fe:"'+$exe+'" "'+($sources -join '" "')+'"'
& cmd.exe /d /s /c $command 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'compile.log')
if($LASTEXITCODE -ne 0){throw "T212 S5 engine compile failed: $LASTEXITCODE"}
& $exe 2>&1 | Tee-Object -LiteralPath (Join-Path $build 'run.log'); $run=$LASTEXITCODE
$record=[ordered]@{schema='ntdos64.t212.s5.engine-cancellation.v1';architecture='x64';runtimeLibrary='/MT';sourceClosure=@('src/bx-mantle/bx_ntvdm_cancellation_controller_v1.cc','src/bx-mantle/bx_ntvdm_engine_contract_v1.[hc]','src/bx-mantle/bx_ntvdm_engine_run_v1.c','tests/bx-mantle/t212_s5_engine_cancellation_test.c');guestExecution=$false;runExitCode=$run;expectedRunExitCode=0;passed=($run -eq 0)}
$record|ConvertTo-Json -Depth 3|Set-Content -LiteralPath (Join-Path $build 't212-s5-engine-cancellation.json') -Encoding utf8
if($run -ne 0){throw "T212 S5 engine fixture failed: $run"}
Write-Host "T212 S5 engine cancellation probe passed: $build"
