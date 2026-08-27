[CmdletBinding()]
param(
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}
$operationsRoot = Join-Path $RepositoryRoot 'docs\etc\operations'

$families = @(
    [ordered]@{ Name = 'win32'; Root = 'src/adapter-win32'; Caller = 'dos/dem/*, dos/command/*, softpc.new/host/src/nt_error.c'; Declaration = 'inc/{oemuni,vdmredir,cmdsvc,vdmtib}.h; historical NT/RTL headers'; Contract = 'historical Win32/NTDLL/OEM declaration and call shape'; Target = 'adapter-mvdm-host-out/win32'; Failure = 'public Win32/NTDLL binding or original-style STATUS_NOT_IMPLEMENTED' },
    [ordered]@{ Name = 'softpc'; Root = 'src/adapter-softpc'; Caller = 'dos/dem/*, dos/command/*, xms.486/*, vdd/*'; Declaration = 'inc/softpc.h; softpc.new/base/inc/sas.h; softpc.new/host/inc/*'; Contract = 'historical SoftPC/CCPU/SAS call shape over bounded session mechanics'; Target = 'adapter-mvdm-host-out/softpc'; Failure = 'typed adapter-bochs/session result; never a raw host pointer' },
    [ordered]@{ Name = 'monitor'; Root = 'src/adapter-vdm-monitor'; Caller = 'v86/monitor/i386/*, ieuvddex/i386/vdmtib.c, dos/command/*'; Declaration = 'inc/{vdmtib,v86def}.h; vdmapi.h'; Contract = 'historical VDM monitor record and call shape'; Target = 'adapter-mvdm-host-out/monitor'; Failure = 'synchronous reached subset or STATUS_NOT_IMPLEMENTED; no kernel/CSR recreation' },
    [ordered]@{ Name = 'redir'; Root = 'src/adapter-redir'; Caller = 'base/mvdm/vdmredir/*'; Declaration = 'inc/vdmredir.h and VDMREDIR product declarations'; Contract = 'historical Redirector host-product boundary'; Target = 'adapter-mvdm-host-out/redir'; Failure = 'deterministic unavailable until original Redirector package is admitted' },
    [ordered]@{ Name = 'wow'; Root = 'src/adapter-wow'; Caller = 'wow32/*, wowexec/*'; Declaration = 'WOW32/WOWEXEC host-product declarations'; Contract = 'historical WOW host-product boundary'; Target = 'adapter-mvdm-host-out/wow'; Failure = 'deterministic unavailable until original WOW package is admitted' },
    [ordered]@{ Name = 'vdd'; Root = 'src/adapter-vdd'; Caller = 'vdd/*, ieuvddex/*, fax/*'; Declaration = 'VDD registration and callback declarations'; Contract = 'historical VDD host-product boundary'; Target = 'adapter-mvdm-host-out/vdd'; Failure = 'deterministic unavailable until original VDD package is admitted' },
    [ordered]@{ Name = 'debugger'; Root = 'src/adapter-debugger'; Caller = 'bde/*, dbg/*, vdmdbg/*, vdmexts/*'; Declaration = 'BDE/DBG/VDMDBG/VDMEXTS declarations'; Contract = 'historical debugger-product boundary'; Target = 'adapter-mvdm-host-out/debugger'; Failure = 'deterministic unavailable until original debugger package is admitted' }
)

function Escape-Tsv([string]$Value) {
    if ($null -eq $Value) { return '' }
    return $Value.Replace("`t", ' ').Replace("`r", ' ').Replace("`n", ' ')
}

function Write-Tsv([string]$Path, [string[]]$Headers, [object[]]$Rows) {
    $lines = [System.Collections.Generic.List[string]]::new()
    $lines.Add(($Headers -join "`t"))
    foreach ($row in $Rows) {
        $values = foreach ($header in $Headers) { Escape-Tsv ([string]$row[$header]) }
        $lines.Add(($values -join "`t"))
    }
    [System.IO.File]::WriteAllLines($Path, $lines, [System.Text.UTF8Encoding]::new($false))
}

$fileRows = [System.Collections.Generic.List[object]]::new()
$symbolRows = [System.Collections.Generic.List[object]]::new()
$consumerRows = [System.Collections.Generic.List[object]]::new()
$ordinal = 1

