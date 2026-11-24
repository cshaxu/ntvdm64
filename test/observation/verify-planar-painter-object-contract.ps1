[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
$cache=(Resolve-Path (Join-Path $repo 'build/.ninja/msvc-x86-release/worker')).Path
$null=New-Item -ItemType Directory -Path $out
& "$repo/tools/build-fixtures.cmd" x86 planar-painter *> "$out/build.log"
if($LASTEXITCODE){throw 'Native painter build failed'}
$source=Join-Path $repo 'test/mvdm-host/planar_painter_object_fixture.c'
$objects=@('nt_cga','nt_ega','nt_munge')|ForEach-Object {Join-Path $cache "obj/host/$_.obj"}
$inputs=@($PSCommandPath,$source,"$cache/build.ninja")+$objects+@('nt_cga','nt_ega','nt_munge'|ForEach-Object {Join-Path $repo "src/mvdm-host/softpc.new/host/src/$_.c"})
Get-FileHash -LiteralPath $inputs|Export-Csv "$out/inputs.csv" -NoTypeInformation
Copy-Item "$cache/planar-object.exe" "$out/planar-object.exe"
Copy-Item "$cache/planar-object.exe.map" "$out/planar-object.map"
function RunOwned($name,$image,$arguments,$expected){
    $start=@{FilePath=$image;WindowStyle='Hidden';WorkingDirectory=$repo;RedirectStandardOutput="$out/$name.stdout.txt";RedirectStandardError="$out/$name.stderr.txt";PassThru=$true}
    if($arguments){$start.ArgumentList=$arguments}
    $p=Start-Process @start
    try{
        @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$image;arguments=$arguments;diagnostic_budget_seconds=120}|ConvertTo-Json|Set-Content "$out/$name.process.json"
        if(!$p.WaitForExit(120000)){throw 'Owned painter diagnostic budget exceeded'}
        $p.WaitForExit();$code=$p.ExitCode
        @{exit=$code;expected=$expected;finished=$p.ExitTime.ToUniversalTime().ToString('o')}|ConvertTo-Json|Set-Content "$out/$name.completion.json"
        if($code -ne $expected){throw "Unexpected $name exit $code"}
    }finally{if(!$p.HasExited){$p.Kill($true);$p.WaitForExit()};$p.Dispose()}
}
RunOwned normal "$out/planar-object.exe" '' 0
RunOwned negative "$out/planar-object.exe" '--wrong-pixel' 1
$normal=[IO.File]::ReadAllText("$out/normal.stdout.txt")
$negative=[IO.File]::ReadAllText("$out/negative.stdout.txt")
if($normal -notmatch 'PLANAR_OBJECT_CONTRACT checks=(\d+) failures=0 callbacks=(\d+) cases=(\d+) wrong_oracle=0'){throw 'Painter result incomplete'}
$checks=$Matches[1];$callbacks=$Matches[2];$cases=$Matches[3]
if([int]$checks -ne 182801 -or [int]$callbacks -ne 26114 -or [int]$cases -ne 26114){throw 'Painter matrix coverage changed'}
$map=[IO.File]::ReadAllText("$out/planar-object.map")
foreach($family in @('med','hi')){foreach($scale in @('std','big','huge')){
    if($map -notmatch ('_nt_ega_'+$family+'_graph_'+$scale+'\s+[^\r\n]*nt_ega.obj')){throw 'Original EGA provider missing'}
}}
foreach($scale in @('std','big','huge')){
    if($map -notmatch ('_nt_cga_colour_med_graph_'+$scale+'\s+[^\r\n]*nt_cga.obj')){throw 'Original CGA provider missing'}
}
foreach($suffix in @('','_big','_huge')){
    if($map -notmatch ('_ega_colour_hi_munge'+$suffix+'\s+[^\r\n]*nt_munge.obj')){throw 'Original munge provider missing'}
}
if($negative -notmatch 'failures=[1-9]\d* callbacks=1 cases=1 wrong_oracle=1'){throw 'Wrong pixel oracle not rejected'}
foreach($r in (Import-Csv "$out/inputs.csv")){if((Get-FileHash -LiteralPath $r.Path).Hash -ne $r.Hash){throw 'Painter input drift'}}
@{result='pass';checks=[int]$checks;callbacks=[int]$callbacks;cases=[int]$cases;fixture_sha256=(Get-FileHash "$out/planar-object.exe").Hash;scope='Actual cached original painter/LUT/munge objects; controlled DIB and invalidation sink, fail-fast traps for untested text/mono services, not guest or full rendering lifecycle'}|ConvertTo-Json|Set-Content "$out/result.json"
$normal
