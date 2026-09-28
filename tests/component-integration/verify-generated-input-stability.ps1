[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$NodeExecutable,
    [Parameter(Mandatory)][string]$NinjaExecutable
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$BuildRoot=(Resolve-Path -LiteralPath $BuildRoot).Path
if(!$BuildRoot.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'Use a prebuilt repository build cache'
}
$targets=@('run16.exe','ntkvm.exe','ntsrv.exe','ntvdm.exe','ntmon.exe','VDMREDIR.dll')
$files=@('generated/softpc-embedded-roms.rc','generated/ntvdm-wow32-provider.def')
$before=@($files | ForEach-Object {
    $path=Join-Path $BuildRoot $_
    [pscustomobject]@{Path=$path;Hash=(Get-FileHash $path).Hash;Ticks=(Get-Item $path).LastWriteTimeUtc.Ticks}
})
for($iteration=0;$iteration -lt 2;$iteration++){
    & (Join-Path $repo 'tools/build/New-T310OriginalSoftpcNinja.ps1') -Architecture x86 `
        -RepositoryRoot $repo -BuildRoot $BuildRoot -NodeExecutable $NodeExecutable -NinjaExecutable $NinjaExecutable
    if(!$?){throw 'Graph generation failed'}
    foreach($entry in $before){
        if((Get-FileHash $entry.Path).Hash -ne $entry.Hash -or
            (Get-Item $entry.Path).LastWriteTimeUtc.Ticks -ne $entry.Ticks){
            throw ('Unchanged generated input was rewritten: '+$entry.Path)
        }
    }
    $plan=(& $NinjaExecutable -C $BuildRoot -n @targets 2>&1 | Out-String)
    if($LASTEXITCODE -or $plan -notmatch 'ninja: no work to do\.'){
        throw ('Prebuilt production graph is not a no-op: '+$plan)
    }
}
Write-Output 'PASS two graph generations preserve ROM/DEF bytes and timestamps, with no production rebuild'
