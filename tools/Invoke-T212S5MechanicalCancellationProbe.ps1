param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
New-Item -ItemType Directory -Path $build | Out-Null
$base = Join-Path $root 'artifacts\build\t198-s74-dem-pdb-termination-r1'
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$sources = @(
    @{ Object='cancellation.obj'; Source='src\bx-mantle\bx_ntvdm_cancellation_controller_v1.cc' },
    @{ Object='machine-stage.obj'; Source='src\bx-mantle\bx_ntvdm_machine_stage_v1.cc' },
    @{ Object='fixture.obj'; Source='tests\bx-mantle\t212_s5_machine_stage_cancellation_fixture.cc' }
)
foreach ($path in @($vs, (Join-Path $base 'link.rsp'), (Join-Path $base 'native-core\config.h')) +
    ($sources | ForEach-Object { Join-Path $root $_.Source })) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing T212 S5 input: $path" }
}
$config = Join-Path $base 'native-core\config.h'
$includes = @('src','src\bochs','src\bochs\instrument\stubs','src\bx-core','src\bx-core\cpu','src\bx-mantle','src\bx-vdm','src\bochs\iodev') | ForEach-Object { '/I "' + (Join-Path $root $_) + '"' }
$compile = 'cl.exe /nologo /c /std:c++14 /EHsc /MT /Gy /DWIN32 /DBX_NTVDM_CANCELLATION_TESTING=1 ' + ($includes -join ' ') + ' /FI "' + $config + '" '
$batch = Join-Path $build 'compile.cmd'; $lines=@('@echo off',('call "'+$vs+'" -arch=x64 -host_arch=x64 >nul'),'if errorlevel 1 exit /b %errorlevel%')
foreach($source in $sources){$lines += $compile + '/Fo"' + (Join-Path $build $source.Object) + '" "' + (Join-Path $root $source.Source) + '"'; $lines += 'if errorlevel 1 exit /b %errorlevel%'}
$lines += 'exit /b 0'; $lines | Set-Content -LiteralPath $batch -Encoding ascii
& cmd.exe /d /s /c ('call "'+$batch+'"') 2>&1 | Tee-Object -FilePath (Join-Path $build 'compile.log')
if($LASTEXITCODE -ne 0){throw "T212 S5 compile failed: $LASTEXITCODE"}
$exe=Join-Path $build 't212-s5-mechanical-cancellation.exe'; $response=Join-Path $build 'link.rsp'; $fixture=Join-Path $build 'fixture.obj'
$link=foreach($line in Get-Content (Join-Path $base 'link.rsp')){if($line -match '\\fixture\.obj"$'){ '"'+$fixture+'"'; continue }; if($line -match '^/OUT:'){ '/OUT:"'+$exe+'"'; continue }; $line}
$link += '"'+(Join-Path $build 'cancellation.obj')+'"'; $link += '"'+(Join-Path $build 'machine-stage.obj')+'"'; $link | Set-Content -LiteralPath $response -Encoding ascii
& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x64 -host_arch=x64 >nul && link.exe @"'+$response+'"') 2>&1 | Tee-Object -FilePath (Join-Path $build 'link.log')
if($LASTEXITCODE -ne 0){throw "T212 S5 link failed: $LASTEXITCODE"}
& cmd.exe /d /s /c ('"'+$exe+'" 2>&1') | Tee-Object -FilePath (Join-Path $build 'run.log'); $run=$LASTEXITCODE
$record=[ordered]@{schema='ntdos64.t212.s5.mechanical-cancellation.v1';architecture='x64';runtimeLibrary='/MT';sourceClosure=@('src/bx-mantle/bx_ntvdm_cancellation_controller_v1.cc','src/bx-mantle/bx_ntvdm_machine_stage_v1.cc','tests/bx-mantle/t212_s5_machine_stage_cancellation_fixture.cc');forbiddenDependencies=@('Windows HANDLE','callback','BOP','OpenNT','guest selector');runExitCode=$run;expectedRunExitCode=0;passed=($run -eq 0)}
$record|ConvertTo-Json -Depth 3|Set-Content -LiteralPath (Join-Path $build 't212-s5-mechanical-cancellation.json') -Encoding utf8
if($run -ne 0){throw "T212 S5 fixture failed: $run"}
Write-Host "T212 S5 mechanical cancellation probe passed: $build"
