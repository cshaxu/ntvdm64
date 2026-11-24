param([Parameter(Mandatory)][string]$BuildRoot,
    [ValidateSet('x86','x64')][string]$Architecture='x86')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
New-Item -ItemType Directory $root|Out-Null
& "$repo/tools/build-fixtures.cmd" $Architecture wow-gdi-alias > "$root/build.log" 2>&1
if($LASTEXITCODE){throw 'Native GDI build failed'}
$exe="$repo/build/.ninja/fixtures-$Architecture/wow-gdi-alias.exe"
$p=Start-Process $exe -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/runtime.out" -RedirectStandardError "$root/runtime.err"
$null=$p.Handle
@{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;timeoutSeconds=30}|ConvertTo-Json|Set-Content "$root/process.json"
if(!$p.WaitForExit(30000)){& taskkill.exe /PID $p.Id /T /F;throw 'Owned GDI deadline'}
$p.WaitForExit()
$p.ExitCode|Set-Content "$root/exit.txt"
if($p.ExitCode -ne 0 -or (Get-Content "$root/runtime.out" -Raw) -notmatch 'WOW_GDI_ALIAS errors=0'){throw 'GDI alias contract failed'}
Get-FileHash $exe|ConvertTo-Json|Set-Content "$root/identity.json"
Get-Content "$root/runtime.out"
