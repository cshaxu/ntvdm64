param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$domain = Join-Path $root 'src\ntvdm-exe\softpc\mvdm_softpc_wow_page_domain.c'
if (-not (Test-Path -LiteralPath $domain -PathType Leaf)) {
    throw "Missing selected page-domain source: $domain"
}

$domainText = Get-Content -LiteralPath $domain -Raw
if (-not $domainText.Contains('if (!desktop_info || !delta) return 0;')) {
    throw 'The page-domain entry no longer rejects the invalid zero desktop/delta pair.'
}

function Assert-ViewRelation([UInt64]$ClientBase, [UInt64]$ClientLimit,
    [UInt64]$ClientObject, [UInt64]$Delta) {
    $max = [UInt64]4294967295
    if ($Delta -eq 0 -or $ClientBase -ge $ClientLimit -or
        $ClientObject -lt $ClientBase -or $ClientObject -ge $ClientLimit) {
        throw 'Invalid test relation input.'
    }
    if ($ClientBase -gt ($max - $Delta) -or
        $ClientLimit -gt ($max - $Delta) -or
        $ClientObject -gt ($max - $Delta)) {
        throw 'A desktop server-view address would wrap 32 bits.'
    }
    $serverBase = $ClientBase + $Delta
    $serverLimit = $ClientLimit + $Delta
    $serverObject = $ClientObject + $Delta
    if (($serverObject -lt $serverBase) -or ($serverObject -ge $serverLimit)) {
        throw 'Server-form object lies outside the server-form desktop bounds.'
    }
    if (($serverObject - $Delta) -ne $ClientObject) {
        throw 'Server-form object does not relocate back to its client address.'
    }
    return [ordered]@{
        clientDeskInfo = ('0x{0:X8}' -f $ClientBase)
        clientObject = ('0x{0:X8}' -f $ClientObject)
        delta = ('0x{0:X8}' -f $Delta)
        serverBase = ('0x{0:X8}' -f $serverBase)
        serverObject = ('0x{0:X8}' -f $serverObject)
        serverLimit = ('0x{0:X8}' -f $serverLimit)
    }
}

$positive = Assert-ViewRelation ([UInt64]1048576) ([UInt64]1114112) ([UInt64]1049600) ([UInt64]1879048192)
$overflowRejected = $false
try {
    $null = Assert-ViewRelation ([UInt64]4026531840) ([UInt64]4026597376) ([UInt64]4026532864) ([UInt64]536870912)
} catch {
    $overflowRejected = $_.Exception.Message -eq 'A desktop server-view address would wrap 32 bits.'
}
if (-not $overflowRejected) {
    throw 'Overflowing server-view relation was not rejected.'
}

[ordered]@{
    result = 'WOW_CLIENT_VIEW_TRANSLATION_CONTRACT_OK'
    evidenceKind = 'bounded arithmetic/design fixture; not a guest desktop acceptance'
    tebDeskInfoForm = 'client'
    rebasedHeapFields = 'server-form, bounded then delta-subtracted'
    positive = $positive
    overflowRejected = $overflowRejected
} | ConvertTo-Json -Depth 3
