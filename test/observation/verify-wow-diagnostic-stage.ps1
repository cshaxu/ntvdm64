[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
$tool="$repo/build/.ninja/build-tools/prepare-wow-diagnostic.cmd"
$images="$repo/assets/release"
$worker="$repo/build/.ninja/msvc-x86-release/worker"
$wow="$repo/build/.ninja/msvc-x86-release/wow32"
function Invoke-Case($name,$output,$inputImages,$diagnostic,$success){
 $line='/d /s /c ""'+$tool+'" "'+$output+'" "'+$inputImages+'" "'+$worker+'" "'+$wow+'"'
 if($diagnostic){$line+=' "'+$diagnostic+'"'}
 $line+='"'
 $p=Start-Process $env:ComSpec -ArgumentList $line -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit(60000)){taskkill.exe /PID $p.Id /T /F;throw 'Stage deadline'}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose()
 $code|Set-Content "$root/$name.exit.txt"
 if(($code -eq 0) -ne $success){throw "Unexpected result $name : $code"}
}
Invoke-Case normal "$root/package" $images '' $true
$manifest=@(Get-Content "$images/release-manifest.json" -Raw|ConvertFrom-Json)
foreach($row in $manifest){if((Get-FileHash "$root/package/WINNT/system32/$($row.Name)").Hash -ne $row.Sha256){throw 'Published identity changed'}}
foreach($pair in @(@('ntvdm.exe',$worker),@('wow32.dll',$wow))){if((Get-FileHash "$root/package/WINNT/system32/$($pair[0]).map").Hash -ne (Get-FileHash "$($pair[1])/$($pair[0]).map").Hash){throw 'MAP copy changed'}}
$before=Get-FileHash "$root/package/prepared.txt"
Invoke-Case existing "$root/package" $images '' $false
if((Get-FileHash $before.Path).Hash -ne $before.Hash){throw 'Rejected existing output changed'}
Invoke-Case unsupported "$root/unsupported" $images "$images/run16.exe" $false
if(Test-Path "$root/unsupported"){throw 'Invalid diagnostic created output'}
$null=New-Item -ItemType Directory "$root/invalid"
[IO.File]::WriteAllText("$root/invalid/release-manifest.json",'[]')
Invoke-Case invalid-manifest "$root/rejected" "$root/invalid" '' $false
if(Test-Path "$root/rejected/prepared.txt"){throw 'Failed stage has completion marker'}
$null=New-Item -ItemType Directory "$root/mismatch"
Copy-Item "$images/release-manifest.json" "$root/mismatch/release-manifest.json"
foreach($row in $manifest){[IO.File]::WriteAllText("$root/mismatch/$($row.Name)",'wrong image')}
Invoke-Case mismatch "$root/bad-hash" "$root/mismatch" '' $false
if(Test-Path "$root/bad-hash/WINNT"){throw 'Hash failure extracted guest media'}
'PASS exact release and MAP copies; existing output, unsupported diagnostic, malformed manifest and hash mismatch rejected'|Set-Content "$root/result.txt"
'0'|Set-Content "$root/exit.txt"
