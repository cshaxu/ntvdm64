[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$task=[IO.Path]::GetFullPath($OutputRoot)
if(!$task.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $task)){throw 'Fresh build output required'}
New-Item -ItemType Directory $task|Out-Null
$source=Join-Path $task 'source'
New-Item -ItemType Directory $source|Out-Null
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$dest='O:\tmp\g'+[Guid]::NewGuid().ToString('N').Substring(0,6)
if(Test-Path -LiteralPath $dest){throw 'Fresh test target required'}
New-Item -ItemType Directory $dest|Out-Null
[IO.File]::WriteAllText((Join-Path $dest '.t479-owner'),$task)
$child=$null;$nestedParent=$null;$nested=$null;$failedDest=$null;$failedTask=$null;$failed95=$null;$task95=$null
try {
 $wrong=Join-Path $task 'wrong';New-Item -ItemType Directory $wrong|Out-Null
 $rejected=$false;try{Remove-OwnedGuestProbeCopy $dest $wrong $source}catch{$rejected=$true}
 if(!$rejected -or !(Test-Path $dest)){throw 'Wrong owner was not rejected'}
 $taskLink=Join-Path $task 'linked-task'
 New-Item -ItemType Junction -Path $taskLink -Target $wrong|Out-Null
 $rejected=$false;try{Remove-OwnedGuestProbeCopy $dest $taskLink $source}catch{$rejected=$true}
 if(!$rejected -or !(Test-Path $dest)){throw 'Linked evidence path was not rejected'}
 Remove-Item -LiteralPath $taskLink
 [IO.File]::WriteAllText((Join-Path $dest '.git'),'test')
 $rejected=$false;try{Remove-OwnedGuestProbeCopy $dest $task $source}catch{$rejected=$true}
 if(!$rejected -or !(Test-Path $dest)){throw 'Git data was not rejected'}
 Remove-Item -LiteralPath (Join-Path $dest '.git')
 $link=Join-Path $dest 'linked'
 New-Item -ItemType Junction -Path $link -Target $source|Out-Null
 $rejected=$false;try{Remove-OwnedGuestProbeCopy $dest $task $source}catch{$rejected=$true}
 if(!$rejected -or !(Test-Path $dest)){throw 'Link was not rejected'}
 Remove-Item -LiteralPath $link
 $child=Start-Process -FilePath (Get-Command python.exe).Source -ArgumentList @('-c','"import time;time.sleep(30)"',"`"$dest\busy`"") -WindowStyle Hidden -PassThru
 $rejected=$false;try{Remove-OwnedGuestProbeCopy $dest $task $source}catch{$rejected=$true}
 if(!$rejected -or !(Test-Path $dest)){throw 'Active process was not rejected'}
 if(!$child.HasExited){$child.Kill();$child.WaitForExit()};$child.Dispose();$child=$null
 New-Item -ItemType Directory (Join-Path $dest 'PATCH')|Out-Null
 [IO.File]::WriteAllText((Join-Path $dest 'PATCH/DONE.BIN'),'evidence')
 Remove-OwnedGuestProbeCopy $dest $task $source
 if((Test-Path $dest) -or [IO.File]::ReadAllText((Join-Path $task 'guest-witnesses/DONE.BIN')) -cne 'evidence'){throw 'Cleanup or evidence preservation failed'}
 $nestedParent=$dest+'n';$nested=Join-Path $nestedParent 'child'
 if(Test-Path -LiteralPath $nestedParent){throw 'Fresh nested parent required'}
 New-Item -ItemType Directory $nestedParent|Out-Null
 New-Item -ItemType Directory $nested|Out-Null
 [IO.File]::WriteAllText((Join-Path $nested '.t479-owner'),$task)
 Remove-OwnedGuestProbeCopy $nested $task $source
 if(Test-Path -LiteralPath $nested){throw 'Nested owned copy was not removed'}
 Remove-Item -LiteralPath $nestedParent
 $nestedParent=$null;$nested=$null
 [IO.File]::WriteAllText((Join-Path $source 'WIN.COM'),'fixture')
 [IO.File]::WriteAllText((Join-Path $source 'SYSTEM.INI'),"[386Enh]`r`n")
 $failedDest=$dest+'f';$failedTask=Join-Path $task 'failed-preparation'
 if(Test-Path -LiteralPath $failedDest){throw 'Fresh failure destination required'}
 $rejected=$false
 try {& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" -DriverImage $PSCommandPath -InstallSource $source -InstallRoot $failedDest -OutputRoot $failedTask|Out-Null}catch{$rejected=$true}
 if(!$rejected -or (Test-Path -LiteralPath $failedDest)){throw 'Failed preparation did not clean its owned copy'}
 $failed95='O:\tmp\'+('h'*($source.Length-'O:\tmp\'.Length));$task95=Join-Path $task 'failed-win95'
 if(Test-Path -LiteralPath $failed95){throw 'Fresh Win95 failure destination required'}
 $rejected=$false
 try {& "$repo/test/integration/ntvdm64-386/New-Win95ProbeCopy.ps1" -DriverImage $PSCommandPath -InstallSource $source -InstallRoot $failed95 -OutputRoot $task95 2>$null|Out-Null}catch{$rejected=$true}
 if(!$rejected -or (Test-Path -LiteralPath $failed95)){throw 'Failed Win95 preparation did not clean its owned copy'}
 'PASS wrong owner, Git, link and active process refused; witness retained and owned copies removed'
} finally {
 if($child){if(!$child.HasExited){$child.Kill();$child.WaitForExit()};$child.Dispose()}
 if(Test-Path -LiteralPath $dest){Remove-OwnedGuestProbeCopy $dest $task $source}
 if($nested -and (Test-Path -LiteralPath $nested)){Remove-OwnedGuestProbeCopy $nested $task $source}
 if($nestedParent -and (Test-Path -LiteralPath $nestedParent)){Remove-Item -LiteralPath $nestedParent}
 if($failedDest -and (Test-Path -LiteralPath $failedDest)){Remove-OwnedGuestProbeCopy $failedDest $failedTask $source}
 if($failed95 -and (Test-Path -LiteralPath $failed95)){Remove-OwnedGuestProbeCopy $failed95 $task95 $source}
}
