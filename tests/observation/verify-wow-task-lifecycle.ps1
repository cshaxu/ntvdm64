param(
    [Parameter(Mandatory=$true)][string]$ProviderBuildRoot,
    [Parameter(Mandatory=$true)][string]$WorkerBuildRoot,
    [string]$BuildRoot,
    [ValidateSet('task-lifecycle','process-retirement','menu-cleanup','menu-resource','native-dispatch-gate','window-procedure','standard-class','class-query','dialog-lifecycle','init-notification','start-notification','style-policy','native-send','native-reply','native-nested','native-seen','native-identity','native-foreign','task-order','class-client','window-borrow','window-reclaim','graph-snapshot','thunk-scope','native-creation')] [string]$Case = 'task-lifecycle',
    [string]$StyleDll,
    [string]$Ninja = 'ninja.exe',
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Links current production owner objects; this is not real-Win16 acceptance.
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$vs = (Resolve-Path -LiteralPath $VsDevCmd).Path

$provider = (Resolve-Path -LiteralPath $ProviderBuildRoot).Path
$worker = (Resolve-Path -LiteralPath $WorkerBuildRoot).Path
if (!$BuildRoot) { $BuildRoot = Join-Path $repo ('build/M0-T422/S2/' + $Case) }
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith(($repo + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Lifecycle fixture output must be under repository build.'
}
New-Item -ItemType Directory -Path $out -Force | Out-Null
function Invoke-Build([string]$command) {
    & cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$provider+'" && '+$command) >> "$out\build.log" 2>&1
    if ($LASTEXITCODE) { Get-Content "$out\build.log" -Tail 25; throw 'Build command failed' }
}
function Invoke-NinjaTarget([string]$directory, [string]$target) {
    & cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$directory+'" && "'+$Ninja+'" '+$target) >> "$out\build.log" 2>&1
    if ($LASTEXITCODE) { Get-Content "$out\build.log" -Tail 25; throw "Ninja target failed: $target" }
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
    (Join-Path $worker 'obj/tests/ccpu_host_fixture_seams.obj'),
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
Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_user_task_lifecycle.obj'
$fixtureName = if ($Case -eq 'class-client') { 'wow_class_client_fixture.c' } else { 'wow_user_task_lifecycle_fixture.c' }
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
if ($Case -eq 'dialog-lifecycle') {
    Invoke-NinjaTarget $provider 'obj/wow32-dll/wow_dialog_creation_binding.obj obj/wow32-dll/wow_window_creation_binding.obj'
    $fixtureName = 'wow_user_task_dialog_lifecycle_fixture.c'
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
$fixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\fixture.obj" "'+$fixturePath+'"')
if ($fixture -eq $compile) { throw 'Fixture substitution failed' }
if ($Case -eq 'native-creation') { $fixture += ' /DWOW_WINDOW_BORROW_FIXTURE /DWOW_NATIVE_CREATION_FIXTURE' }
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
if ($Case -in @('native-send','native-reply','native-nested','native-seen','native-identity','native-foreign')) { $fixture += ' /DWOW_NATIVE_SEND_FIXTURE' }
if ($Case -eq 'native-reply') { $fixture += ' /DWOW_NATIVE_REPLY_FIXTURE' }
if ($Case -eq 'native-nested') { $fixture += ' /DWOW_NATIVE_NESTED_FIXTURE' }
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
if ($Case -eq 'native-creation') {
    # Real CCPU session/storage setup is shared with the page-domain fixture;
    # publication may not be replaced by a success-only parent stub.
    $creationFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\native-creation.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_native_creation_binding_fixture.c"')
    if ($creationFixture -eq $compile) { throw 'Native creation substitution failed' }
    Invoke-Build ($creationFixture.Replace('/Gz', '/Gd') + ' /Dmain=wow_native_creation_fixture')
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
    & cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$worker+'" && '+$faultCompile) >> "$out\build.log" 2>&1
    if ($LASTEXITCODE) { throw 'Reclaim boundary build failed' }
    $parentObjects += Join-Path $out 'page-reclaim.obj'
}
if ($Case -eq 'menu-resource') {
    # Controlled W32 resource inputs only in this separate executable. Ordinary
    # window-borrow acceptance continues to link production resource callbacks.
    $resourceFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\resource-fixture.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_original_menu_resource_fixture.c"')
    if ($resourceFixture -eq $compile) { throw 'Resource fixture substitution failed' }
    Invoke-Build ($resourceFixture.Replace('/Gz', '/Gd') + ' /DWOW_RESOURCE_BOUND_FIXTURE')
    $parentObjects += Join-Path $out 'resource-fixture.obj'
    & (Join-Path $repo 'tests/observation/extract-wow-menu-fixture.ps1') -BuildRoot $out.Substring($repo.Length + 1) >> "$out\build.log" 2>&1
    $thunkFixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\menu-thunk-fixture.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_original_menu_thunk_fixture.c"')
    if ($thunkFixture -eq $compile) { throw 'Menu thunk fixture substitution failed' }
    # Extracted original bodies call the production /Gz helpers (including
    # MBToWCSEx). Keep that ABI; the fixture entry explicitly uses __cdecl.
    Invoke-Build ($thunkFixture + ' /DWOW_MENU_THUNK_BOUND_FIXTURE /I"'+$out+'"')
    $parentObjects += Join-Path $out 'menu-thunk-fixture.obj'
}
if ($Case -notin @('window-borrow','window-reclaim','graph-snapshot','menu-resource','native-creation')) {
$parentStub = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\parent-stub.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_page_domain_parent_stub.c"')
if ($parentStub -eq $compile) { throw 'Parent-stub substitution failed' }
if ($Case -in @('class-client','class-query')) { $parentStub += ' /DWOW_CLASS_CLIENT_FIXTURE' }
else { $parentStub += ' /DWOW_TASK_CLASS_PROJECTION_FIXTURE' }
if ($Case -eq 'menu-cleanup') { $parentStub += ' /DWOW_MENU_CLEANUP_FIXTURE' }
Invoke-Build ($parentStub.Replace('/Gz', '/Gd'))
$parentObjects = @((Join-Path $out 'parent-stub.obj'))
}
$lib = & cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && where lib.exe')
$lib = @($lib | Where-Object { Test-Path -LiteralPath $_ })[0]
$link = $commands | Where-Object { $_ -match '/out:wow32\.dll' } | Select-Object -Last 1
if (!$link) { throw 'Missing production provider link command' }
# Only objects actually selected by the DLL graph, never stale directory objects.
$objects = @([regex]::Matches($link, '(?<!\S)obj/[^\s"]+\.obj') |
    ForEach-Object { Join-Path $provider $_.Value } | Select-Object -Unique)
if ($objects.Count -lt 77) { throw 'Incomplete production provider object selection' }
& $lib /nologo "/out:$out\provider.lib" @objects >> "$out\build.log" 2>&1
if ($LASTEXITCODE) { throw 'Provider archive failed' }
Invoke-Build ('link /nologo /force:multiple /NODEFAULTLIB:ntvdm.lib /out:"'+$out+'\fixture.exe" /map:"'+$out+'\fixture.map" "'+$out+'\fixture.obj" '+(($parentObjects | ForEach-Object { '"'+$_+'"' }) -join ' ')+' "'+$out+'\provider.lib" '+((@($workerObjects)+@($fixtureSupportObjects)+@($fixtureLibraries) | ForEach-Object { '"'+$_+'"' }) -join ' ')+' legacy-wow-user32.lib legacy-wow-kernel32.lib legacy-wow-ntdll.lib version.lib comdlg32.lib rpcrt4.lib kernel32.lib user32.lib gdi32.lib shell32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
if (!(Test-Path -LiteralPath "$out\fixture.exe" -PathType Leaf)) {
    throw "Task cleanup fixture link produced no executable: $out\fixture.exe"
}
$process=Start-Process "$out/fixture.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/stdout.txt" -RedirectStandardError "$out/stderr.txt"
$null=$process.Handle
if(!$process.WaitForExit(60000)) {
    & taskkill /PID $process.Id /T /F
    throw "Task cleanup fixture timeout: case=$Case"
}
$process.Refresh()
$fixtureOutput = @(Get-Content "$out/stdout.txt"; Get-Content "$out/stderr.txt")
$fixtureExitCode = $process.ExitCode
$fixtureExitCode | Set-Content -LiteralPath "$out/fixture-exit.txt"
if ($Case -eq 'graph-snapshot' -and
    !($fixtureOutput -match '^WOW_GRAPH_SNAPSHOT_CCPU errors=0 fresh=1 child_id=1 dead_withdrawal=1 retained_reader=1$')) {
    throw 'Whole snapshot failed fresh publication, relation copying or retained-reader retirement'
}
if ($Case -eq 'graph-snapshot' -and
    !($fixtureOutput -match '^WOW_NATIVE_SLOT_REUSE_CCPU errors=0 attempts=[1-9][0-9]* old=[0-9a-f]{8} new=[0-9a-f]{8} old_backing=[0-9a-f]{8} new_backing=[0-9a-f]{8}$')) {
    throw 'Real native handle-slot reuse and retained-generation isolation were not verified'
}
$fixtureOutput | Write-Output
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_MANAGED_SLOT_REUSE_CCPU errors=0 creation=1 null_class=1 retained=1 released=1$')) {
    throw 'Managed creation or null-class slot reuse failed retained-reader isolation/cleanup'
}
if ($Case -eq 'native-creation' -and
    !($fixtureOutput -match '^WOW_NATIVE_CREATION_BINDING errors=0 .* table_empty=1$')) {
    throw 'Native creation failed explicit/implicit enrollment or final ownership cleanup'
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_GRAPH_NATIVE_MUTATION_CCPU errors=0 child_id=1 reparent=1 deferred_geometry=1$')) {
    throw 'Native mutation return did not synchronize child ID, parent or deferred geometry'
}
if ($Case -in @('window-borrow','window-reclaim','graph-snapshot','menu-resource') -and
    !($fixtureOutput -match '^WOW_GRAPH_MUTATION_GROUP_CCPU errors=0 class=1 scroll=1 bars=1 iconic=1 popups=1 invalid=1$')) {
    throw 'Native mutation group failed numeric class, scrolling, frame or iconic publication'
}
if ($Case -eq 'window-reclaim' -and
    !($fixtureOutput -match '^WOW_DEFERRED_RECLAIM_CCPU errors=0 consumed_once=1 class_guard=1 retry=1 retained_owner=1 released=1$')) {
    throw 'Failed physical release lost its reclamation owner or consumed a reference twice'
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
    !($fixtureOutput -match '^WOW_WINDOW_PROCEDURE errors=0 tagged_calls=3$')) {
    throw 'Window procedure fixture exited without its successful completion marker'
}
if ($Case -eq 'window-procedure' -and
    !($fixtureOutput -match '^WOW_NATIVE_SUBCLASS errors=0 forwarded=3 preserved=1 missing_owner_rejected=1$')) {
    throw 'Native subclass carrier replaced or bypassed the logical WOW procedure owner'
}
if ($Case -eq 'standard-class' -and
    !($fixtureOutput -match '^WOW_STANDARD_PRIVATE_CLEANUP errors=0$')) {
    throw 'Standard-named private class did not follow original cleanup restoration'
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
    !($fixtureOutput -match '^WOW_GRAPH_PREPARATION_CCPU errors=0 descriptor_failure=1 stale_snapshot=1 unpublished=1 released=1$')) {
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
    !($fixtureOutput -match '^WOW_CLASS_COMMIT_CCPU errors=0 cases=2 original_words=1 original_task=1 released=1$')) {
    throw 'Completed original class registration did not reach real CCPU memory and release it'
}
if ($Case -eq 'dialog-lifecycle' -and
    !($fixtureOutput -match '^WOW_DIALOG_CALLBACK_CACHE errors=0 paired=1 exceptions=1$')) {
    throw 'Dialog callback cache did not restore across nesting, failure and exception'
}
if ($Case -eq 'dialog-lifecycle' -and
    !($fixtureOutput -match '^WOW_CREATION_ROLLBACK cases=4 errors=0$')) {
    throw 'Dialog creation rollback did not release its owned WW allocation'
}
if ($Case -eq 'dialog-lifecycle' -and
    !($fixtureOutput -match '^WOW_USER_TASK_DIALOG_LIFECYCLE errors=0 init=2 terminal=2$')) {
    throw 'Dialog fixture exited without its successful completion marker'
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
Write-Output ("WOW_FIXTURE_OK case=" + $Case)
