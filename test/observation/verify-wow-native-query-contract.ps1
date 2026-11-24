[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
$provider="$repo/build/.ninja/msvc-x86-release/wow32"
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$compiler="$provider/msvc-x86.cmd";$fixture="$PSScriptRoot/wow_native_query_contract.c"
try {
    & "$repo/tools/build-fixtures.cmd" x86 wow-native-references *> "$root/build.log"
    if($LASTEXITCODE){throw 'Native reference compilation failed'}
    Copy-Item "$provider/native-query.exe" "$root/probe.exe"
    $p=Start-Process "$root/probe.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/result.txt" -RedirectStandardError "$root/error.txt"
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');deadlineSeconds=30;image="$root/probe.exe"}|ConvertTo-Json|Set-Content "$root/process.json"
    if(!$p.WaitForExit(30000)){Stop-Process -Id $p.Id -Force;throw 'Owned query-contract timeout'}
    $p.WaitForExit();if($p.ExitCode){throw "Query-contract failed $($p.ExitCode)"}
    if((Get-Content "$root/result.txt" -Raw) -notmatch 'WOW_NATIVE_QUERY_CONTRACT checks=\d+ errors=0 rows=8'){throw 'Missing query matrix success'}
    $objects=@(Get-Content "$provider/native-query-objects.txt")
    & $compiler dumpbin.exe /symbols @objects *> "$root/product-symbols.txt"
    if($LASTEXITCODE){throw 'Current product symbol read failed'}
    $symbols=Get-Content "$root/product-symbols.txt" -Raw
    foreach($name in @('GetClientRect','GetWindowRect','GetClassNameA','GetPropA','SetPropA','RemovePropA','ExcludeUpdateRgn','InvalidateRgn','ValidateRgn')){
        if($symbols -match "wow_native_$name" -or $symbols -notmatch "UNDEF[^\r\n]*_$name@") {throw "Original native query binding missing: $name"}
    }
    Get-FileHash (@($fixture,$PSCommandPath,$compiler,"$root/probe.exe")+$objects)|Export-Csv -NoTypeInformation "$root/inputs.csv"
    0|Set-Content "$root/exit.txt"
}catch {$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";throw}
