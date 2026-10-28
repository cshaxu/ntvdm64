$ErrorActionPreference='Stop'
$repo='O:/repos.hobby/ntvdm64';$out="$repo/build/t473-s10/addon-utilities-r4"
New-Item -ItemType Directory -Path $out|Out-Null
foreach($row in @(@{id='szdd';exe='SZDD.EXE'},@{id='w31patch';exe='W31PATCH.EXE'})){
 $a='/d /s /c ""'+$repo+'/src/addon/'+$row.id+'/BUILD.CMD" "'+$out+'/'+$row.exe+'""'
 $p=Start-Process $env:ComSpec -ArgumentList $a -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$($row.id).stdout.txt" -RedirectStandardError "$out/$($row.id).stderr.txt"
 @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$env:ComSpec;arguments=$a}|ConvertTo-Json|Set-Content "$out/$($row.id).process.json"
 $p.WaitForExit();$p.WaitForExit();$c=$p.ExitCode;$p.Dispose();@{exit=$c}|ConvertTo-Json|Set-Content "$out/$($row.id).completion.json";if($c){throw 'Utility build failed'}
}
& "$repo/tools/build/Stage-AddonUtilityRelease.ps1" -SzddBuild "$out/SZDD.EXE" -W31PatchBuild "$out/W31PATCH.EXE"
& "$repo/build/t473-s10/verify-utilities-r4.ps1"
'PASS independent utilities and five updated Setup groups'
