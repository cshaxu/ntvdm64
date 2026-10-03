[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$graph = Get-Content -LiteralPath (Join-Path $BuildRoot 'build.ninja')
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
# Presentation attachment is live; execution submission to a frontend is not.
# A stale RPC method must not survive merely because no EXE calls it today.
foreach ($path in @('src/common/protocol/service.idl',
        'src/ntsrv-exe/opennt/source/base_rpc_client.c',
        'src/ntsrv-exe/opennt/source/base_service.c',
        'src/ntsrv-exe/opennt/source/service_core.c',
        'src/ntsrv-exe/opennt/source/worker_registry.c',
        'src/ntsrv-exe/opennt/source/frontend_registry.c',
        'src/ntsrv-exe/opennt/source/native_commands.c',
        'src/ntsrv-exe/opennt/source/lifecycle.c',
        'src/ntsrv-exe/opennt/source/management.c',
        'src/run16-exe/native_request_client.c')) {
    $source = Get-Content -LiteralPath (Join-Path $repo $path) -Raw
    if ($source -match 'SubmitFrontendChannel|TakeFrontendChannel') {
        throw "Retired frontend execution entry remains: $path"
    }
}
# The surviving run16 submit wrapper calls NTSRV, not a frontend receiver.
foreach($path in @('src/common/protocol/frontend_protocol.h',
        'src/ntsrv-exe/opennt/source/base_service.c',
        'src/ntsrv-exe/opennt/source/service_core.c',
        'src/ntsrv-exe/opennt/source/worker_registry.c',
        'src/ntsrv-exe/opennt/source/frontend_registry.c',
        'src/ntsrv-exe/opennt/source/native_commands.c',
        'src/ntsrv-exe/opennt/source/lifecycle.c',
        'src/ntsrv-exe/opennt/source/management.c',
        'src/ntvwm-exe/execution.c')) {
    $source=Get-Content -LiteralPath (Join-Path $repo $path) -Raw
    if($source -match 'native_request_completion|broker_native_completion_status|QueueNativeChannel|TakeWorkerChannel|native_request_header|native_request_reply|frontend_bootstrap_reply') {
        throw "Retired native/bootstrap control pipe protocol or test seam remains: $path"
    }
}
$requestClient = Get-Content -LiteralPath (Join-Path $repo 'src/run16-exe/native_request_client.c') -Raw
if ($requestClient -notmatch 'OpenNtBaseClientSubmitNativeRequest' -or
    $requestClient -match 'CreateNamedPipe|CreateFile|native_request_transfer|WriteFile|ReadFile') {
    throw 'Launcher native submission must remain a service client, not direct I/O'
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
    function Get-FrontendSources([string]$Target,[string]$Owner='ntkvm-exe',[string]$Root='src') {
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
                if ($input -match ('/'+[regex]::Escape($Root)+'/'+[regex]::Escape($Owner)+'/(.+\.c)$')) {
                    [void]$sources.Add($Matches[1])
                } elseif ($input -match '\.(obj|lib)$' -and $edges.ContainsKey($input)) {
                    $pending.Push($input)
                }
            }
        }
        return @($sources | Sort-Object)
    }
    foreach($target in @('run16.exe','ntsrv.exe','ntvdm.exe','ntvwm.exe','ntkvm.exe','ntmon.exe')) {
        if(@(Get-FrontendSources $target 'adapter-basesrv' 'tests').Count) {
            throw "$target includes service test-only translation/hooks"
        }
    }
    $fixtureSources=@(Get-FrontendSources 'basesrv-service-reservation-test.exe' 'adapter-basesrv' 'tests')
    if(@(Compare-Object @('base_service_reservation_test.c','base_service_fixture.c') $fixtureSources).Count) {
        throw 'Service fixture must select its private hooks and reservation test'
    }
    $client = @('bootstrap_client.c',
        'native_request_client.c') | Sort-Object
    foreach ($target in @('frontend-client.lib', 'run16.exe')) {
        if (@(Get-FrontendSources $target).Count) {
            throw "$target must not depend on NTKVM-private implementation"
        }
        $actual = @(Get-FrontendSources $target 'run16-exe' |
            Where-Object { $target -eq 'frontend-client.lib' -or $_ -in $client })
        if (@(Compare-Object $client $actual).Count) {
            throw "$target frontend source ownership mismatch: $($actual -join ', ')"
        }
    }
    # Shared I/O clients belong to common, never NTKVM-private code.
    $worker = @(Get-FrontendSources 'ntvdm.exe')
    if ($worker.Count) {
        throw "ntvdm frontend client-only boundary mismatch: $worker"
    }
    $native = @(Get-FrontendSources 'ntvwm.exe')
    if ($native.Count) {
        throw "ntvwm frontend client-only boundary mismatch: $native"
    }
    foreach($target in @('frontend-client.lib','ntkvm.exe')) {
        if(@(Get-FrontendSources $target 'ntvwm-exe').Count) {
            throw "$target must not depend on NTVWM-private implementation"
        }
        $actual=@(Get-FrontendSources $target 'run16-exe')
        if('native_launch.c' -in $actual) {
            throw "$target must not contain target creation"
        }
        foreach($required in $client) {
            if($required -notin $actual){throw "$target omits shared startup client $required"}
        }
    }
    $launcherNative=@(Get-FrontendSources 'run16.exe' 'ntvwm-exe')
    if($launcherNative.Count -or 'native_launch.c' -notin @(Get-FrontendSources 'run16.exe' 'run16-exe')) {
        throw 'run16 must retain bounded GUI creation, not NTVWM execution/presentation'
    }
    foreach($required in @('native_launch.c')) {
        if($required -notin @(Get-FrontendSources 'ntvwm.exe' 'run16-exe')) {
            throw "NTVWM omits shared native launch primitive $required"
        }
    }
    foreach($archive in @('frontend-client.lib','opennt-base-bindings.lib','worker-base.lib')) {
        if(@(Get-FrontendSources $archive 'common').Count) {
            throw "$archive must not embed neutral common providers"
        }
    }
    foreach($archive in @('common-codec.lib','common-transport.lib','common-console.lib','common-rpc.lib')) {
        foreach($owner in @('run16-exe','ntsrv-exe','ntkvm-exe','ntvwm-exe','ntvdm-exe','worker-base')) {
            if(@(Get-FrontendSources $archive $owner).Count) {
                throw "$archive reverse-depends on $owner"
            }
        }
    }
    if(@(Compare-Object @('codec/native_launch.c') @(Get-FrontendSources 'common-codec.lib' 'common')).Count) {
        throw 'Common native packet codec must have exactly one provider'
    }
    if(@(Compare-Object @('transport/pipe_transfer.c') @(Get-FrontendSources 'common-transport.lib' 'common')).Count) {
        throw 'Common pipe mechanics must have exactly one provider'
    }
    if(@(Compare-Object @('console/members.c','console/client.c') @(Get-FrontendSources 'common-console.lib' 'common')).Count) {
        throw 'Common Console snapshot/client must have exactly one provider'
    }
    if(@(Compare-Object @('rpc/local_binding.c','rpc/native_command.c','rpc/frontend_control.c','rpc/worker_control.c','rpc/management.c') @(Get-FrontendSources 'common-rpc.lib' 'common')).Count) {
        throw 'Common RPC binding/native-command/frontend-control/worker-control clients must each have exactly one provider'
    }
    foreach($target in @('run16.exe','ntvdm.exe','ntvwm.exe','ntkvm.exe','ntmon.exe')) {
        if('rpc/local_binding.c' -notin @(Get-FrontendSources $target 'common')) {
            throw "$target omits shared local RPC binding"
        }
    }
    foreach($target in @('run16.exe','ntsrv.exe','ntvwm.exe','ntkvm.exe')) {
        if('codec/native_launch.c' -notin @(Get-FrontendSources $target 'common')) {
            throw "$target omits the common native packet codec"
        }
        if('console/members.c' -notin @(Get-FrontendSources $target 'common')) {
            throw "$target omits the common Console snapshot implementation"
        }
    }
    $shared = @(Get-FrontendSources 'worker-base.lib')
    if ($shared.Count) {
        throw 'Worker protocol library contains a non-client implementation'
    }
    foreach($target in @('worker-base.lib','ntvdm.exe','ntvwm.exe')) {
        $actual=@(Get-FrontendSources $target 'worker-base')
        if(@(Compare-Object @('connection.c') $actual).Count) {
            throw "$target does not use the complete common worker implementation"
        }
    }
    foreach($target in @('ntvdm.exe','ntvwm.exe','console-client-test.exe','ntvwm-presentation-test.exe')) {
        if('console/client.c' -notin @(Get-FrontendSources $target 'common')) {
            throw "$target omits the common frontend protocol client"
        }
    }
    $ntvdmNative=@(Get-FrontendSources 'ntvdm.exe' 'ntvwm-exe')
    $ntconNative=@(Get-FrontendSources 'ntvwm.exe' 'ntvwm-exe')
    if('next_command.c' -in $ntvdmNative -or 'next_command.c' -notin $ntconNative) {
        throw 'NTVWM-only native GetNext wrapper must not enter NTVDM or worker-base'
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
            'lib/kvm-window/win32/component.c')) {
        if ($required -notin $service) { throw "ntkvm.exe omits $required" }
    }
}

