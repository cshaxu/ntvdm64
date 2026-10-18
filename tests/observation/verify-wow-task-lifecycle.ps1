param(
    [Parameter(Mandatory=$true)][string]$ProviderBuildRoot,
    [Parameter(Mandatory=$true)][string]$WorkerBuildRoot,
    [string]$BuildRoot,
    [ValidateSet('task-lifecycle','process-retirement','menu-cleanup','menu-resource','native-dispatch-gate','window-procedure','standard-class','class-query','class-procedure','class-lifetime','dialog-lifecycle','init-notification','start-notification','style-policy','native-send','native-reply','native-nested','native-seen','native-identity','native-foreign','task-order','class-client','window-borrow','window-reclaim','graph-snapshot','thunk-scope','native-creation')] [string]$Case = 'task-lifecycle',
    [string]$StyleDll,
    [switch]$InitRollback,
    [switch]$CreationUnwind,
    [switch]$OriginalModalOnly,
    [switch]$RealDialogLifecycleOnly,
    [string]$Ninja = 'ninja.exe',
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Links current production owner objects; this is not real-Win16 acceptance.
$ErrorActionPreference = 'Stop'
# Preserve the public case name, but select actual CCPU storage and the
# registered original-owner lifetime. No numeric page-domain stand-in remains.
$requestedCase = $Case
if ($Case -eq 'dialog-lifecycle') { $Case='window-borrow'; $RealDialogLifecycleOnly=$true }
if ($InitRollback -and $Case -ne 'task-lifecycle') { throw 'InitRollback requires task-lifecycle' }
if ($CreationUnwind -and $Case -ne 'native-creation') { throw 'CreationUnwind requires native-creation' }
if ($RealDialogLifecycleOnly) { $OriginalModalOnly = $true }
if ($OriginalModalOnly -and $Case -ne 'window-borrow') { throw 'OriginalModalOnly requires window-borrow' }
if ($StyleDll -and $Case -notin @('style-policy','window-borrow')) { throw 'StyleDll requires style-policy or window-borrow' }
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$vs = (Resolve-Path -LiteralPath $VsDevCmd).Path

$provider = (Resolve-Path -LiteralPath $ProviderBuildRoot).Path
$worker = (Resolve-Path -LiteralPath $WorkerBuildRoot).Path
if (!$BuildRoot) { $BuildRoot = Join-Path $repo ('build/M0-T422/S2/' + $requestedCase) }
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith(($repo + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Lifecycle fixture output must be under repository build.'
}
New-Item -ItemType Directory -Path $out -Force | Out-Null
if ($requestedCase -eq 'style-policy') {
    if (!$StyleDll) { throw 'style-policy requires a matching style DLL' }
    # One process-level registration lifetime per executable. Keep policy/peer
    # model assertions and require real original-modal acceptance separately.
    $shell=(Get-Process -Id $PID).Path
    $line='-NoProfile -File "'+$PSCommandPath+'" -ProviderBuildRoot "'+$provider+'" -WorkerBuildRoot "'+$worker+'" -BuildRoot "'+$out+'/original-modal" -Case window-borrow -OriginalModalOnly -StyleDll "'+(Resolve-Path $StyleDll).Path+'" -VsDevCmd "'+$vs+'" -Ninja "'+$Ninja+'"'
    $modal=Start-Process $shell -ArgumentList $line -WindowStyle Hidden -PassThru -WorkingDirectory $repo -RedirectStandardOutput "$out/original-modal.stdout.txt" -RedirectStandardError "$out/original-modal.stderr.txt"
    $null=$modal.Handle
    @{pid=$modal.Id;start=$modal.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=300} |
        ConvertTo-Json | Set-Content "$out/original-modal.process.json"
    if (!$modal.WaitForExit(300000)) { & taskkill.exe /PID $modal.Id /T /F; throw 'Original modal prerequisite deadline' }
    $modal.WaitForExit();$code=$modal.ExitCode;$code|Set-Content "$out/original-modal.exit.txt";$modal.Dispose()
    if ($code) { throw 'Actual original modal/AW/style/lifetime prerequisite failed' }
}
# Initialize the compiler shell once. Only compiler children receive the
# resulting environment; the verifier and acceptance children never inherit it.
$environmentStart = [DateTimeOffset]::UtcNow
$environmentInfo = New-Object Diagnostics.ProcessStartInfo
$environmentInfo.FileName = 'cmd.exe'
$environmentInfo.Arguments = '/u /d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && set"'
$environmentInfo.UseShellExecute = $false
$environmentInfo.CreateNoWindow = $true
$environmentInfo.RedirectStandardOutput = $true
$environmentInfo.RedirectStandardError = $true
$environmentInfo.StandardOutputEncoding = [Text.Encoding]::Unicode
$environmentInfo.StandardErrorEncoding = [Text.Encoding]::Unicode
$environmentProcess = New-Object Diagnostics.Process
$environmentProcess.StartInfo = $environmentInfo
if (!$environmentProcess.Start()) { throw 'Compiler environment start failed' }
$null = $environmentProcess.Handle
@{pid=$environmentProcess.Id;started=$environmentStart.ToString('o');timeoutSeconds=120;
    command=$environmentInfo.FileName+' '+$environmentInfo.Arguments} |
    ConvertTo-Json | Set-Content "$out/compiler-environment.process.json"
$environmentOutput = $environmentProcess.StandardOutput.ReadToEndAsync()
$environmentError = $environmentProcess.StandardError.ReadToEndAsync()
if (!$environmentProcess.WaitForExit(120000)) {
    & taskkill.exe /PID $environmentProcess.Id /T /F
    throw 'Compiler environment deadline exceeded'
}
$environmentProcess.WaitForExit()
$environmentCode = $environmentProcess.ExitCode
$compilerLines = $environmentOutput.GetAwaiter().GetResult() -split "`r?`n"
$null = $environmentError.GetAwaiter().GetResult()
$environmentProcess.Dispose()
$environmentCode | Set-Content "$out/compiler-environment.exit.txt"
if ($environmentCode) { throw 'Compiler environment initialization failed' }
$compilerEnvironment = @{}
# These are build-tool inputs, not arbitrary inherited application variables.
# In particular do not round-trip multiline secrets or unrelated settings
# through CMD's textual SET output. /u retains non-ASCII compiler paths.
$compilerNames = '^(PATH|INCLUDE|LIB|LIBPATH|EXTERNAL_INCLUDE|DevEnvDir|ExtensionSdkDir|CommandPromptType|Platform|PreferredToolArchitecture|VisualStudioVersion|VSINSTALLDIR|VCINSTALLDIR|NETFXSDKDir|UCRTVersion)$|^(VCTools|VSCMD_|__VSCMD_|WindowsSDK|UniversalCRT|Framework)'
foreach ($line in $compilerLines) {
    $equals = $line.IndexOf('=')
    if ($equals -gt 0 -and $line.Substring(0,$equals) -match $compilerNames) {
        $compilerEnvironment[$line.Substring(0,$equals)] = $line.Substring($equals+1)
    }
}
if (!$compilerEnvironment.ContainsKey('VCToolsInstallDir')) { throw 'Missing compiler toolchain environment' }
@{script=$vs;sha256=(Get-FileHash $vs).Hash;architecture='x86';host='x64';
    toolset=$compilerEnvironment['VCToolsVersion'];started=$environmentStart.ToString('o');
    elapsedSeconds=([DateTimeOffset]::UtcNow-$environmentStart).TotalSeconds} |
    ConvertTo-Json | Set-Content "$out/compiler-environment.json"
function Invoke-CompilerCommand([string]$directory,[string]$command,[switch]$Capture) {
    $originalEnvironment = [Environment]::GetEnvironmentVariables('Process')
    $info = New-Object Diagnostics.ProcessStartInfo
    $info.FileName = 'cmd.exe'
    $info.Arguments = '/d /s /c "'+$command+'"'
    $info.WorkingDirectory = $directory
    $info.UseShellExecute = $false
    $info.CreateNoWindow = $true
    $info.RedirectStandardOutput = $true
    $info.RedirectStandardError = $true
    foreach ($name in $compilerEnvironment.Keys) { $info.Environment[$name] = $compilerEnvironment[$name] }
    $process = New-Object Diagnostics.Process
    $process.StartInfo = $info
    $script:compilerCommandNumber++
    $prefix = "$out/compiler-command-$($script:compilerCommandNumber)"
    try {
        if (!$process.Start()) { throw 'Compiler command start failed' }
        $null = $process.Handle
        @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');
            directory=$directory;command=$command;timeoutSeconds=1200} |
            ConvertTo-Json | Set-Content "$prefix.process.json"
        $stdout = $process.StandardOutput.ReadToEndAsync()
        $stderr = $process.StandardError.ReadToEndAsync()
        if (!$process.WaitForExit(1200000)) {
            & taskkill.exe /PID $process.Id /T /F
            throw 'Compiler command deadline exceeded'
        }
        $process.WaitForExit()
        $code = $process.ExitCode
        $outputText = $stdout.GetAwaiter().GetResult()
        $errorText = $stderr.GetAwaiter().GetResult()
        $code | Set-Content "$prefix.exit.txt"
        if (!$Capture) { $outputText | Add-Content "$out/build.log" }
        if ($errorText) { $errorText | Add-Content "$out/build.log" }
        if ($code) {
            Get-Content "$out\build.log" -Tail 25
            throw "Compiler command failed: exit=$code"
        }
        if ($Capture) { $outputText.TrimEnd() -split "`r?`n" }
    } finally {
        $process.Dispose()
        $restoredEnvironment = [Environment]::GetEnvironmentVariables('Process')
        if ($restoredEnvironment.Count -ne $originalEnvironment.Count) { throw 'Build scope changed runtime environment' }
        foreach ($name in $originalEnvironment.Keys) {
            if ($restoredEnvironment[$name] -cne $originalEnvironment[$name]) {
                throw 'Build scope failed exact runtime-environment restoration'
            }
        }
    }
}
function Invoke-Build([string]$command) {
    Invoke-CompilerCommand $provider $command
}
function Invoke-NinjaTarget([string]$directory, [string]$target) {
    Invoke-CompilerCommand $directory ('"'+$Ninja+'" '+$target)
}
# T422 owns these worker-local objects. Build exactly those selected objects;
# do not rely on a provider-only build or on an incidental aggregate archive.
$workerObjects = @(
    (Join-Path $worker 'base-rpc-client-core.lib'),
    (Join-Path $worker 'base-rpc-client-launch-style.lib'),
    (Join-Path $worker 'obj/worker/stub.obj'),
    (Join-Path $worker 'obj/adapter-wow-worker/wow_gdi_alias.obj'),
    (Join-Path $worker 'obj/adapter-wow-worker/wow_user_runtime.obj'),
    (Join-Path $worker 'obj/adapter-wow-worker/wow_user_session_binding.obj'),
    (Join-Path $worker 'obj/ntvdm/session/guest_memory_lease.obj'),
    (Join-Path $worker 'obj/ntvdm/session/session.obj')
)
Invoke-NinjaTarget $worker (($workerObjects | ForEach-Object {
    $_.Substring($worker.Length + 1).Replace('\', '/')
}) -join ' ')
foreach ($object in $workerObjects) {
    if (!(Test-Path -LiteralPath $object -PathType Leaf)) {
        throw "Missing selected worker object: $object"
    }
}
$fixtureSupportObjects = @(
    (Join-Path $worker 'obj/host/softpc-resource.res')
)
Invoke-NinjaTarget $worker (($fixtureSupportObjects | ForEach-Object {
    $_.Substring($worker.Length + 1).Replace('\', '/')
}) -join ' ')
foreach ($object in $fixtureSupportObjects) {
    if (!(Test-Path -LiteralPath $object -PathType Leaf)) {
        throw "Missing selected fixture support object: $object"
    }
}
$fixtureLibraries = @(
    'worker-base.lib', 'common-root.lib', 'common-rpc.lib',
    'nthook-installer.lib', 'nthook-context.lib',
    'common-transport.lib', 'common-codec.lib', 'common-console.lib',
    'opennt-base-client.lib', 'opennt-base-bindings.lib', 'broker-transport.lib',
    'worker-shell.lib', 'worker-command-bindings.lib',
    'original-softpc-host-fixture-roots.lib', 'original-softpc-support.lib',
    'original-softpc-bios.lib', 'original-softpc-keymouse.lib',
    'original-softpc-system.lib', 'original-softpc-disks.lib',
    'original-softpc-video.lib', 'original-softpc-cvidc.lib',
    'original-softpc-comms.lib', 'original-softpc-dos.lib',
    'original-mvdm-dem.lib', 'original-mvdm-command.lib',
    'original-mvdm-xms.lib', 'original-mvdm-dpmi32.lib',
    'original-mvdm-host-suballoc.lib', 'original-mvdm-host-oemuni.lib',
    'original-softpc-base-trace.lib', 'original-opennt-base-vdm.lib',
    'original-opennt-rtl-x86.lib', 'softpc-fixture-bindings.lib', 'softpc-bindings.lib',
    'vdmredir-dll-bindings.lib', 'vdd-bindings.lib', 'softpc-win32-bindings.lib',
    'monitor-bindings.lib', 'kernel-vdm-printer.lib', 'debugger-bindings.lib',
    'session.lib', 'mvdm-softpc-effective-address.lib',
    'softpc-ccpu-vector-defaults.lib', 'softpc-activity-check.lib',
    'original-ccpu386.lib'
)
Invoke-NinjaTarget $worker ($fixtureLibraries -join ' ')
$fixtureLibraries = @($fixtureLibraries | ForEach-Object { Join-Path $worker $_ })
foreach ($library in $fixtureLibraries) {
    if (!(Test-Path -LiteralPath $library -PathType Leaf)) {
        throw "Missing selected fixture library: $library"
    }
}
$commands = & $ninja -C $provider -t commands wow32.dll
$compile = $commands | Where-Object { $_ -match 'wow_user_task_lifecycle\.c$' } | Select-Object -Last 1
if (!$compile) { throw 'Missing production compile command' }
$combinedSeams = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\combined-seams.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_combined_fixture_seams.c"')
if ($combinedSeams -eq $compile) { throw 'Combined fixture seam substitution failed' }
Invoke-Build ($combinedSeams.Replace('/Gz', '/Gd'))
$fixtureSupportObjects += Join-Path $out 'combined-seams.obj'
Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_user_task_lifecycle.obj'
$fixtureName = if ($Case -eq 'class-client') { 'wow_class_client_fixture.c' } else { 'wow_user_task_lifecycle_fixture.c' }
if ($Case -eq 'class-lifetime') { $fixtureName = 'wow_window_class_lifetime_fixture.c' }
if ($Case -eq 'process-retirement') { $fixtureName = 'wow_process_retirement_fixture.c' }
if ($Case -eq 'native-dispatch-gate') {
    Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_window_words_binding.obj'
    $fixtureName = 'wow_native_dispatch_gate_fixture.c'
}
if ($Case -eq 'window-procedure') {
    Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_window_words_binding.obj obj/wow32-dll/wow_class_words_binding.obj obj/wow32-dll/wow_user_private_access.obj'
    $fixtureName = 'wow_window_procedure_fixture.c'
}
if ($Case -eq 'standard-class') {
    Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_standard_class_bindings.obj obj/wow32-dll/wow_class_words_binding.obj'
    $fixtureName = 'wow_standard_class_fixture.c'
}
if ($Case -eq 'class-query') {
    Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_class_client_bindings.obj obj/wow32-dll/wow_user_private_access.obj'
    $fixtureName = 'wow_class_query_fixture.c'
}
if ($Case -eq 'init-notification') {
    Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_user_registration_bridge.obj'
    $fixtureName = 'wow_init_notification_fixture.c'
}
if ($Case -eq 'task-order') { $fixtureName = 'wow_task_order_fixture.c' }
if ($Case -eq 'thunk-scope') { $fixtureName = 'wow_user_thunk_scope_fixture.c' }
if ($Case -eq 'native-creation') { $fixtureName = 'wow_native_creation_binding_fixture.c' }
$fixturePath = Join-Path $repo ('tests/adapter-mvdm-host-out/wow/' + $fixtureName)
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource')) { $fixturePath = Join-Path $repo 'tests/mvdm-host/wow_page_domain_fixture.c' }
if ($Case -eq 'native-creation') { $fixturePath = Join-Path $repo 'tests/mvdm-host/wow_page_domain_fixture.c' }
if ($Case -eq 'class-procedure') { $fixturePath = Join-Path $repo 'tests/mvdm-host/wow_page_domain_fixture.c' }
if ($Case -eq 'standard-class') { $fixturePath = Join-Path $repo 'tests/mvdm-host/wow_page_domain_fixture.c' }
$fixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\fixture.obj" "'+$fixturePath+'"')
if ($fixture -eq $compile) { throw 'Fixture substitution failed' }
if ($InitRollback) { $fixture += ' /DWOW_INIT_ROLLBACK_FIXTURE' }
if ($Case -eq 'native-creation') { $fixture += ' /DWOW_WINDOW_BORROW_FIXTURE /DWOW_NATIVE_CREATION_FIXTURE' }
if ($Case -eq 'class-procedure') { $fixture += ' /DWOW_WINDOW_BORROW_FIXTURE /DWOW_CLASS_PROCEDURE_FIXTURE' }
if ($OriginalModalOnly) { $fixture += ' /DWOW_ORIGINAL_MODAL_FIXTURE' }
if ($RealDialogLifecycleOnly) { $fixture += ' /DWOW_REAL_DIALOG_LIFECYCLE_FIXTURE' }
if ($Case -eq 'standard-class') { $fixture += ' /DWOW_WINDOW_BORROW_FIXTURE /DWOW_STANDARD_CLASS_FIXTURE' }
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource')) { $fixture += ' /DWOW_WINDOW_BORROW_FIXTURE' }
if ($Case -eq 'window-reclaim') { $fixture += ' /DWOW_RECLAIM_FAILURE_FIXTURE' }
if ($Case -eq 'graph-snapshot') { $fixture += ' /DWOW_GRAPH_SNAPSHOT_FIXTURE' }
if ($Case -eq 'menu-resource') { $fixture += ' /DWOW_RESOURCE_CLIENT_FIXTURE' }
if ($Case -eq 'start-notification') { $fixture += ' /DWOW_START_NOTIFICATION_FIXTURE' }
if ($Case -eq 'menu-cleanup') { $fixture += ' /DWOW_MENU_CLEANUP_FIXTURE' }
if ($Case -eq 'task-order') { $fixture += ' /we4113 /we4047' }
if ($Case -eq 'style-policy') {
    if(!$StyleDll){throw 'style-policy requires the tested ntw16s32.dll'}
    Copy-Item -LiteralPath (Resolve-Path $StyleDll).Path -Destination "$out/ntw16s32.dll"
    $fixture += ' /DWOW_TITLE_POLICY_FIXTURE'
}
if ($Case -eq 'window-borrow' -and $StyleDll) {
    Copy-Item -LiteralPath (Resolve-Path $StyleDll).Path -Destination "$out/ntw16s32.dll"
    $fixture += ' /DWOW_STYLED_TERMINAL_FIXTURE'
}
if ($Case -in @('native-send','native-reply','native-nested','native-seen','native-identity','native-foreign')) { $fixture += ' /DWOW_NATIVE_SEND_FIXTURE' }
if ($Case -eq 'native-reply') { $fixture += ' /DWOW_NATIVE_REPLY_FIXTURE' }
if ($Case -eq 'native-nested') {
    $fixture += ' /DWOW_NATIVE_NESTED_FIXTURE /I"'+$out+'"'
    # Compile the actual current production macro, not a handwritten copy of
    # its exception/finally algorithm. Only the native throwing leaf differs.
    $nativeOwner = Join-Path $repo 'src/wow32-dll/source/wow_user_task_lifecycle.c'
    $nativeText = [IO.File]::ReadAllText($nativeOwner).Replace("`r`n","`n")
    $afterStart = $nativeText.IndexOf('#define WOW_NATIVE_CALL_AFTER(')
    $afterEnd = $nativeText.IndexOf('#define WOW_NATIVE_CALL_RESULT(', $afterStart)
    $messageStart = $nativeText.IndexOf('#define WOW_NATIVE_MESSAGE(')
    $messageEnd = $nativeText.IndexOf('WOW_NATIVE_MESSAGE(SendMessageA,', $messageStart)
    if ($afterStart -lt 0 -or $afterEnd -le $afterStart -or
            $messageStart -lt 0 -or $messageEnd -le $messageStart) {
        throw 'Production native-call macro source shape changed'
    }
    $macroText = $nativeText.Substring($afterStart,$afterEnd-$afterStart) +
        $nativeText.Substring($messageStart,$messageEnd-$messageStart)
    $macroPath = "$out/wow_native_fixture_call.h"
    Set-Content -LiteralPath $macroPath -Value $macroText -Encoding ascii
    @{source=$nativeOwner;sourceSha256=(Get-FileHash $nativeOwner).Hash;
        macrosSha256=(Get-FileHash $macroPath).Hash;nativeLeaf='native_service_fault'} |
        ConvertTo-Json | Set-Content "$out/native-exception-source.json"
}
if ($Case -eq 'native-seen') { $fixture += ' /DWOW_NATIVE_SEEN_FIXTURE' }
if ($Case -eq 'native-identity') { $fixture += ' /DWOW_NATIVE_IDENTITY_FIXTURE' }
if ($Case -eq 'native-foreign') { $fixture += ' /DWOW_NATIVE_FOREIGN_FIXTURE' }
# The thunk fixture supplies original LPFNW32 callbacks. With this profile's
# FASTCALL definition they inherit /Gz, exactly like the production provider.
# Compiling those callbacks as /Gd corrupts the stack at the first invocation.
if ($Case -eq 'thunk-scope') { Invoke-Build ($fixture + ' /we4113 /we4047') }
else { Invoke-Build ($fixture.Replace('/Gz', '/Gd')) }
$staticImports = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\worker-static-imports.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_worker_static_imports.c"')
if ($staticImports -eq $compile) { throw 'Static import substitution failed' }
Invoke-Build ($staticImports.Replace('/Gz', '/Gd'))
$workerObjects += Join-Path $out 'worker-static-imports.obj'
# Linker directives in an otherwise symbol-free archive member are not
# extracted. Select the exact production ABI aliases explicitly instead.
$workerObjects += Join-Path $provider 'obj/wow32-system-import-abi-aliases.obj'
$workerObjects += Join-Path $provider 'obj/wow32-parent-import-abi-aliases.obj'
$parentObjects = @()
if ($RealDialogLifecycleOnly) {
    $workerCommands = & $ninja -C $worker -t commands softpc-bindings.lib
    $pageCompile = @($workerCommands | Where-Object { $_ -match 'mvdm_softpc_wow_page_domain\.c$' })
    if ($pageCompile.Count -ne 1) { throw 'Expected exact real page-domain compile command' }
    $faultCompile = $pageCompile[0] -replace '/Fo\S+\s+\S+mvdm_softpc_wow_page_domain\.c$', ('/Fo"'+$out+'\dialog-page-faults.obj" "'+$repo+'\tests\mvdm-host\wow_dialog_page_faults.c"')
    if ($faultCompile -eq $pageCompile[0]) { throw 'Dialog fault output/source substitution failed' }
    Invoke-CompilerCommand $worker $faultCompile
    $parentObjects += Join-Path $out 'dialog-page-faults.obj'
}
if ($Case -eq 'class-procedure') {
    # Real creation publishes page-domain WND/CLS storage. Keep the same
    # active CCPU driver as other creation rows, not a class-only storage mock.
    $classFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\class-procedure.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_class_procedure_fixture.c"')
    if ($classFixture -eq $compile) { throw 'Class procedure substitution failed' }
    Invoke-Build ($classFixture.Replace('/Gz', '/Gd') + ' /Dmain=wow_class_procedure_fixture')
    $parentObjects += Join-Path $out 'class-procedure.obj'
    $typedFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\typed-procedure-callers.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_typed_procedure_callers.c"')
    if ($typedFixture -eq $compile) { throw 'Typed original caller substitution failed' }
    Invoke-Build $typedFixture
    $parentObjects += Join-Path $out 'typed-procedure-callers.obj'
}
if ($Case -eq 'standard-class') {
    # Same real page-domain owner as the creation and callback suites. Do not
    # link parent-stub definitions beside the selected real CCPU implementation.
    $standardFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\standard-class.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_standard_class_fixture.c"')
    if ($standardFixture -eq $compile) { throw 'Standard-class substitution failed' }
    Invoke-Build ($standardFixture.Replace('/Gz', '/Gd') + ' /Dmain=wow_standard_class_fixture')
    $parentObjects += Join-Path $out 'standard-class.obj'
}
if ($Case -eq 'native-creation') {
    # Real CCPU session/storage setup is shared with the page-domain fixture;
    # publication may not be replaced by a success-only parent stub.
    $creationFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\native-creation.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_native_creation_binding_fixture.c"')
    if ($creationFixture -eq $compile) { throw 'Native creation substitution failed' }
    $creationFlags = ' /Dmain=wow_native_creation_fixture'
    if ($CreationUnwind) { $creationFlags += ' /DWOW_CREATION_UNWIND_FIXTURE' }
    Invoke-Build ($creationFixture.Replace('/Gz', '/Gd') + $creationFlags)
    $parentObjects += Join-Path $out 'native-creation.obj'
}
if ($Case -eq 'window-reclaim') {
    # Separate negative row: the ordinary window-borrow row still selects the
    # unmodified production object. Replace only its free leaf in this row.
    $workerCommands = & $ninja -C $worker -t commands softpc-bindings.lib
    $pageCompile = $workerCommands | Where-Object { $_ -match 'mvdm_softpc_wow_page_domain\.c$' } | Select-Object -Last 1
    if (!$pageCompile) { throw 'Missing production page-domain compile command' }
    $faultCompile = $pageCompile -replace '/Fo\S+\s+\S+mvdm_softpc_wow_page_domain\.c$', ('/Fo"'+$out+'\page-reclaim.obj" "'+$repo+'\tests\mvdm-host\wow_page_domain_reclaim_fixture.c"')
    if ($faultCompile -eq $pageCompile) { throw 'Reclaim boundary substitution failed' }
    Invoke-CompilerCommand $worker $faultCompile
    $parentObjects += Join-Path $out 'page-reclaim.obj'
}
if ($Case -eq 'menu-resource') {
    # Namespace controlled W32 inputs and their exact original resource-client
    # copies. Both this executable and ordinary window-borrow keep production
    # callbacks linked, with no archive-order replacement or FORCE:MULTIPLE.
    & (Join-Path $repo 'tests/observation/extract-wow-menu-fixture.ps1') -BuildRoot $out.Substring($repo.Length + 1) >> "$out\build.log" 2>&1
    $resourceFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\resource-fixture.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_original_menu_resource_fixture.c"')
    if ($resourceFixture -eq $compile) { throw 'Resource fixture substitution failed' }
    # Original cover is /Gz like its WINAPI declaration; the fixture's
    # externally called entry already declares __cdecl.
    Invoke-Build ($resourceFixture + ' /DWOW_RESOURCE_BOUND_FIXTURE /I"'+$out+'"')
    $parentObjects += Join-Path $out 'resource-fixture.obj'
    $thunkFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\menu-thunk-fixture.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_original_menu_thunk_fixture.c"')
    if ($thunkFixture -eq $compile) { throw 'Menu thunk fixture substitution failed' }
    # Extracted original bodies call the production /Gz helpers (including
    # MBToWCSEx). Keep that ABI; the fixture entry explicitly uses __cdecl.
    Invoke-Build ($thunkFixture + ' /DWOW_MENU_THUNK_BOUND_FIXTURE /I"'+$out+'"')
    $parentObjects += Join-Path $out 'menu-thunk-fixture.obj'
}
if ($Case -notin @('window-borrow','window-reclaim','graph-snapshot','menu-resource','native-creation','class-procedure','standard-class','window-procedure')) {
$parentStub = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\parent-stub.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_page_domain_parent_stub.c"')
if ($parentStub -eq $compile) { throw 'Parent-stub substitution failed' }
if ($Case -in @('class-client','class-query','class-lifetime')) { $parentStub += ' /DWOW_CLASS_CLIENT_FIXTURE' }
else { $parentStub += ' /DWOW_TASK_CLASS_PROJECTION_FIXTURE' }
if ($Case -eq 'menu-cleanup') { $parentStub += ' /DWOW_MENU_CLEANUP_FIXTURE' }
Invoke-Build ($parentStub.Replace('/Gz', '/Gd'))
$parentObjects = @((Join-Path $out 'parent-stub.obj'))
    # These native-only fixtures deliberately replace seven storage leaves.
    # Current provider dependencies also reach the real page-domain module;
    # select it explicitly, with only the replaced definitions renamed. Do
    # not allow archive order or /FORCE:MULTIPLE to choose their owners.
    # No CCPU-layout acceptance is claimed by a parent-stub fixture.
    $workerCommands = & $ninja -C $worker -t commands softpc-bindings.lib
    $pageCompile = $workerCommands | Where-Object { $_ -match 'mvdm_softpc_wow_page_domain\.c$' } | Select-Object -Last 1
    if (!$pageCompile) { throw 'Missing production page-domain compile command' }
    $pageFixture = $pageCompile -replace '/Fo\S+', ('/Fo"'+$out+'\page-native-fixture.obj"')
    if ($pageFixture -eq $pageCompile) { throw 'Native page-domain output substitution failed' }
    $replacedLeaves = @('publish_menu','publish_menus','retire_menu',
        'publish_named_class','retire_class','release_class','set_expected_windows_version')
    if ($Case -eq 'process-retirement') { $replacedLeaves += 'handle_metadata' }
    foreach ($leaf in $replacedLeaves) {
        $symbol = 'mvdm_softpc_wow_page_domain_' + $leaf
        $pageFixture += ' /D' + $symbol + '=wow_fixture_real_page_' + $leaf
    }
    Invoke-CompilerCommand $worker $pageFixture
    $parentObjects += Join-Path $out 'page-native-fixture.obj'
}
if ($Case -eq 'init-notification') {
    $startupTd = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\startup-td.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_startup_td_fixture.c"')
    if ($startupTd -eq $compile) { throw 'Startup TD fixture substitution failed' }
    Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_task_cleanup_entry.obj'
    Invoke-Build $startupTd
    $parentObjects += Join-Path $out 'startup-td.obj'
}
$lib = Invoke-CompilerCommand $provider 'where lib.exe' -Capture
$lib = @($lib | Where-Object { Test-Path -LiteralPath $_ })[0]
$link = $commands | Where-Object { $_ -match '/out:wow32\.dll' } | Select-Object -Last 1
if (!$link) { throw 'Missing production provider link command' }
# Only objects actually selected by the DLL graph, never stale directory objects.
$objects = @([regex]::Matches($link, '(?<!\S)obj/[^\s"]+\.obj') |
    ForEach-Object { Join-Path $provider $_.Value } | Select-Object -Unique)
if ($RealDialogLifecycleOnly) {
    $scopeCompile = @($commands | Where-Object { $_ -match 'wow_dialog_finalization_binding\.c$' })
    $scopeObject = @($objects | Where-Object { $_ -match 'wow_dialog_finalization_binding\.obj$' })
    if ($scopeCompile.Count -ne 1 -or $scopeObject.Count -ne 1) { throw 'Expected exact production dialog scope owner' }
    $copy = $scopeCompile[0] -replace '/Fo\S+\s+\S+wow_dialog_finalization_binding\.c$', ('/Fo"'+$out+'\dialog-scope-faults.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_dialog_scope_faults.c"')
    if ($copy -eq $scopeCompile[0]) { throw 'Dialog scope source/output substitution failed' }
    Invoke-Build $copy
    $objects = @($objects | Where-Object { $_ -notin $scopeObject })
    $parentObjects += Join-Path $out 'dialog-scope-faults.obj'
}
if ($objects.Count -lt 77) { throw 'Incomplete production provider object selection' }
# A selected path is not evidence that its existing object is current. The
# support group can change outside the few case-specific targets above.
# Refresh every selected production object before assembling provider.lib;
# otherwise a fixture silently combines the new caller with stale owners.
# Keep command lines below cmd.exe's limit without forcing a DLL link or
# adding unselected source files. Ninja retains normal incremental caching.
$selectedTargets = @($objects | ForEach-Object {
    $_.Substring($provider.Length + 1).Replace('\', '/')
})
for ($first = 0; $first -lt $selectedTargets.Count; $first += 32) {
    $last = [Math]::Min($first + 31, $selectedTargets.Count - 1)
    Invoke-NinjaTarget $provider ($selectedTargets[$first..$last] -join ' ')
}
foreach ($object in $objects) {
    if (!(Test-Path -LiteralPath $object -PathType Leaf)) {
        throw "Missing current selected provider object: $object"
    }
}
# EXE and DLL each own this variable in production. A combined executable
# must preserve two distinct cells, not merge their independent load state.
# Recompile only the provider TU with a private test linkage spelling; its
# original algorithms and all references in that TU remain unchanged.
$redirCompile = $commands | Where-Object { $_ -match '[/\\]wkfileio\.c$' } | Select-Object -Last 1
if (!$redirCompile) { throw 'Missing original provider file-I/O compile command' }
$redirFixture = $redirCompile -replace '/Fo\S+', ('/Fo"'+$out+'\provider-wkfileio.obj"')
if ($redirFixture -eq $redirCompile) { throw 'Provider state namespace substitution failed' }
Invoke-Build ($redirFixture + ' /DVdmRedirLoaded=wow_fixture_provider_VdmRedirLoaded')
$redirObjects = @($objects | Where-Object { $_ -match '[/\\]wkfileio\.obj$' })
if ($redirObjects.Count -ne 1) { throw 'Expected exactly one provider redirector state owner' }
$objects = @($objects | Where-Object { $_ -notin $redirObjects })
$objects += Join-Path $out 'provider-wkfileio.obj'
if ($CreationUnwind) {
    # Same production translation unit, only native-create/terminal leaves
    # are wrapped in this separate fault row. Normal native-creation is intact.
    $creationCompile = $commands | Where-Object { $_ -match 'wow_window_creation_binding\.c$' } | Select-Object -Last 1
    if (!$creationCompile) { throw 'Missing production creation compile command' }
    $unwindCompile = $creationCompile -replace '/Fo\S+\s+\S+wow_window_creation_binding\.c$', ('/Fo"'+$out+'\creation-unwind.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_creation_unwind_injection.c"')
    if ($unwindCompile -eq $creationCompile) { throw 'Creation unwind substitution failed' }
    Invoke-Build $unwindCompile
    $originalCreationObjects = @($objects | Where-Object { $_ -match '[\\/]wow32-dll[\\/]wow_window_creation_binding\.obj$' })
    if ($originalCreationObjects.Count -ne 1) { throw 'Expected exactly one production creation owner' }
    $objects = @($objects | Where-Object { $_ -notin $originalCreationObjects })
    $parentObjects += Join-Path $out 'creation-unwind.obj'
}
if ($Case -in @('window-procedure','native-dispatch-gate')) {
    # Keep one real lifecycle provider and one real (inactive here) page domain.
    # Instrument scope entries without overriding their behavior or linking
    # the partial parent storage mock beside the actual CCPU owner.
    $scopeCompile = $compile -replace '/Fo\S+\s+', ('/Fo"'+$out+'\procedure-scope.obj" ')
    if ($scopeCompile -eq $compile) { throw 'Procedure scope substitution failed' }
    Invoke-Build ($scopeCompile + ' /Dwow_task_callback_enter=wow_fixture_real_callback_enter /Dwow_task_callback_leave=wow_fixture_real_callback_leave')
    $scopeObjects = @($objects | Where-Object { $_ -match '[\\/]wow32-dll[\\/]wow_user_task_lifecycle\.obj$' })
    if ($scopeObjects.Count -ne 1) { throw 'Expected exactly one lifecycle owner' }
    $objects = @($objects | Where-Object { $_ -notin $scopeObjects })
    $parentObjects += Join-Path $out 'procedure-scope.obj'
}
if ($InitRollback) {
    $queueCompile = $commands | Where-Object { $_ -match '/kernel/queue\.c$' } | Select-Object -Last 1
    if (!$queueCompile) { throw 'Missing original InitTask compile command' }
    $queueFixture = $queueCompile -replace '/Fo\S+\s+\S+queue\.c$', ('/Fo"'+$out+'\init-allocation.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_init_allocation_fixture.c"')
    $releaseFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\init-release.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_init_release_fixture.c"')
    if ($queueFixture -eq $queueCompile -or $releaseFixture -eq $compile) { throw 'Init fixture substitution failed' }
    Invoke-Build $queueFixture
    Invoke-Build $releaseFixture
    $objects = @($objects | Where-Object { $_ -notmatch '[\\/]opennt-user[\\/]queue\.obj$|[\\/]wow32-dll[\\/]wow_user_task_lifecycle\.obj$' })
    $parentObjects += @("$out/init-allocation.obj", "$out/init-release.obj")
}
& $lib /nologo "/out:$out\provider.lib" @objects >> "$out\build.log" 2>&1
if ($LASTEXITCODE) { throw 'Provider archive failed' }
# Duplicate providers invalidate the claimed production-owner selection.
# Fault rows above explicitly exclude replaced objects; never let link order
# silently choose a test stub or stale owner over a recovered original body.
Invoke-Build ('link /nologo /NODEFAULTLIB:ntvdm.lib /out:"'+$out+'\fixture.exe" /map:"'+$out+'\fixture.map" "'+$out+'\fixture.obj" '+(($parentObjects | ForEach-Object { '"'+$_+'"' }) -join ' ')+' "'+$out+'\provider.lib" '+((@($workerObjects)+@($fixtureSupportObjects)+@($fixtureLibraries) | ForEach-Object { '"'+$_+'"' }) -join ' ')+' legacy-wow-user32.lib legacy-wow-kernel32.lib legacy-wow-ntdll.lib version.lib comdlg32.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib shell32.lib advapi32.lib ntdll.lib oldnames.lib libcmt.lib libvcruntime.lib libucrt.lib')
if (!(Test-Path -LiteralPath "$out\fixture.exe" -PathType Leaf)) {
    throw "Task cleanup fixture link produced no executable: $out\fixture.exe"
}
$process=Start-Process "$out/fixture.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/stdout.txt" -RedirectStandardError "$out/stderr.txt"
$null=$process.Handle
if(!$process.WaitForExit(60000)) {
    & taskkill /PID $process.Id /T /F
    throw "Task cleanup fixture timeout: case=$Case"
}
$process.WaitForExit()
$process.Refresh()
$fixtureOutput = @(Get-Content "$out/stdout.txt"; Get-Content "$out/stderr.txt")
$fixtureExitCode = $process.ExitCode
$fixtureExitCode | Set-Content -LiteralPath "$out/fixture-exit.txt"
if ($requestedCase -eq 'style-policy' -and
    (!($fixtureOutput -match '^WOW_TITLE_POLICY bound task, Off isolation and dynamic caption checked$') -or
     !($fixtureOutput -match '^WOW_TITLE_POLICY same-runtime Off peer and parent policy retention checked$'))) {
    throw 'Style policy/peer isolation contract failed after real modal prerequisite'
}
if ($OriginalModalOnly) {
    # Dedicated scope, not a substitute pass for the independent whole CCPU
    # window/class/menu matrices. The actual registered lifetime gets its own
    # process so its legitimate process cleanup cannot perturb their baselines.
    $modalModes = if ($StyleDll) {3} else {1}
    if ($RealDialogLifecycleOnly) {
        if ($fixtureExitCode -ne 0 -or
            !($fixtureOutput -match '^WOW_REAL_DIALOG_SCOPE errors=0 callback_reject=1 capture_exception=1 exact_production_finally=1 cache_restored=1 context_restored=1 owner_restored=1$') -or
            !($fixtureOutput -match '^WOW_REAL_DIALOG_ROLLBACK errors=0 cases=4 physical_free=1 publications=2 vetoes=2 no_app_callback=1$') -or
            !($fixtureOutput -match '^WOW_REAL_DIALOG_RESOURCE_OWNER errors=0 normal_consume=1 exit_invoke=1 conflict_rejected=1 reentrant_once=1$') -or
            !($fixtureOutput -match '^WOW_NATIVE_DIALOG_RESOURCE_OWNER errors=0 no_original_fnid=1 resource_once=1$') -or
            !($fixtureOutput -match '^WOW_REAL_DIALOG_LIFECYCLE errors=0 init=3 terminal=2 nested=1 exception_local=1 callback_cache=1 pdlg_retired=1 dying_callback_denied=1$') -or
            !($fixtureOutput -match '^WOW_PAGE_DOMAIN_LIFECYCLE_OK$')) {
            throw 'Real registered-owner dialog lifecycle/physical rollback contract failed'
        }
        return
    }
    if ($fixtureExitCode -ne 0 -or
        !($fixtureOutput -match '^WOW_ORIGINAL_MODAL_CARRIERS errors=0 full=1 zero16=1 signed16=1 original_end=1$') -or
        !($fixtureOutput -match ('^WOW_ORIGINAL_MODAL_CCPU errors=0 modes='+$modalModes+' aw=2 returned=49 pdlg_retired=1$')) -or
        !($fixtureOutput -match '^WOW_ORIGINAL_MODAL_SCOPE real_ccpu=1 registered_owner=1 normal_cleanup=1$') -or
        !($fixtureOutput -match '^WOW_PAGE_DOMAIN_LIFECYCLE_OK$')) {
        throw 'Dedicated original modal manager/registered-owner lifecycle contract failed'
    }
    return
}
if ($Case -eq 'class-client' -and
    !($fixtureOutput -match '^WOW_CLASS_CLIENT errors=0 version_calls=[0-9]+ version_domains=5 server_owner=1 cpd=[0-9]+$')) {
    throw 'Class-client assertions completed without the actual captured marker'
}
if ($Case -eq 'class-lifetime' -and
    !($fixtureOutput -match '^WOW_WINDOW_CLASS_SHARED_LIFETIME errors=0 callbacks=1 native_failure_retains_node=32$')) {
    throw 'Window/class retained storage, replacement or rollback was not verified'
}
if ($Case -eq 'init-notification' -and !($fixtureOutput -match '^WOW_INIT_PRECOMMIT_CLEANUP errors=0 retries=7 registered_cleanup=1$')) {
    throw 'Precommit original-task registered cleanup was not verified'
}
if ($InitRollback -and !($fixtureOutput -match '^WOW_INIT_ROLLBACK errors=0 failed_allocations=2 name_releases=2$')) {
    throw 'Original InitTask rollback/name release/error preservation was not verified'
}
if ($InitRollback -and !($fixtureOutput -match '^WOW_IDLE_OBSERVER_LIFECYCLE errors=0 install_failures=3 live=0$')) {
    throw 'Observer installation failure, rollback or retirement leaked hooks'
}
if ($Case -eq 'graph-snapshot' -and
    (!($fixtureOutput -match '^WOW_EXTERNAL_OBSERVATION_CCPU errors=0 separate_process=1 local_updates=32$') -or
     !($fixtureOutput -match '^WOW_EXTERNAL_REFRESH_BOUNDARY_CCPU errors=0 retained=1 explicit_refresh=1 native_mutation=1 local_fresh=1$') -or
     !($fixtureOutput -match '^WOW_EXTERNAL_CAPTURE_CCPU errors=0 dead_native=1 committed_observation=1$') -or
     !($fixtureOutput -match '^WOW_POST_NATIVE_FAILURE_CCPU errors=0 consumed_once=1 worker_exit=c0000420$'))) {
    throw 'External churn or post-native consumed-message failure boundary was not verified'
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_REMOVED_MESSAGE_CCPU consumed_once=1 restored=1 no_replay=1$')) {
    throw 'Native removed-message result was lost or replayed'
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_RESOURCE_EXECUTION_SCOPE ready=1 restored=1 no_scheduler=1$')) {
    throw 'Native resource-only execution scope was not preserved independently of InitTask'
}
if ($Case -eq 'graph-snapshot' -and
    !($fixtureOutput -match '^WOW_GRAPH_SNAPSHOT_CCPU errors=0 fresh=1 child_id=1 dead_withdrawal=1 retained_reader=1$')) {
    throw 'Whole snapshot failed fresh publication, relation copying or retained-reader retirement'
}
if ($Case -eq 'graph-snapshot' -and
    !($fixtureOutput -match '^WOW_NATIVE_SLOT_REUSE_CCPU errors=0 attempts=[1-9][0-9]* old=[0-9a-f]{8} new=[0-9a-f]{8} old_backing=[0-9a-f]{8} new_backing=[0-9a-f]{8}$')) {
    throw 'Real native handle-slot reuse and retained-generation isolation were not verified'
}
$fixtureOutput | Write-Output
if ($Case -eq 'window-borrow' -and $StyleDll -and
    !($fixtureOutput -match '^WOW_STYLED_TERMINAL_CCPU errors=0 style_before=1 neutralized=1 dialog=1 button=1 gdi=[0-9]+/[0-9]+$')) {
    throw 'Styled real-CCPU terminal cleanup did not prove actual attachment, neutralization and GDI retirement'
}
if ($Case -eq 'graph-snapshot') {
    if (!($fixtureOutput -match '^WOW_CLASS_RETIREMENT_FAILURE_CCPU errors=0 native_removed=1 stale_projection_rejected=1 terminal=1$')) {
        throw 'Irreversible native class removal did not reject stale projection as terminal'
    }
    foreach ($marker in @('WOW_NATIVE_ENTRY_FAILURE_CCPU','WOW_CALLBACK_OWNER_FAILURE_CCPU','WOW_CALLBACK_BORROW_FAILURE_CCPU',
            'WOW_WINDOW_CACHE_FAILURE_CCPU','WOW_DIALOG_CACHE_FAILURE_CCPU')) {
        if (!($fixtureOutput -match ('^' + $marker + ' errors=0 consumed_once=1 worker_exit=c0000420$'))) {
            throw "Missing production callback restoration rejection: $marker"
        }
    }
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_MANAGED_SLOT_REUSE_CCPU errors=0 creation=1 null_class=1 retained=1 released=1$')) {
    throw 'Managed creation or null-class slot reuse failed retained-reader isolation/cleanup'
}
if ($Case -eq 'native-creation' -and
    !($fixtureOutput -match '^WOW_NATIVE_CREATION_BINDING errors=0 .* table_empty=1$')) {
    throw 'Native creation failed explicit/implicit enrollment or final ownership cleanup'
}
if ($CreationUnwind -and
    !($fixtureOutput -match '^WOW_CREATION_UNWIND errors=0 tls=1 hooks=0 scope_release=1 restored=1 retry=1$')) {
    throw 'Creation exception did not prove scope, hook, reference and execution restoration'
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_GRAPH_NATIVE_MUTATION_CCPU errors=0 child_id=1 reparent=1 deferred_geometry=1$')) {
    throw 'Native mutation return did not synchronize child ID, parent or deferred geometry'
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource')) {
    if (!($fixtureOutput -match '^WOW_CONTROL_NATIVE_RETURN_CCPU errors=0 paint=1 scroll=1 caret=1 lock=1$')) {
        throw 'Recovered-control native paint/scroll/caret return ownership failed'
    }
    if (!($fixtureOutput -match '^WOW_MESSAGE_OPERANDS_CCPU errors=0 typed=1 numeric=1 menu=1 position=1 nested=1$')) {
        throw 'Typed message window operands or numeric/menu/structure exclusions failed'
    }
    if (!($fixtureOutput -match '^WOW_RETURNED_WINDOWS_CCPU errors=0 parent=1 owner=1 numeric=1 previous=1 capture=1 dead=1$')) {
        throw 'Returned window identities, parent/owner distinction or capture publication failed'
    }
}
if ($Case -eq 'graph-snapshot') {
    foreach ($held in 0,1) {
        if (!($fixtureOutput -match "^WOW_PROPERTY_DISCOVERY_CCPU errors=0 held=$held atom=1 string=1 signed=1 stop=1 exception=1 removal=1 invalid=1$")) {
            throw "Property callback publication or entry lock restoration failed (held=$held)"
        }
        foreach ($dead in 0,1) {
            if (!($fixtureOutput -match "^WOW_WINDOW_THREAD_QUERY_CCPU errors=0 held=$held dead=$dead full=1 unsigned16=1 signed16=1 native_output=1$")) {
                throw "Historical window identity query/output/lock contract failed (held=$held dead=$dead)"
            }
        }
    }
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_GRAPH_MUTATION_GROUP_CCPU errors=0 class=1 scroll=1 bars=1 iconic=1 popups=1 invalid=1$')) {
    throw 'Native mutation group failed numeric class, scrolling, frame or iconic publication'
}
if ($Case -eq 'window-reclaim' -and
    !($fixtureOutput -match '^WOW_DEFERRED_RECLAIM_CCPU errors=0 consumed_once=1 class_guard=1 retry=1 retained_owner=1 released=1$')) {
    throw 'Failed physical release lost its reclamation owner or consumed a reference twice'
}
if ($Case -eq 'window-reclaim' -and
    !($fixtureOutput -match '^WOW_MENU_DEFERRED_RECLAIM_CCPU errors=0 atomic_update=1 logical_retirement=1 retry=1 released=1$')) {
    throw 'Menu publication/retirement lost deferred physical storage or failed atomicity'
}
if ($Case -eq 'process-retirement' -and
    !($fixtureOutput -match '^WOW_PROCESS_RETIREMENT errors=0 ')) {
    throw 'Production session/provider retirement did not prove release and refusal/retry'
}
if ($Case -eq 'native-dispatch-gate' -and
    (!($fixtureOutput -match '^WOW_NATIVE_DISPATCH_GATE errors=0 native=3 guest-entry=1$') -or
     !($fixtureOutput -match '^WOW_NATIVE_CALLPROC token=FFFF[0-9A-F]{4} calls=2 restored=1$'))) {
    throw 'Native dispatch fixture exited without its successful completion marker'
}
if ($Case -eq 'window-procedure' -and
    !($fixtureOutput -match '^WOW_CLASS_CALLPROC restored=1 isolated=1 errors=0$')) {
    throw 'Class CallProc restoration or window snapshot isolation failed'
}
if ($Case -eq 'window-procedure' -and
    !($fixtureOutput -match '^WOW_WINDOW_PROCEDURE errors=0 tagged_calls=7$')) {
    throw 'Window procedure fixture exited without its successful completion marker'
}
if ($Case -eq 'window-procedure' -and
    !($fixtureOutput -match '^WOW_NATIVE_SUBCLASS errors=0 forwarded=2 preserved=1 missing_owner_rejected=1$')) {
    throw 'Actual native subclass forwarding/restoration or saved guest identity failed'
}
if ($Case -eq 'window-procedure') {
    foreach ($encoding in 0,1) {
        if (!($fixtureOutput -match "^WOW_TRANSFERRED_CALLPROC_LIVE unicode=$encoding text_calls=4 errors=0$") -or
            !($fixtureOutput -match "^WOW_TRANSFERRED_CALLPROC unicode=$encoding source_retired=1 independent_destinations=2 text_calls=4 errors=0$")) {
            throw 'Native CPD encoding, source retirement or independent destination lifetime was not proved'
        }
    }
}
if ($Case -eq 'standard-class' -and
    !($fixtureOutput -match '^WOW_STANDARD_PRIVATE_CLEANUP errors=0$')) {
    throw 'Standard-named private class did not follow original cleanup restoration'
}
if ($Case -eq 'native-creation' -and
    !($fixtureOutput -match '^WOW_CLASS_TOKEN_ORIGIN errors=0 guest_native_same_word=1 native_replaced=1 guest_restored=1$')) {
    throw 'A live native CPD word was not distinguished from the same explicitly guest-typed word'
}
if ($Case -eq 'standard-class' -and
    !($fixtureOutput -match '^WOW_STANDARD_CLEANUP errors=0 restored=9 original_policy=1$')) {
    throw 'Original standard-class cleanup did not restore native procedure identities'
}
if ($Case -eq 'standard-class' -and
    !($fixtureOutput -match '^WOW_STANDARD_CLASS errors=0 resolved=9 original_table=1$')) {
    throw 'Standard class fixture exited without its successful completion marker'
}
if ($Case -eq 'standard-class' -and
    !($fixtureOutput -match '^WOW_STANDARD_STORAGE errors=0 classes=9 precedence=1 retirement=1$')) {
    throw 'Standard control metadata lost shared storage, lookup precedence or native lifetime'
}
if ($Case -eq 'class-query' -and
    !($fixtureOutput -match '^WOW_CLASS_QUERY errors=0 queries=6 logical=1 native=1$')) {
    throw 'Original GetClassInfo client did not preserve logical and native query contracts'
}
if ($Case -eq 'class-procedure' -and
    !($fixtureOutput -match '^WOW_CLASS_PROCEDURE errors=0 callbacks=[0-9]+$')) {
    throw 'Class-only mutation, existing-window isolation or nested future creation failed'
}
if ($Case -eq 'class-procedure' -and
    !($fixtureOutput -match '^WOW_TYPED_PROCEDURE_CALLERS errors=0 actual_wu32=4 guest_ffff=1 setters=4 getters=4$')) {
    throw 'Actual original WU32 get/set callers did not preserve typed guest procedure identity'
}
if ($Case -eq 'class-query' -and
    !($fixtureOutput -match '^WOW_CLASS_COMMIT errors=0 publications=3 rollback=1 original_task=1$')) {
    throw 'Class projection did not commit the completed original registration or roll back failure'
}
if ($Case -eq 'menu-resource' -and
    (!($fixtureOutput -match '^WOW_ORIGINAL_MENU_RESOURCE errors=0 resource_cases=16 parser_cases=7 invalid_repeats=64$') -or
     !($fixtureOutput -match '^WOW_OWNER_DRAW_RESOURCE errors=0$') -or
     !($fixtureOutput -match '^WOW_ORIGINAL_MENU_THUNKS errors=0 cases=6 allocations=0$') -or
     !($fixtureOutput -match '^WOW_NATIVE_MENU_ENTRY_CCPU result=1$'))) {
    throw 'Bound original resource-client acceptance did not complete all resource/parser/ownerdraw cases'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_RESOURCE_MENU_CCPU registered=1 nested=1 rollback=1$')) {
    throw 'Original menu parser did not register and publish its tree or retire failed creation'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_MENU_MESSAGE_CCPU borrowed=1 detached=1 terminal=1 invalid_passthrough=1$')) {
    throw 'Message-carried menu publication lost detached views or changed invalid/terminal dispatch'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_SYSTEM_MENU_CCPU borrowed=1 observer_preserved=1 revert_retired=1$')) {
    throw 'System-menu observation invented creator ownership or failed native revert retirement'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_MENU_TRACKING_CCPU cancelled=1 reentered=1 unlocked=1 mutation_published=1 detached_published=1$')) {
    throw 'Cancelled native popup tracking lost callback mutation or inherited-lock restoration'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_MENU_NATIVE_RETURN_CCPU default_close=1 dispatch_close=1 retired=1 lock_restored=1$')) {
    throw 'Native default processing or dispatch returned with a stale destroyed-menu view'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_MENU_CREATION_ROLLBACK_CCPU result=1 modes=2$')) {
    throw 'Implicit failed creation did not retire menu views and restore both caller lock states'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_ORIGINAL_MENU_ENTRY_CCPU create=1 popup=1 registered=1$')) {
    throw 'Original WOW menu creation thunks bypassed task-associated publication'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_MENU_OBSERVATION_CCPU unowned=1 cleanup_skips=1 native_survives=1 released=1$')) {
    throw 'Menu observation acquired native destruction ownership or leaked its view'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_NATIVE_CREATE_CCPU errors=0 forward=1 subclass=1 veto=1 retired=1 internal_combo=1 internal_dialog=1$')) {
    throw 'Native-class enrollment lost forwarding, subclass chain, internal controls or creation rollback'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_TERMINAL_PREPARE_CCPU errors=0 dialog_suppressed=1 native_button=1 creation_denied=1 callback_denied=1$')) {
    throw 'Terminal preparation failed native control cleanup or guest-entry suppression'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_UNOWNED_VIEW_CCPU errors=0 foreign_root=1 local_unowned=1 class_guard=1 native_untouched=1 released=1$')) {
    throw 'Unowned shared view changed native ownership or left a dangling class/allocation'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_VIEW_PROMOTION_CCPU errors=0 same_backing=1 words=1 stale_rejected=1 ownership=1 released=1$')) {
    throw 'Observed-to-managed projection lost identity, retained words or ownership separation'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_VIEW_RETIREMENT_CCPU errors=0 nested_borrow=1 class_guard=1 consumed_once=1 released=1$')) {
    throw 'Observed retirement lost nested backing/class lifetime or consumed a release twice'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_TERMINAL_RETRY_CCPU errors=0 property_removed=1 table_retained=1 native_gone=1 released=1$')) {
    throw 'Terminal failure lost its table-owned cleanup obligation after native destruction'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_GRAPH_PREPARATION_CCPU errors=0 descriptor_failure=1 missing_desktop=1 unpublished=1 released=1$')) {
    throw 'Graph preparation exposed a partial window or lost rollback ownership'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_DEAD_GRAPH_CCPU errors=0 atomic=1 live_guard=1 retained=1 released=1$')) {
    throw 'Dead graph replacement partially changed surviving edges or lost retained backing'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_VIEW_WORDS_CCPU errors=0 same_alias=1 creator_words=1 duplicate_safe=1 nested_retirement=1$')) {
    throw 'Observed WW enrollment replaced a live alias or lost creation/retirement state'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_DESKTOP_QUERY_CCPU errors=0 single_root=1 class_name=1 retained=1 idempotent=1$')) {
    throw 'Desktop query projection duplicated its root or lost its retained class/name'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_DIALOG_CACHE_CCPU errors=0 nested_destroy=1 released=1$')) {
    throw 'Dialog provider did not preserve real CCPU callback backing across nested destruction'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_WINDOW_LINKS_CCPU atomic=1 rebase=1 intermediate=1 retirement=1 reuse=1$')) {
    throw 'Window relationship transport lost atomicity, identity or retirement protection'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_WINDOW_VIEW_BORROWS_CCPU retained_class=1 release_orders=2 duplicate=1 words=1$')) {
    throw 'Shared window borrow lost retained class/words or released storage prematurely'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_NATIVE_CLASS_CCPU errors=0 projected=9 native_pointers=0 released=1$')) {
    throw 'Native class shared projection leaked native identities, duplicated storage or failed release'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_CLASS_RELEASE_CCPU errors=0 live_rejected=1 borrowed_safe=1 no_resurrection=1 reclaimed=1$')) {
    throw 'Released copied class lost retained readers, allowed resurrection or leaked backing'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_CLASS_REFERENCE_SET_CCPU errors=0 classes=32 live=16 retired_borrowed=16 retained=1 reclaimed=1$')) {
    throw 'Class collection lost its mixed live/retired reference-set or final reclamation proof'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_CLASS_COMMIT_CCPU errors=0 cases=2 original_words=1 original_task=1 released=1$')) {
    throw 'Completed original class registration did not reach real CCPU memory and release it'
}
if ($Case -eq 'init-notification' -and
    !($fixtureOutput -match '^WOW_INIT_NOTIFICATION errors=0 ')) {
    throw 'Init notification fixture exited without its successful completion marker'
}
if ($Case -eq 'start-notification' -and
    !($fixtureOutput -match '^WOW_START_NOTIFICATION .* errors=0$')) {
    throw 'Start notification fixture exited without its successful completion marker'
}
if ($null -eq $fixtureExitCode -or $fixtureExitCode -ne 0) { throw "Task cleanup fixture failed: exit=$fixtureExitCode case=$Case" }
if ($Case -eq 'menu-cleanup' -and
    !($fixtureOutput -match '^WOW_MENU_CLEANUP errors=0$')) {
    throw 'Menu cleanup fixture exited without its successful completion marker'
}
if ($Case -eq 'task-order' -and
    !($fixtureOutput -match '^WOW_ORIGINAL_TASK_ORDER errors=0(?:\s|$)')) {
    throw 'Task order fixture exited without its successful completion marker'
}
if ($Case -eq 'task-order' -and
    !($fixtureOutput -match '^WOW_ORIGINAL_TASK_ELIGIBILITY errors=0 persistent=1 events_preserved=1 lock=1 idle=1 hardware=1$')) {
    throw 'Persistent native-stack eligibility did not preserve original selection/count/lock/hardware behavior'
}
if ($Case -in @('native-send','native-reply','native-nested','native-seen','native-identity','native-foreign') -and
    !($fixtureOutput -match '^WOW_NATIVE_EXECUTION_SCOPES errors=0 inline=1 nested=1 wait=1 unbind_rejected=1$')) {
    throw 'Actual native execution scopes did not preserve nested eligibility and retirement refusal'
}
if ($Case -eq 'graph-snapshot' -and
    !($fixtureOutput -match '^WOW_EXPORTED_CALLBACK_REJECT errors=0 output_untouched=1 no_guest_entry=1$')) {
    throw 'Original exported callback argument rejection did not preserve its output contract'
}
if ($Case -eq 'native-nested') {
    foreach ($held in 0,1) {
        if (!($fixtureOutput -match ("^WOW_NATIVE_RECEIVER_EXIT_RETURN held="+$held+" error=(0|1400|1460) retired=1 owner=1$"))) {
            throw 'Actual native send did not restore ownership after receiver thread retirement'
        }
        if (!($fixtureOutput -match ("^WOW_NATIVE_CALLBACK_EXCEPTION_RETURN held="+$held+" caught_in_callback=1 result=117 owner=1$"))) {
            throw 'Native callback-local exception handling lost result/error/execution ownership'
        }
        if (!($fixtureOutput -match ("^WOW_NATIVE_EXCEPTION_RETURN held="+$held+" caught=1 owner=1 production_macro=1$"))) {
            throw 'Native exception did not unwind through the actual wrapper with its original owner'
        }
        if (!($fixtureOutput -match ("^WOW_NATIVE_TIMEOUT_RETURN held="+$held+" error=(0|1460) callbacks="+($held+1)+"$"))) {
            throw 'Native timeout did not preserve callback completion and caller lock state'
        }
    }
    if (!($fixtureOutput -match '^WOW_NATIVE_DEAD_TARGET_RETURN error=1400 owner=1$')) {
        throw 'Destroyed native receiver did not preserve failure and execution ownership'
    }
    if (!($fixtureOutput -match '^WOW_NATIVE_DESTROY_DURING_RETURN result=116 dead=1 owner=1$')) {
        throw 'Receiver window destroyed during the native call did not preserve return ownership'
    }
}
Write-Output ("WOW_FIXTURE_OK case=" + $Case)
