[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BaseRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BaseRoot)) { $BaseRoot = Join-Path $repository 'artifacts\build\current\t189-boot-file-predicate-derivative-r2' }
$base = [IO.Path]::GetFullPath($BaseRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
function Hash([string]$Path) { if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing input: $Path" }; (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash }
function Copy-Verified([string]$Source, [string]$Destination) { $hash = Hash $Source; Copy-Item -LiteralPath $Source -Destination $Destination -Force; if ((Hash $Destination) -ne $hash) { throw "Copy hash mismatch: $Source" }; $hash }

$baseEngine = Join-Path $base 'ntdos64-t188-copied-state.exe'
if ((Hash $baseEngine) -ne 'A008832FBE0E6094A1F91D196E14ED9527BDAAE99FDE1042EC507BFDBE0C8608') { throw 'Base root is not the accepted T189 S5 engine.' }
& robocopy $base $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "Base copy failed: $LASTEXITCODE" }

$exceptionHash = Copy-Verified (Join-Path $repository 'src\bochs\cpu\exception.cc') (Join-Path $build 'cpu\exception.cc')
$runtimeHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_adapter_runtime.c') (Join-Path $build 'adapter\bx_ntvdm_adapter_runtime.c')
$runtimeHeaderHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_adapter_runtime.h') (Join-Path $build 'adapter\bx_ntvdm_adapter_runtime.h')
$comspecHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_cmd_comspec_bootstrap_service.c') (Join-Path $build 'adapter\bx_ntvdm_cmd_comspec_bootstrap_service.c')
$comspecHeaderHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_cmd_comspec_bootstrap_service.h') (Join-Path $build 'adapter\bx_ntvdm_cmd_comspec_bootstrap_service.h')
$baseMake = Join-Path $base 'ntdos64-t188-copied-state.mak'
$linkLine = Get-Content -LiteralPath $baseMake | Where-Object { $_ -like 'link /nologo *' }
if (@($linkLine).Count -ne 1) { throw 'Base derivative does not contain one link line.' }
$make = @(
    '# Generated T190 S3 derivative: exactly three object targets and final link.', '!INCLUDE Makefile', '',
    'cpu\exception.o: cpu\exception.cc',
    "`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1 /DBX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC=1 /DBX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1 /DBX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC=1 /DBX_NTVDM_ENABLE_STARTUP_TRANSACTION=0 /DBX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0 /DBX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1 /DBX_NTVDM_ENABLE_MACHINE_COMPOSITION=1 /Iadapter /Icli /Imachine /Tpcpu\exception.cc /Focpu\exception.o", '',
    'adapter\bx_ntvdm_adapter_runtime.obj: adapter\bx_ntvdm_adapter_runtime.c adapter\bx_ntvdm_adapter_runtime.h',
    "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Foadapter\bx_ntvdm_adapter_runtime.obj adapter\bx_ntvdm_adapter_runtime.c", '',
    'adapter\bx_ntvdm_cmd_comspec_bootstrap_service.obj: adapter\bx_ntvdm_cmd_comspec_bootstrap_service.c adapter\bx_ntvdm_cmd_comspec_bootstrap_service.h',
    "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Foadapter\bx_ntvdm_cmd_comspec_bootstrap_service.obj adapter\bx_ntvdm_cmd_comspec_bootstrap_service.c", '',
    'ntdos64-t190-command-bootstrap.exe: cpu\exception.o adapter\bx_ntvdm_adapter_runtime.obj adapter\bx_ntvdm_cmd_comspec_bootstrap_service.obj',
    "`t$linkLine"
)
$makePath = Join-Path $build 'ntdos64-t190-command-bootstrap.mak'
[IO.File]::WriteAllText($makePath, ($make -join "`r`n"), [Text.UTF8Encoding]::new($false))
$text = Get-Content -LiteralPath $makePath -Raw
if (($text | Select-String -AllMatches -Pattern '(?m)^.*\.(?:o|obj):').Matches.Count -ne 3) { throw 'Unauthorized derivative target.' }
$record = [ordered]@{
    schema='ntdos64.t190.s3.command-bootstrap-derivative.v1'
    baseRoot=$base; baseEngineSha256=Hash $baseEngine
    objectTargets=@('cpu\exception.o','adapter\bx_ntvdm_adapter_runtime.obj','adapter\bx_ntvdm_cmd_comspec_bootstrap_service.obj')
    retainedBootFileObjectSha256=Hash (Join-Path $build 'adapter\bx_ntvdm_cmd_boot_file_service.obj')
    retainedProviderObjectSha256=Hash (Join-Path $build 'adapter\bx_ntvdm_boot_namespace_provider_v1.obj')
    exceptionSourceSha256=$exceptionHash; runtimeSourceSha256=$runtimeHash; runtimeHeaderSha256=$runtimeHeaderHash
    comspecSourceSha256=$comspecHash; comspecHeaderSha256=$comspecHeaderHash; makefileSha256=Hash $makePath
    permittedBuild='nmake /f ntdos64-t190-command-bootstrap.mak ntdos64-t190-command-bootstrap.exe'
}
[IO.File]::WriteAllText((Join-Path $build 't190-s3-command-bootstrap-derivative.json'), ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared T190 S3 three-object derivative: $build"
