$ErrorActionPreference='Stop'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
# In-process negative fixture: the current test process must remain alive.
# Every forged row fails validation before any termination can be issued.
$self=Get-CimInstance Win32_Process -Filter "ProcessId=$PID"
$wrongCreation=[pscustomobject]@{ProcessId=$PID;ExecutablePath=$self.ExecutablePath;
    CreationDate=$self.CreationDate.AddSeconds(-1)}
$rejected=$false
try {Stop-IdentityCheckedProcesses @($wrongCreation) @($self.ExecutablePath)}
catch {if($_.Exception.Message -notmatch 'Cleanup PID no longer'){throw};$rejected=$true}
if(!$rejected){throw 'Stale creation identity accepted'}
$rejected=$false
try {Stop-IdentityCheckedProcesses @($self) @('Z:\not-this-process.exe')}
catch {if($_.Exception.Message -notmatch 'Unowned cleanup image'){throw};$rejected=$true}
if(!$rejected){throw 'Unowned path accepted'}
"PASS stale creation identity and unowned image rejected before kill; test process $PID survives"
