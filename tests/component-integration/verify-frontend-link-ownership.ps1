[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$graph = Get-Content -LiteralPath (Join-Path $BuildRoot 'build.ninja')
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
# Presentation attachment is live; execution submission to a frontend is not.
# A stale RPC method must not survive merely because no EXE calls it today.
foreach ($path in @('src/interface/service.idl',
        'src/ntsrv-exe/opennt/source/base_rpc_client.c',
        'src/ntsrv-exe/opennt/source/base_service.c',
        'src/ntkvm-exe/native_request_client.c')) {
    $source = Get-Content -LiteralPath (Join-Path $repo $path) -Raw
    if ($source -match 'SubmitFrontendChannel|TakeFrontendChannel|run16_native_request_submit') {
        throw "Retired frontend execution entry remains: $path"
    }
}

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
    function Get-FrontendSources([string]$Target,[string]$Owner='ntkvm-exe') {
        if (!$edges.ContainsKey($Target)) { throw "Missing graph target: $Target" }
        $pending = [Collections.Generic.Stack[string]]::new()
        $seen = [Collections.Generic.HashSet[string]]::new()
        $sources = [Collections.Generic.HashSet[string]]::new()
        $pending.Push($Target)
        while ($pending.Count) {
            $node = $pending.Pop()
            if (!$seen.Add($node)) { continue }
            foreach ($input in $edges[$node]) {
                # A retired archive may exist in an old incremental cache
                # even though it no longer has a source edge in this graph.
                if ($input -in @('frontend-terminal.lib','ntkvm-worker-client.lib')) {
                    throw 'Retired frontend archive re-entered a production link'
                }
                if ($input -match ('/src/'+[regex]::Escape($Owner)+'/(.+\.c)$')) {
                    [void]$sources.Add($Matches[1])
                } elseif ($input -match '\.(obj|lib)$' -and $edges.ContainsKey($input)) {
                    $pending.Push($input)
                }
            }
        }
        return @($sources | Sort-Object)
    }
    $client = @('bootstrap_client.c',
        'native_request_client.c', 'native_request_io.c') | Sort-Object
    foreach ($target in @('frontend-client.lib', 'run16.exe')) {
        $actual = @(Get-FrontendSources $target)
        if (@(Compare-Object $client $actual).Count) {
            throw "$target frontend source ownership mismatch: $($actual -join ', ')"
        }
    }
    # Shared worker clients now belong to worker-base, never NTKVM-private code.
    $worker = @(Get-FrontendSources 'ntvdm.exe')
    if ($worker.Count) {
        throw "ntvdm frontend client-only boundary mismatch: $worker"
    }
    $native = @(Get-FrontendSources 'ntcon.exe')
    if ($native.Count) {
        throw "ntcon frontend client-only boundary mismatch: $native"
    }
    foreach($target in @('frontend-client.lib','ntkvm.exe')) {
        $actual=@(Get-FrontendSources $target 'ntcon-exe')
        if(@(Compare-Object @('launch_packet.c') $actual).Count) {
            throw "$target must contain only the NTCON packet codec, not target creation"
        }
    }
    $launcherNative=@(Get-FrontendSources 'run16.exe' 'ntcon-exe')
    if(@(Compare-Object @('launch.c','launch_packet.c') $launcherNative).Count) {
        throw 'run16 must retain bounded GUI creation, not NTCON execution/presentation'
    }
    $shared = @(Get-FrontendSources 'worker-base.lib')
    if ($shared.Count) {
        throw 'Worker protocol library contains a non-client implementation'
    }
    foreach($target in @('worker-base.lib','ntvdm.exe','ntcon.exe')) {
        $actual=@(Get-FrontendSources $target 'worker-base')
        if(@(Compare-Object @('connection.c','console_client.c') $actual).Count) {
            throw "$target does not use the complete common worker implementation"
        }
    }
    $ntvdmNative=@(Get-FrontendSources 'ntvdm.exe' 'ntcon-exe')
    $ntconNative=@(Get-FrontendSources 'ntcon.exe' 'ntcon-exe')
    if('next_command.c' -in $ntvdmNative -or 'next_command.c' -notin $ntconNative) {
        throw 'NTCON-only native GetNext wrapper must not enter NTVDM or worker-base'
    }
    foreach($target in @('run16.exe','ntsrv.exe','ntkvm.exe','ntmon.exe')) {
        if(@(Get-FrontendSources $target 'worker-base').Count) {
            throw "$target must not acquire worker-side mechanisms"
        }
    }
    $service = @(Get-FrontendSources 'ntkvm.exe')
    if ('native_console_host.c' -in $service) { throw 'Retired helper entered ntkvm.exe' }
    if ('native_console_request.c' -in $service) { throw 'Native execution receiver entered presentation-only ntkvm.exe' }
    foreach ($backend in @('native_conpty.c','native_terminal.c','native_console_backend.c',
            'native_console_view.c','native_console_capture.c','native_console_launch.c')) {
        if ($backend -in $service) { throw "Native backend entered presentation-only ntkvm.exe: $backend" }
    }
    foreach ($required in @('main.c', 'session_service.c', 'console_channel.c',
            'console_frontend.c', 'console_video.c', 'native_console_frontend.c',
            'window_controller.c', 'window_frame.c',
            'window_keyboard.c', 'window_input_queue.c',
            'lib/kvm-window/win32/component.c') + $client) {
        if ($required -notin $service) { throw "ntkvm.exe omits $required" }
    }
}

