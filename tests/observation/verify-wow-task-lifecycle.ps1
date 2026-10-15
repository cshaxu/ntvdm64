param(
    [Parameter(Mandatory=$true)][string]$ProviderBuildRoot,
    [Parameter(Mandatory=$true)][string]$WorkerBuildRoot,
    [string]$BuildRoot,
    [ValidateSet('task-lifecycle','menu-cleanup','native-dispatch-gate','window-procedure','standard-class','class-query','dialog-lifecycle','init-notification','start-notification','style-policy','native-send','native-reply','native-nested','native-seen','native-identity','native-foreign','task-order','class-client','window-borrow','thunk-scope')] [string]$Case = 'task-lifecycle',
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
Invoke-Build $compile
$fixtureName = if ($Case -eq 'class-client') { 'wow_class_client_fixture.c' } else { 'wow_user_task_lifecycle_fixture.c' }
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
$fixturePath = Join-Path $repo ('tests/adapter-mvdm-host-out/wow/' + $fixtureName)
if ($Case -eq 'window-borrow') { $fixturePath = Join-Path $repo 'tests/mvdm-host/wow_page_domain_fixture.c' }
$fixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\fixture.obj" "'+$fixturePath+'"')
if ($fixture -eq $compile) { throw 'Fixture substitution failed' }
if ($Case -eq 'window-borrow') { $fixture += ' /DWOW_WINDOW_BORROW_FIXTURE' }
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
Invoke-Build ($fixture.Replace('/Gz', '/Gd'))
$staticImports = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\worker-static-imports.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_worker_static_imports.c"')
if ($staticImports -eq $compile) { throw 'Static import substitution failed' }
Invoke-Build ($staticImports.Replace('/Gz', '/Gd'))
$workerObjects += Join-Path $out 'worker-static-imports.obj'
# Linker directives in an otherwise symbol-free archive member are not
# extracted. Select the exact production ABI aliases explicitly instead.
$workerObjects += Join-Path $provider 'obj/wow32-system-import-abi-aliases.obj'
$workerObjects += Join-Path $provider 'obj/wow32-parent-import-abi-aliases.obj'
$parentObjects = @()
if ($Case -ne 'window-borrow') {
$parentStub = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\parent-stub.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_page_domain_parent_stub.c"')
if ($parentStub -eq $compile) { throw 'Parent-stub substitution failed' }
if ($Case -in @('class-client','class-query')) { $parentStub += ' /DWOW_CLASS_CLIENT_FIXTURE' }
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
$fixtureOutput | Write-Output
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
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_NATIVE_CLASS_CCPU errors=0 projected=9 native_pointers=0 released=1$')) {
    throw 'Native class shared projection leaked native identities, duplicated storage or failed release'
}
if ($Case -eq 'window-borrow' -and
    !($fixtureOutput -match '^WOW_CLASS_COMMIT_CCPU errors=0 cases=2 original_words=1 original_task=1 released=1$')) {
    throw 'Completed original class registration did not reach real CCPU memory and release it'
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
