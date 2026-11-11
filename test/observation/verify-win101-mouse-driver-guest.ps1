param(
    [Parameter(Mandatory)][string]$DriverBuild,
    [Parameter(Mandatory)][string]$PackageInput,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [ValidateSet('Mock','Real')][string]$Provider='Mock'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
$buildPrefix=(Join-Path $repo 'build')+'\'
if (!$build.StartsWith($buildPrefix,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)) {
    throw 'Fresh build-owned output required'
}
$input=(Resolve-Path -LiteralPath $PackageInput).Path
$driver=(Resolve-Path -LiteralPath $DriverBuild).Path
$observerPath=(Resolve-Path -LiteralPath $Observer).Path
if (!$input.StartsWith($buildPrefix,[StringComparison]::OrdinalIgnoreCase) -or
    !$driver.StartsWith($buildPrefix,[StringComparison]::OrdinalIgnoreCase)) { throw 'Build-owned inputs required' }
if (Test-Path Z:\) { throw 'Z: occupied; preserve mapping' }
New-Item -ItemType Directory -Path $build | Out-Null
$runtime=Join-Path $build 'runtime'
New-Item -ItemType Directory -Path $runtime | Out-Null
Copy-Item -Path "$input/*" -Destination $runtime -Recurse
$fixture=if($Provider -eq 'Mock'){'M101TEST.COM'}else{'M101REAL.COM'}
$marker=if($Provider -eq 'Mock'){'MOUSE101-MOCK-36-PASS'}else{'MOUSE101-REAL-11-PASS'}
Copy-Item -LiteralPath "$driver/$fixture" -Destination "$runtime/tests/$fixture"
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=$null
$mapped=$false
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$oldWindow=$env:MVDM_OBSERVER_WINDOW_INPUT
try {
    & subst.exe Z: $runtime
    if ($LASTEXITCODE) { throw 'Z: mapping failed' }
    $mapped=$true
    $scope=New-IsolatedPackageScope $runtime 'Z:\'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
    $report=Join-Path $build 'guest.txt'
    $proc=Start-Process -FilePath $observerPath -ArgumentList @(
        'Z:\system32\run16.exe','Z:\system32',$report,"Z:\tests\$fixture",
        '--observation-timeout-ms','20000') -WindowStyle Hidden -PassThru
    try {
        if (!$proc.WaitForExit(30000)) {
            $proc.Kill()
            if (!$proc.WaitForExit(5000)) { throw 'Owned observer did not stop' }
            throw 'Observer did not complete'
        }
        if ($proc.ExitCode -ne 0) { throw "Observer failed $($proc.ExitCode)" }
    } finally { $proc.Dispose() }
    if (!(Test-Path $report)) { throw 'Missing guest report' }
    $result=Get-Content $report -Raw
    $screen=Get-Content "$report.console.txt" -Raw
    if ($result -notmatch '(?m)^result=exited\r?$' -or
        $result -notmatch '(?m)^exit=0x00000000\r?$' -or
        $screen -notmatch $marker -or $screen -match 'MOUSE101-(MOCK|REAL)-FAIL') {
        throw "Guest contract failure; inspect $report"
    }
    "PASS $marker through selected CCPU40; not Windows interaction acceptance"
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
    $env:MVDM_OBSERVER_WINDOW_INPUT=$oldWindow
    try {
        if ($scope) { Stop-IsolatedPackageScope $scope }
    } finally {
        if ($mapped) { & subst.exe Z: /d }
    }
}
