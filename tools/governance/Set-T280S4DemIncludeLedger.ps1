[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$demRoot = Join-Path $root 'src/mvdm-host/dos/dem'
$target = Join-Path $root 'docs/etc/operations/m0-t280-s4-dem-include-abi-ledger.tsv'
$sourceNames = @('dem.c','demfcb.c','demdata.c','demdir.c','demdisp.c','demerror.c','demfile.c','demgset.c','demhndl.c','demioctl.c','demlock.c','demmisc.c','demmsg.c','demsrch.c','demdasd.c','demlabel.c')

function Get-Disposition([string]$Header) {
    switch -Regex ($Header.ToLowerInvariant()) {
        '^(dem\.h|demmsg\.h|demdasd\.h)$' { return @('local-mirror', 'mvdm-host/dos/dem', 'byte-exact original DEM header') }
        '^(softpc\.h|mvdm\.h|vrnmpipe\.h|exterr\.h|dbgsvc\.h)$' { return @('support-mirror', 'mvdm-support/inc', 'byte-exact selected MVDM support header') }
        '^nt_vdd\.h$' { return @('platform-abi-mirror', 'mvdm-platform-abi', 'byte-exact public SDK declaration mirror') }
        '^(windows\.h|winbase\.h|stdio\.h|string\.h|memory\.h|io\.h|fcntl\.h)$' { return @('public-sdk-or-crt', 'modern SDK/CRT', 'public compiler/platform header; no project replacement') }
        '^winbasep\.h$' { return @('adapter-gap', 'adapter-win32', 'include-only historical private Base header; no DEM-reached declaration, so only a behavior-free same-name include carrier is permitted') }
        '^vdm\.h$' { return @('adapter-gap', 'adapter-win32 -> session', 'DEM reaches VDMQUERYDIRINFO, VdmQueryDir and NtVdmControl; recover only their same-shaped declaration and scoped session-backed query behavior') }
        default { throw "Unclassified DEM direct include: $Header" }
    }
}

$rows = foreach ($name in $sourceNames) {
    $path = Join-Path $demRoot $name
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing original DEM source: $name" }
    $lineNumber = 0
    foreach ($line in Get-Content -LiteralPath $path) {
        $lineNumber++
        if ($line -match '^\s*#include\s*[<"]([^>"]+)[>"]') {
            $header = $Matches[1]
            $result = Get-Disposition $header
            [pscustomobject]@{
                dem_source = "dos/dem/$name"
                source_line = $lineNumber
                direct_include = $header
                disposition = $result[0]
                final_owner = $result[1]
                source_provenance_or_action = $result[2]
                admission_state = 'S4-audited; no DEM body enabled'
            }
        }
    }
}
if (@($rows | Select-Object -ExpandProperty dem_source -Unique).Count -ne 16) { throw 'DEM ledger does not cover exactly 16 sources.' }
$rows | Sort-Object dem_source,source_line | Export-Csv -LiteralPath $target -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote T280 S4 DEM include/ABI ledger: $($rows.Count) direct includes across 16 original bodies."
