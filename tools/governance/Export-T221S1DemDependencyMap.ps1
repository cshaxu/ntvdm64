[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath($RepositoryRoot)
$sourceMapPath = Join-Path $root 'docs/etc/research/t221-s1-dem-source-owner-map-001.json'
if (-not (Test-Path -LiteralPath $sourceMapPath -PathType Leaf)) {
    throw 'T221 S1 P2 source-owner map is required before dependency mapping'
}
$sourceMap = ConvertFrom-Json -InputObject (Get-Content -LiteralPath $sourceMapPath -Raw)
if ($sourceMap.serviceCount -ne 73 -or @($sourceMap.byOriginalImplementation).Count -eq 0) {
    throw 'Source-owner map is incomplete'
}

$categories = [ordered]@{
    # `Select-String` is case-insensitive by default.  Only lower-case
    # SoftPC register macros such as `getAX` belong here; Win32 `Get*` calls
    # belong to the ordinary-host capability category.
    legacyGuestAbi = '\b(GetVDMAddr|Sim32[A-Za-z0-9_]*)\b|(?-i:\bget[A-Z][A-Za-z0-9_]*)\b'
    legacyVddOrSoftpc = '\b(VDD[A-Za-z0-9_]*|Host[A-Za-z0-9_]*|softpc)\b'
    legacyEngineLifecycle = '\b(TerminateVDM|GetNextVDMCommand|WaitIfIdle|WakeUpNow)\b'
    ordinaryFilesystem = '\b(CreateFile[A-Za-z0-9_]*|CloseHandle|ReadFile|WriteFile|SetFilePointer|FindFirstFile[A-Za-z0-9_]*|FindNextFile[A-Za-z0-9_]*|GetFileTime|SetFileTime|GetFileAttributes|SetFileAttributes|DeleteFile[A-Za-z0-9_]*|MoveFile[A-Za-z0-9_]*|CreateDirectory[A-Za-z0-9_]*|RemoveDirectory[A-Za-z0-9_]*|GetVolumeInformation[A-Za-z0-9_]*|SetCurrentDirectory[A-Za-z0-9_]*|GetCurrentDirectory[A-Za-z0-9_]*)\b'
    rawDevice = '\b(DeviceIoControl|CreateFileA|DiskIo|HostFloppyReset|HostFdiskReset)\b'
}

$modules = foreach ($module in $sourceMap.byOriginalImplementation) {
    $relative = [string]$module.source
    $absolute = Join-Path $root $relative.Replace('/', '\\')
    if (-not (Test-Path -LiteralPath $absolute -PathType Leaf)) { throw "Original source disappeared: $relative" }
    $text = Get-Content -LiteralPath $absolute -Raw
    $hits = [ordered]@{}
    foreach ($category in $categories.Keys) {
        $names = @(Select-String -InputObject $text -AllMatches -Pattern $categories[$category] |
            ForEach-Object { $_.Matches } | ForEach-Object { $_.Value } |
            Sort-Object -Unique)
        $hits[$category] = $names
    }
    [pscustomobject]@{
        originalImplementation = $relative
        services = @($module.services)
        serviceCount = [int]$module.serviceCount
        dependencies = $hits
        staticFinding = 'presence scan only; exact ABI/order/failure inspection remains required'
    }
}
if (($modules | Measure-Object -Property serviceCount -Sum).Sum -ne 73) {
    throw 'Dependency map lost DEM service coverage'
}

$output = [IO.Path]::GetFullPath($OutputPath)
$parent = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $parent)) { New-Item -ItemType Directory -Path $parent | Out-Null }
[ordered]@{
    schema = 'runner.t221.s1.dem-dependency-map.v1'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    serviceCount = 73
    staticOnly = $true
    categories = @($categories.Keys)
    modules = $modules
} | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $output -Encoding utf8
Write-Host "Exported dependency presence map for $($modules.Count) DEM source modules to $output"
