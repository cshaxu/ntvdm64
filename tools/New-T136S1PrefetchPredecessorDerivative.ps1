[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$R5Root = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }

function Get-Sha256([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing required input: $Path" }
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
}

& (Join-Path $PSScriptRoot 'New-T98S1CurrentAdapterEngineDerivative.ps1') `
    -RepositoryRoot $repository -R5Root $R5Root -BuildRoot $build `
    -DeferredStartupPlan -MachineComposition
if (-not $?) { throw 'T98 retained-engine derivative generator failed.' }

$cpuSource = Join-Path $repository 'src\bochs\cpu\cpu.cc'
$cpuDestination = Join-Path $build 'cpu\cpu.cc'
Copy-Item -LiteralPath $cpuSource -Destination $cpuDestination -Force
$cpuHash = Get-Sha256 $cpuSource
if ((Get-Sha256 $cpuDestination) -ne $cpuHash) { throw 'Copied CPU source hash mismatch.' }

$baseShim = Join-Path $build 'ntdos64-t98-current-adapter.mak'
$text = Get-Content -LiteralPath $baseShim -Raw
$text = $text.Replace('ntdos64-t98-current-adapter.exe', 'ntdos64-t136-prefetch-observation.exe')
$text = $text.Replace('cpu\exception.o iodev/libiodev.a', 'cpu\exception.o cpu\cpu_trace.obj iodev/libiodev.a')
$text = [regex]::Replace(
    $text,
    '(ntdos64-t136-prefetch-observation\.exe:\s+[^\r\n]*cpu\\exception\.o)',
    '$1 cpu\cpu_trace.obj',
    [Text.RegularExpressions.RegexOptions]::Singleline)
$text += @"

cpu\cpu_trace.obj: cpu\cpu.cc
	`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC=1 /Tpcpu\cpu.cc /Focpu\cpu_trace.obj
"@
$shim = Join-Path $build 'ntdos64-t136-prefetch-observation.mak'
[IO.File]::WriteAllText($shim, $text, [Text.UTF8Encoding]::new($false))

$shimText = Get-Content -LiteralPath $shim -Raw
if ($shimText -notmatch 'cpu\\cpu_trace\.obj: cpu\\cpu\.cc') { throw 'Missing CPU override object rule.' }
if ($shimText -notmatch 'BX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC=1') { throw 'Missing diagnostic opt-in.' }
if ($shimText -notmatch 'ntdos64-t136-prefetch-observation\.exe:\s+[^\r\n]*cpu\\exception\.o cpu\\cpu_trace\.obj') { throw 'CPU override object is absent from target prerequisites.' }
if ($shimText -notmatch 'cpu\\exception\.o cpu\\cpu_trace\.obj iodev/libiodev\.a') { throw 'CPU override object does not precede retained CPU archive.' }
if ($shimText -match '(^|\r?\n)cpu/libcpu\.a:') { throw 'Derivative must not rebuild cpu/libcpu.a.' }

$manifest = [ordered]@{
    schema = 'ntdos64.t136.s1.prefetch-predecessor-derivative.v1'
    mode = 'generator-only-no-compiler-linker-archive-or-executable-invocation'
    retainedGenerator = 'New-T98S1CurrentAdapterEngineDerivative.ps1'
    baseManifestSha256 = Get-Sha256 (Join-Path $build 't98-s1-current-adapter-engine-derivative.json')
    cpuSourceSha256 = $cpuHash
    shimSha256 = Get-Sha256 $shim
    permittedBuild = 'nmake /f ntdos64-t136-prefetch-observation.mak ntdos64-t136-prefetch-observation.exe'
}
[IO.File]::WriteAllText((Join-Path $build 't136-s1-prefetch-predecessor-derivative.json'), ($manifest | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fresh T136 S1 prefetch-predecessor derivative: $build"
