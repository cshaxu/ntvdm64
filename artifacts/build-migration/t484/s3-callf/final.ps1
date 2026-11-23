$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$started=[DateTimeOffset]::UtcNow.ToString('o');$code=1
try {
 $root='build/.ninja/msvc-x86-release/worker'
 $rows=@()
 foreach($name in 'ccpu-high-linear-page-test.exe','ccpu-far-exception-test.exe','ccpu-port-privilege.exe') {
  $rows+=@{name=$name;hash=(Get-FileHash "$root/$name").Hash;time=(Get-Item "$root/$name").LastWriteTimeUtc.Ticks}
 }
 & cmd.exe /d /c build\.ninja\msvc-x86-release\worker\run-ninja-parallel.cmd ccpu-high-linear-page-test.exe ccpu-far-exception-test.exe ccpu-port-privilege.exe
 if($LASTEXITCODE){throw 'Cached native build failed'}
 foreach($row in $rows){if((Get-FileHash "$root/$($row.name)").Hash -ne $row.hash -or (Get-Item "$root/$($row.name)").LastWriteTimeUtc.Ticks -ne $row.time){throw 'Cache identity changed'}}
 & cmd.exe /d /c tools\build-fixtures.cmd x64 ccpu-callback
 if($LASTEXITCODE -ne 2){throw 'x64 CALLF accepted'}
 & "$root/ccpu-far-exception-test.exe" unknown
 if($LASTEXITCODE -ne 2){throw 'Unknown far fixture argument accepted'}
 & "$root/ccpu-far-exception-test.exe" valid extra
 if($LASTEXITCODE -ne 2){throw 'Extra far fixture argument accepted'}
 $rows|ConvertTo-Json|Set-Content "$PSScriptRoot/cache-identity.json"
 $source=Get-Content build/t484-s3-addon/deliver.ps1 -Raw
 $a=$source.IndexOf(' $rows=Get-Content');$b=$source.IndexOf(' $args=@{}')
 & ([scriptblock]::Create($source.Substring($a,$b-$a)))
 & tools/governance/Verify-DocumentationGovernance.ps1
 if($LASTEXITCODE){throw 'Governance failed'}
 & git diff --check
 if($LASTEXITCODE){throw 'Diff check failed'}
 $code=0
} catch {$_|Out-String|Write-Output}
@{pid=$PID;started=$started;finished=[DateTimeOffset]::UtcNow.ToString('o');exit=$code}|ConvertTo-Json|Set-Content "$PSScriptRoot/final-result.json"
exit $code