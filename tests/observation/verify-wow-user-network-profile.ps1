[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [Parameter(Mandatory = $true)] [string]$RuntimeRoot,
    [Parameter(Mandatory = $true)] [string]$Wow32Provider,
    [int]$ObservationSeconds = 12
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$provider = (Resolve-Path -LiteralPath $Wow32Provider).Path
$profile = Join-Path $runtime 'SYSTEM.INI'
$network = Join-Path $runtime 'WFWNET.DRV'
$providerDestination = Join-Path $runtime 'WOW32.DLL'
$logs = Join-Path $runtime 'logs'
$fileTrace = Join-Path $logs 't422-s2-wow-network-file.log'
$dispatchTrace = Join-Path $logs 't422-s2-wow-network-dispatch.log'
$registrationTrace = Join-Path $logs 't422-s2-wow-network-registration.log'
$classTrace = Join-Path $logs 't422-s2-wow-network-class.log'
$windowTrace = Join-Path $logs 't422-s2-wow-network-window.log'
$callbackTrace = Join-Path $logs 't422-s2-wow-network-callback.log'
$output = Join-Path $logs 't422-s2-wow-network-console.log'
$errorOutput = Join-Path $logs 't422-s2-wow-network-console.stderr.log'

if ($ObservationSeconds -lt 3) { throw 'ObservationSeconds must be at least 3.' }
foreach ($name in @('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe')) {
    if (!(Test-Path -LiteralPath (Join-Path $build $name) -PathType Leaf)) {
        throw "Required selected build artifact is absent: $name"
    }
}
if (!(Test-Path -LiteralPath $provider -PathType Leaf)) {
    throw "Required WOW32 provider is absent: $provider"
}
foreach ($path in @($profile, $network)) {
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required original package media is absent: $path"
    }
}
if (@(Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue).Count -ne 0) {
    throw 'The package-profile observation requires no existing product session.'
}

