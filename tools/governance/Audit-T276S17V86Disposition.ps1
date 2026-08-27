[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$input = Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
if (-not (Test-Path -LiteralPath $input)) { throw "Missing input: $input" }

$files = @(Import-Csv -LiteralPath $input -Delimiter "`t" |
    Where-Object { $_.target_path -like 'v86/*' } |
    Sort-Object target_path)
if ($files.Count -ne 35) { throw "Expected 35 V86 paths, found $($files.Count)." }

$result = foreach ($file in $files) {
    $path = $file.target_path
    $role = $null
    $owner = 'mvdm-host'
    $boundary = 'none'
    $families = 'none'
    $summary = $null
    $disposition = $null

    switch -Regex ($path) {
        '^v86/(dirs|monitor/makefile|monitor/sources|monitor/i386/sources|scaffold/makefile|scaffold/sources|scaffold/i386/sources)$' {
            $role = 'historical-build-description'
            $owner = 'mvdm-host'
            $boundary = 'build governance'
            $summary = 'original build selection and profile evidence only; no modern build edge is enabled by this audit'
            $disposition = 'build-description-only'
        }
        '^v86/monitor/i386/(monitor\.c|int\.c|thread\.c|sas\.c|fastpm\.asm|spcstubs\.c|proflib\.c)$' {
            $role = 'historical-kernel-vdm-monitor'
            $boundary = 'adapter-vdm-monitor; adapter-softpc; adapter-bochs; session guest-memory mapping manager'
            $families = 'NT VDM_TIB/event/NtVdmControl; CCPU/SAS/SoftPC; BOP/interrupt/port callbacks; checked guest-memory leases'
            $summary = 'original monitor dispatches NT kernel VDM events and exposes register, BOP, interrupt, port and memory callbacks; retain the source/API evidence, but no kernel-VDM monitor body is composed'
            $disposition = 'historical-monitor-body profile exclusion; same-shaped monitor API recovery prerequisite'
        }
        '^v86/monitor/i386/monitorp\.h$' {
            $role = 'historical-vdm-monitor-declaration'
            $boundary = 'adapter-vdm-monitor; adapter-softpc; adapter-bochs; session guest-memory mapping manager'
            $families = 'VDM_TIB/event/NtVdmControl; CCPU/SAS/SoftPC; checked guest-memory leases'
            $summary = 'original monitor declaration and data-layout evidence; any recovered callback form must use copied CPU frames and the unique mapping manager, never a raw host VDM pointer'
            $disposition = 'declaration and monitor-API recovery prerequisite'
        }
        '^v86/scaffold/i386/(softpc\.c|fakebop\.c|fakeinit\.c|fakekbd\.c|fakevid\.c|stubs\.c)$' {
            $role = 'historical-ntvdm-product-scaffold'
            $owner = 'app'
            $boundary = 'app; adapter-mvdm-host-in; adapter-softpc; adapter-bochs; adapter-mvdm-host-out/win32; session'
            $families = 'historical ntvdm.exe startup/termination; SoftPC initialization; BOP/BIOS/video/keyboard placeholders; console and BaseSrv APIs'
            $summary = 'original standalone NTVDM product startup/scaffold and temporary BIOS stubs; app owns modern composition and cannot import this product shell as a second engine or duplicate startup path'
            $disposition = 'historical-product-shell exclusion; source-shaped composition evidence only'
        }
        '^v86/scaffold/i386/(fun\.h|x86\.h|xbios\.h|xbiosdsk\.h|xbioskbd\.h|xbiosvid\.h|xguest\.h|xwincon\.h)$' {
            $role = 'historical-scaffold-declaration'
            $owner = 'app'
            $boundary = 'app; adapter-mvdm-host-in; adapter-softpc; adapter-bochs; adapter-mvdm-host-out/win32; session'
            $families = 'historical product startup, BIOS placeholder and console declarations'
            $summary = 'declaration/layout evidence for the excluded standalone scaffold; individual same-shaped boundary declarations may be recovered only through their named component owner'
            $disposition = 'declaration evidence; no standalone scaffold composition'
        }
        '^v86/scaffold/(ntvdm\.def|vdm\.rc|vdm\.ico|wow\.ico)$' {
            $role = 'historical-ntvdm-product-resource-or-export'
            $owner = 'app'
            $boundary = 'app; adapter-softpc; adapter-vdm-monitor'
            $families = 'historical ntvdm.exe exports and UI resources'
            $summary = 'original product export/resource evidence; modern app does not claim the historical executable identity or export surface by this audit'
            $disposition = 'historical-product-shell exclusion; export/resource evidence only'
        }
        '^v86/util/(makefile|quit\.asm)$' {
            $role = 'historical-16bit-vdm-utility'
            $owner = 'mvdm-tools'
            $boundary = 'tools/opennt; adapter-mvdm-host-in'
            $families = '16-bit BOP_UNSIMULATE termination utility'
            $summary = 'original independently-built 16-bit quit utility; retain as tool/source evidence and do not link it into the host runtime'
            $disposition = 'tool-only profile exclusion'
        }
        default { throw "Missing V86 disposition: $path" }
    }

    [pscustomobject][ordered]@{
        file_id = $file.file_id
        source_path = $path
        file_kind = $file.file_kind
        original_role_group = $role
        provisional_component_owner = $owner
        required_boundary_owner = $boundary
        lexical_external_families = $families
        original_contract_summary = $summary
        provisional_disposition = $disposition
        state = 'provisional-not-enabled'
        evidence = "$path; v86/monitor/sources; v86/monitor/i386/sources; v86/scaffold/sources; v86/scaffold/i386/sources"
    }
}

$result | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" |
    Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s17-v86-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S17 V86 disposition generated: $($result.Count) rows."
