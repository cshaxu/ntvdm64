[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Nasm,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Prefix,
    [string]$PackageRoot='O:\winnt'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'BuildRoot must be below repository build'
}
if($Prefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid evidence prefix'}
$report=Join-Path $PackageRoot "logs\$Prefix.txt"
$guest=Join-Path $PackageRoot 'tests\S2EXIT7.COM'
$batch=Join-Path $PackageRoot 'tests\S2NZ.BAT'
if(Test-Path -LiteralPath $report){throw "Refusing to overwrite $report"}
New-Item -ItemType Directory -Path $build -Force | Out-Null
$image=Join-Path $build 'S2EXIT7.COM'
& $Nasm -f bin (Join-Path $PSScriptRoot 'dpmi_exec_child.asm') -o $image
if($LASTEXITCODE){throw 'Guest test probe assembly failed'}
# Existing independently authored probe: valid tail returns 7, absent tail 99.
# Original guest media and production configuration are never modified.
foreach($pair in @(@($image,$guest),@((Join-Path $PSScriptRoot 'dos-nonzero-continue.bat'),$batch))){
    if((Test-Path -LiteralPath $pair[1]) -and
        (Get-FileHash -LiteralPath $pair[0]).Hash -ne (Get-FileHash -LiteralPath $pair[1]).Hash){
        throw "Refusing to replace different test input $($pair[1])"
    }
    Copy-Item -LiteralPath $pair[0] -Destination $pair[1]
}
$prior=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP')
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    & $Observer (Join-Path $PackageRoot 'run16.exe') $PackageRoot $report COMMAND.COM /c tests\S2NZ.BAT `
        --observation-timeout-ms 40000
    if($LASTEXITCODE){throw 'Observer failed'}
    $result=Get-Content -LiteralPath $report -Raw
    if($result -notmatch '(?m)^result=exited\s*$' -or $result -notmatch '(?m)^exit=0x00000000\s*$'){
        throw 'COMMAND /c batch did not finish successfully'
    }
    $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
    foreach($marker in @('S38_EXEC_CHILD_TAIL_OK','S2-DOS-EXIT-7-RETAINED',
        'S38_FAIL_EXEC_CHILD_TAIL','S2-DOS-EXIT-99-RETAINED',
        'bytes total conventional memory','S2-DOS-AFTER-NONZERO')){
        if(!$screen.Contains($marker)){throw "Missing actual guest output: $marker"}
    }
    if($screen.Contains('S2-DOS-NONZERO-FAILED')){throw 'Guest errorlevel assertion failed'}
    'PASS: DOS exits 7 and 99 retain errorlevel; live COMMAND runs MEM; batch finishes with 0'
} finally {
    [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP',$prior)
}
