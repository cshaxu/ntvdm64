[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
# Current exact-body ownership contracts; not guest or DDE transport proof.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Fresh build-owned evidence directory required'
}
New-Item -ItemType Directory $root|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-dde > "$root/build.log" 2>&1
if($LASTEXITCODE){throw 'Native DDE build failed'}
foreach($kind in 'alias','execute'){
    $exe="$repo/build/.ninja/fixtures-x86/wow-dde-$kind.exe"
    $p=Start-Process $exe -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$kind.stdout.log" -RedirectStandardError "$root/$kind.stderr.log"
    $null=$p.Handle
    @{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');deadlineSeconds=30;exe=$exe}|ConvertTo-Json|Set-Content "$root/$kind.process.json"
    if(!$p.WaitForExit(30000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned DDE deadline expired'}
    $p.WaitForExit()
    $p.ExitCode|Set-Content "$root/$kind.exit"
    if($p.ExitCode -ne 0){throw "DDE $kind failed"}
    $text=Get-Content "$root/$kind.stdout.log" -Raw
    $marker=if($kind -eq 'alias'){'DDE_ALIAS cases=96 errors=0 nodes=0 guest_proof=0 transport_proof=0'}else{'DDE_EXECUTE cases=38 errors=0 modes=7 guest_proof=0'}
    if(!$text.Contains($marker) -or $text -match '(?m)^FAIL '){throw 'Incomplete current DDE assertions'}
    $text|Write-Output
    Get-FileHash $exe|ConvertTo-Json|Set-Content "$root/$kind.identity.json"
}
