[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'docs/etc/operations',
    [string]$CanonicalMvdmRoot = 'O:\repos.external\opennt-4.5\nt\private\mvdm'
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$out = Join-Path $root $OutputDirectory
$canonical = (Resolve-Path -LiteralPath $CanonicalMvdmRoot).Path

function Get-RelativeFiles([string]$component) {
    $path = Join-Path $root ('src/mvdm-host/' + $component)
    $prefix = (Resolve-Path -LiteralPath $path).Path.Length + 1
    return @(Get-ChildItem -LiteralPath $path -Recurse -File | ForEach-Object {
        ($component + '/' + $_.FullName.Substring($prefix).Replace('\', '/'))
    } | Sort-Object)
}

function Normalize-Owners([string]$owners) {
    return $owners.Replace('adapter-vdm-monitor', 'adapter-mvdm-host-out/monitor').
        Replace('adapter-softpc', 'adapter-mvdm-host-out/softpc').
        Replace('adapter-win32', 'adapter-mvdm-host-out/win32').
        Replace('adapter-wow', 'adapter-mvdm-host-out/wow')
}

function Get-MappingInstance([string]$requirement) {
    $items = [System.Collections.Generic.List[string]]::new()
    if ($requirement -match 'guest-memory|VDM frame|guest structure|16-bit address') {
        $items.Add('session.guest_memory')
    }
    if ($requirement -match 'host-resource|native identity|native endpoint|HPRINTER|HDC') {
        $items.Add('session.host_resource')
    }
    if ($requirement -match 'broker|shared section|cross-process') {
        $items.Add('broker copied IDs')
    }
    if ($items.Count -eq 0) { return 'none; copied scalar/layout only' }
    return [string]::Join('; ', $items)
}

function Get-WowDisposition([psobject]$file, [hashtable]$subdomains) {
    if ($file.file_kind -eq 'declaration') { return 'binding-only' }
    if ($file.file_kind -ne 'source') { return 'not-host-runtime' }
    if ($file.original_role_group -match 'unselected') { return 'not-host-runtime' }
    $subdomain = $subdomains[$file.source_path].original_subdomain
    if ($subdomain -in @('thunk-frame-and-structure',
            'message-and-window-procedure', 'gdi-and-window-object',
            'kernel-task-and-dos-session', 'user-interface')) {
        return 'overlay-required; future whole-subdomain package'
    }
    return 'adapter-backed; future whole-subdomain package'
}

function Get-FaxDisposition([psobject]$file) {
    if ($file.file_kind -eq 'declaration') { return 'binding-only' }
    if ($file.file_kind -ne 'source') { return 'not-host-runtime' }
    if ($file.original_role_group -eq 'historical-gdi-driver-provider-body') {
        return 'not-host-runtime; private GDI driver product shell'
    }
    return 'adapter-backed; future FAX UI package'
}

$wowInput = @(Import-Csv (Join-Path $root 'docs/etc/operations/m0-t276-s22-wow32-file-disposition-ledger.tsv') -Delimiter "`t")
$faxInput = @(Import-Csv (Join-Path $root 'docs/etc/operations/m0-t276-s23-fax-file-disposition-ledger.tsv') -Delimiter "`t")
$subdomainInput = @(Import-Csv (Join-Path $root 'docs/etc/operations/m0-t276-s22-wow32-active-subdomain-ledger.tsv') -Delimiter "`t")
$wowInterfaces = @(Import-Csv (Join-Path $root 'docs/etc/operations/m0-t276-s22-wow32-interface-family-ledger.tsv') -Delimiter "`t")
$faxInterfaces = @(Import-Csv (Join-Path $root 'docs/etc/operations/m0-t276-s23-fax-interface-family-ledger.tsv') -Delimiter "`t")

$subdomains = @{}
foreach ($entry in $subdomainInput) {
    if ($subdomains.ContainsKey($entry.source_path)) { throw "Duplicate WOW32 subdomain: $($entry.source_path)" }
    $subdomains[$entry.source_path] = $entry
}
if ($subdomains.Count -ne 77) { throw "Expected 77 active WOW32 bodies, found $($subdomains.Count)." }

$allInput = @($wowInput + $faxInput)
$seen = @{}
foreach ($entry in $allInput) {
    if ($seen.ContainsKey($entry.source_path)) { throw "Duplicate source path: $($entry.source_path)" }
    $seen[$entry.source_path] = $entry
}
$live = @(Get-RelativeFiles 'wow32') + @(Get-RelativeFiles 'fax')
if ($live.Count -ne 222 -or $seen.Count -ne 222) {
    throw "Expected 222 WOW32/FAX paths; live=$($live.Count) ledger=$($seen.Count)."
}
foreach ($path in $live) { if (-not $seen.ContainsKey($path)) { throw "Live path absent from audit input: $path" } }
foreach ($path in $seen.Keys) { if ($path -notin $live) { throw "Audit input absent from live mirror: $path" } }
foreach ($path in $subdomains.Keys) {
    if (-not $seen.ContainsKey($path) -or $seen[$path].original_role_group -notmatch 'active-provider') {
        throw "Active WOW32 subdomain lacks active source row: $path"
    }
}

$fileRows = foreach ($path in ($seen.Keys | Sort-Object)) {
    $file = $seen[$path]
    $component = if ($path.StartsWith('wow32/')) { 'wow32' } else { 'fax' }
    $fullPath = Join-Path $root ('src/mvdm-host/' + $path)
    $subdomain = if ($subdomains.ContainsKey($path)) { $subdomains[$path].original_subdomain } else { 'carrier-or-inactive-form' }
    $mapping = if ($subdomains.ContainsKey($path)) { Get-MappingInstance $subdomains[$path].mapping_requirement } else { 'none; declaration/resource/inactive form' }
    $disposition = if ($component -eq 'wow32') { Get-WowDisposition $file $subdomains } else { Get-FaxDisposition $file }
    [PSCustomObject]@{
        audit_id = $file.file_id
        source_path = $path
        owner_package = $component
        file_kind = $file.file_kind
        original_role_group = $file.original_role_group
        original_subdomain = $subdomain
        final_product_disposition = $disposition
        boundary_owners = Normalize-Owners $file.required_boundary_owner
        mapping_instance = $mapping
        current_sha256 = (Get-FileHash -LiteralPath $fullPath -Algorithm SHA256).Hash
        source_evidence = $file.evidence
    }
}

$identityRows = foreach ($path in ($seen.Keys | Sort-Object)) {
    $currentPath = Join-Path $root ('src/mvdm-host/' + $path)
    $canonicalPath = Join-Path $canonical $path
    if (-not (Test-Path -LiteralPath $canonicalPath)) {
        throw "Selected canonical WOW/FAX path is missing: $canonicalPath"
    }
    $currentHash = (Get-FileHash -LiteralPath $currentPath -Algorithm SHA256).Hash
    $canonicalHash = (Get-FileHash -LiteralPath $canonicalPath -Algorithm SHA256).Hash
    $identity = if ($currentHash -eq $canonicalHash) { 'byte-exact' } elseif ($path -eq 'wow32/wkfileio.c') { 'registered MVDM-HOST-DIV-007' } else { throw "Unregistered mirror difference: $path" }
    [PSCustomObject]@{
        source_path = $path
        current_sha256 = $currentHash
        canonical_source_path = ('opennt-4.5/nt/private/mvdm/' + $path)
        canonical_sha256 = $canonicalHash
        mirror_identity = $identity
    }
}
if (@($identityRows | Where-Object { $_.mirror_identity -ne 'byte-exact' }).Count -ne 1) {
    throw 'Expected exactly one registered WOW/FAX mirror divergence.'
}

$interfaceRows = foreach ($family in @($wowInterfaces + $faxInterfaces)) {
    [PSCustomObject]@{
        family_id = $family.family_id
        owner_package = if ($family.family_id.StartsWith('WOW32-')) { 'wow32' } else { 'fax' }
        original_interface_family = $family.original_interface_family
        source_proven_callers = $family.source_proven_callers
        original_contract = $family.original_contract
        boundary_owners = Normalize-Owners $family.single_boundary_owner
        mapping_instance = Get-MappingInstance $family.mapping_requirement
        final_s1_disposition = Normalize-Owners $family.provisional_recovery_disposition
        source_evidence = $family.evidence
    }
}
if ($interfaceRows.Count -ne 15) { throw "Expected 15 WOW/FAX interface families, found $($interfaceRows.Count)." }

$fileRows | Export-Csv (Join-Path $out 'm0-t291-s1-wow-file-subdomain-disposition-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
$interfaceRows | Export-Csv (Join-Path $out 'm0-t291-s1-wow-interface-family-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
$identityRows | Export-Csv (Join-Path $out 'm0-t291-s1-wow-mirror-identity-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "T291 S1 WOW audit exported: $($fileRows.Count) files, $($subdomains.Count) active WOW32 bodies, $($interfaceRows.Count) interface families, 1 registered mirror divergence."
