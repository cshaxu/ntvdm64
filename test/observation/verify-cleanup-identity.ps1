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

# Read-only identity lookup uses the already-built native resource utility.
$reader=(Resolve-Path "$PSScriptRoot/../../build/.ninja/build-tools/ntvdm-package-id.exe").Path
$fixture=Join-Path (Split-Path $reader) ('identity-test-'+[guid]::NewGuid().ToString('N')+'.exe')
try {
    if($null -ne (Get-IsolatedPackageIdentity $fixture)){throw 'Missing image accepted'}
    Copy-Item -LiteralPath $reader -Destination $fixture
    if($null -ne (Get-IsolatedPackageIdentity $fixture)){throw 'Unstamped image accepted'}
    $identity=[guid]::NewGuid()
    & $reader $identity.ToString() $fixture
    if($LASTEXITCODE){throw 'Fixture identity stamping failed'}
    $before=Get-IsolatedFileSha256 $fixture
    $expected=([BitConverter]::ToString($identity.ToByteArray())).Replace('-','')
    if((Get-IsolatedPackageIdentity $fixture) -cne $expected){throw 'Identity bytes mismatch'}
    if((Get-IsolatedFileSha256 $fixture) -ne $before){throw 'Identity read changed image'}
    'PASS native identity lookup, missing/unstamped rejection and read-only bytes'
} finally {
    if(Test-Path -LiteralPath $fixture){Remove-Item -LiteralPath $fixture}
}