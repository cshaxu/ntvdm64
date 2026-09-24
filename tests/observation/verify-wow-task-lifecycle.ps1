param(
    [Parameter(Mandatory=$true)][string]$ProviderBuildRoot,
    [Parameter(Mandatory=$true)][string]$WorkerBuildRoot,
    [string]$BuildRoot,
    [ValidateSet('task-lifecycle','native-send','native-reply','native-nested','native-seen','task-order','class-client','window-borrow','thunk-scope')] [string]$Case = 'task-lifecycle',
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
    (Join-Path $worker 'obj/adapter-wow-worker/wow_gdi_alias.obj'),
    (Join-Path $worker 'obj/adapter-wow-worker/wow_user_runtime.obj'),
    (Join-Path $worker 'obj/adapter-wow-worker/wow_user_session_binding.obj'),
    (Join-Path $worker 'obj/ntvdm/session/guest_memory_lease.obj'),
    (Join-Path $worker 'obj/ntvdm/session/session.obj')
)
foreach ($object in $workerObjects) {
    $target = $object.Substring($worker.Length + 1).Replace('\', '/')
    Invoke-NinjaTarget $worker $target
}
foreach ($object in $workerObjects) {
    if (!(Test-Path -LiteralPath $object -PathType Leaf)) {
        throw "Missing selected worker object: $object"
    }
}
$fixtureSupportObjects = @(
    (Join-Path $worker 'obj/tests/ccpu_host_fixture_seams.obj'),
    (Join-Path $worker 'obj/host/softpc-resource.res')
)
foreach ($object in $fixtureSupportObjects) {
    if (!(Test-Path -LiteralPath $object -PathType Leaf)) {
        throw "Missing selected fixture support object: $object"
    }
}
$fixtureLibraries = @(
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
    'original-opennt-rtl-x86.lib', 'softpc-fixture-bindings.lib',
    'vdmredir-dll-bindings.lib', 'vdd-bindings.lib', 'softpc-win32-bindings.lib',
    'monitor-bindings.lib', 'kernel-vdm-printer.lib', 'debugger-bindings.lib',
    'session.lib', 'mvdm-softpc-effective-address.lib',
    'softpc-ccpu-vector-defaults.lib', 'softpc-activity-check.lib',
    'original-ccpu386.lib'
)
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
if ($Case -eq 'task-order') { $fixtureName = 'wow_task_order_fixture.c' }
if ($Case -eq 'thunk-scope') { $fixtureName = 'wow_user_thunk_scope_fixture.c' }
$fixturePath = Join-Path $repo ('tests/adapter-mvdm-host-out/wow/' + $fixtureName)
if ($Case -eq 'window-borrow') { $fixturePath = Join-Path $repo 'tests/mvdm-host/wow_page_domain_fixture.c' }
$fixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\fixture.obj" "'+$fixturePath+'"')
if ($fixture -eq $compile) { throw 'Fixture substitution failed' }
if ($Case -eq 'window-borrow') { $fixture += ' /DWOW_WINDOW_BORROW_FIXTURE' }
if ($Case -eq 'task-order') { $fixture += ' /we4113 /we4047' }
if ($Case -in @('native-send','native-reply','native-nested','native-seen')) { $fixture += ' /DWOW_NATIVE_SEND_FIXTURE' }
if ($Case -eq 'native-reply') { $fixture += ' /DWOW_NATIVE_REPLY_FIXTURE' }
if ($Case -eq 'native-nested') { $fixture += ' /DWOW_NATIVE_NESTED_FIXTURE' }
if ($Case -eq 'native-seen') { $fixture += ' /DWOW_NATIVE_SEEN_FIXTURE' }
Invoke-Build ($fixture.Replace('/Gz', '/Gd'))
$parentObjects = @()
if ($Case -ne 'window-borrow') {
$parentStub = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\parent-stub.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_page_domain_parent_stub.c"')
if ($parentStub -eq $compile) { throw 'Parent-stub substitution failed' }
if ($Case -eq 'class-client') { $parentStub += ' /DWOW_CLASS_CLIENT_FIXTURE' }
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
Invoke-Build ('link /nologo /force:multiple /NODEFAULTLIB:ntvdm.lib /out:"'+$out+'\fixture.exe" /map:"'+$out+'\fixture.map" "'+$out+'\fixture.obj" '+(($parentObjects | ForEach-Object { '"'+$_+'"' }) -join ' ')+' "'+$out+'\provider.lib" '+((@($workerObjects)+@($fixtureSupportObjects)+@($fixtureLibraries) | ForEach-Object { '"'+$_+'"' }) -join ' ')+' kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
if (!(Test-Path -LiteralPath "$out\fixture.exe" -PathType Leaf)) {
    throw "Task cleanup fixture link produced no executable: $out\fixture.exe"
}
$fixtureOutput = @(& "$out\fixture.exe")
$fixtureExitCode = $LASTEXITCODE
$fixtureOutput | Write-Output
if ($fixtureExitCode) { throw "Task cleanup fixture failed: exit=$fixtureExitCode case=$Case" }
if ($Case -eq 'task-order' -and
    !($fixtureOutput -match '^WOW_ORIGINAL_TASK_ORDER errors=0(?:\s|$)')) {
    throw 'Task order fixture exited without its successful completion marker'
}
Write-Output ("WOW_FIXTURE_OK case=" + $Case)
