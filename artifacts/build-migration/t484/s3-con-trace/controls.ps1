$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$started=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 $cache='build/.ninja/msvc-x86-release/worker'
 & test/observation/rebuild-win101-control-fixtures.ps1 -BuildCache $cache
 if($LASTEXITCODE){throw 'Public controls wrapper failed'}
 $rows=@()
 foreach($name in 'frontend-request-client-test.exe','broker-frontend-bootstrap-test.exe','native-gui-startup-probe.exe') {
  $rows+=@{name=$name;sha256=(Get-FileHash "$cache/$name").Hash;time=(Get-Item "$cache/$name").LastWriteTimeUtc.Ticks}
 }
 & test/observation/rebuild-win101-control-fixtures.ps1 -BuildCache $cache
 if($LASTEXITCODE){throw 'Cached controls wrapper failed'}
 foreach($row in $rows){if((Get-FileHash "$cache/$($row.name)").Hash -ne $row.sha256 -or (Get-Item "$cache/$($row.name)").LastWriteTimeUtc.Ticks -ne $row.time){throw 'Control cache changed'}}
 & cmd.exe /d /c tools\build-fixtures.cmd x64 win101-control-fixtures $cache
 if($LASTEXITCODE -ne 2){throw 'Unsupported controls architecture accepted'}
 $p=Start-Process "$cache/frontend-request-client-test.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$PSScriptRoot/client.out" -RedirectStandardError "$PSScriptRoot/client.err"
 if(!$p.WaitForExit(60000)){$p.Kill($true);throw 'Owned client test timed out'}
 $p.Refresh()
 if($p.ExitCode){throw "Client test failed $($p.ExitCode)"}
 if((Get-Content "$PSScriptRoot/client.out" -Raw) -notmatch 'PASS client-only RPC submission'){throw 'Client witness missing'}
 $rows|ConvertTo-Json|Set-Content "$PSScriptRoot/controls-identity.json"
 & test/observation/prepare-win31-con-trace.ps1 -BuildRoot "$PSScriptRoot/prepared" -PackageRoot 'O:\winnt' -WindowsRoot 'O:\w31' -ReferencePif 'O:\winnt\DOS4GW.PIF'
 if($LASTEXITCODE){throw 'Console preparation failed'}
 $old=(& git show b5d636880:test/observation/prepare-win31-con-trace.ps1) -join "`n"
 $old=$old.Replace('$repo=(Resolve-Path "$PSScriptRoot/../..").Path',"`$repo='$pwd'")
 $old=$old.Replace('$PSScriptRoot/win31_con_trace.asm',"$pwd/test/observation/win31_con_trace.asm")
 & ([scriptblock]::Create($old)) -BuildRoot "$PSScriptRoot/reference-prepared" -PackageRoot 'O:\winnt' -WindowsRoot 'O:\w31' -ReferencePif 'O:\winnt\DOS4GW.PIF'
 if($LASTEXITCODE){throw 'Reference preparation failed'}
 foreach($name in 'CONTRACE.COM','CONTRACE.lst','T436.BAT','T436BASE.BAT','TRACE.PIF','BASE.PIF') {
  if((Get-FileHash "$PSScriptRoot/prepared/$name").Hash -ne (Get-FileHash "$PSScriptRoot/reference-prepared/$name").Hash){throw "Preparation output differs: $name"}
 }
 $source=Get-Content build/t484-s3-addon/deliver.ps1 -Raw
 $a=$source.IndexOf(' $rows=Get-Content');$b=$source.IndexOf(' $args=@{}')
 & ([scriptblock]::Create($source.Substring($a,$b-$a)))
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$started;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/controls-result.json"
exit $code