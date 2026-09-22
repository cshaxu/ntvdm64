param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$header = Join-Path $root 'src\opennt-abi\source\public\internal\windows\inc\wowuserp.h'
$wow32 = Join-Path $root 'src\mvdm\wow32\wow32.c'
$bridge = Join-Path $root 'src\wow32-dll\source\wow_user_registration_bridge.c'
foreach ($path in @($header, $wow32, $bridge)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing selected registration source: $path" }
}

function Get-StructPfnFields([string]$Text, [string]$Name) {
    $match = [regex]::Match($Text, '(?s)typedef\s+struct\s+' + [regex]::Escape($Name) +
        '\s*\{(.*?)\}\s*PFNWOWHANDLERS')
    if (-not $match.Success) { throw "Cannot find registration structure $Name." }
    return @([regex]::Matches($match.Groups[1].Value, '\b(pfn[A-Za-z0-9_]+)\s*;') |
        ForEach-Object { $_.Groups[1].Value })
}

$headerText = Get-Content -LiteralPath $header -Raw
$wowText = Get-Content -LiteralPath $wow32 -Raw
$bridgeText = Get-Content -LiteralPath $bridge -Raw
$inputFields = @(Get-StructPfnFields $headerText 'tagAPFNWOWHANDLERSIN')
$outputFields = @(Get-StructPfnFields $headerText 'tagAPFNWOWHANDLERSOUT')
if ($inputFields.Count -ne 21) { throw "Expected 21 original input slots, found $($inputFields.Count)." }
if ($outputFields.Count -ne 20) { throw "Expected 20 original output slots, found $($outputFields.Count)." }

$inputAssignments = @([regex]::Matches($wowText, '\bpfnIn\.(pfn[A-Za-z0-9_]+)\s*=') |
    ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$outputAssignments = @([regex]::Matches($bridgeText, '\boutput->(pfn[A-Za-z0-9_]+)\s*=') |
    ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$missingInput = @(Compare-Object $inputFields $inputAssignments -PassThru | Where-Object { $_ -in $inputFields })
$missingOutput = @(Compare-Object $outputFields $outputAssignments -PassThru | Where-Object { $_ -in $outputFields })
if ($missingInput.Count -ne 0) { throw "Original input slots not assigned by W32Init: $($missingInput -join ', ')" }
if ($missingOutput.Count -ne 0) { throw "Provider output slots not assigned: $($missingOutput -join ', ')" }
if (-not $bridgeText.Contains('wow_input_handlers = *input;')) { throw 'Provider does not retain the original input callback record.' }
if (-not $bridgeText.Contains('output->dwBldInfo = 0x84000000u;')) { throw 'Provider does not declare the pinned USER build layout.' }
if (-not $bridgeText.Contains('output->pfnGetFullUserHandle = unsupported_full_handle;')) {
    throw 'The one known incomplete output slot is no longer explicit.'
}

[ordered]@{
    result = 'WOW32_REGISTRATION_SURFACE_OK'
    evidenceKind = 'ABI slot assignment audit; not callback/runtime acceptance'
    inputSlots = $inputFields.Count
    outputSlots = $outputFields.Count
    explicitFailureOutputSlots = @('pfnGetFullUserHandle')
    pinnedBuildInfo = '84000000'
} | ConvertTo-Json
