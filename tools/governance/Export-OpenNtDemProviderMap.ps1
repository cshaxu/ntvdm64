[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [Parameter(Mandatory = $true)]
    [string]$ProviderMapJson,
    [Parameter(Mandatory = $true)]
    [string]$OutputRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$providerMapPath = [IO.Path]::GetFullPath($ProviderMapJson)
$output = [IO.Path]::GetFullPath($OutputRoot)
if (-not (Test-Path -LiteralPath $providerMapPath -PathType Leaf)) { throw "Missing provider map JSON: $providerMapPath" }
if (Test-Path -LiteralPath $output) { throw "Refusing to overwrite existing DEM provider-map root: $output" }

function Get-Sha256([string]$Path) { (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant() }
function Get-Relative([string]$Path) { $Path.Substring($repository.Length + 1).Replace('\', '/') }

$providerMap = Get-Content -Raw -LiteralPath $providerMapPath | ConvertFrom-Json
if ($providerMap.schema -ne 'runner.opennt-bop-provider-module-map.v1') {
    throw "Unexpected provider-map schema: $($providerMap.schema)"
}
$dem = @($providerMap.service_families | Where-Object { $_.selector_name -eq 'BOP_DOS' })
if ($dem.Count -ne 1) { throw 'Expected exactly one BOP_DOS family.' }

$demRoot = Join-Path $repository 'src\opennt-bop\dem'
$definitions = @{}
Get-ChildItem -LiteralPath $demRoot -Filter *.c -File | ForEach-Object {
    $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $_.FullName) {
        $lineNumber++
        if ($line -match '^\s*(?:VOID|void|BOOL|DWORD|ULONG)\s+([A-Za-z_][A-Za-z0-9_]*)\s*(?:\([^;]*\)|$)') {
            $name = $Matches[1]
            if (-not $definitions.ContainsKey($name)) {
                $definitions[$name] = [ordered]@{ source = Get-Relative $_.FullName; line = $lineNumber }
            }
        }
    }
}

$services = @()
foreach ($service in $dem[0].services) {
    $definition = $null
    if ($null -ne $service.original_handler -and $definitions.ContainsKey($service.original_handler)) {
        $definition = $definitions[$service.original_handler]
    }
    $disposition = if ($service.dispatch_kind -eq 'dispatcher-bound-sentinel') {
        'not-callable-sentinel'
    } elseif ($service.original_handler -eq 'demNotYetImplemented') {
        'original-unavailable'
    } elseif ($null -eq $definition) {
        'source-definition-unresolved'
    } else {
        'source-pending-composition'
    }
    $services += [ordered]@{
        name = $service.name; value = $service.value
        original_handler = $service.original_handler
        dispatcher_source = $service.handler_source
        definition_source = if ($null -eq $definition) { $null } else { "$($definition.source):$($definition.line)" }
        disposition = $disposition
    }
}

$record = [ordered]@{
    schema = 'runner.opennt-dem-provider-map.v1'
    provider_map_sha256 = Get-Sha256 $providerMapPath
    interpretation = 'Static source-closure map only. source-pending-composition does not assert a linkable provider or enabled guest behavior.'
    services = $services
}
New-Item -ItemType Directory -Path $output -Force | Out-Null
$jsonPath = Join-Path $output 'opennt-dem-provider-map.json'
[IO.File]::WriteAllText($jsonPath, ($record | ConvertTo-Json -Depth 6), [Text.UTF8Encoding]::new($false))
$counts = @($services | ForEach-Object {
    [pscustomobject]@{ disposition = $_['disposition'] }
} | Group-Object disposition)
$lines = @('# OpenNT DEM Provider Map', '', 'Static source-closure mapping only.', '', '| Disposition | Count |', '| --- | ---: |')
foreach ($count in $counts | Sort-Object Name) { $lines += "| $($count.Name) | $($count.Count) |" }
$lines += @('', "Provider-map JSON SHA-256: $(Get-Sha256 $providerMapPath)", "DEM-map JSON SHA-256: $(Get-Sha256 $jsonPath)")
[IO.File]::WriteAllText((Join-Path $output 'opennt-dem-provider-map.md'), ($lines -join [Environment]::NewLine), [Text.UTF8Encoding]::new($false))
Write-Output "OpenNT DEM provider map exported: $output"
