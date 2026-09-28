[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$graph = Get-Content -LiteralPath (Join-Path $BuildRoot 'build.ninja')

function Assert-FrontendOwnership([string[]]$Lines) {
    $launcherEdge = @($Lines | Where-Object { $_ -match '^build run16\.exe:' })
    if ($launcherEdge.Count -ne 1 -or
        $launcherEdge[0] -notmatch '\|\|\s+ntkvm\.exe\s*$' -or
        ($launcherEdge[0] -split '\s+\|\|\s+')[0] -match '\bntkvm\.exe\b') {
        throw 'ntkvm.exe must be only an order-only launcher runtime prerequisite'
    }
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
                if ($input -match '/src/ntkvm-exe/(.+\.c)$') {
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
    $service = @(Get-FrontendSources 'ntkvm.exe')
    if ('native_console_host.c' -in $service) { throw 'Retired helper entered ntkvm.exe' }
    foreach ($required in @('main.c', 'session_service.c', 'console_channel.c',
            'console_frontend.c', 'console_video.c', 'native_conpty.c', 'native_terminal.c',
            'native_console_backend.c', 'native_console_view.c',
            'native_console_capture.c', 'native_console_frontend.c',
            'native_console_request.c', 'window_controller.c', 'window_frame.c',
            'window_keyboard.c', 'window_input_queue.c',
            'lib/kvm-window/win32/component.c') + $client) {
        if ($required -notin $service) { throw "ntkvm.exe omits $required" }
    }
}

Assert-FrontendOwnership $graph
# Replacing the runtime-order edge with a rebuild dependency wastes relinks.
$mutated = @($graph | ForEach-Object {
    if ($_ -match '^build run16\.exe:') { $_ -replace '\|\|', '|' } else { $_ }
})
$rejected = $false
try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
if (!$rejected) { throw 'Launcher rebuild dependency negative control was accepted' }
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
# Window libraries contain nested source paths, not only owner-root C files.
# Reject both full-archive leakage and a library-only leaf object leakage.
foreach ($leakInput in @('frontend-window.lib', 'frontend-terminal.lib', 'obj/ownership-leak.obj')) {
    $mutated = @($graph | ForEach-Object {
        if ($_ -match '^build ntvdm\.exe(?: |:)') {
            $_ -replace ': (\S+) ', (': $1 ' + $leakInput + ' ')
        } else { $_ }
    })
    $mutated += 'build obj/ownership-leak.obj: cc O$:/repo/src/ntkvm-exe/lib/kvm-window/win32/component.c'
    $rejected = $false
    try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
    if (!$rejected) { throw "Window ownership negative control did not reject $leakInput" }
}
$mutated = @($graph | ForEach-Object {
    if ($_ -match '^build ntkvm\.exe(?: |:)') {
        $_ -replace ': (\S+) ', ': $1 obj/retired-helper.obj '
    } else { $_ }
})
$mutated += 'build obj/retired-helper.obj: cc O$:/repo/src/ntkvm-exe/native_console_host.c'
$rejected = $false
try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
if (!$rejected) { throw 'Retired helper negative control was accepted' }
Write-Output 'PASS frontend ConPTY/Window ownership, client-only launcher, no worker frontend; seven leakage controls and launcher rebuild control rejected'
