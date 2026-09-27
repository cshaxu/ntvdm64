[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$graph = Get-Content -LiteralPath (Join-Path $BuildRoot 'build.ninja')

function Assert-FrontendOwnership([string[]]$Lines) {
    $edges = @{}
    foreach ($line in $Lines) {
        if ($line -notmatch '^build (.+?): (\S+)\s*(.*)$') { continue }
        $outputs = ($Matches[1] -split '\s+\|\s+')[0] -split '\s+'
        # Implicit/order-only dependencies cause builds, not archive membership
        # or link inputs. Only recursively follow object/archive explicit inputs.
        $inputs = ($Matches[3] -split '\s+\|\|?\s+')[0] -split '\s+'
        foreach ($output in $outputs) { $edges[$output] = $inputs }
    }
    function Get-FrontendSources([string]$Target) {
        if (!$edges.ContainsKey($Target)) { throw "Missing graph target: $Target" }
        $pending = [Collections.Generic.Stack[string]]::new()
        $seen = [Collections.Generic.HashSet[string]]::new()
        $sources = [Collections.Generic.HashSet[string]]::new()
        $pending.Push($Target)
        while ($pending.Count) {
            $node = $pending.Pop()
            if (!$seen.Add($node)) { continue }
            foreach ($input in $edges[$node]) {
                if ($input -match '/src/frontend-exe/([^/]+\.c)$') {
                    [void]$sources.Add($Matches[1])
                } elseif ($input -match '\.(obj|lib)$' -and $edges.ContainsKey($input)) {
                    $pending.Push($input)
                }
            }
        }
        return @($sources | Sort-Object)
    }
    $client = @('bootstrap_client.c', 'native_console_launch.c',
        'native_request_client.c', 'native_request_io.c') | Sort-Object
    foreach ($target in @('frontend-client.lib', 'run16.exe')) {
        $actual = @(Get-FrontendSources $target)
        if (@(Compare-Object $client $actual).Count) {
            throw "$target frontend source ownership mismatch: $($actual -join ', ')"
        }
    }
    $worker = @(Get-FrontendSources 'ntvdm.exe')
    if ($worker.Count) { throw "ntvdm links frontend implementation: $worker" }
    $service = @(Get-FrontendSources 'frontend.exe')
    foreach ($required in @('main.c', 'session_service.c', 'console_channel.c',
            'console_frontend.c', 'console_video.c', 'native_console_host.c',
            'native_console_backend.c', 'native_console_view.c',
            'native_console_capture.c', 'native_console_frontend.c',
            'native_console_request.c') + $client) {
        if ($required -notin $service) { throw "frontend.exe omits $required" }
    }
}

Assert-FrontendOwnership $graph
# Red controls prove both direct and archive-hidden leakage are rejected.
foreach ($target in @('run16.exe', 'frontend-client.lib', 'ntvdm.exe')) {
    $mutated = @($graph | ForEach-Object {
        if ($_ -match ('^build ' + [regex]::Escape($target) + '(?: |:)')) {
            $_ -replace ': (\S+) ', ': $1 obj/run16/console_frontend.obj '
        } else { $_ }
    })
    $rejected = $false
    try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
    if (!$rejected) { throw "Ownership negative control did not reject $target" }
}
Write-Output 'PASS frontend service ownership, client-only launcher, no worker frontend; three leakage controls rejected'