Assert-FrontendOwnership $graph
# The packet codec is shared with callers; native target creation is not.
foreach($target in @('frontend-client.lib','ntkvm.exe')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($target)+'(?: |:)')) {
            $_ -replace ': (\S+) ', ': $1 obj/ntcon/launch.obj '
        } else { $_ }
    })
    $rejected=$false
    try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
    if(!$rejected){throw "Native target creation leakage accepted: $target"}
}
# Replacing the runtime-order edge with a rebuild dependency wastes relinks.
$mutated = @($graph | ForEach-Object {
    if ($_ -match '^build run16\.exe:') { $_ -replace '\|\|', '|' } else { $_ }
})
$rejected = $false
try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
if (!$rejected) { throw 'Launcher rebuild dependency negative control was accepted' }
# Red controls prove both direct and archive-hidden leakage are rejected.
foreach ($target in @('run16.exe', 'frontend-client.lib', 'ntvdm.exe','worker-base.lib')) {
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
foreach ($leakInput in @('frontend-window.lib', 'frontend-terminal.lib', 'ntkvm-worker-client.lib', 'obj/ownership-leak.obj')) {
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
$mutated = @($graph | ForEach-Object {
    if ($_ -match '^build ntkvm\.exe(?: |:)') {
        $_ -replace ': (\S+) ', ': $1 obj/run16/native_console_request.obj '
    } else { $_ }
})
$mutated += 'build obj/run16/native_console_request.obj: cc O$:/repo/src/ntkvm-exe/native_console_request.c'
$rejected = $false
try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
if (!$rejected) { throw 'Native execution receiver negative control was accepted' }
foreach ($backend in @('native_conpty.c','native_terminal.c','native_console_backend.c',
        'native_console_view.c','native_console_capture.c','native_console_launch.c')) {
    $mutated = @($graph | ForEach-Object {
        if ($_ -match '^build ntkvm\.exe(?: |:)') {
            $_ -replace ': (\S+) ', ': $1 obj/retired-backend.obj '
        } else { $_ }
    })
    $mutated += "build obj/retired-backend.obj: cc O`$:/repo/src/ntkvm-exe/$backend"
    $rejected=$false
    try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
    if (!$rejected) { throw "Native backend leakage control accepted $backend" }
}
Write-Output 'PASS frontend link ownership: no native execution/backend/helper, client-only launcher, worker protocol client only; leakage controls rejected. Physical retired-source cleanup is a separate gate.'
