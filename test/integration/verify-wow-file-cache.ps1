param([string]$Object='', [string]$Output='build/wow-file-cache')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
$null=New-Item -ItemType Directory -Force $out
function Owned($name,$exe,$arguments,$seconds){
 $start=@{FilePath=$exe;WorkingDirectory=$out;WindowStyle='Hidden';PassThru=$true;RedirectStandardOutput="$out/$name.stdout.txt";RedirectStandardError="$out/$name.stderr.txt"}
 if($arguments){$start.ArgumentList=$arguments}
 $p=Start-Process @start
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$out/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F|Out-Null;throw "$name deadline"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$out/$name.exit.txt"
 return $code
}
$code=Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-file-cache"') 300
if($code){throw "Native file cache build failed: $code"}
$native="$repo/build/.ninja/msvc-x86-release/wow32"
$actual=(Get-Content "$native/file-cache-production-object.txt" -Raw).Trim()
if($Object -and [IO.Path]::GetFullPath((Join-Path $repo $Object)) -ne [IO.Path]::GetFullPath($actual)){throw 'Object must be the selected current production wkfileio object'}
Get-FileHash $actual|ConvertTo-Json|Set-Content "$out/production-object.json"
Copy-Item "$native/wow-file-cache.exe" "$out/cache-test.exe"
$code=Owned fixture "$out/cache-test.exe" '' 30
Get-Content "$out/fixture.stdout.txt"|Tee-Object "$out/run.txt"
if($code){exit $code}
if((Get-Content "$out/run.txt" -Raw) -notmatch 'WOW_FILE_CACHE checks=[1-9][0-9]* failures=0'){throw 'Missing cache contract summary'}
