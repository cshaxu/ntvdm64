param([string]$BuildRoot='build/M0-T422/S2/user-session-binding')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out=[IO.Path]::GetFullPath($BuildRoot, $repo)
$buildPrefix=[IO.Path]::GetFullPath((Join-Path $repo 'build')) + [IO.Path]::DirectorySeparatorChar
if (!$out.StartsWith($buildPrefix,[StringComparison]::OrdinalIgnoreCase)) { throw 'Build root must be under build' }
New-Item -ItemType Directory -Path $out -Force | Out-Null
$sources=@('test/adapter-mvdm-host-out/wow/wow_user_session_binding_fixture.c',
    'src/compat/wow_user_runtime.c', 'src/compat/wow_worker_state.c',
    'src/compat/wow_user_session_binding.c',
    'src/ntvdm-exe/session/session.c', 'src/compat/host_context.c',
    'src/compat/guest_memory_lease.c')
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$args=($sources | ForEach-Object {'"'+(Join-Path $repo $_)+'"'}) -join ' '
# Actual production carrier sources, with test-owned operations at the lifecycle binding.
# This host fixture makes no guest/CCPU or complete provider-teardown claim.
& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x86 >nul && cd /d "'+$out+'" && cl /nologo /W4 /MT /I"'+$repo+'\src" /I"'+$repo+'\src\ntvdm-exe\wow\include" /I"'+$repo+'\src\compat\include" /Fefixture.exe '+$args+'')
if ($LASTEXITCODE) { throw 'Session binding fixture build failed' }
& (Join-Path $out 'fixture.exe')
if ($LASTEXITCODE) { throw 'Session binding fixture failed' }

# The inner carrier must also link and run without session or WOW providers.
& cmd.exe /d /s /c ('call "'+$vs+'" -arch=x86 -host_arch=x86 >nul && cd /d "'+$out+'" && cl /nologo /W4 /MT /I"'+$repo+'\src\compat\include" /Feruntime.exe "'+$repo+'\test\compat\wow_user_runtime_fixture.c" "'+$repo+'\src\compat\wow_user_runtime.c"')
if ($LASTEXITCODE) { throw 'Independent runtime fixture build failed' }
& (Join-Path $out 'runtime.exe')
if ($LASTEXITCODE) { throw 'Independent runtime fixture failed' }
