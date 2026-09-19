[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Builder,
    [Parameter(Mandatory)][string]$Probe,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$PackageRoot='O:\winnt'
)
$ErrorActionPreference='Stop'
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid prefix' }
$Observer=(Resolve-Path $Observer).Path
$Builder=(Resolve-Path $Builder).Path
$Probe=(Resolve-Path $Probe).Path
$PackageRoot=(Resolve-Path $PackageRoot).Path
$programs=@('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object {Join-Path $PackageRoot $_}
function Processes {
    @(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $programs})
}
if ((Processes).Count) {throw 'Package in use'}
if ((Get-ItemPropertyValue 'HKLM:\SYSTEM\CurrentControlSet\Control\Nls\CodePage' OEMCP) -ne '437') {throw 'Requires recorded OEM437 profile'}
$root=Join-Path $PackageRoot 'tests\P37RUN'
$runtimeProbe=Join-Path $PackageRoot 'tests\OP37.COM'
$runtimePif=Join-Path $PackageRoot 'tests\OP37.PIF'
foreach($path in @($root,$runtimeProbe,$runtimePif)) {
    if(Test-Path -LiteralPath $path){throw "Test path already exists: $path"}
}
New-Item -ItemType Directory $root | Out-Null
Copy-Item -LiteralPath $Probe -Destination $runtimeProbe
$savedEnvironment=[Environment]::GetEnvironmentVariable('S37_CONFIGROOT','Process')
try {
    foreach($name in @('ASCII',('D'+[char]0xa3))) {
        if ((Processes).Count) {throw 'Previous runtime remains'}
        $dir=Join-Path $root $name
        New-Item -ItemType Directory $dir | Out-Null
        & $Builder $dir --oem-paths
        if($LASTEXITCODE){throw 'PIF builder failed'}
        Copy-Item -LiteralPath (Join-Path $dir 'P30.PIF') -Destination $runtimePif
        foreach($pair in @(@('config.nt','P30.NT'),@('autoexec.nt','P30AUTO.NT'))) {
            $source=Join-Path $PackageRoot $pair[0]
            $dest=Join-Path $dir $pair[1]
            Copy-Item -LiteralPath $source -Destination $dest
            if((Get-FileHash $source).Hash -ne (Get-FileHash $dest).Hash){throw 'Media copy mismatch'}
        }
        $env:S37_CONFIGROOT=$dir
        $route=if($name -eq 'ASCII'){'ascii'}else{'oem'}
        $report=Join-Path $PackageRoot "logs\$LogPrefix-$route.txt"
        if(Test-Path $report){throw 'Use fresh log prefix'}
        $launcher=0
        try {
            $p=Start-Process -FilePath $Observer -ArgumentList @($programs[0],$PackageRoot,$report,'tests\OP37.COM','--observation-timeout-ms','15000') -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
            if(!$p.WaitForExit(30000)){throw 'Observer still active'}
            $record=Get-Content $report -Raw
            $launcher=[int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
            $screen=[Text.Encoding]::GetEncoding(437).GetString([IO.File]::ReadAllBytes("$report.console.txt"))
            if($record -notmatch 'result=exited' -or $record -notmatch 'exit=0x00000000' -or
                $screen -notmatch 'S37_PIF_GUEST_OK' -or $screen -match 'Bad command or filename') {throw 'PIF guest failed'}
            $expected=$PackageRoot.Substring(3).TrimEnd('\')
            if($screen -notmatch ('S37_PIF_CWD='+[regex]::Escape($expected)+'(?:\r?\n|$)')){throw 'Inherited Console cwd policy changed'}
            Write-Host "PASS PIF $route configuration path"
        } finally {
            if($launcher) {
                foreach($child in (Processes)) {
                    if($child.ParentProcessId -eq $launcher){Stop-Process -Id $child.ProcessId -Force}
                }
            }
        }
        foreach($file in @('P30.PIF','P30.NT','P30AUTO.NT')){Remove-Item -LiteralPath (Join-Path $dir $file)}
        Remove-Item -LiteralPath $dir
    }
    Remove-Item -LiteralPath $runtimeProbe,$runtimePif
    Remove-Item -LiteralPath $root
} finally {
    [Environment]::SetEnvironmentVariable('S37_CONFIGROOT',$savedEnvironment,'Process')
}
