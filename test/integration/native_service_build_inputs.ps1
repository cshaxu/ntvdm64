[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$NativeService,
    [Parameter(Mandatory)][string]$I386Service,
    [Parameter(Mandatory)][string]$ConsoleImage,
    [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=(Join-Path $repo 'build')+'\'
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Require fresh build-owned evidence'}
$null=New-Item -ItemType Directory -Path $log
$script=Join-Path $repo 'tools/build/Stage-NativeWorkerImage.ps1'
$rows=[Collections.Generic.List[object]]::new()
function Reject([string]$Name,[string]$InputPath,[string]$OutputPath){
    $rejected=$false
    try{& $script -InputFile $InputPath -OutputFile $OutputPath -Subsystem Windows}
    catch{$rejected=$true;$_.Exception.Message|Set-Content (Join-Path $log "$Name.txt")}
    if(!$rejected -or (Test-Path $OutputPath)){throw "Negative not rejected before copy: $Name"}
    $rows.Add(@{Case=$Name;Passed=$true})
}
Reject 'wrong-machine' $I386Service (Join-Path $log 'i386-rejected.exe')
Reject 'wrong-subsystem' $ConsoleImage (Join-Path $log 'console-rejected.exe')
# A nonexistent outside-build destination must remain nonexistent.
$outside=Join-Path ([IO.Path]::GetTempPath()) ('ntsrv-rejected-'+[guid]::NewGuid().ToString('N')+'.exe')
Reject 'outside-build-output' $NativeService $outside
$output=Join-Path $log 'ntsrv.exe'
& $script -InputFile $NativeService -OutputFile $output -Subsystem Windows
if((Get-FileHash $output).Hash -ne (Get-FileHash $NativeService).Hash){throw 'Positive import identity mismatch'}
$rows.Add(@{Case='native-windows-service';Passed=$true})
$rows|ConvertTo-Json|Set-Content (Join-Path $log 'results.json')
'PASS native service import and three input-boundary negatives'
