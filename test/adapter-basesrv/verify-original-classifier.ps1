param([Parameter(Mandatory)][string]$Fixture,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path "$PSScriptRoot/../..").Path
$fixturePath=(Resolve-Path $Fixture).Path
New-Item -ItemType Directory -Force $BuildRoot | Out-Null
$case=(Resolve-Path $BuildRoot).Path
$image=Join-Path $case 'original-classifier.exe'
Copy-Item -LiteralPath $fixturePath -Destination $image
foreach($suffix in @('COM','pif','exe','bin')) {
 [IO.File]::WriteAllBytes((Join-Path $case "suffix.$suffix"),[byte[]](1..64 | ForEach-Object {65}))
}
. "$root/test/observation/guest_archive.ps1"
$identities=@(Copy-VerifiedGuestArchiveMembers -Destination "$case/media" -Members ([ordered]@{
 'WINNT/SYSTEM32/MEM.EXE'='MEM.EXE'
 'WINNT/SYSTEM32/COMMAND.COM'='COMMAND.COM'
 'WINNT/EDIT.COM'='EDIT.COM'
 'WINNT/WRITE.EXE'='WRITE.EXE'
}))
$media=@($identities|ForEach-Object {$_.path})
$p=Start-Process $image -ArgumentList ($media | ForEach-Object {'"'+$_+'"'}) -WorkingDirectory $case -WindowStyle Hidden -PassThru -RedirectStandardOutput "$case/stdout.txt" -RedirectStandardError "$case/stderr.txt"
if(-not $p.WaitForExit(15000)) { $p.Kill(); throw 'Original classifier timed out' }
[ordered]@{fixture=$fixturePath;sha256=(Get-FileHash $image).Hash;pid=$p.Id;exit=$p.ExitCode;media=$identities} | ConvertTo-Json -Depth 5 | Set-Content "$case/result.json"
Get-Content "$case/stdout.txt"
if($p.ExitCode){throw "Original classifier failed: $($p.ExitCode)"}
