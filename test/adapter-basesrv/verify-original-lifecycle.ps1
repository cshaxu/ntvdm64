param([Parameter(Mandatory)][string]$Fixture,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$fixturePath=(Resolve-Path $Fixture).Path
$case=[IO.Path]::GetFullPath($BuildRoot)
if(!$case.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Require evidence directory under build'}
New-Item -ItemType Directory -Force "$case/system32"|Out-Null
. "$repo/test/observation/guest_archive.ps1"
$identities=@(Copy-VerifiedGuestArchiveMembers -Destination $case -Members ([ordered]@{
 'WINNT/SYSTEM32/MEM.EXE'='MEM.EXE'
 'WINNT/WRITE.EXE'='system32/WRITE.EXE'
 'WINNT/SYSTEM32/KRNL386.EXE'='system32/KRNL386.exe'
}))
$p=Start-Process $fixturePath -WorkingDirectory $case -WindowStyle Hidden -PassThru -RedirectStandardOutput "$case/stdout.txt" -RedirectStandardError "$case/stderr.txt"
if(!$p.WaitForExit(30000)){$p.Kill();throw 'Original lifecycle timed out'}
@{fixture=$fixturePath;sha256=(Get-FileHash $fixturePath).Hash;pid=$p.Id;exit=$p.ExitCode;media=$identities}|ConvertTo-Json -Depth 5|Set-Content "$case/result.json"
Get-Content "$case/stdout.txt"
if($p.ExitCode){Get-Content "$case/stderr.txt";throw "Original lifecycle failed: $($p.ExitCode)"}
