[CmdletBinding()]
param(
    [string]$BuildCache='build/.ninja/msvc-x86-release/worker',
    [Parameter(Mandatory)][string]$OutputRoot,
    [Parameter(Mandatory)][string]$VsDevCmd
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cache=(Resolve-Path (Join-Path $repo $BuildCache)).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
$vs=(Resolve-Path -LiteralPath $VsDevCmd).Path
$graph=Get-Content "$cache/build.ninja"
$flags=($graph|Where-Object {$_ -like 'host_cflags = *'}).Substring(14).Replace('$:',':')
$release=($graph|Where-Object {$_ -like 'release_cflags = *'}).Substring(17)
if(!$flags.Contains('/DCPU_40_STYLE') -or !$flags.Contains('/DV7VGA')){throw 'Expected actual production x86 Video7 profile'}
New-Item -ItemType Directory $out|Out-Null
$source=Join-Path $repo 'src/mvdm/softpc.new/host/src/nt_vga.c'
$fixture=Join-Path $repo 'tests/mvdm-host/video7_packed_painter_test.c'
"$flags $release /Gy /Fo`"$out/painter.obj`" `"$source`""|Set-Content "$out/source.rsp"
"$($flags.Replace('/D_NO_CRT_STDIO_INLINE','')) $release /Gy /Fo`"$out/fixture.obj`" `"$fixture`""|Set-Content "$out/fixture.rsp"
@("@echo off","call `"$vs`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',"cd /d `"$out`"",'cl @source.rsp > source.log 2>&1','if errorlevel 1 exit /b %errorlevel%','cl @fixture.rsp > fixture.log 2>&1','if errorlevel 1 exit /b %errorlevel%','link /nologo /opt:ref /map:painter-test.map /out:painter-test.exe fixture.obj painter.obj kernel32.lib user32.lib gdi32.lib > link.log 2>&1','if errorlevel 1 exit /b %errorlevel%','painter-test.exe > test.txt 2>&1','exit /b %errorlevel%')|Set-Content "$out/run.cmd"
$start=[Diagnostics.ProcessStartInfo]::new('cmd.exe')
$start.UseShellExecute=$false;$start.WindowStyle='Hidden';$start.WorkingDirectory=$out
foreach($arg in @('/d','/c',"$out/run.cmd")){$start.ArgumentList.Add($arg)}
$p=[Diagnostics.Process]::Start($start)
try {
    @{pid=$p.Id;started=$p.StartTime;command="$out/run.cmd"}|ConvertTo-Json|Set-Content "$out/process.json"
    if(!$p.WaitForExit(60000)){$p.Kill($true);$p.WaitForExit();throw 'Owned painter-test deadline'}
    if($p.ExitCode){throw "Actual painter build/test failed: $($p.ExitCode); see output logs"}
    if((Get-Content "$out/painter-test.map" -Raw) -notmatch '_nt_v7vga_hi_graph_std\s+.*painter.obj'){throw 'Actual painter provider absent'}
    if((Get-Content "$out/test.txt" -Raw) -notmatch 'PASS actual packed painter'){throw 'Missing acceptance marker'}
    @{passed=$true;sourceSha256=(Get-FileHash $source).Hash;fixtureSha256=(Get-FileHash $fixture).Hash}|ConvertTo-Json|Set-Content "$out/result.json"
    Get-Content "$out/test.txt"
} finally {if(!$p.HasExited){$p.Kill($true);$p.WaitForExit()};$p.Dispose()}
