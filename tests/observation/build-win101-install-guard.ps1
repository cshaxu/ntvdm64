param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$FormalCache='build/M0-T434/S3/r001-formal',
    [string]$Baseline='build/M0-T434/S5/r037-runtime/system32/ntvdm.exe',
    [string]$ProbeSource='',
    [ValidateSet('win101_guard_entry','win31_console_trace_entry','win101_palette_trace_entry')][string]$EntrySymbol='win101_guard_entry'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)) {
    throw 'Fresh build-owned output required'
}
$cache=(Resolve-Path (Join-Path $repo $FormalCache)).Path
$baselinePath=(Resolve-Path (Join-Path $repo $Baseline)).Path
if ((Get-FileHash "$cache/ntvdm.exe").Hash -ne (Get-FileHash $baselinePath).Hash) {
    throw 'Formal cache differs from selected package'
}
$graph=Get-Content "$cache/build.ninja" -Raw
$match=[regex]::Match($graph,'(?m)^build ntvdm\.exe[^\r\n]*: worker_link (?<inputs>[^\r\n|]+)')
if (!$match.Success) { throw 'Missing formal NTVDM link edge' }
$inputs=@($match.Groups['inputs'].Value.Trim() -split '\s+' | ForEach-Object {
    $path=Join-Path $cache $_
    if (!(Test-Path -LiteralPath $path)) { throw "Missing cache input $path" }
    '"'+$path+'"'
})
New-Item -ItemType Directory -Path $build | Out-Null
if(!$ProbeSource){$ProbeSource="$PSScriptRoot/win101_install_write_guard.c"}
$ProbeSource=(Resolve-Path $ProbeSource).Path
if(!$ProbeSource.StartsWith((Join-Path $repo 'tests')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Test-owned probe source required'}
$sources=@($ProbeSource)
if([IO.Path]::GetExtension($ProbeSource) -ine '.c'){throw 'C diagnostic source required'}
foreach($name in @('startup_hooks.c','minhook/src/buffer.c','minhook/src/trampoline.c','minhook/src/hde/hde32.c')){
    $sources+=Join-Path $repo "src/lib-win32/process-hook/$name"
}
$commands=@('@echo off',
    'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
    'if errorlevel 1 exit /b %errorlevel%')
$objects=@()
foreach($source in $sources) {
    $obj=Join-Path $build ([IO.Path]::GetFileNameWithoutExtension($source)+'.obj')
    $flags='/std:c11 /FI"'+$repo+'/src/compat/include/process_hook_msvc.h"'
    $commands+='cl.exe /nologo /MT '+$flags+' /c /Fo"'+$obj+'" "'+$source+'"'
    $commands+='if errorlevel 1 exit /b %errorlevel%'
    $objects+='"'+$obj+'"'
}
$response=@('/nologo','/machine:x86','/subsystem:console','/opt:ref',('/include:_'+$EntrySymbol),
    ('/out:"'+$build+'\ntvdm.exe"'),('/map:"'+$build+'\ntvdm.exe.map"'),
    ('/implib:"'+$build+'\ntvdm.lib"'),('/def:"'+$cache+'\generated\ntvdm-wow32-provider.def"'))
$response+=$objects
$response+=$inputs
$response+=@('rpcrt4.lib','kernel32.lib','user32.lib','gdi32.lib','advapi32.lib','ntdll.lib','libcmt.lib','libvcruntime.lib','libucrt.lib')
[IO.File]::WriteAllLines("$build/link.rsp",$response,[Text.Encoding]::UTF8)
$commands+='link.exe @"'+$build+'\link.rsp"'
$commands+='if errorlevel 1 exit /b %errorlevel%'
[IO.File]::WriteAllLines("$build/build.cmd",$commands,[Text.Encoding]::ASCII)
& "$build/build.cmd" *> "$build/build.log"
if ($LASTEXITCODE) { throw "Guard build failed; inspect $build/build.log" }
[ordered]@{
    role=$(if($EntrySymbol -eq 'win101_guard_entry'){'test-only-restricted-write-relink-not-product'}elseif($EntrySymbol -eq 'win101_palette_trace_entry'){'test-only-palette-trace-relink-not-product'}else{'test-only-console-trace-relink-not-product'})
    baselineSha256=(Get-FileHash $baselinePath).Hash
    formalCache=$cache
    sources=@($sources | ForEach-Object { @{path=$_;sha256=(Get-FileHash $_).Hash} })
    includedGuardSha256=$(if([IO.Path]::GetFileName($ProbeSource) -ieq 'win31_setup_write_guard.c'){(Get-FileHash "$PSScriptRoot/win101_install_write_guard.c").Hash}else{$null})
    executableSha256=(Get-FileHash "$build/ntvdm.exe").Hash
    unchangedLinkInputs=@($inputs)
} | ConvertTo-Json -Depth 6 | Set-Content "$build/build.json"
'PASS test-only NTVDM write guard link; never publish this image'
