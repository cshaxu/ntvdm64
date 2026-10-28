$ErrorActionPreference='Stop'
$repo='O:/repos.hobby/ntvdm64';Set-Location $repo
$root="$repo/build/t473-s10/addon-renames-r8";New-Item -ItemType Directory $root|Out-Null
$pwsh='C:/Users/neko/.cache/codex-runtimes/codex-primary-runtime/dependencies/native/powershell/pwsh.exe'
$record=[ordered]@{result='running';started=[DateTime]::UtcNow.ToString('o');runs=@()}
function Run($name,$arguments){
 $p=Start-Process $pwsh -ArgumentList $arguments -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 $record.runs+=@{name=$name;pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$pwsh;arguments=$arguments};$record|ConvertTo-Json -Depth 6|Set-Content "$root/result.json"
 $p.WaitForExit();$p.WaitForExit();$c=$p.ExitCode;$p.Dispose();$record.runs[-1].exit=$c;if($c){throw "$name returned $c"}
}
try{
 Run grp-tagless ('-NoProfile -File tests/component-integration/grp_tool_test.ps1 -Grp "'+$repo+'/assets/release/GRP.EXE" -FixtureRoot O:/w31 -BuildRoot "'+$root+'/tagless"')
 $fixture="$root/tagged-fixture";New-Item -ItemType Directory $fixture|Out-Null
 [IO.File]::WriteAllText("$fixture/WINFILE.EXE",'fixture path marker, never executed')
 $original=[IO.File]::ReadAllBytes('O:/w31/MAIN.GRP');if([BitConverter]::ToUInt16($original,6) -ne $original.Length){throw 'Expected frozen tagless input for augmented fixture'}
 $body=[Text.Encoding]::ASCII.GetBytes("C:\WINDOWS`0")
 [byte[]]$tags=@(0,128,255,255,10,0,80,77,67,67,1,129,0,0)+[BitConverter]::GetBytes([uint16](6+$body.Length))+$body+@(255,255,255,255,0,0)
 [byte[]]$bytes=$original+$tags;$bytes[4]=0;$bytes[5]=0;$sum=0
 for($i=0;$i+1 -lt $bytes.Length;$i+=2){$sum=($sum+[BitConverter]::ToUInt16($bytes,$i)) -band 65535}
 [BitConverter]::GetBytes([uint16]((65536-$sum) -band 65535)).CopyTo($bytes,4)
 [IO.File]::WriteAllBytes("$fixture/MAIN.GRP",$bytes)
 Run grp-tagged ('-NoProfile -File tests/component-integration/grp_tool_test.ps1 -Grp "'+$repo+'/assets/release/GRP.EXE" -FixtureRoot "'+$fixture+'" -BuildRoot "'+$root+'/tagged"')
 Run szdd '-NoProfile -File tests/component-integration/szdd_roundtrip_test.ps1 -BuildRoot build/t473-s10/renamed-szdd-r8'
 $old=Get-Content build/t473-s10/addon-renames-r7/result.json -Raw|ConvertFrom-Json
 foreach($row in $old.sources){if((Get-FileHash $row.path).Hash -ne $row.sha256){throw 'Component source changed since build'}}
 foreach($row in $old.core_before){if((Get-FileHash "assets/release/$($row.name)").Hash -ne $row.sha256){throw 'Host package changed'}}
 $record.result='pass'
}catch{$record.result='fail';$record.error=$_.Exception.Message}
$record.finished=[DateTime]::UtcNow.ToString('o');$record|ConvertTo-Json -Depth 6|Set-Content "$root/result.json"
if($record.result -ne 'pass'){throw $record.error}
'PASS tagged and tagless GRP roundtrips with strict tag/checksum checks, and SZDD; unchanged built source/core inputs'
