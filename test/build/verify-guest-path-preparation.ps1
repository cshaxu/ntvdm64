[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,[string]$Destination='O:\tmp\gpr')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$task=[IO.Path]::GetFullPath($OutputRoot)
if(!$task.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $task)){throw 'Fresh build evidence required'}
New-Item -ItemType Directory $task|Out-Null
. "$repo/test/observation/isolated_package_cleanup.ps1"
$owned=$false
try {
 & "$repo/tools/create-guest-probe.cmd" $Destination $task
 if($LASTEXITCODE){throw 'Owned test directory creation failed'}
 $owned=$true
 [IO.File]::WriteAllText("$Destination/test.ini",'O:\w31\SYSTEM')
 $before=(Get-FileHash "$Destination/test.ini").Hash
 & "$repo/tools/rebase-guest-paths.cmd" $Destination O:/w31 $task 95 *> "$task/missing-pif.log"
 if($LASTEXITCODE -ne 2 -or (Get-FileHash "$Destination/test.ini").Hash -ne $before){throw 'Missing Win95 PIF changed input or returned success'}
 [IO.File]::WriteAllText("$task/path-rebase.candidate",'owned sentinel')
 & "$repo/tools/rebase-guest-paths.cmd" $Destination O:/w31 $task 31 *> "$task/existing-candidate.log"
 if(!$LASTEXITCODE -or (Get-FileHash "$Destination/test.ini").Hash -ne $before -or [IO.File]::ReadAllText("$task/path-rebase.candidate") -cne 'owned sentinel'){throw 'Native failure not propagated before copy/delete'}
 'PASS missing PIF and existing candidate preserve inputs and fail'|Set-Content "$task/result.txt"
} finally {
 if($owned){Remove-OwnedGuestProbeCopy $Destination $task O:/w31}
}
$global:LASTEXITCODE=0
