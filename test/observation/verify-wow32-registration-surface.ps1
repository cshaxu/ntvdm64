param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$header = Join-Path $root 'src\non-mvdm\public\internal\windows\inc\wowuserp.h'
$wow32 = Join-Path $root 'src\mvdm-host\wow32\wow32.c'
$registrationOwner = Join-Path $root 'src\non-mvdm\windows\core\ntuser\client\clglobal.c'
foreach ($path in @($header, $wow32, $registrationOwner)) {
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
$registrationOwnerText = Get-Content -LiteralPath $registrationOwner -Raw
$compatState = Join-Path $root 'src/compat/wow_user_registration_state.c'
$registrationCode = $registrationOwnerText + [Environment]::NewLine +
    (Get-Content -LiteralPath $compatState -Raw)
$inputFields = @(Get-StructPfnFields $headerText 'tagAPFNWOWHANDLERSIN')
$outputFields = @(Get-StructPfnFields $headerText 'tagAPFNWOWHANDLERSOUT')
if ($inputFields.Count -ne 25) { throw "Expected 25 XP input slots, found $($inputFields.Count)." }
if ($outputFields.Count -ne 20) { throw "Expected 20 original output slots, found $($outputFields.Count)." }

$inputAssignments = @([regex]::Matches($wowText, '\bpfnIn\.(pfn[A-Za-z0-9_]+)\s*=') |
    ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$outputAssignments = @([regex]::Matches($registrationOwnerText, '\boutput->(pfn[A-Za-z0-9_]+)\s*=') |
    ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$missingInput = @(Compare-Object $inputFields $inputAssignments -PassThru | Where-Object { $_ -in $inputFields })
# N020 P19 retains the XP module slot, but the selected four-argument
# cleanup ABI still owns module cleanup. Require the unused slot to stay zero.
$unboundOutput = @('pfnWOWModuleUnload')
if (!$registrationOwnerText.Contains('ZeroMemory(output, sizeof(*output));') -or
    $outputAssignments -contains $unboundOutput[0] -or
    $wowText -match 'pfnOut\.pfnWOWModuleUnload') {
    throw 'Unused XP module callback no longer has its recorded zero/unconsumed contract.'
}
$missingOutput = @(Compare-Object $outputFields $outputAssignments -PassThru |
    Where-Object { $_ -in $outputFields -and $_ -notin $unboundOutput })
if ($missingInput.Count -ne 0) { throw "Original input slots not assigned by W32Init: $($missingInput -join ', ')" }
if ($missingOutput.Count -ne 0) { throw "Provider output slots not assigned: $($missingOutput -join ', ')" }
if (-not $registrationCode.Contains('wow_input_handlers = *input;')) { throw 'Provider does not retain the original input callback record.' }
if (-not $registrationOwnerText.Contains('output->dwBldInfo = 0x84000000u;')) { throw 'Provider does not declare the pinned USER build layout.' }
if (-not $registrationOwnerText.Contains('output->pfnGetFullUserHandle = registered_full_handle;')) {
    throw 'The full-handle output is not bound to the worker-local window association.'
}
if (-not $registrationOwnerText.Contains('output->pfnWOWLoadBitmapA = wow_bitmap_load;')) {
    throw 'The bitmap output must use the context-owning converter exercised by the bitmap fixtures.'
}
foreach ($entry in @(
    @('registered_directed_yield', 'wow_user_task_lifecycle_directed_yield'),
    @('registered_yield_task', 'wow_user_task_lifecycle_yield'),
    @('registered_wait_for_message', 'wow_user_task_lifecycle_wait')
)) {
    $body = [regex]::Match($registrationCode, '(?s)(?:static\s+)?\w+\s+WINAPI\s+' +
        $entry[0] + '\([^)]*\)\s*\{(.*?)\n\}')
    if (!$body.Success -or !$body.Groups[1].Value.Contains($entry[1] + '(') -or
        $body.Groups[1].Value -match '\bxxx(?:DirectedYield|UserYield|SleepTask)\s*\(') {
        throw "Scheduler registration bypasses the USER lock owner: $($entry[0])"
    }
}

[ordered]@{
    result = 'WOW32_REGISTRATION_SURFACE_OK'
    evidenceKind = 'ABI slot assignment audit; not callback/runtime acceptance'
    inputSlots = $inputFields.Count
    outputSlots = $outputFields.Count
    unboundUnusedOutputSlots = $unboundOutput
    pinnedBuildInfo = '84000000'
} | ConvertTo-Json
