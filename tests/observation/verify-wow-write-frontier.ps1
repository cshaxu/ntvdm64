[CmdletBinding()]
param([Parameter(Mandatory)][string]$Snapshot)
# Checks saved observe-wow-frontiers output only. This proves retained startup
# windows, not editing, saving, normal application exit or absence of all bugs.
$ErrorActionPreference='Stop'
$text=Get-Content -LiteralPath $Snapshot -Raw
$samples=@([regex]::Split($text,'(?m)^sample-ms=')|Select-Object -Skip 1)
if($samples.Count -ne 4){throw 'WRITE frontier requires all four observation samples'}
$worker=$null
foreach($sample in $samples){
    $identity=[regex]::Match($sample,'(?m)^pid=(\d+) image=([^\r\n]+) alive=1\r?$')
    if(!$identity.Success){throw 'WRITE worker absent from snapshot'}
    $current=$identity.Groups[1].Value+'|'+$identity.Groups[2].Value
    if($worker -and $worker -ne $current){throw 'WRITE worker identity changed between samples'}
    $worker=$current
    foreach($class in @('MSWRITE_MENU','MSWRITE_DOC','MSWRITE_PAGEINFO')){
        if($sample -notmatch ('(?m)^window=[0-9A-Fa-f]+ class='+$class+' visible=1 text=')){
            throw "WRITE startup frontier missing visible $class"
        }
    }
    if($sample -match 'The NTVDM CPU has encountered an illegal instruction'){
        throw 'WRITE has an illegal-instruction dialog'
    }
}
Write-Output 'WOW_WRITE_STARTUP_FRONTIER samples=4 main=1 document=1 pageinfo=1 editing=unverified exit=unverified'
