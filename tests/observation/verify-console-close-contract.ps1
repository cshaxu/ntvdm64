[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$PackageRoot='O:\winnt',
    [Parameter(Mandatory)][string]$Prefix
)
$ErrorActionPreference='Stop'
if($Prefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid prefix'}
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
function Check([string]$file,[string]$name,[string]$mode,[string]$expected){
    $report=Join-Path $PackageRoot "logs\$Prefix-$name.txt"
    if(Test-Path -LiteralPath $report){throw 'Use a fresh report prefix'}
    $extra=@();if($mode){$extra=@($mode)}
    & $Observer (Join-Path $BuildRoot $file) $PackageRoot $report --observation-timeout-ms 20000 @extra
    if($LASTEXITCODE -or (Get-Content -LiteralPath $report -Raw) -notmatch "(?m)^exit=0x$expected\s*$"){
        throw "Failed $name; see $report and its .console.txt"
    }
    if((Get-Content -LiteralPath ($report+'.console.txt') -Raw) -notmatch 'PASS'){
        throw "Missing test assertion output: $name"
    }
    Write-Output "PASS $name"
}
# The callback is a test substitute; real guest closure is tested separately.
Check console-client-test.exe callback '' 00000049
Check console-client-test.exe hung-callback --close-hang c000013a
Check console-client-test.exe pipe-error --broken-pipe 00000000
$source=Get-Content (Join-Path $PSScriptRoot '..\adapter-basesrv\base_service_reservation_test.c') -Raw
$block=[regex]::Match($source,'static const char \*modes\[\]=\{([\s\S]*?)\};').Groups[1].Value
if(!$block){throw 'Missing service fixture mode catalogue'}
$modes=@('')+@([regex]::Matches($block,'"(--[a-z-]+)"') |
    ForEach-Object {$_.Groups[1].Value} | Where-Object {$_ -ne '--reservation-child'})
foreach($mode in $modes){
    $name=if($mode){$mode.TrimStart('-')}else{'default'}
    Check basesrv-service-reservation-test.exe "service-$name" $mode 00000000
}