foreach ($family in $families) {
    $absoluteRoot = Join-Path $RepositoryRoot $family.Root
    $productionFiles = if (Test-Path -LiteralPath $absoluteRoot) {
        Get-ChildItem -LiteralPath $absoluteRoot -Recurse -File |
            Where-Object { $_.Name -ne 'README.md' } |
            Sort-Object FullName
    } else { @() }

    $consumerRows.Add([ordered]@{
        family = $family.Name; selected_consumer_package = $family.Caller
        current_dependency_root = $family.Root; future_dependency_root = $family.Target
        direction = 'mvdm-host -> adapter-mvdm-host-out/' + $family.Name
        prohibited_direction = 'adapter-mvdm-host-out/' + $family.Name + ' -> bochs-core'
        source_basis = $family.Declaration
    })

    $readme = Join-Path $absoluteRoot 'README.md'
    if (Test-Path -LiteralPath $readme) {
        $readmeRelative = $readme.Substring($RepositoryRoot.Length + 1).Replace('\\', '/')
        $readmeHash = (Get-FileHash -LiteralPath $readme -Algorithm SHA256).Hash.ToLowerInvariant()
        $fileRows.Add([ordered]@{
            record_id = ('T284-F{0:d3}' -f $ordinal); family = $family.Name
            current_root = $family.Root; current_path = $readmeRelative; source_sha256 = $readmeHash
            source_kind = 'root-documentation'; original_caller = $family.Caller
            original_declaration = $family.Declaration; observable_contract = $family.Contract
            current_disposition = 'root-boundary-documentation'; final_disposition = 'replace-with-final-family-readme'
            final_component = 'adapter-mvdm-host-out'; final_path = (($family.Target + '/README.md').Replace('\\', '/'))
            failure_or_boundary = $family.Failure; x86_selection = 'not applicable'; x64_selection = 'not applicable'
        }); $ordinal++
    }

    if ($productionFiles.Count -eq 0) {
        $fileRows.Add([ordered]@{
            record_id = ('T284-F{0:d3}' -f $ordinal); family = $family.Name
            current_root = $family.Root; current_path = '(none)'; source_sha256 = '(none)'
            source_kind = 'source-proven-family-gap'; original_caller = $family.Caller
            original_declaration = $family.Declaration; observable_contract = $family.Contract
            current_disposition = 'no-current-implementation'; final_disposition = 'create-named-unavailable-family'
            final_component = 'adapter-mvdm-host-out'; final_path = $family.Target
            failure_or_boundary = $family.Failure; x86_selection = 'not yet selected'; x64_selection = 'not yet selected'
        }); $ordinal++
        continue
    }

    foreach ($file in $productionFiles) {
        $relative = $file.FullName.Substring($RepositoryRoot.Length + 1).Replace('\', '/')
        $subpath = $relative.Substring($family.Root.Length + 1)
        $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
        $kind = if ($file.Extension -eq '.h') { 'public-header' } else { 'implementation' }
        $fileRows.Add([ordered]@{
            record_id = ('T284-F{0:d3}' -f $ordinal); family = $family.Name
            current_root = $family.Root; current_path = $relative; source_sha256 = $hash
            source_kind = $kind; original_caller = $family.Caller
            original_declaration = $family.Declaration; observable_contract = $family.Contract
            current_disposition = 'audited-migration-input'; final_disposition = 'move-with-family'
            final_component = 'adapter-mvdm-host-out'; final_path = (($family.Target + '/' + $subpath).Replace('\', '/'))
            failure_or_boundary = $family.Failure; x86_selection = 'carry forward; reselect in S3-S8'; x64_selection = 'carry forward; reselect in S3-S8'
        }); $ordinal++

        if ($file.Extension -eq '.h') {
            $text = Get-Content -LiteralPath $file.FullName -Raw
            $matches = [regex]::Matches($text, '(?m)^\s*(?:[A-Za-z_][A-Za-z0-9_\s\*]*?\s+)?(?:NTAPI\s+|APIENTRY\s+)?([A-Za-z_][A-Za-z0-9_]*)\s*\(')
            $seen = @{}
            foreach ($match in $matches) {
                $symbol = $match.Groups[1].Value
                if ($seen.ContainsKey($symbol) -or $symbol -in @('if','while','for','switch')) { continue }
                $seen[$symbol] = $true
                $symbolRows.Add([ordered]@{
                    symbol_id = ('T284-S{0:d3}' -f $symbolRows.Count); family = $family.Name
                    declaration_path = $relative; symbol = $symbol; symbol_kind = 'callable declaration'
                    original_caller = $family.Caller; final_component = 'adapter-mvdm-host-out'
                    final_subfamily = $family.Name; binding_disposition = 'preserve exact exposed spelling; audit implementation in family migration'
                    failure_or_boundary = $family.Failure
                })
            }
            if ($seen.Count -eq 0) {
                $symbolRows.Add([ordered]@{
                    symbol_id = ('T284-S{0:d3}' -f $symbolRows.Count); family = $family.Name
                    declaration_path = $relative; symbol = '(declaration-carrier types/macros only)'; symbol_kind = 'declaration carrier'
                    original_caller = $family.Caller; final_component = 'adapter-mvdm-host-out'
                    final_subfamily = $family.Name; binding_disposition = 'preserve carrier; no behavior added'
                    failure_or_boundary = $family.Failure
                })
            }
        }
    }

}

Write-Tsv (Join-Path $operationsRoot 'm0-t284-s1-adapter-convergence-file-inventory-001.tsv') @('record_id','family','current_root','current_path','source_sha256','source_kind','original_caller','original_declaration','observable_contract','current_disposition','final_disposition','final_component','final_path','failure_or_boundary','x86_selection','x64_selection') $fileRows
Write-Tsv (Join-Path $operationsRoot 'm0-t284-s1-adapter-convergence-public-symbol-inventory-001.tsv') @('symbol_id','family','declaration_path','symbol','symbol_kind','original_caller','final_component','final_subfamily','binding_disposition','failure_or_boundary') $symbolRows
Write-Tsv (Join-Path $operationsRoot 'm0-t284-s1-adapter-convergence-consumer-inventory-001.tsv') @('family','selected_consumer_package','current_dependency_root','future_dependency_root','direction','prohibited_direction','source_basis') $consumerRows

Write-Host "files=$($fileRows.Count) public_symbols=$($symbolRows.Count) consumers=$($consumerRows.Count)"
