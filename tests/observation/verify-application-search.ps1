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
$cwd=Join-Path $RunRoot 'cwd space'
$first=Join-Path $RunRoot 'path-first'
$second=Join-Path $RunRoot 'path-二'
foreach($directory in @($runtime,$cwd,$first,$second)){
    $null=New-Item -ItemType Directory -Path $directory
}
Copy-Item -Path (Join-Path $PackageRoot '*') -Destination $runtime -Recurse
$leaf='search-identity.exe'
foreach($directory in @($runtime,$cwd,$first,$second)){
    Copy-Item -LiteralPath $Probe -Destination (Join-Path $directory $leaf)
}
Copy-Item -LiteralPath $Probe -Destination (Join-Path $first 'directory-first.exe')
Copy-Item -LiteralPath $Probe -Destination (Join-Path $second 'directory-first.com')
Copy-Item -LiteralPath $Probe -Destination (Join-Path $cwd 'suffix.com')
Copy-Item -LiteralPath $Probe -Destination (Join-Path $cwd 'suffix.exe')
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $runtime
$results=@()
function Run-Case([string]$Name,[string]$Argument,[string]$Expected,
    [string]$PathValue=($first+';'+$second),[string]$WorkingDirectory=$cwd,[switch]$UnsetPath){
    $report=Join-Path $RunRoot ($Name+'.image.txt')
    $start=[Diagnostics.ProcessStartInfo]::new((Join-Path $runtime 'run16.exe'))
    $start.UseShellExecute=$false
    $start.CreateNoWindow=$true
    $start.WorkingDirectory=$WorkingDirectory
    $start.ArgumentList.Add('--wait');$start.ArgumentList.Add($Argument)
    if($UnsetPath){$null=$start.Environment.Remove('PATH')}else{$start.Environment['PATH']=$PathValue}
    $start.Environment['SEARCH_IDENTITY_REPORT']=$report
    $start.Environment['NtvdmSystemRoot']='C:\not-the-package'
    $process=[Diagnostics.Process]::Start($start)
    try {
        if(!$process.WaitForExit(20000)){throw "Live launcher timeout: $($process.Id)"}
        if($Expected){
            if($process.ExitCode -ne 37 -or !(Test-Path $report)){
                throw "Launch failed: $Name exit=$($process.ExitCode)"
            }
            $actual=[IO.File]::ReadAllText($report,[Text.Encoding]::Unicode)
            if($actual -ine $Expected){throw "Wrong image: $Name actual=$actual expected=$Expected"}
        }else{
            if($process.ExitCode -eq 0 -or (Test-Path $report)){throw "Unexpected fallback: $Name exit=$($process.ExitCode)"}
            $actual='not executed'
        }
        $script:results+=[pscustomobject]@{Case=$Name;Image=$actual;Exit=$process.ExitCode;Expected=$Expected}
        "PASS $Name $actual exit=$($process.ExitCode)"
    }finally{$process.Dispose()}
}
try {
    Run-Case 'cwd-before-package-and-path' $leaf (Join-Path $cwd $leaf)
    Run-Case 'explicit-second' (Join-Path $second $leaf) (Join-Path $second $leaf)
    Run-Case 'suffix-com-before-exe' 'suffix' (Join-Path $cwd 'suffix.com')
    Run-Case 'explicit-suffix-exe' 'suffix.exe' (Join-Path $cwd 'suffix.exe')
    Run-Case 'relative-explicit' '.\suffix' (Join-Path $cwd 'suffix.com')
    Run-Case 'drive-relative' ($cwd.Substring(0,2)+'suffix') (Join-Path $cwd 'suffix.com')
    Run-Case 'explicit-missing-never-path' '.\directory-first.exe' ''
    Run-Case 'drive-relative-missing-never-path' ($cwd.Substring(0,2)+'directory-first.exe') ''
    Run-Case 'directory-before-extension' 'directory-first' (Join-Path $first 'directory-first.exe')
    Remove-Item -LiteralPath (Join-Path $cwd $leaf)
    Run-Case 'first-path-not-package' $leaf (Join-Path $first $leaf)
    Run-Case 'reverse-path-Unicode' $leaf (Join-Path $second $leaf) ($second+';'+$first)
    Run-Case 'empty-path-does-not-find-package' $leaf '' ''
    Run-Case 'unset-path-does-not-find-package' $leaf '' -UnsetPath
    Run-Case 'empty-path-cwd-still-valid' 'suffix' (Join-Path $cwd 'suffix.com') ''
    Run-Case 'quoted-and-empty-path-entries' $leaf (Join-Path $second $leaf) (';"'+$second+'";;'+$first+';')
    Run-Case 'package-explicit-PATH' $leaf (Join-Path $runtime $leaf) ($runtime+';'+$first)
    Run-Case 'package-ordinary-CWD' $leaf (Join-Path $runtime $leaf) '' $runtime
    $results | ConvertTo-Json | Set-Content (Join-Path $RunRoot 'results.json')
}finally{Stop-IsolatedPackageScope $scope}
