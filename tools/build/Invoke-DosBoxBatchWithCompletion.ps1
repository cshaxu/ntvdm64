[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$WorkingDirectory,
    [Parameter(Mandatory = $true)][string]$BatchFile,
    [Parameter(Mandatory = $true)][string[]]$CompletionFiles,
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
if (-not (Test-Path -LiteralPath $working -PathType Container)) { throw "WorkingDirectory does not exist: $working" }
if (-not (Test-Path -LiteralPath $DosBoxPath -PathType Leaf)) { throw "DOSBox executable is unavailable: $DosBoxPath" }
if ([IO.Path]::IsPathRooted($BatchFile)) { throw 'BatchFile must be a relative stage path.' }
$batch = Resolve-ContainedPath $working (Join-Path $working $BatchFile) 'BatchFile'
if (-not (Test-Path -LiteralPath $batch -PathType Leaf)) { throw "BatchFile does not exist: $batch" }
if ($CompletionFiles.Count -eq 0) { throw 'CompletionFiles must name at least one expected stage output.' }
$completion = foreach ($file in $CompletionFiles) {
    if ([IO.Path]::IsPathRooted($file)) { throw "CompletionFile must be a relative stage path: $file" }
    Resolve-ContainedPath $working (Join-Path $working $file) 'CompletionFile'
}

$dosBatch = $BatchFile.Replace('/', '\')
$mount = 'mount c ' + $working
$arguments = '-noconsole -c "' + $mount + '" -c "c:" -c "call ' + $dosBatch + '" -c "exit"'
$process = Start-Process -FilePath $DosBoxPath -ArgumentList $arguments -PassThru -WindowStyle Hidden
$deadline = [DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
$previous = $null
$stableSamples = 0
$completed = $false
while ([DateTime]::UtcNow -lt $deadline) {
    $snapshot = @($completion | ForEach-Object {
        if (Test-Path -LiteralPath $_ -PathType Leaf) { (Get-Item -LiteralPath $_).Length } else { -1 }
    })
    if (($snapshot -notcontains -1) -and $null -ne $previous -and (@($snapshot) -join ',') -eq (@($previous) -join ',')) {
        $stableSamples++
        if ($stableSamples -ge 10) {
            $completed = $true
            break
        }
    } else {
        $stableSamples = 0
    }
    $previous = $snapshot
    Start-Sleep -Milliseconds 500
}
if (-not $process.HasExited) { Stop-Process -Id $process.Id -Force }
if (-not $completed) { throw "DOS batch did not produce stable declared outputs within $TimeoutSeconds seconds: $batch" }
Write-Host "DOSBox batch produced stable outputs: $batch"