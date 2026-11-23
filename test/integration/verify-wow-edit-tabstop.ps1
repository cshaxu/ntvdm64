param([string]$Object='', [string]$Output='build/wow-edit-contracts')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
$null=New-Item -ItemType Directory -Force $out
function Owned($name,$exe,$arguments,$seconds){
 $p=Start-Process $exe -ArgumentList $arguments -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.stdout.txt" -RedirectStandardError "$out/$name.stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$out/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F|Out-Null;throw "$name deadline"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$out/$name.exit.txt"
 return $code
}
$code=Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 wow-edit-contracts"') 300
if($code){throw "Native edit contracts build failed: $code"}
$native="$repo/build/.ninja/msvc-x86-release/wow32"
$actual=(Get-Content "$native/edit-production-object.txt" -Raw).Trim()
if($Object -and [IO.Path]::GetFullPath((Join-Path $repo $Object)) -ne [IO.Path]::GetFullPath($actual)){throw 'Object must be the selected current production wmsgem object'}
Get-FileHash $actual|ConvertTo-Json|Set-Content "$out/production-object.json"
Copy-Item "$native/edit-tabstop.exe" "$out/test.exe"
Copy-Item "$native/edit-wordbreak.exe" "$out/wordbreak.exe"
$rows=@()
foreach($count in @(0,1,-1)){foreach($heap in @(0,1)){foreach($result in @(0,1)){foreach($cached in @(0,1)){
 $code=Owned "tabstop-$count-$heap-$result-$cached" "$out/test.exe" "$count $heap $result $cached" 15
 $line=Get-Content "$out/tabstop-$count-$heap-$result-$cached.stdout.txt"
 $rows+=@{case="tabstop-$count-$heap-$result-$cached";exit=$code;output=($line -join "`n")}
}}}}
foreach($case in @('zero','guest-ldt','guest-high','native-fail','native-success','native-cached')){
 $code=Owned "wordbreak-$case" "$out/wordbreak.exe" $case 15
 $line=Get-Content "$out/wordbreak-$case.stdout.txt"
 $rows+=@{case="wordbreak-$case";exit=$code;output=($line -join "`n")}
}
$rows|ConvertTo-Json -Depth 4|Set-Content "$out/results.json"
$failed=@($rows|Where-Object {$_.exit -ne 0}).Count
"EDIT_TABSTOP cases=$($rows.Count) failures=$failed"|Tee-Object "$out/summary.txt"
if($failed){exit 1}