Assert-FrontendOwnership $graph
foreach($target in @('run16.exe','ntsrv.exe','ntvdm.exe','ntvwm.exe','ntkvm.exe','ntmon.exe')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($target)+'(?: |:)')) {
            $_ -replace ': (\S+) ', ': $1 obj/tests/base_service_fixture.obj '
        } else { $_ }
    })
    $rejected=$false
    try {Assert-FrontendOwnership $mutated} catch {$rejected=$true}
    if(!$rejected){throw "Service fixture contamination accepted: $target"}
}
# Neutral code must be selected once, not copied into private archives.
foreach($archive in @('frontend-client.lib','opennt-base-bindings.lib','worker-base.lib')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($archive)+':')) {
            $_ -replace ': (\S+) ', ': $1 obj/common/native_launch.obj '
        } else { $_ }
    })
    $rejected=$false
    try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
    if(!$rejected){throw "Common provider embedding accepted: $archive"}
}
foreach($archive in @('common-codec.lib','common-transport.lib','common-console.lib','common-rpc.lib')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($archive)+':')) {
            $_ -replace ': (\S+) ', ': $1 obj/run16/native_launch.obj '
        } else { $_ }
    })
    $rejected=$false
    try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
    if(!$rejected){throw "Common reverse dependency accepted: $archive"}
}
# The packet codec is shared with callers; native target creation is not.
foreach($target in @('frontend-client.lib','ntkvm.exe')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($target)+'(?: |:)')) {
            $_ -replace ': (\S+) ', ': $1 obj/run16/native_launch.obj '
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
