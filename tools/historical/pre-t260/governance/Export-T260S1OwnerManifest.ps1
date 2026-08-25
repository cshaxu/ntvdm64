[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$ManifestPath = 'tools/build/t260-s8-component-manifest.json',
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
$representedPaths = @{}

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
    $representedPaths[$normalized] = $true
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

# The formal Ninja manifest is deliberately switched only in T260/S8.  During
# the preceding layout packets, report the existing source input at its live
# component path when a content-preserving git move has already happened.  The
# build-graph declaration itself remains an S8 obligation; this only prevents
# the ownership inventory from representing a non-existent transitional file.
function Resolve-T260LivePath([string]$Path) {
    $p = $Path.Replace('\\', '/')
    if (Test-Path -LiteralPath (Join-Path $root $p)) { return $p }

    $prefixes = @(
        @('src/cli/', 'src/app/'),
        @('src/bx-mantle/bx_ntvdm_', 'src/adapter-softpc/bx_ntvdm_'),
        @('src/opennt/base/mvdm/dos/v86/', 'src/opennt-guest/dos-v86/'),
        @('src/opennt/base/mvdm/wow16/', 'src/opennt-guest/wow16/'),
        @('src/bx-vdm/bop/opennt/dem/', 'src/opennt-bop/dem/'),
        @('src/bx-vdm/bop/opennt/command/', 'src/opennt-bop/command/'),
        @('src/bx-vdm/bop/opennt/xms/', 'src/opennt-bop/xms/'),
        @('src/bx-vdm/bop/opennt/dpmi/', 'src/opennt-bop/dpmi/'),
        @('src/bx-vdm/bop/opennt/softpc/', 'src/opennt-bop/softpc/'),
        @('src/opennt/base/mvdm/dos/dem/', 'src/opennt-bop/dem/'),
        @('src/opennt/base/mvdm/dos/command/', 'src/opennt-bop/command/'),
        @('src/opennt/base/mvdm/xms.486/', 'src/opennt-bop/xms/'),
        @('src/opennt/base/mvdm/dpmi32/', 'docs/etc/legacy_code/opennt-bop/original/dpmi32/'),
        @('src/opennt/base/mvdm/dpmi/', 'docs/etc/legacy_code/opennt-bop/original/dpmi/')
    )
    foreach ($prefix in $prefixes) {
        if (-not $p.StartsWith($prefix[0], [System.StringComparison]::OrdinalIgnoreCase)) { continue }
        $candidate = $prefix[1] + $p.Substring($prefix[0].Length)
        if (Test-Path -LiteralPath (Join-Path $root $candidate)) { return $candidate }
    }

    if ($p -like 'src/bx-vdm/bop/*' -and
        $p -notlike 'src/bx-vdm/bop/shim/*' -and
        $p -notlike 'src/bx-vdm/bop/observation/*') {
        $candidate = 'src/opennt-bop/ingress/' + [System.IO.Path]::GetFileName($p)
        if (Test-Path -LiteralPath (Join-Path $root $candidate)) { return $candidate }
    }
    if ($p -like 'src/bx-vdm/bx_ntvdm_bios_selector_map*') {
        $candidate = 'src/opennt-bop/ingress/' + [System.IO.Path]::GetFileName($p)
        if (Test-Path -LiteralPath (Join-Path $root $candidate)) { return $candidate }
    }
    if ($p -like 'src/bx-vdm/bx_ntvdm_*') {
        $name = [System.IO.Path]::GetFileName($p)
        $startup = $name -match '^bx_ntvdm_(initial_state|preentry|ntio_preentry|startup_)'
        if ($startup) { $candidate = 'src/app/' + $name }
        else { $candidate = 'src/adapter-softpc/' + $name }
        if (Test-Path -LiteralPath (Join-Path $root $candidate)) { return $candidate }
    }
    return $p
}

function Get-SourceDisposition([string]$Path, [string]$CurrentModule) {
    $p = $Path.Replace('\', '/')
    if ($p -like 'src/app/*') { return @('app', 'project-authored app component record', 'retain', 'S2', 'target component root record') }
    if ($p -like 'src/adapter-softpc/*') { return @('adapter-softpc', 'project-authored SoftPC adapter record', 'retain', 'S2', 'target component root record') }
    if ($p -like 'src/adapter-win32/*') { return @('adapter-win32', 'project-authored Win32 adapter record', 'retain', 'S2', 'target component root record') }
    if ($p -like 'src/opennt-bop/command/*' -or $p -like 'src/opennt-bop/dem/*' -or $p -like 'src/opennt-bop/xms/*' -or $p -like 'src/opennt-bop/dpmi/*' -or $p -like 'src/opennt-bop/softpc/*') { return @('opennt-bop', 'minimal-change OpenNT BOP production source', 'retain', 'S8', 'original owner-family production source; local divergence register required') }
    if ($p -like 'src/opennt-bop/ingress/*') { return @('opennt-bop', 'project-authored BOP ingress/route', 'retain', 'S5', 'selector/service routing and provider composition') }
    if ($p -like 'src/opennt-bop/*') { return @('opennt-bop', 'OpenNT BOP component record', 'retain', 'S2', 'target component root record') }
    if ($p -like 'src/opennt-guest/*') { return @('opennt-guest', 'OpenNT guest component record', 'retain', 'S2', 'target component root record') }
    if ($p -like 'src/opennt-host/*') { return @('opennt-host', 'OpenNT host component record', 'retain', 'S2', 'target component root record') }
    if ($p -like 'src/bx-core/*') {
        return @('bx-core', 'adopted Bochs 2.6', 'retain', 'S3', 'adopted core; README exception register required')
    }
    if ($p -like 'src/cli/*') {
        return @('app', 'project-authored CLI/composition', 'git mv', 'S4', 'CLI and final component composition')
    }
    if ($p -like 'src/opennt/local/compat/host/*') {
        return @('opennt-host', 'OpenNT host capability/source-derived host component', 'git mv', 'S6', 'host capability owner')
    }
    if ($p -like 'src/opennt/base/mvdm/dos/v86/*' -or $p -like 'src/opennt/base/mvdm/wow16/*') {
        return @('opennt-guest', 'imported OpenNT guest source', 'git mv', 'S4', 'guest DOS/WOW source or build input')
    }
    if ($p -like 'src/opennt/base/mvdm/dos/dem/*' -or $p -like 'src/opennt/base/mvdm/dos/command/*' -or $p -like 'src/opennt/base/mvdm/xms.486/*' -or $p -like 'src/opennt/base/mvdm/dpmi*/*') {
        return @('opennt-bop', 'imported OpenNT BOP/provider mirror', 'git mv', 'S5', 'host BOP dispatcher/provider source; never guest image input')
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

function Get-HeaderDisposition([string]$Path) {
    $d = Get-SourceDisposition $Path ''
    if ($d[0] -ne 'UNCLASSIFIED') { return $d }
    $p = $Path.Replace('\', '/')
    if ($p -like 'src/opennt/base/mvdm/softpc.new/*') {
        return @('adapter-softpc', 'imported SoftPC/CCPU header', 'git mv', 'S7', 'same-shaped SoftPC/CCPU interface input')
    }
    if ($p -like 'src/opennt/base/mvdm/inc/*') {
        $name = [System.IO.Path]::GetFileName($p).ToLowerInvariant()
        if ($name -match '^(bop|cmd|command|dem|xms|dpmi|rdr|dbg)') {
            return @('opennt-bop', 'imported OpenNT BOP header', 'git mv', 'S5', 'BOP/provider interface input')
        }
        return @('opennt-host', 'imported OpenNT host ABI header', 'review/split', 'S6', 'host ABI default; S6 resolves shared declarations')
    }
    if ($p -like 'src/opennt/public/internal/windows/inc/*') {
        return @('adapter-win32', 'imported historical Win32/NTDLL header', 'review/split', 'S6', 'historical API facade declaration')
    }
    return @('HEADER-REVIEW', 'unknown reached header', 'review', 'S1', 'manual unique owner decision required')
}

function Get-FixtureDisposition($Fixture) {
    $p = $Fixture.source.Replace('\', '/').ToLowerInvariant()
    $name = [System.IO.Path]::GetFileName($p)
    if ($p -like 'tests/runner/*') {
        return @('app', 'project-authored app fixture', 'git mv', 'S8', 'CLI/final-composition fixture')
    }
    if ($p -like 'tests/opennt/*') {
        return @('opennt-host', 'OpenNT host component fixture', 'git mv', 'S8', 'host-capability provider fixture')
    }
    if ($p -like 'tests/bx-mantle/*') {
        if ($name -match 'native_pic|mantle_watchdog|budget_terminal|large_reverse|software_interrupt|interrupt_return|segment_access|headless_8042') {
            return @('bx-mantle', 'Bochs-only mantle fixture', 'git mv', 'S8', 'tests pure mantle/core mechanics')
        }
        return @('adapter-softpc', 'SoftPC/CCPU adapter fixture', 'git mv', 'S8', 'tests a VDM-facing or legacy-machine contract')
    }
    if ($p -like 'tests/bx-vdm/*') {
        if ($name -match 'guest-pointer|host-handle|xmem|dpmi|softpc|machine_pending|engine_contract|physical|headless_mouse|printer') {
            return @('adapter-softpc', 'SoftPC/CCPU adapter fixture', 'git mv', 'S8', 'tests same-shaped machine or mapping contract')
        }
        return @('opennt-bop', 'OpenNT BOP fixture', 'git mv', 'S8', 'tests BOP ingress, provider mirror, or owner package route')
    }
    return @('TEST-REVIEW', 'unknown formal fixture', 'review', 'S1', 'manual unique owner decision required')
}

$seedPaths = [System.Collections.Generic.List[string]]::new()

foreach ($module in $manifest.modules) {
    foreach ($source in @($module.sources)) {
        $liveSource = Resolve-T260LivePath $source
        $d = Get-SourceDisposition $liveSource $module.name
        $reason = $d[4]
        if ($liveSource -ne $source) {
            $reason += '; formal Ninja declaration remains at ' + $source.Replace('\\', '/') + ' until S8'
        }
        Add-Row $liveSource 'source' $module.name $d[0] $d[1] $d[2] $d[3] $reason
        $seedPaths.Add($liveSource)
    }
}

foreach ($fixture in $manifest.fixtures) {
    $d = Get-FixtureDisposition $fixture
    Add-Row $fixture.source 'fixture' 'formal-fixture' $d[0] $d[1] $d[2] $d[3] ($d[4] + '; libraries: ' + ($fixture.libraries -join ', '))
    $seedPaths.Add($fixture.source)
}

foreach ($target in $manifest.targets) {
    Add-Row $target.source 'target' 'formal-target' 'app' 'project-authored executable entry' 'git mv' 'S4' ("libraries: " + ($target.libraries -join ', '))
    $seedPaths.Add($target.source)
}

foreach ($includeRoot in $manifest.includeRoots) {
    Add-Row $includeRoot 'include-root' 'formal-include-root' 'tools/build' 'formal build include-root' 'retain/update-paths' 'S8' 'reached headers are separately enumerated; this row is a build-graph input, not a source-owner classification'
}

foreach ($toolPath in @($ManifestPath, 'tools/build/New-T260S8FullNinjaGraph.ps1')) {
    Add-Row $toolPath 'build-input' 'build-tool' 'tools/build' 'project-authored build tool' 'retain/update-paths' 'S8' 'formal build graph input'
}

foreach ($guestRoot in @('src/opennt-guest/dos-v86', 'src/opennt-guest/wow16')) {
    $guestFullPath = Join-Path $root $guestRoot
    if (-not (Test-Path -LiteralPath $guestFullPath)) { continue }
    Get-ChildItem -LiteralPath $guestFullPath -Recurse -File | ForEach-Object {
        $relativePath = $_.FullName.Substring($root.Length).TrimStart('\').Replace('\', '/')
        Add-Row $relativePath 'guest-source-candidate' 'OpenNT guest import' 'opennt-guest' 'imported OpenNT guest source/input' 'git mv or retain-as-guest-artifact-input' 'S4' 'S4 identifies source-built closure membership versus retained source evidence'
    }
}

$includeDirectories = @($manifest.includeRoots | ForEach-Object { Join-Path $root $_ })
$pending = [System.Collections.Generic.Queue[string]]::new()
$queued = @{}
foreach ($seedPath in $seedPaths) {
    if (-not $queued.ContainsKey($seedPath)) { $pending.Enqueue($seedPath); $queued[$seedPath] = $true }
}
while ($pending.Count -gt 0) {
    $includingPath = $pending.Dequeue()
    $includingFullPath = Join-Path $root $includingPath
    if (-not (Test-Path -LiteralPath $includingFullPath -PathType Leaf)) { continue }
    foreach ($line in Get-Content -LiteralPath $includingFullPath) {
        if ($line -notmatch '^\s*#\s*include\s*[<"]([^>"]+)[>"]') { continue }
        $includeName = $Matches[1]
        $candidates = [System.Collections.Generic.List[string]]::new()
        $candidates.Add((Join-Path (Split-Path -Parent $includingFullPath) $includeName))
        foreach ($includeDirectory in $includeDirectories) { $candidates.Add((Join-Path $includeDirectory $includeName)) }
        $resolved = $candidates | Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } | Select-Object -First 1
        if ($null -eq $resolved) { continue }
        $relativePath = $resolved.Substring($root.Length).TrimStart('\').Replace('\', '/')
        $d = Get-HeaderDisposition $relativePath
        Add-Row $relativePath 'header' 'reached-include' $d[0] $d[1] $d[2] $d[3] $d[4]
        if (-not $queued.ContainsKey($relativePath)) { $pending.Enqueue($relativePath); $queued[$relativePath] = $true }
    }
}

foreach ($legacyRoot in @('src/bx-core', 'src/bx-mantle', 'src/bx-vdm', 'src/cli', 'src/opennt', 'src/app', 'src/adapter-softpc', 'src/adapter-win32', 'src/opennt-bop', 'src/opennt-guest', 'src/opennt-host')) {
    $legacyFullPath = Join-Path $root $legacyRoot
    if (-not (Test-Path -LiteralPath $legacyFullPath)) { continue }
    Get-ChildItem -LiteralPath $legacyFullPath -Recurse -File | ForEach-Object {
        $relativePath = $_.FullName.Substring($root.Length).TrimStart('\').Replace('\', '/')
        if ($representedPaths.ContainsKey($relativePath)) { return }
        $d = Get-SourceDisposition $relativePath ''
        if ($relativePath -like 'src/opennt/*' -and $d[0] -ne 'UNCLASSIFIED') {
            Add-Row $relativePath 'nonformal-legacy-input' 'OpenNT source import' $d[0] $d[1] $d[2] $d[3] ($d[4] + '; not present in formal module graph')
            return
        }
        if ($relativePath -like 'src/opennt/*') {
            Add-Row $relativePath 'nonformal-evidence-input' 'OpenNT source import' 'docs/etc/legacy_code' 'unreached imported OpenNT source' 'move to legacy evidence input' 'S8' 'not a declared product input; retain immutable source provenance outside src/'
            return
        }
        Add-Row $relativePath 'nonformal-legacy-input' 'legacy source tree' $d[0] $d[1] $d[2] $d[3] ($d[4] + '; not present in formal module graph')
    }
}

$header = 'path', 'kind', 'current_owner', 'target_owner', 'provenance', 'disposition', 'planned_s', 'reason' -join "`t"

$body = $rows |
    Sort-Object Kind, Path |
    ForEach-Object { @($_.Path, $_.Kind, $_.CurrentOwner, $_.TargetOwner, $_.Provenance, $_.Disposition, $_.PlannedS, $_.Reason) -join "`t" }

Set-Content -LiteralPath $outputFullPath -Value (@($header) + $body) -Encoding utf8
Write-Host "Wrote T260 S1 manifest rows: $($rows.Count) to $OutputPath"
