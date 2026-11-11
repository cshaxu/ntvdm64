[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$graph = Get-Content -LiteralPath (Join-Path $BuildRoot 'build.ninja')
# The x86 product graph imports five AMD64 executables. Audit their retained
# source recipes, not the import/copy edge, for this source-ownership check.
# The architecture-local final graphs/maps are checked separately.
$graph = @($graph | ForEach-Object {
    if ($_ -match '^build (ntcon|ntmon|run16|ntsrv)\.exe: native_\1_import(?:\s|$)' -or
        $_ -match '^build (ntvwm)\.exe: native_worker_import(?:\s|$)') {
        $name=$Matches[1]
        $recipe=@($graph | Where-Object { $_ -match ('^build '+$name+'-source-closure: phony ') })
        if($recipe.Count -ne 1){throw "Missing unique source recipe for imported $name"}
        $recipe[0] -replace ('^build '+$name+'-source-closure:'),('build '+$name+'.exe:')
    } else { $_ }
})
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
function Assert-WorkerBackendRole([string]$Text) {
    if($Text -match '#include\s*[<"](?:common/frontend-base/|common/base_srv/frontend_client\.h)' -or
        $Text -match '\bOpenNtBaseClient(?:RegisterEmbeddedFrontend|RegisterFrontendRoot|FrontendRequest|AttachFrontendRequest|CloseFrontendInstance|UnregisterFrontendInstance)\s*\(') {
        throw 'Worker Base cannot compose a frontend role; the executable owns VM frontend adapters'
    }
}
foreach($file in Get-ChildItem (Join-Path $repo 'src/worker-base') -File -Recurse |
        Where-Object Extension -in '.c','.h') {
    Assert-WorkerBackendRole (Get-Content $file.FullName -Raw)
}
foreach($negative in @('#include "common/frontend-base/service.h"',
        '#include "common/base_srv/frontend_client.h"',
        'OpenNtBaseClientRegisterEmbeddedFrontend(location,key,notify,retire);')) {
    $rejected=$false
    try { Assert-WorkerBackendRole $negative } catch { $rejected=$true }
    if(!$rejected){throw 'Worker/frontend role negative control was accepted'}
}
if((Get-Content (Join-Path $repo 'src/common/rpc/worker_control.c') -Raw) -match
    '\bcommon_rpc_frontend_io_disconnected\s*\(') {
    throw 'Frontend release acknowledgement belongs to frontend_control, not worker_control'
}
if((Get-Content (Join-Path $repo 'src/common/base_srv/base_rpc_client_core.c') -Raw) -match
    '(?m)^DWORD OpenNtBaseClient(?:RegisterEmbeddedFrontend|RegisterFrontendRoot|FrontendRequest|AttachFrontendRequest|CloseFrontendInstance)\s*\(') {
    throw 'Frontend role implementation belongs to frontend_client.c, not the process connection core'
}
foreach($owner in @('ntcon-exe','ntsrv-exe','ntvwm-exe')) {
    foreach($file in Get-ChildItem (Join-Path $repo "src/$owner") -File -Recurse |
            Where-Object Extension -in '.c','.h') {
        if((Get-Content $file.FullName -Raw) -match '#include\s*[<"][^>"\r\n]*run16-exe/(?:frontend_bootstrap|native_launch)\.h[>"]') {
            throw "Launcher-private header escaped ownership: $($file.FullName)"
        }
    }
}
foreach($file in Get-ChildItem (Join-Path $repo 'src/lib-win32'),(Join-Path $repo 'src/lib-nxvm') -File -Recurse | Where-Object Extension -in '.c','.h','.cpp') {
    if((Get-Content $file.FullName -Raw) -match '#include\s*[<"](?:common|worker-base|ntsrv-exe|run16-exe|ntcon-exe|ntvdm-exe|ntvwm-exe|ntmon-exe|nthook32-dll)/') {
        throw "Generic library depends on product code: $($file.FullName)"
    }
}
function Assert-FrontendPrivateNames([string]$Source) {
    if($Source -match '\b(?:run16_native_frontend|run16_console_(?:channel|frontend|video|dispatch|prepare_text)|frontend_native_keyboard|FRONTEND_NATIVE_KEY_RECORDS)(?:\b|_)|\bnative_console_frontend\.h\b') {
        throw 'Retired launcher/worker-owned spelling in frontend-private API'
    }
}
foreach($file in Get-ChildItem (Join-Path $repo 'src/ntcon-exe') -File | Where-Object Extension -in '.c','.h') {
    Assert-FrontendPrivateNames (Get-Content $file.FullName -Raw)
}
foreach($spelling in @('run16_native_frontend','run16_console_channel','frontend_native_keyboard',
        'FRONTEND_NATIVE_KEY_RECORDS','native_console_frontend.h','run16_native_frontend_create')) {
    $rejected=$false
    try { Assert-FrontendPrivateNames $spelling } catch { $rejected=$true }
    if(!$rejected){throw "Retired private-name negative control accepted $spelling"}
}
if($graph -match '^build obj/run16/(?:native_console_frontend|frontend_session|console_frontend|console_video|console_channel)\.obj:') {
    throw 'Frontend-private compilation retained under launcher object ownership'
}
# Presentation attachment is live; execution submission to a frontend is not.
# A stale RPC method must not survive merely because no EXE calls it today.
foreach ($path in @('src/common/protocol/service.idl',
        'src/common/base_srv/base_rpc_client_core.c',
        'src/common/base_srv/frontend_client.c',
        'src/common/base_srv/base_rpc_client_launch.c',
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
        $launcherEdge[0] -notmatch '\|\|\s+ntcon\.exe\s*$' -or
        ($launcherEdge[0] -split '\s+\|\|\s+')[0] -match '\bntcon\.exe\b') {
        throw 'ntcon.exe must be only an order-only launcher runtime prerequisite'
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
    function Get-FrontendSources([string]$Target,[string]$Owner='ntcon-exe',[string]$Root='src') {
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
                if ($input -in @('frontend-terminal.lib','ntcon-worker-client.lib')) {
                    throw 'Retired frontend archive re-entered a production link'
                }
                if ($input -match ('/'+[regex]::Escape($Root)+'/'+[regex]::Escape($Owner)+'/(.+\.(?:c|cpp))$')) {
                    [void]$sources.Add($Matches[1])
                } elseif ($input -match '\.(obj|lib)$' -and $edges.ContainsKey($input)) {
                    $pending.Push($input)
                }
            }
        }
        return @($sources | Sort-Object)
    }
    foreach($target in @('run16.exe','ntsrv.exe','ntvdm.exe','ntvwm.exe','ntcon.exe','ntmon.exe')) {
        if(@(Get-FrontendSources $target 'adapter-basesrv' 'test').Count) {
            throw "$target includes service test-only translation/hooks"
        }
    }
    $fixtureSources=@(Get-FrontendSources 'basesrv-service-reservation-test.exe' 'adapter-basesrv' 'test')
    if(@(Compare-Object @('base_service_reservation_test.c','base_service_fixture.c') $fixtureSources).Count) {
        throw 'Service fixture must select its private hooks and reservation test'
    }
    if(@(Get-FrontendSources 'nthook32.dll' 'run16-exe').Count) {
        throw 'Hook32 must not compile or link Run16-private implementation'
    }
    $client = @('bootstrap_client.c',
        'native_request_client.c') | Sort-Object
    foreach ($target in @('frontend-client.lib', 'run16.exe')) {
        if (@(Get-FrontendSources $target).Count) {
            throw "$target must not depend on NTCON-private implementation"
        }
        $actual = @(Get-FrontendSources $target 'run16-exe' |
            Where-Object { $target -eq 'frontend-client.lib' -or $_ -in $client })
        if (@(Compare-Object $client $actual).Count) {
            throw "$target frontend source ownership mismatch: $($actual -join ', ')"
        }
    }
    # Shared I/O clients belong to common, never NTCON-private code.
    $worker = @(Get-FrontendSources 'ntvdm.exe')
    if ($worker.Count) {
        throw "ntvdm frontend client-only boundary mismatch: $worker"
    }
    $native = @(Get-FrontendSources 'ntvwm.exe')
    if ($native.Count) {
        throw "ntvwm frontend client-only boundary mismatch: $native"
    }
    foreach($target in @('frontend-client.lib','ntcon.exe')) {
        if(@(Get-FrontendSources $target 'ntvwm-exe').Count) {
            throw "$target must not depend on NTVWM-private implementation"
        }
        $actual=@(Get-FrontendSources $target 'run16-exe')
        if('native_process.c' -in @(Get-FrontendSources $target 'common')) {
            throw "$target must not contain target creation"
        }
        if($target -eq 'frontend-client.lib') {
            foreach($required in $client) {
                if($required -notin $actual){throw "$target omits launcher startup client $required"}
            }
        } elseif($actual.Count) {
            throw "ntcon.exe must not depend on Run16-private implementation: $actual"
        }
    }
    $launcherNative=@(Get-FrontendSources 'run16.exe' 'ntvwm-exe')
    if($launcherNative.Count -or 'native_process.c' -in @(Get-FrontendSources 'run16.exe' 'common')) {
        throw 'run16 submits to NTSRV; it must not select native target creation'
    }
    foreach($required in @('native_process.c')) {
        if($required -notin @(Get-FrontendSources 'ntvwm.exe' 'common')) {
            throw "NTVWM omits shared native launch primitive $required"
        }
    }
    # Existing path/image selection providers are explicit archive members.
    # Do not silently admit another common provider (especially process creation).
    foreach($archive in @('frontend-client.lib','opennt-base-bindings.lib','worker-base.lib')) {
        $neutral=@(Get-FrontendSources $archive 'common')
        if($archive -eq 'opennt-base-bindings.lib') {
            $neutral=@($neutral | Where-Object { $_ -ne 'native_image.c' })
        }
        if($neutral.Count) {
            throw "$archive must not embed neutral common providers"
        }
    }
    foreach($archive in @('common-codec.lib','lib-pipe.lib','lib-process-image.lib','lib-rpc.lib','lib-process-launch.lib','lib-process-observation.lib','lib-handle.lib','lib-file.lib','lib-process-hook.lib','common-console.lib','common-rpc.lib')) {
        foreach($owner in @('run16-exe','ntsrv-exe','ntcon-exe','ntvwm-exe','ntvdm-exe','worker-base')) {
            if(@(Get-FrontendSources $archive $owner).Count) {
                throw "$archive reverse-depends on $owner"
            }
        }
    }
    foreach($archive in @('lib-rpc.lib','lib-pipe.lib','lib-process-image.lib','lib-process-launch.lib','lib-process-observation.lib','lib-handle.lib','lib-file.lib','lib-process-hook.lib')) {
        if(@(Get-FrontendSources $archive 'common').Count) {
            throw "$archive must not depend on product common sources"
        }
    }
    if(@(Compare-Object @('codec/native_launch.c') @(Get-FrontendSources 'common-codec.lib' 'common')).Count) {
        throw 'Common native packet codec must have exactly one provider'
    }
    if(@(Compare-Object @('pipe/pipe_transfer.c','pipe/pipe_pair.c') @(Get-FrontendSources 'lib-pipe.lib' 'lib-win32')).Count) { throw 'Pipe mechanisms must have one provider' }
    if(@(Compare-Object @('console/client.c') @(Get-FrontendSources 'common-console.lib' 'common')).Count) {
        throw 'Common Console client must have exactly one provider'
    }
    if(@(Compare-Object @('process-image/image.c') @(Get-FrontendSources 'lib-process-image.lib' 'lib-win32')).Count) { throw 'Win32 process image provider missing' }
    if(@(Get-FrontendSources 'lib-process-hook.lib' 'lib-win32') -contains 'process-image/image.c') { throw 'Hook implementation must not own process image selection' }
    if ('process-hook/payload.c' -notin @(Get-FrontendSources 'lib-process-hook.lib' 'lib-win32')) { throw 'Win32 process-hook payload provider missing' }
    if(@(Compare-Object @('handle/duplicate.c') @(Get-FrontendSources 'lib-handle.lib' 'lib-win32')).Count) { throw 'Win32 handle library must own the duplication implementation' }
    if(@(Compare-Object @('file/identity.c','file/native_path.c') @(Get-FrontendSources 'lib-file.lib' 'lib-win32')).Count) { throw 'Win32 file library must own file identity' }
    if(@(Compare-Object @('process-observation/observation.c','process-observation/console_members.c') @(Get-FrontendSources 'lib-process-observation.lib' 'lib-win32')).Count) { throw 'Win32 process observation provider missing' }
    if(@(Compare-Object @('process-launch/attributes.c','process-launch/process.c','process-launch/command_line.c') @(Get-FrontendSources 'lib-process-launch.lib' 'lib-win32')).Count) { throw 'Win32 process library must own the attribute-list implementation' }
    if(@(Compare-Object @('rpc/local_binding.c','rpc/peer.c') @(Get-FrontendSources 'lib-rpc.lib' 'lib-win32')).Count) {
        throw 'Generic RPC binding must have exactly one provider'
    }
    if(@(Compare-Object @('rpc/native_command.c','rpc/frontend_control.c','rpc/worker_control.c','rpc/management.c','rpc/dos_observation.c','rpc/interface_query.c') @(Get-FrontendSources 'common-rpc.lib' 'common')).Count) {
        throw 'Common RPC protocol clients must each have exactly one provider'
    }
    foreach($target in @('run16.exe','ntvdm.exe','ntvwm.exe','ntcon.exe','ntmon.exe')) {
        if('rpc/local_binding.c' -notin @(Get-FrontendSources $target 'lib-win32')) {
            throw "$target omits shared local RPC binding"
        }
    }
    foreach($target in @('run16.exe','ntsrv.exe','ntvwm.exe','ntcon.exe')) {
        if('codec/native_launch.c' -notin @(Get-FrontendSources $target 'common')) {
            throw "$target omits the common native packet codec"
        }
        if('process-observation/console_members.c' -notin @(Get-FrontendSources $target 'lib-win32')) {
            throw "$target omits the common Console snapshot implementation"
        }
    }
    $shared = @(Get-FrontendSources 'worker-base.lib')
    if ($shared.Count) {
        throw 'Worker protocol library contains a non-client implementation'
    }
    foreach($target in @('worker-base.lib','ntvdm.exe','ntvwm.exe')) {
        $actual=@(Get-FrontendSources $target 'worker-base')
        if(@(Compare-Object @('connection.c','shutdown_close.c','publication.c','input_watch.c','task_observation.c') $actual).Count) {
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
    foreach($target in @('run16.exe','ntsrv.exe','ntcon.exe','ntmon.exe','nthook32.dll','VDMREDIR.dll')) {
        if(@(Get-FrontendSources $target 'worker-base').Count) {
            throw "$target must not acquire worker-side mechanisms"
        }
    }
    # Current product composition does not select KVM in other executables.
    foreach ($target in @('run16.exe','ntsrv.exe','ntvdm.exe','ntvwm.exe','ntmon.exe')) {
        if (@(Get-FrontendSources $target 'lib-nxvm' | Where-Object { $_ -like 'lib/kvm-*/*' }).Count) { throw "$target unexpectedly links the frontend window implementation" }
    }
    $service = @(Get-FrontendSources 'ntcon.exe')
    if ('native_console_host.c' -in $service) { throw 'Retired helper entered ntcon.exe' }
    if ('native_console_request.c' -in $service) { throw 'Native execution receiver entered presentation-only ntcon.exe' }
    foreach ($backend in @('native_conpty.c','native_terminal.c','native_console_backend.c',
            'native_console_view.c','native_console_capture.c','native_console_launch.c')) {
        if ($backend -in $service) { throw "Native backend entered presentation-only ntcon.exe: $backend" }
    }
    foreach ($required in @('main.c', 'session_service.c', 'console_channel.c',
            'console_frontend.c', 'frontend_session.c',
            'window_controller.c', 'window_frame.c',
            'window_keyboard.c', 'window_input_queue.c')) {
        if ($required -notin $service) { throw "ntcon.exe omits $required" }
    }
    if ('lib/kvm-window/win32/component.c' -notin @(Get-FrontendSources 'ntcon.exe' 'lib-nxvm')) { throw 'ntcon.exe omits shared window implementation' }
    if ('video.c' -notin @(Get-FrontendSources 'ntcon.exe' 'common/frontend-base')) { throw 'ntcon.exe omits shared frame receiver' }
}

Assert-FrontendOwnership $graph
foreach($target in @('run16.exe','ntsrv.exe','ntvdm.exe','ntvwm.exe','ntcon.exe','ntmon.exe')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($target)+'(?: |:)')) {
            $_ -replace ': (\S+) ', ': $1 obj/tests/base_service_fixture.obj '
        } else { $_ }
    })
    $rejected=$false
    try {Assert-FrontendOwnership $mutated} catch {$rejected=$true}
    if(!$rejected){throw "Service fixture contamination accepted: $target"}
}
# Reject a direct Run16 object on the Hook32 product link.
$hookMutated=@($graph | ForEach-Object {
    if($_ -match '^build nthook32\.dll:') { $_ -replace ': (\S+) ', ': $1 obj/run16/image_classification.obj ' } else { $_ }
})
$hookRejected=$false
try { Assert-FrontendOwnership $hookMutated } catch { $hookRejected=$true }
if(!$hookRejected){throw 'Hook32 Run16 dependency accepted'}
# NTCON must reject a Run16 archive even when the linker would discard it.
$mutated=@($graph | ForEach-Object {
    if($_ -match '^build ntcon\.exe:') { $_ -replace ': (\S+) ', ': $1 frontend-client.lib ' } else { $_ }
})
$rejected=$false
try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
if(!$rejected){throw 'NTCON Run16 client dependency accepted'}
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
foreach($archive in @('common-codec.lib','lib-pipe.lib','lib-process-image.lib','lib-rpc.lib','lib-process-launch.lib','lib-process-observation.lib','lib-handle.lib','lib-file.lib','lib-process-hook.lib','common-console.lib','common-rpc.lib')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($archive)+':')) {
            $_ -replace ': (\S+) ', ': $1 obj/run16/frontend_scope.obj '
        } else { $_ }
    })
    $rejected=$false
    try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
    if(!$rejected){throw "Common reverse dependency accepted: $archive"}
}
# The packet codec is shared with callers; native target creation is not.
foreach($target in @('frontend-client.lib','ntcon.exe')) {
    $mutated=@($graph | ForEach-Object {
        if($_ -match ('^build '+[regex]::Escape($target)+'(?: |:)')) {
            $_ -replace ': (\S+) ', ': $1 obj/common/native_process.obj '
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
            $_ -replace ': (\S+) ', ': $1 obj/frontend/console_frontend.obj '
        } else { $_ }
    })
    $rejected = $false
    try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
    if (!$rejected) { throw "Ownership negative control did not reject $target" }
}
# A non-worker must not even select the worker archive. Check a deliberate
# VDMREDIR link-edge regression, not only currently resolved symbols.
$mutated=@($graph | ForEach-Object {
    if($_ -match '^build VDMREDIR\.dll(?: |:)') {
        $_ -replace ': (\S+) ', ': $1 worker-base.lib '
    } else { $_ }
})
$rejected=$false
try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
if(!$rejected){throw 'VDMREDIR Worker Base negative control was accepted'}
# Window libraries contain nested source paths, not only owner-root C files.
# Reject both full-archive leakage and a library-only leaf object leakage.
foreach ($leakInput in @('frontend-window.lib', 'frontend-terminal.lib', 'ntcon-worker-client.lib', 'obj/ownership-leak.obj')) {
    $mutated = @($graph | ForEach-Object {
        if ($_ -match '^build ntvdm\.exe(?: |:)') {
            $_ -replace ': (\S+) ', (': $1 ' + $leakInput + ' ')
        } else { $_ }
    })
    $mutated += 'build obj/ownership-leak.obj: cc O$:/repo/src/lib-nxvm/lib/kvm-window/win32/component.c'
    $rejected = $false
    try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
    if (!$rejected) { throw "Window ownership negative control did not reject $leakInput" }
}
$mutated = @($graph | ForEach-Object {
    if ($_ -match '^build ntcon\.exe(?: |:)') {
        $_ -replace ': (\S+) ', ': $1 obj/retired-helper.obj '
    } else { $_ }
})
$mutated += 'build obj/retired-helper.obj: cc O$:/repo/src/ntcon-exe/native_console_host.c'
$rejected = $false
try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
if (!$rejected) { throw 'Retired helper negative control was accepted' }
$mutated = @($graph | ForEach-Object {
    if ($_ -match '^build ntcon\.exe(?: |:)') {
        $_ -replace ': (\S+) ', ': $1 obj/run16/native_console_request.obj '
    } else { $_ }
})
$mutated += 'build obj/run16/native_console_request.obj: cc O$:/repo/src/ntcon-exe/native_console_request.c'
$rejected = $false
try { Assert-FrontendOwnership $mutated } catch { $rejected = $true }
if (!$rejected) { throw 'Native execution receiver negative control was accepted' }
foreach ($backend in @('native_conpty.c','native_terminal.c','native_console_backend.c',
        'native_console_view.c','native_console_capture.c','native_console_launch.c')) {
    $mutated = @($graph | ForEach-Object {
        if ($_ -match '^build ntcon\.exe(?: |:)') {
            $_ -replace ': (\S+) ', ': $1 obj/retired-backend.obj '
        } else { $_ }
    })
    $mutated += "build obj/retired-backend.obj: cc O`$:/repo/src/ntcon-exe/$backend"
    $rejected=$false
    try { Assert-FrontendOwnership $mutated } catch { $rejected=$true }
    if (!$rejected) { throw "Native backend leakage control accepted $backend" }
}
Write-Output 'PASS frontend link ownership: no native execution/backend/helper, client-only launcher, worker protocol client only; leakage controls rejected. Physical retired-source cleanup is a separate gate.'
