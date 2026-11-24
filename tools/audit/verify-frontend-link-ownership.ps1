[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$NativeBuildRoot
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
function Assert-WorkerBackendRole([string]$Text) {
    if($Text -match '#include\s*[<"](?:frontend-base/|common/base_srv/frontend_client\.h)' -or
        $Text -match '\bOpenNtBaseClient(?:RegisterEmbeddedFrontend|RegisterFrontendRoot|FrontendRequest|AttachFrontendRequest|CloseFrontendInstance|UnregisterFrontendInstance)\s*\(') {
        throw 'Worker Base cannot compose a frontend role; the executable owns VM frontend adapters'
    }
}
foreach($file in Get-ChildItem (Join-Path $repo 'src/worker-base') -File -Recurse |
        Where-Object Extension -in '.c','.h') {
    Assert-WorkerBackendRole (Get-Content $file.FullName -Raw)
}
foreach($negative in @('#include "frontend-base/service.h"',
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
    if((Get-Content $file.FullName -Raw) -match '#include\s*[<"](?:common|frontend-base|worker-base|ntsrv-exe|run16-exe|ntcon-exe|ntvdm-exe|ntvwm-exe|ntmon-exe|nthook-dll)/') {
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

. "$PSScriptRoot/native-source-graph.ps1"
$workerPreset=Split-Path (Resolve-Path $BuildRoot).Path -Parent
$products=@(
    @($BuildRoot,'ntvdm.exe','ntvdm-exe'),
    @($BuildRoot,'nthook32.dll','nthook-dll'),
    @($BuildRoot,'VDMREDIR.dll','vdmredir-dll'),
    @($BuildRoot,'VCDEX.dll','mvdm-host'),
    @((Join-Path $workerPreset 'wow32'),'wow32.dll','wow32-dll'),
    @((Join-Path $NativeBuildRoot 'launcher'),'run16.exe','run16-exe'),
    @((Join-Path $NativeBuildRoot 'frontend'),'ntcon.exe','ntcon-exe'),
    @((Join-Path $NativeBuildRoot 'worker'),'ntvwm.exe','ntvwm-exe'),
    @((Join-Path $NativeBuildRoot 'monitor'),'ntmon.exe','ntmon-exe'),
    @((Join-Path $NativeBuildRoot 'service'),'ntsrv.exe','ntsrv-exe'),
    @((Join-Path $NativeBuildRoot 'hook'),'nthook64.dll','nthook-dll'))
$fOwners=@('ntcon-exe','nthook-dll','ntmon-exe','ntsrv-exe','ntvdm-exe',
    'ntvwm-exe','run16-exe','vdmredir-dll','wow32-dll')
function Assert-ProductSources([string]$Target,[string]$Owner,[hashtable]$Graph) {
    $sources=@(Get-GraphSources $Target $Graph)
    foreach($source in $sources){
        if($source -match '/test/'){throw "Test provider entered $Target : $source"}
        if($source -match '/src/([^/]+)/'){
            $provider=$Matches[1]
            if($provider -in $fOwners -and $provider -ne $Owner){
                throw "Private provider entered $Target : $source"
            }
            if($provider -eq 'worker-base' -and $Owner -notin @('ntvdm-exe','ntvwm-exe')){
                throw "Worker provider entered $Target : $source"
            }
        }
        if($Owner -ne 'ntcon-exe' -and $source -match '/src/lib-nxvm/lib/kvm-'){
            throw "Window provider entered $Target : $source"
        }
        if($Owner -in @('run16-exe','ntcon-exe') -and $source -match '/src/common/native_process.c$'){
            throw "Execution provider entered $Target : $source"
        }
    }
    if($Owner -eq 'ntcon-exe' -and !($sources -match '/src/lib-nxvm/lib/kvm-window/')){
        throw 'NTCON omits the shared window implementation'
    }
    if($Owner -eq 'ntvwm-exe' -and !($sources -match '/src/common/native_process.c$')){
        throw 'NTVWM omits native execution provider'
    }
    "PASS $Target sources=$($sources.Count)"
}
foreach($product in $products){
    $graph=Read-Graph $product[0]
    Assert-ProductSources $product[1] $product[2] $graph
    # Reject actual archive-hidden contamination, not merely unknown targets.
    foreach($control in @(@('test/adapter-basesrv/base_service_fixture.c','Test provider'),
        @(('src/'+$(if($product[2] -eq 'ntcon-exe'){'run16-exe'}else{'ntcon-exe'})+'/private.c'),'Private provider'))){
        $mutated=$graph.Clone()
        $mutated[$product[1]]=@($graph[$product[1]])+@('audit-leak.lib')
        $mutated['audit-leak.lib']=@('audit-leak.obj')
        $mutated['audit-leak.obj']=@('O:/repo/'+$control[0])
        $rejected=$false
        try {Assert-ProductSources $product[1] $product[2] $mutated|Out-Null}
        catch {if($_ -notmatch [regex]::Escape($control[1])){throw};$rejected=$true}
        if(!$rejected){throw "Contamination control accepted for $($product[1])"}
    }
}
'PASS native product ownership and 22 archive-hidden contamination controls'
