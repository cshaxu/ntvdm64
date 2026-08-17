[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$WorkingDirectory,
    [Parameter(Mandatory = $true)][string]$BatchFile,
    [Parameter(Mandatory = $true)][string]$CompletionMarker,
    [ValidateRange(1, 60)][int]$TimeoutSeconds = 30,
    [string]$DosBoxPath = 'C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Resolve-ContainedPath([string]$Root, [string]$Candidate, [string]$Label) {
    $full = [IO.Path]::GetFullPath($Candidate)
    $prefix = $Root + [IO.Path]::DirectorySeparatorChar
    if (-not $full.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) {
        throw "$Label must remain below ${Root}: $full"
    }
    return $full
}

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$buildRoot = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
$working = Resolve-ContainedPath $buildRoot $WorkingDirectory 'WorkingDirectory'
if (-not (Test-Path -LiteralPath $working -PathType Container)) {
    throw "WorkingDirectory does not exist: $working"
}
if (-not (Test-Path -LiteralPath $DosBoxPath -PathType Leaf)) {
    throw "DOSBox executable is unavailable: $DosBoxPath"
}

if ([IO.Path]::IsPathRooted($BatchFile) -or [IO.Path]::IsPathRooted($CompletionMarker)) {
    throw 'BatchFile and CompletionMarker must be relative stage paths.'
}
$batch = Resolve-ContainedPath $working (Join-Path $working $BatchFile) 'BatchFile'
$marker = Resolve-ContainedPath $working (Join-Path $working $CompletionMarker) 'CompletionMarker'
if (-not (Test-Path -LiteralPath $batch -PathType Leaf)) {
    throw "BatchFile does not exist: $batch"
}
if (Test-Path -LiteralPath $marker) {
    throw "CompletionMarker already exists: $marker"
}

$dosBatch = $BatchFile.Replace('/', '\')
$mount = 'mount c "' + $working + '"'
& $DosBoxPath -noconsole -c $mount -c 'c:' -c ('call ' + $dosBatch) -c 'exit'

$deadline = [DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
while ([DateTime]::UtcNow -lt $deadline) {
    if (Test-Path -LiteralPath $marker -PathType Leaf) {
        Write-Host "DOS batch completed: $marker"
        exit 0
    }
    Start-Sleep -Milliseconds 200
}

throw "DOS batch completion marker was not observed within $TimeoutSeconds seconds: $marker"
