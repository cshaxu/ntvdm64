param(
    [Parameter(Mandatory=$true)][string]$X86,
    [Parameter(Mandatory=$true)][string]$X64,
    [Parameter(Mandatory=$true)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$trees=@{x86=(Resolve-Path -LiteralPath $X86).Path;x64=(Resolve-Path -LiteralPath $X64).Path}
$logs=[IO.Path]::GetFullPath($LogRoot)
New-Item -ItemType Directory -Force $logs | Out-Null
function Run([string]$label,[string]$exe,[string[]]$arguments) {
    $quoted=@($arguments|ForEach-Object{'"'+$_+'"'})
    $p=Start-Process -FilePath $exe -ArgumentList $quoted -WorkingDirectory (Split-Path $exe) -WindowStyle Hidden -PassThru -RedirectStandardOutput "$logs/$label.stdout" -RedirectStandardError "$logs/$label.stderr"
    "pid=$($p.Id) start=$($p.StartTime.ToUniversalTime().ToString('o'))"|Set-Content "$logs/$label.process"
    if(!$p.WaitForExit(60000)){
        & taskkill.exe /PID $p.Id /T /F | Out-Null
        throw "$label exceeded fixture deadline"
    }
    $p.WaitForExit()
    "exit=$($p.ExitCode)"|Add-Content "$logs/$label.process"
    if($p.ExitCode){throw "$label failed: $($p.ExitCode); see $logs"}
    Get-Content "$logs/$label.stdout"
}
foreach($from in @('x86','x64')) {
    Run "image-$from" "$($trees[$from])/shared-image-machine.exe" @("$($trees.x86)/shared-image-machine.exe","$($trees.x64)/shared-image-machine.exe")
    foreach($to in @('x86','x64')) {
        Run "direct-$from-$to" "$($trees[$from])/shared-hook-payload_direct.exe" @("$($trees[$to])/shared-hook-payload_direct.exe")
        $mode=if($from -eq $to){'same'}else{'helper'}
        Run "helper-$from-$to" "$($trees[$from])/shared-cross-payload.exe" @("$($trees[$to])/shared-cross-payload.exe","$($trees[$to])/cross-helper.dll",$mode)
        if($from -ne $to){
            Run "reject-$from-$to" "$($trees[$from])/shared-cross-payload.exe" @("$($trees[$to])/shared-cross-payload.exe","$($trees[$to])/cross-helper.dll",'reject')
        }
    }
}
Run 'low-exhaustion' "$($trees.x64)/shared-hook-payload_direct.exe" @("$($trees.x64)/shared-hook-payload_direct.exe",'--exhaust-low')
'PASS shared matrix: machine access, four direct pairs, four helper/staging pairs, two rejections, low-address exhaustion'
