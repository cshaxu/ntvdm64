param(
    [string]$OperationsDirectory = (Join-Path $PSScriptRoot '..\..\docs\etc\operations')
)

$ErrorActionPreference = 'Stop'

function Write-Tsv([object[]]$Rows, [string]$Path, [string[]]$Columns) {
    $Rows | Select-Object $Columns |
        Export-Csv -LiteralPath $Path -Delimiter "`t" -NoTypeInformation -Encoding utf8
}

$effectivePath = Join-Path $OperationsDirectory 'm0-t277-effective-source-correlation-ledger.tsv'
$profileInputPath = Join-Path $OperationsDirectory 'm0-t277-build-profile-ledger.tsv'
$edgesPath = Join-Path $OperationsDirectory 'm0-t277-effective-package-boundary-edge-ledger.tsv'
$cyclesPath = Join-Path $OperationsDirectory 'm0-t277-effective-package-cycle-ledger.tsv'
$profilesPath = Join-Path $OperationsDirectory 'm0-t277-effective-build-profile-ledger.tsv'

$effective = Import-Csv -LiteralPath $effectivePath -Delimiter "`t"
$reviewed = @($effective | Where-Object { $_.effective_state -eq 'reviewed-future-host' })
if ($reviewed.Count -ne 325) { throw "Expected 325 reviewed future-host calls; found $($reviewed.Count)." }

$edges = @()
$number = 0
foreach ($group in ($reviewed | Group-Object caller_package, family, owner | Sort-Object Name)) {
    $first = $group.Group | Select-Object -First 1
    $number++
    $edges += [pscustomobject]@{
        edge_id = ('MVDM-T277-S8-EDGE-{0:d3}' -f $number)
        from_package = $first.caller_package
        source_family = $first.family
        owner_boundary = $first.owner
        reviewed_call_count = $group.Count
        evidence = (($group.Group | Select-Object -First 3 | ForEach-Object { $_.evidence }) -join '; ')
        admission_rule = 'source-form-reviewed only; no direct build edge inferred'
    }
}
Write-Tsv $edges $edgesPath @('edge_id','from_package','source_family','owner_boundary','reviewed_call_count','evidence','admission_rule')

$cycles = @([pscustomobject]@{
    cycle_id = 'MVDM-T277-S8-CYCLE-001'
    source_packages = 'dos <-> softpc.new'
    source_evidence = 'softpc.new/host/src/nt_msscs.c calls dos/command/CMDInit; DOS source-form rows retain old SoftPC interface calls'
    classification = 'original source composition cycle; not a static-library link cycle'
    required_resolution = 'co-link the reached DOS/DEM/COMMAND and SoftPC host-control units within one mvdm-host composition unit, or introduce a one-way source-shaped facade; never create cyclic static libraries'
    machine_rule = 'adapter-softpc -> adapter-bochs is the sole machine route; no CCPU executor is admitted'
})
Write-Tsv $cycles $cyclesPath @('cycle_id','source_packages','source_evidence','classification','required_resolution','machine_rule')

$profilePolicy = @{
    'bde'        = @('tool-only; separately recoverable debugger extension','tool-only; separately recoverable debugger extension')
    'dbg'        = @('tool-only; separately recoverable debugger product','tool-only; separately recoverable debugger product')
    'dirs'       = @('not-host-runtime; original build control only','not-host-runtime; original build control only')
    'dos'        = @('candidate W2 host-control/DEM and W3 COMMAND; co-link cycle explicit','candidate W2 host-control/DEM and W3 COMMAND; co-link cycle explicit')
    'dpmi'       = @('guest-only; not a host-library candidate','guest-only; not a host-library candidate')
    'dpmi32'     = @('candidate W6; protected-machine interfaces require complete profile','candidate W6; protected-machine interfaces require complete profile')
    'fax'        = @('candidate W8/W9 extension after WOW/monitor evidence','candidate W8/W9 extension after WOW/monitor evidence')
    'ieuvddex'   = @('tool-only; monitor-extension product excluded from runtime','tool-only; monitor-extension product excluded from runtime')
    'inc'        = @('declaration-only; source-shaped headers','declaration-only; source-shaped headers')
    'makefil0'   = @('not-host-runtime; original build control only','not-host-runtime; original build control only')
    'oemuni'     = @('candidate W1 original support package','candidate W1 original support package')
    'sim16'      = @('declaration-only W5 monitor interface evidence; no monitor provider enabled','declaration-only W5 monitor interface evidence; no monitor provider enabled')
    'sim32'      = @('declaration-only W5 monitor interface evidence; no monitor provider enabled','declaration-only W5 monitor interface evidence; no monitor provider enabled')
    'softpc.new' = @('candidate W2 host-control; CPU/device executor replaced by Bochs route','candidate W2 host-control; CPU/device executor replaced by Bochs route')
    'suballoc'   = @('candidate W1 original support package','candidate W1 original support package')
    'v86'        = @('not-host-runtime; historical monitor product excluded','not-host-runtime; historical monitor product excluded')
    'vdd'        = @('not-host-runtime; VDD product not admitted','not-host-runtime; VDD product not admitted')
    'vdmdbg'     = @('tool-only; debugger extension','tool-only; debugger extension')
    'vdmexts'    = @('tool-only; debugger extension','tool-only; debugger extension')
    'vdmredir'   = @('candidate W7; original protocol package after broker boundary closes','candidate W7; original protocol package after broker boundary closes')
    'vdmutils'   = @('tool-only; independently buildable historical tools','tool-only; independently buildable historical tools')
    'wow32'      = @('candidate W8; original WOW package after monitor/Redirector closure','candidate W8; original WOW package after monitor/Redirector closure')
    'xms.486'    = @('candidate W4; raw-pointer branches require mapping lease overlay','candidate W4; raw-pointer branches require mapping lease overlay')
}

$inputProfiles = @(Import-Csv -LiteralPath $profileInputPath -Delimiter "`t")
if ($inputProfiles.Count -ne 23) { throw "Expected 23 original package profiles; found $($inputProfiles.Count)." }
$profiles = foreach ($input in $inputProfiles) {
    if (-not $profilePolicy.ContainsKey($input.package_root)) { throw "No S8 profile policy for $($input.package_root)." }
    $policy = $profilePolicy[$input.package_root]
    [pscustomobject]@{
        profile_id = ('MVDM-T277-S8-PROFILE-' + $input.package_root.ToUpper().Replace('.','-'))
        package_root = $input.package_root
        x86_admission_state = ('not-enabled; ' + $policy[0])
        x64_admission_state = ('not-enabled; ' + $policy[1])
        build_claim = 'no compile, link or provider enablement claimed by T277'
        source_basis = 'T276 final file disposition + T277 source-form interface closure'
    }
}
Write-Tsv $profiles $profilesPath @('profile_id','package_root','x86_admission_state','x64_admission_state','build_claim','source_basis')

Write-Host "Wrote $($edges.Count) package-boundary edges, $($cycles.Count) source cycle and $($profiles.Count) profile states."
