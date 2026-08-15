[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$InputRoot = '',
    [string]$AnalysisRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($InputRoot)) {
    $InputRoot = Join-Path $repository 'artifacts\analysis\t95-s6-startup-session-msvc-objects-003-20260811-001'
}
if ([string]::IsNullOrWhiteSpace($AnalysisRoot)) {
    $AnalysisRoot = Join-Path $repository 'artifacts\analysis\t95-s6-startup-session-symbols-001-20260811-001'
}
$input = [IO.Path]::GetFullPath($InputRoot)
$analysis = [IO.Path]::GetFullPath($AnalysisRoot)
if (-not (Test-Path -LiteralPath $input -PathType Container)) { throw "Missing input root: $input" }
if (Test-Path -LiteralPath $analysis) { throw "Refusing to overwrite existing analysis directory: $analysis" }

$msvcRoot = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.43.34808\bin\Hostx86\x86'
$dumpbin = Join-Path $msvcRoot 'dumpbin.exe'
if (-not (Test-Path -LiteralPath $dumpbin -PathType Leaf)) { throw "Missing MSVC x86 dumpbin: $dumpbin" }

$relativeObjects = @(
    'adapter\bx_ntvdm_startup_session.obj',
    'adapter\bx_ntvdm_cpu_state_abi.obj',
    'adapter\bx_ntvdm_guest_range.obj',
    'adapter\bx_ntvdm_guest_write_abi.obj',
    'adapter\bx_ntvdm_startup_plan_abi.obj',
    'cli\byob_profile.obj',
    'cli\byob_identity.obj',
    'cli\byob_image.obj'
)

New-Item -ItemType Directory -Path $analysis -Force | Out-Null
$manifest = @()
foreach ($relativeObject in $relativeObjects) {
    $object = Join-Path $input $relativeObject
    if (-not (Test-Path -LiteralPath $object -PathType Leaf)) { throw "Missing declared object: $object" }
    $stem = $relativeObject -replace '[\\/]', '_' -replace '\.obj$', ''
    $symbolOutput = Join-Path $analysis "$stem.symbols.txt"
    $directiveOutput = Join-Path $analysis "$stem.directives.txt"
    & $dumpbin /symbols $object *> $symbolOutput
    if ($LASTEXITCODE -ne 0) { throw "dumpbin /symbols failed for $relativeObject" }
    & $dumpbin /directives $object *> $directiveOutput
    if ($LASTEXITCODE -ne 0) { throw "dumpbin /directives failed for $relativeObject" }
    $manifest += [ordered]@{
        relativePath = $relativeObject
        sha256 = (Get-FileHash -LiteralPath $object -Algorithm SHA256).Hash
        symbolsOutput = [IO.Path]::GetFileName($symbolOutput)
        directivesOutput = [IO.Path]::GetFileName($directiveOutput)
    }
}

$record = [ordered]@{
    schema = 'ntdos64.t95-s6.startup-session-static-symbol-inventory.v1'
    inputRoot = $input
    tool = $dumpbin
    command = 'dumpbin /symbols and dumpbin /directives for each declared object'
    objectManifest = $manifest
}
[IO.File]::WriteAllText((Join-Path $analysis 'symbol-inventory.json'), ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Startup-session symbol inventory completed: $analysis"
