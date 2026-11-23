$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$started=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 & cmd.exe /d /c tools\build-fixtures.cmd x86 win31-con-trace
 if($LASTEXITCODE){throw 'Native target failed'}
 & nasm.exe -f bin "$pwd/test/observation/win31_con_trace.asm" -o "$PSScriptRoot/CONTRACE.COM" -l "$PSScriptRoot/CONTRACE.lst"
 if($LASTEXITCODE){throw 'Reference assembly failed'}
 $rows=@();$native='build/.ninja/fixtures-x86/guest-probes/win31-con-trace'
 foreach($name in 'CONTRACE.COM','CONTRACE.lst') {
  $hash=(Get-FileHash "$native/$name").Hash
  if($hash -ne (Get-FileHash "$PSScriptRoot/$name").Hash){throw "Reference mismatch $name"}
  $rows+=@{name=$name;sha256=$hash;time=(Get-Item "$native/$name").LastWriteTimeUtc.Ticks}
 }
 & cmd.exe /d /c tools\build-fixtures.cmd x86 win31-con-trace
 if($LASTEXITCODE){throw 'Cached build failed'}
 foreach($row in $rows){if((Get-FileHash "$native/$($row.name)").Hash -ne $row.sha256 -or (Get-Item "$native/$($row.name)").LastWriteTimeUtc.Ticks -ne $row.time){throw 'Cache changed'}}
 & cmd.exe /d /c tools\build-fixtures.cmd x64 win31-con-trace
 if($LASTEXITCODE -ne 2){throw 'Wrong architecture accepted'}
 $rows|ConvertTo-Json|Set-Content "$PSScriptRoot/identity.json"
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$started;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/build-result.json"
exit $code