New-Item -ItemType Directory -Force -Path $logs | Out-Null
$originalProfile = [IO.File]::ReadAllBytes($profile)
$originalHash = (Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash
$expectedNetworkHash = '4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA'
if ((Get-FileHash -LiteralPath $network -Algorithm SHA256).Hash -ne $expectedNetworkHash) {
    throw 'WFWNET.DRV is not the hash-pinned original OpenNT media.'
}

function Set-PackageNetworkDriver([string]$value) {
    $text = [Text.Encoding]::Default.GetString($originalProfile)
    $text = [Text.RegularExpressions.Regex]::Replace($text,
        '(?im)^network\.drv\s*=.*(?:\r?\n|$)', '')
    if ($text -notmatch '(?im)^\[boot\]\s*$') { $text += "`r`n[boot]`r`n" }
    $boot = [Text.RegularExpressions.Regex]::new('(?im)^(\[boot\]\s*\r?\n)')
    $text = $boot.Replace($text,
        "`$1; T422 S2 package-only controlled USER network-driver observation`r`nnetwork.drv=$value`r`n", 1)
    [IO.File]::WriteAllText($profile, $text, [Text.Encoding]::Default)
}

function Invoke-NetworkProfileBranch([string]$name, [string]$driver) {
    Set-PackageNetworkDriver $driver
    Remove-Item -LiteralPath $fileTrace,$dispatchTrace,$registrationTrace,$classTrace,$windowTrace,$callbackTrace,$output,$errorOutput -Force -ErrorAction SilentlyContinue
    $env:MVDM_WOW_FILE_TRACE_PATH = $fileTrace
    $env:MVDM_WOW_DISPATCH_TRACE_PATH = $dispatchTrace
    $env:MVDM_WOW_REGISTRATION_TRACE_PATH = $registrationTrace
    $env:MVDM_WOW_CLASS_TRACE_PATH = $classTrace
    $env:MVDM_WOW_WINDOW_TRACE_PATH = $windowTrace
    $env:MVDM_WOW_CALLBACK_TRACE_PATH = $callbackTrace
    $process = Start-Process -FilePath (Join-Path $runtime 'run16.exe') `
        -ArgumentList 'WINMINE.EXE' -WorkingDirectory $runtime `
        -RedirectStandardOutput $output -RedirectStandardError $errorOutput `
        -WindowStyle Hidden -PassThru
    try {
        Start-Sleep -Seconds $ObservationSeconds
        $dispatch = if (Test-Path -LiteralPath $dispatchTrace) {
            Get-Content -LiteralPath $dispatchTrace -Raw
        } else { '' }
        $files = if (Test-Path -LiteralPath $fileTrace) {
            Get-Content -LiteralPath $fileTrace -Raw
        } else { '' }
        $registration = if (Test-Path -LiteralPath $registrationTrace) {
            Get-Content -LiteralPath $registrationTrace -Raw
        } else { '' }
        $window = if (Test-Path -LiteralPath $windowTrace) {
            Get-Content -LiteralPath $windowTrace -Raw
        } else { '' }
        $callback = if (Test-Path -LiteralPath $callbackTrace) {
            Get-Content -LiteralPath $callbackTrace -Raw
        } else { '' }
        if ($dispatch -notmatch 'W32Dispatch') {
            throw "$name did not enter the actual WOW32 dispatch path."
        }
        if ($files -match '(?i)source=NETWORK\.DRV|normalized=.*NETWORK\.DRV') {
            throw "$name still fell through to the original NETWORK.DRV default."
        }
        if ($registration -notmatch 'RegisterClass result=0000[0-9A-F]{4}' -or
                $registration -notmatch 'CreateWindowEx result=') {
            throw "$name did not complete the original class lookup-to-window gateway."
        }
        if ($window -notmatch 'WindowDispatch .*message=0081' -or
                $window -notmatch 'WindowDispatch .*message=0001') {
            throw "$name did not dispatch the native creation messages through the original WOW window gateway."
        }
        if ($callback -notmatch 'W32WndProc message=0081 .* success=1' -or
                $callback -notmatch 'W32WndProc message=0001 .* success=1') {
            throw "$name did not complete original CallBack16 for window creation messages."
        }
        Write-Output "T422_S2_NETWORK_PROFILE_${name}_OK"
    }
    finally {
        if ($process -and !$process.HasExited) {
            Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
        }
        Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue |
            Stop-Process -Force -ErrorAction SilentlyContinue
        Remove-Item Env:MVDM_WOW_FILE_TRACE_PATH,Env:MVDM_WOW_DISPATCH_TRACE_PATH,Env:MVDM_WOW_REGISTRATION_TRACE_PATH,Env:MVDM_WOW_CLASS_TRACE_PATH,Env:MVDM_WOW_WINDOW_TRACE_PATH,Env:MVDM_WOW_CALLBACK_TRACE_PATH -ErrorAction SilentlyContinue
    }
}

try {
    foreach ($name in @('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe')) {
        Copy-Item -LiteralPath (Join-Path $build $name) -Destination (Join-Path $runtime $name) -Force
    }
    # WOW32 is loaded beside the worker by the package loader.  Keep this
    # deployment rooted in the caller-supplied package directory; system32 is
    # guest media and is not the provider load location.
    Copy-Item -LiteralPath $provider -Destination $providerDestination -Force
    if ((Get-FileHash -LiteralPath $providerDestination -Algorithm SHA256).Hash -ne
        (Get-FileHash -LiteralPath $provider -Algorithm SHA256).Hash) {
        throw 'The runtime WOW32.DLL does not match the selected provider build.'
    }
    Invoke-NetworkProfileBranch 'EMPTY' ''
    Invoke-NetworkProfileBranch 'WFWNET' 'wfwnet.drv'
    Write-Output 'T422_S2_WOW_USER_PACKAGE_NETWORK_PROFILE_OK'
}
finally {
    [IO.File]::WriteAllBytes($profile, $originalProfile)
    $restoredHash = (Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash
    if ($restoredHash -ne $originalHash) {
        throw 'SYSTEM.INI baseline was not restored byte-identically.'
    }
}
