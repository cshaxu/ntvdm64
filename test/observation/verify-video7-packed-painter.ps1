[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cache=(Resolve-Path (Join-Path $repo 'build/.ninja/msvc-x86-release/worker')).Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
$source=Join-Path $repo 'src/mvdm-host/softpc.new/host/src/nt_vga.c'
$fixture=Join-Path $repo 'test/mvdm-host/video7_packed_painter_test.c'
& "$repo/tools/build-fixtures.cmd" x86 video7-packed-painter *> "$out/build.log"
if($LASTEXITCODE){throw 'Native Video7 painter build failed'}
Copy-Item "$cache/video7-packed-painter.exe" "$out/painter-test.exe"
Copy-Item "$cache/video7-packed-painter.exe.map" "$out/painter-test.map"
$start=[Diagnostics.ProcessStartInfo]::new("$out/painter-test.exe")
$start.RedirectStandardOutput=$true;$start.RedirectStandardError=$true;$start.UseShellExecute=$false;$start.WindowStyle='Hidden';$start.WorkingDirectory=$out
$p=[Diagnostics.Process]::Start($start)
try {
    @{pid=$p.Id;started=$p.StartTime;command="$out/painter-test.exe"}|ConvertTo-Json|Set-Content "$out/process.json"
    if(!$p.WaitForExit(60000)){$p.Kill($true);$p.WaitForExit();throw 'Owned painter-test deadline'}
    $p.StandardOutput.ReadToEnd()|Set-Content "$out/test.txt"
    $p.StandardError.ReadToEnd()|Set-Content "$out/test.stderr.txt"
    if($p.ExitCode){throw "Actual painter build/test failed: $($p.ExitCode); see output logs"}
    if((Get-Content "$out/painter-test.map" -Raw) -notmatch '_nt_v7vga_hi_graph_std\s+.*nt_vga.obj'){throw 'Actual painter provider absent'}
    if((Get-Content "$out/test.txt" -Raw) -notmatch 'PASS actual packed painter'){throw 'Missing acceptance marker'}
    @{passed=$true;sourceSha256=(Get-FileHash $source).Hash;fixtureSha256=(Get-FileHash $fixture).Hash}|ConvertTo-Json|Set-Content "$out/result.json"
    Get-Content "$out/test.txt"
} finally {if(!$p.HasExited){$p.Kill($true);$p.WaitForExit()};$p.Dispose()}
