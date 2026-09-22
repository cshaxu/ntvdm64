param(
    [Parameter(Mandatory=$true)][string]$ProviderBuildRoot,
    [Parameter(Mandatory=$true)][string]$WorkerBuildRoot,
    [string]$BuildRoot,
    [string]$Ninja = 'ninja.exe',
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Links current production owner objects; this is not real-Win16 acceptance.
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$vs = (Resolve-Path -LiteralPath $VsDevCmd).Path

$provider = (Resolve-Path -LiteralPath $ProviderBuildRoot).Path
$worker = (Resolve-Path -LiteralPath $WorkerBuildRoot).Path
if (!$BuildRoot) { $BuildRoot = Join-Path $repo 'build/M0-T420/S42/task-cleanup-failure' }
$out = [IO.Path]::GetFullPath($BuildRoot)
New-Item -ItemType Directory -Path $out -Force | Out-Null
function Invoke-Build([string]$command) {
    & cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cd /d "'+$provider+'" && '+$command) >> "$out\build.log" 2>&1
    if ($LASTEXITCODE) { Get-Content "$out\build.log" -Tail 25; throw 'Build command failed' }
}
$commands = & $ninja -C $provider -t commands wow32.dll
$compile = $commands | Where-Object { $_ -match 'wow_user_task_lifecycle\.c$' } | Select-Object -Last 1
if (!$compile) { throw 'Missing production compile command' }
Invoke-Build $compile
$fixture = $compile -replace '/Fo\S+\s+\S+wow_user_task_lifecycle\.c$', ('/Fo"'+$out+'\fixture.obj" "'+$repo+'\tests\adapter-mvdm-host-out\wow\wow_user_task_lifecycle_fixture.c"')
if ($fixture -eq $compile) { throw 'Fixture substitution failed' }
Invoke-Build ($fixture.Replace('/Gz', '/Gd'))
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
Invoke-Build ('link /nologo /out:"'+$out+'\fixture.exe" /map:"'+$out+'\fixture.map" "'+$out+'\fixture.obj" "'+$out+'\provider.lib" "'+$worker+'\wow-worker-bindings.lib" "'+$worker+'\session.lib" "'+$worker+'\original-opennt-rtl-x86.lib" "'+$worker+'\softpc-win32-bindings.lib" kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
& "$out\fixture.exe"
if ($LASTEXITCODE) { throw 'Task cleanup fixture failed' }
