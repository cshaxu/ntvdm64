[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$ManifestPath = 'tools/build/t225-s7-full-module-manifest.json',
    [string]$OutputPath = 'docs/etc/operations/m0-t260-s1-owner-provenance-build-manifest.tsv'
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$manifestFullPath = Join-Path $root $ManifestPath
$outputFullPath = Join-Path $root $OutputPath
$manifest = Get-Content -LiteralPath $manifestFullPath -Raw | ConvertFrom-Json
$rows = [System.Collections.Generic.List[object]]::new()
$seen = @{}

function Add-Row(
    [string]$Path,
    [string]$Kind,
    [string]$CurrentOwner,
    [string]$TargetOwner,
    [string]$Provenance,
    [string]$Disposition,
    [string]$PlannedS,
    [string]$Reason
) {
    $normalized = $Path.Replace('\', '/')
    $key = "$Kind|$normalized"
    if ($seen.ContainsKey($key)) { return }
    $seen[$key] = $true
    $rows.Add([pscustomobject]@{
        Path = $normalized
        Kind = $Kind
        CurrentOwner = $CurrentOwner
        TargetOwner = $TargetOwner
        Provenance = $Provenance
        Disposition = $Disposition
        PlannedS = $PlannedS
        Reason = $Reason
    })
}

function Get-SourceDisposition([string]$Path, [string]$CurrentModule) {
    $p = $Path.Replace('\', '/')
    if ($p -like 'src/bx-core/*') {
        return @('bx-core', 'adopted Bochs 2.6', 'retain', 'S3', 'adopted core; README exception register required')
    }
    if ($p -like 'src/cli/*') {
        return @('app', 'project-authored CLI/composition', 'git mv', 'S4', 'CLI and final component composition')
    }
    if ($p -like 'src/opennt/local/compat/host/*') {
        return @('opennt-host', 'OpenNT host capability/source-derived host component', 'git mv', 'S6', 'host capability owner')
    }
    if ($p -like 'src/opennt/base/mvdm/dos/*' -or $p -like 'src/opennt/base/mvdm/wow16/*') {
        return @('opennt-guest', 'imported OpenNT guest source', 'git mv', 'S4', 'guest DOS/WOW source or build input')
    }
    if ($p -like 'src/bx-mantle/*') {
        $name = [System.IO.Path]::GetFileName($p)
        if ($name -in @('logio.cc', 'paramtree.cc', 'paramtree.h', 'pc_system.cc', 'pc_system.h', 'pic.cc', 'pic.h', 'keyboard.cc', 'keyboard.h', 'scancodes.cc', 'scancodes.h', 'minimal_port_space.cc', 'minimal_product_shell.cc')) {
            return @('bx-mantle', 'project-authored Bochs-only assembly', 'retain', 'S3', 'candidate native Bochs-only source; S3 semantic scan required')
        }
        return @('adapter-softpc', 'project-authored legacy machine boundary', 'git mv', 'S3', 'candidate foreign-semantic mantle source; retain in mantle only if S3 proves Bochs-only')
    }
    if ($p -like 'src/bx-vdm/bop/opennt/*') {
        return @('opennt-bop', 'imported OpenNT BOP mirror', 'git mv', 'S5', 'original BOP/provider mirror')
    }
    if ($p -like 'src/bx-vdm/bop/observation/*') {
        return @('adapter-softpc', 'project-authored mechanical observation', 'git mv', 'S7', 'bounded machine observation; no BOP meaning')
    }
    if ($p -like 'src/bx-vdm/bop/shim/opennt_rtl_compat*' -or $p -like 'src/bx-vdm/bop/shim/opennt_error_dialog_facade*') {
        return @('adapter-win32', 'project-authored source-shaped Win32/NTDLL facade', 'git mv', 'S6', 'same-shaped unavailable API recovery')
    }
    if ($p -like 'src/bx-vdm/bop/shim/softpc_*' -or $p -like 'src/bx-vdm/bop/shim/spckbd_*' -or $p -like 'src/bx-vdm/bop/shim/dpmi_*' -or $p -like 'src/bx-vdm/bop/shim/bx_ntvdm_*') {
        return @('adapter-softpc', 'project-authored SoftPC/CCPU facade', 'git mv', 'S7', 'same-shaped Bochs-backed machine adaptation')
    }
    if ($p -like 'src/bx-vdm/bop/shim/*') {
        return @('opennt-host', 'project-authored OpenNT host capability shim', 'review/split', 'S6', 'default host-capability classification; S6 verifies no route or API facade semantics')
    }
    if ($p -like 'src/bx-vdm/bop/*') {
        return @('opennt-bop', 'project-authored BOP route/composition', 'git mv', 'S5', 'BOP ingress or provider routing')
    }
    if ($p -like 'src/bx-vdm/bx_ntvdm_bios_selector_map*') {
        return @('opennt-bop', 'project-authored BOP/machine selector route', 'git mv', 'S5', 'historical selector meaning belongs with BOP route')
    }
    if ($p -like 'src/bx-vdm/bx_ntvdm_host_drive_policy*' -or $p -like 'src/bx-vdm/bx_ntvdm_host_namespace*' -or $p -like 'src/bx-vdm/bx_ntvdm_namespace_root_set*') {
        return @('opennt-host', 'project-authored host capability', 'git mv', 'S6', 'host path/drive namespace semantics')
    }
    if ($p -like 'src/bx-vdm/bx_ntvdm_startup_*' -or $p -like 'src/bx-vdm/bx_ntvdm_preentry_*' -or $p -like 'src/bx-vdm/bx_ntvdm_initial_state_*') {
        return @('app', 'project-authored product composition', 'review/split', 'S4', 'final startup/image/component composition; S4 separates mechanical subrecords')
    }
    if ($p -like 'src/bx-vdm/*') {
        return @('adapter-softpc', 'project-authored SoftPC/CCPU facade', 'git mv', 'S7', 'typed mechanical CCPU/SoftPC-to-Bochs boundary')
    }
    return @('UNCLASSIFIED', 'unknown', 'review', 'S1', 'manual owner decision required')
}

foreach ($module in $manifest.modules) {
    foreach ($source in @($module.sources)) {
        $d = Get-SourceDisposition $source $module.name
        Add-Row $source 'source' $module.name $d[0] $d[1] $d[2] $d[3] $d[4]
    }
}

foreach ($fixture in $manifest.fixtures) {
    Add-Row $fixture.source 'fixture' 'formal-fixture' 'TEST-CLASSIFY' 'project-authored test' 'review/move-with-owner' 'S8' ("libraries: " + ($fixture.libraries -join ', '))
}

foreach ($target in $manifest.targets) {
    Add-Row $target.source 'target' 'formal-target' 'app' 'project-authored executable entry' 'git mv' 'S4' ("libraries: " + ($target.libraries -join ', '))
}

foreach ($includeRoot in $manifest.includeRoots) {
    Add-Row $includeRoot 'include-root' 'formal-include-root' 'HEADER-CLASSIFY' 'mixed include-root' 'expand-to-live-headers' 'S1' 'S1 must expand reached headers from this root; directory ownership is not inferred from the root name'
}

foreach ($toolPath in @($ManifestPath, 'tools/build/New-T225S7FullNinjaGraph.ps1')) {
    Add-Row $toolPath 'build-input' 'build-tool' 'tools/build' 'project-authored build tool' 'retain/update-paths' 'S8' 'formal build graph input'
}

$header = 'path', 'kind', 'current_owner', 'target_owner', 'provenance', 'disposition', 'planned_s', 'reason' -join "`t"

$body = $rows |
    Sort-Object Kind, Path |
    ForEach-Object { @($_.Path, $_.Kind, $_.CurrentOwner, $_.TargetOwner, $_.Provenance, $_.Disposition, $_.PlannedS, $_.Reason) -join "`t" }

Set-Content -LiteralPath $outputFullPath -Value (@($header) + $body) -Encoding utf8
Write-Host "Wrote T260 S1 manifest rows: $($rows.Count) to $OutputPath"
