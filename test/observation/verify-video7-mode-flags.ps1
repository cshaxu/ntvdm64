[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$ObserverPath,
    [Parameter(Mandatory)][string]$InputPif,
    [Parameter(Mandatory)][string]$CommandCom,
    [Parameter(Mandatory)][string]$ProbeDirectory,
    [Parameter(Mandatory)][string]$OutputRoot,
    [switch]$AllModes
)
$ErrorActionPreference='Stop'
$runtime=(Resolve-Path -LiteralPath $RuntimeRoot).Path
$observer=(Resolve-Path -LiteralPath $ObserverPath).Path
$inputFile=(Resolve-Path -LiteralPath $InputPif).Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(Test-Path -LiteralPath $out){throw 'A fresh owned output directory is required'}
$probe=Join-Path (Resolve-Path -LiteralPath $ProbeDirectory).Path 'V7FLAGS.COM'
if(Test-Path -LiteralPath $probe){throw 'Preserve existing V7FLAGS.COM'}
$null=New-Item -ItemType Directory -Path $out
$pifTool=Join-Path $runtime 'system32\PIF.EXE'
$pifInfo=& $pifTool show $inputFile
if($LASTEXITCODE){throw 'Cannot read input PIF'}
$guestDirectory=($pifInfo|Where-Object {$_ -like 'DIRECTORY=*'}).Substring(10)
if(!(Test-Path -LiteralPath $guestDirectory)){throw 'PIF working directory does not exist'}
$command=(Resolve-Path -LiteralPath $CommandCom).Path
$process=$null
try {
    $defines=@(if($AllModes){'-DVIDEO7_ALL_MODES=1'})
    & nasm @defines -f bin (Join-Path $PSScriptRoot 'video7_mode_flags.asm') -o (Join-Path $out 'V7FLAGS.COM')
    if($LASTEXITCODE){throw 'NASM failed'}
    Copy-Item -LiteralPath (Join-Path $out 'V7FLAGS.COM') -Destination $probe
    $pif=Join-Path $out 'FLAGS.PIF'
    Copy-Item -LiteralPath $inputFile -Destination $pif
    & (Join-Path $runtime 'system32\PIF.EXE') update $pif --title 'Video7 BIOS mode flags' --program $command --arguments "/c $probe"
    if($LASTEXITCODE){throw 'PIF edit failed'}
    $start=[Diagnostics.ProcessStartInfo]::new($observer)
    $start.UseShellExecute=$false;$start.WindowStyle='Hidden'; $start.WorkingDirectory=$guestDirectory
    $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.Environment['MVDM_OBSERVER_OWN_JOB']='1'
    $start.Environment['PATH']="$(Split-Path $command);$env:SystemRoot\System32"
    $start.Environment['TEMP']=$env:TEMP
    $start.Environment['TMP']=$env:TMP
    $log=Join-Path $out 'observer.txt'
    foreach($arg in @((Join-Path $runtime 'system32\run16.exe'),$guestDirectory,$log,$pif,'--observation-timeout-ms','90000')){$start.ArgumentList.Add($arg)}
    $process=[Diagnostics.Process]::Start($start)
    @{pid=$process.Id;command=$start.FileName;arguments=@($start.ArgumentList);started=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content (Join-Path $out 'process.json')
    if(!$process.WaitForExit(100000)){$process.Kill($true);$process.WaitForExit();throw 'Owned BIOS diagnostic deadline'}
    $text=[IO.File]::ReadAllText($log+'.console.txt')
    $expected=if($AllModes){'VIDEO7_MODE_FLAGS_PASS modes=32 preserve=1 clear=1 invalid=2'}else{'VIDEO7_MODE_FLAGS_PASS modes=2 preserve=1 clear=1 invalid=2'}
    $passed=$text.Contains($expected) -and !$text.Contains('VIDEO7_MODE_FLAGS_FAIL')
    @{passed=$passed;observerExitCode=$process.ExitCode;workerSha256=(Get-FileHash (Join-Path $runtime 'system32\ntvdm.exe')).Hash;probeSha256=(Get-FileHash $probe).Hash;computerUse=$false;inputInjected=$false}|ConvertTo-Json|Set-Content (Join-Path $out 'result.json')
    $text
    if(!$passed){throw 'Video7 BIOS mode flags failed'}
} finally {
    if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
    if(Test-Path -LiteralPath $probe){Remove-Item -LiteralPath $probe}
}
