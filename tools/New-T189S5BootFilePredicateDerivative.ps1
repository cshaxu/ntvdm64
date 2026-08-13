[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BaseRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
function Hash([string]$Path) { if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing input: $Path" }; (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash }
function Copy-Verified([string]$Source, [string]$Destination) { $hash = Hash $Source; Copy-Item -LiteralPath $Source -Destination $Destination -Force; if ((Hash $Destination) -ne $hash) { throw "Copy hash mismatch: $Source" }; $hash }

$prepare = @{ RepositoryRoot = $repository; BuildRoot = $build }
if (-not [string]::IsNullOrWhiteSpace($BaseRoot)) { $prepare.BaseRoot = $BaseRoot }
& (Join-Path $PSScriptRoot 'New-T188S7CopiedStateTraceDerivative.ps1') @prepare

$cmdSourceHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_cmd_boot_file_service.c') (Join-Path $build 'adapter\bx_ntvdm_cmd_boot_file_service.c')
$cmdHeaderHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_cmd_boot_file_service.h') (Join-Path $build 'adapter\bx_ntvdm_cmd_boot_file_service.h')
$providerSourceHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_boot_namespace_provider_v1.c') (Join-Path $build 'adapter\bx_ntvdm_boot_namespace_provider_v1.c')
$providerHeaderHash = Copy-Verified (Join-Path $repository 'src\bx-ntvdm-adapter\bx_ntvdm_boot_namespace_provider_v1.h') (Join-Path $build 'adapter\bx_ntvdm_boot_namespace_provider_v1.h')
$makePath = Join-Path $build 'ntdos64-t188-copied-state.mak'
$extension = @(
    '',
    'adapter\bx_ntvdm_cmd_boot_file_service.obj: adapter\bx_ntvdm_cmd_boot_file_service.c adapter\bx_ntvdm_cmd_boot_file_service.h',
    "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Foadapter\bx_ntvdm_cmd_boot_file_service.obj adapter\bx_ntvdm_cmd_boot_file_service.c",
    '',
    'adapter\bx_ntvdm_boot_namespace_provider_v1.obj: adapter\bx_ntvdm_boot_namespace_provider_v1.c adapter\bx_ntvdm_boot_namespace_provider_v1.h',
    "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Foadapter\bx_ntvdm_boot_namespace_provider_v1.obj adapter\bx_ntvdm_boot_namespace_provider_v1.c",
    '',
    'ntdos64-t188-copied-state.exe: adapter\bx_ntvdm_cmd_boot_file_service.obj adapter\bx_ntvdm_boot_namespace_provider_v1.obj'
)
[IO.File]::AppendAllText($makePath, (($extension -join "`r`n") + "`r`n"), [Text.UTF8Encoding]::new($false))
$text = Get-Content -LiteralPath $makePath -Raw
if (($text | Select-String -AllMatches -Pattern '(?m)^.*\.(?:o|obj):').Matches.Count -ne 4) { throw 'Unauthorized derivative target.' }
$record = [ordered]@{
    schema='ntdos64.t189.s5.boot-file-predicate-derivative.v1'
    baseDerivative=$build
    objectTargets=@('cpu\exception.o','adapter\bx_ntvdm_adapter_runtime.obj','adapter\bx_ntvdm_cmd_boot_file_service.obj','adapter\bx_ntvdm_boot_namespace_provider_v1.obj')
    cmdBootFileSourceSha256=$cmdSourceHash
    cmdBootFileHeaderSha256=$cmdHeaderHash
    providerSourceSha256=$providerSourceHash
    providerHeaderSha256=$providerHeaderHash
    makefileSha256=Hash $makePath
    permittedBuild='nmake /f ntdos64-t188-copied-state.mak ntdos64-t188-copied-state.exe'
}
[IO.File]::WriteAllText((Join-Path $build 't189-s5-boot-file-predicate-derivative.json'), ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared T189 S5 four-object derivative: $build"
