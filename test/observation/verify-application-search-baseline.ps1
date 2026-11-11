param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$Probe,
      [Parameter(Mandatory)][string]$RunRoot)
$ErrorActionPreference='Stop'
$PackageRoot=(Resolve-Path $PackageRoot).Path
$Probe=(Resolve-Path $Probe).Path
$RunRoot=[IO.Path]::GetFullPath($RunRoot)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$RunRoot.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path $RunRoot)){throw 'Require a fresh repository-build run root'}
$null=New-Item -ItemType Directory -Path $RunRoot
$runtime=Join-Path $RunRoot 'runtime'
$cwd=Join-Path $RunRoot 'cwd'
$first=Join-Path $RunRoot 'path-first'
$second=Join-Path $RunRoot 'path-second'
foreach($directory in @($runtime,$cwd,$first,$second)){
    $null=New-Item -ItemType Directory -Path $directory
}
foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe',
                  'ntmon.exe','WOW32.DLL','VDMREDIR.DLL')){
    Copy-Item -LiteralPath (Join-Path $PackageRoot $name) -Destination $runtime
}
$leaf='search-identity.exe'
foreach($directory in @($runtime,$cwd,$first,$second)){
    Copy-Item -LiteralPath $Probe -Destination (Join-Path $directory $leaf)
}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $runtime
$results=@()
function Run-Case([string]$Name,[string]$Argument,[string]$Expected){
    $report=Join-Path $RunRoot ($Name+'.image.txt')
    $start=[Diagnostics.ProcessStartInfo]::new((Join-Path $runtime 'run16.exe'))
    $start.UseShellExecute=$false
    $start.CreateNoWindow=$true
    $start.WorkingDirectory=$cwd
    $start.Arguments='--wait "'+$Argument+'"'
    $start.EnvironmentVariables['PATH']=$first+';'+$second
    $start.EnvironmentVariables['SEARCH_IDENTITY_REPORT']=$report
    $process=[Diagnostics.Process]::Start($start)
    try {
        if(!$process.WaitForExit(20000)){throw "Live launcher timeout: $($process.Id)"}
        if($process.ExitCode -ne 37 -or !(Test-Path $report)){
            throw "Baseline launch failed: $Name exit=$($process.ExitCode)"
        }
        $actual=[IO.File]::ReadAllText($report,[Text.Encoding]::Unicode)
        if($actual -ine $Expected){throw "Wrong image: $Name actual=$actual expected=$Expected"}
        $script:results+=[pscustomobject]@{Case=$Name;Image=$actual;Exit=$process.ExitCode}
        "CONFIRMED $Name $actual exit=37"
    }finally{$process.Dispose()}
}
try {
    # These assertions document the defect; they are NOT desired-policy tests.
    Run-Case 'package-shadows-cwd' $leaf (Join-Path $runtime $leaf)
    Run-Case 'explicit-cwd' (Join-Path $cwd $leaf) (Join-Path $cwd $leaf)
    Remove-Item -LiteralPath (Join-Path $cwd $leaf)
    Run-Case 'package-shadows-path' $leaf (Join-Path $runtime $leaf)
    Remove-Item -LiteralPath (Join-Path $runtime $leaf)
    Run-Case 'ordered-path-without-package' $leaf (Join-Path $first $leaf)
    $results | ConvertTo-Json | Set-Content (Join-Path $RunRoot 'results.json')
}finally{Stop-IsolatedPackageScope $scope}
