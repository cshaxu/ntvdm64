[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$input = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
if (-not (Test-Path -LiteralPath $input)) { throw "Missing input: $input" }

$files = @(Import-Csv -LiteralPath $input -Delimiter "`t" |
    Where-Object { $_.target_path -like 'ieuvddex/*' } |
    Sort-Object target_path)
if ($files.Count -ne 23) { throw "Expected 23 IEUVDDEx paths, found $($files.Count)." }

$result = foreach ($file in $files) {
    $path = $file.target_path
    $role = $null
    $boundary = 'adapter-debugger; session diagnostic snapshot/export boundary'
    $families = 'historical NTSD/WinDbg extension API; VDM_TIB/context/LDT/PIC/DPMI trace inspection'
    $summary = $null
    $disposition = $null

    switch -Regex ($path) {
        '^ieuvddex/(makefile|sources|i386/sources)$' {
            $role = 'historical-debugger-extension-build-description'
            $boundary = 'build governance; adapter-debugger'
            $families = 'historical NTSD/WinDbg extension build selection'
            $summary = 'original debugger-extension build-selection evidence only; it does not enable a DLL or debugger API edge'
            $disposition = 'build-description-only'
        }
        '^ieuvddex/(alpha|mips|ppc)/(helpp\.c|sources)$' {
            $role = 'historical-non-x86-debugger-extension-form'
            $boundary = 'adapter-debugger; build governance'
            $families = 'processor-specific debugger-extension help form'
            $summary = 'original Alpha/MIPS/PPC help/build form; retain for source/provenance completeness but exclude from x86/x64 host recovery profile'
            $disposition = 'non-x86 profile exclusion; source evidence only'
        }
        '^ieuvddex/(data\.c|help\.c|pmode\.c|procdep\.c|system\.c|trace\.c)$' {
            $role = 'historical-debugger-extension-body'
            $summary = 'original NTSD/WinDbg debugger extension body that reads a separate NTVDM process by symbols and ReadProcessMemory; preserve API and display-order evidence, but it is not an in-process VDD runtime provider'
            $disposition = 'debugger-product profile exclusion; same-shaped diagnostic API recovery prerequisite'
        }
        '^ieuvddex/i386/(helpp\.c|profile\.c|reflect\.c|regs\.c|vdmtib\.c)$' {
            $role = 'historical-x86-debugger-extension-body'
            $summary = 'original x86 NTSD/WinDbg extension body for VDM_TIB, context, profiling and reflection inspection; any future recovery reads a copied session diagnostic snapshot, never internal process addresses or raw guest pointers'
            $disposition = 'debugger-product profile exclusion; copied diagnostic-snapshot API recovery prerequisite'
        }
        '^ieuvddex/ieuvddex\.h$' {
            $role = 'historical-debugger-extension-declaration'
            $summary = 'original debugger-extension declaration/layout and callback API evidence; preserve the extension-facing form but bind a later diagnostic facade to fixed-width copied session state'
            $disposition = 'declaration and diagnostic-API recovery prerequisite'
        }
        '^ieuvddex/(ntvdm\.def|vdmdll\.rc)$' {
            $role = 'historical-debugger-extension-export-or-resource'
            $boundary = 'adapter-debugger; app packaging'
            $families = 'historical NTSD/WinDbg command exports and resource metadata'
            $summary = 'original debugger-extension export/resource evidence; no current application claims the ntvdm.dll product identity'
            $disposition = 'debugger-product exclusion; export/resource evidence only'
        }
        default { throw "Missing IEUVDDEx disposition: $path" }
    }

    [pscustomobject][ordered]@{
        file_id = $file.file_id
        source_path = $path
        file_kind = $file.file_kind
        original_role_group = $role
        provisional_component_owner = 'opennt-mvdm-host'
        required_boundary_owner = $boundary
        lexical_external_families = $families
        original_contract_summary = $summary
        provisional_disposition = $disposition
        state = 'provisional-not-enabled'
        evidence = "$path; ieuvddex/sources; ieuvddex/i386/sources; ieuvddex/ntvdm.def"
    }
}

$result | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" |
    Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s18-ieuvddex-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S18 IEUVDDEx disposition generated: $($result.Count) rows."
