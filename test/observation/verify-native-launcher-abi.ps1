[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$NativeBuild,
    [Parameter(Mandatory)][string]$X86Build,
    [Parameter(Mandatory)][string]$PreviousLauncher,
    [Parameter(Mandatory)][string]$Hook64,
    [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
    throw 'Require fresh build-only evidence'
}
$null=New-Item -ItemType Directory -Path $log
$native=(Resolve-Path $NativeBuild).Path
$x86=(Resolve-Path $X86Build).Path
$results=[Collections.Generic.List[object]]::new()
function Run-Fixture([string]$Root,[string]$Name,[string[]]$Arguments=@()) {
    $tag=(Split-Path $Root -Leaf)+'-'+$Name
    & (Join-Path $Root $Name) @Arguments *> (Join-Path $log ($tag+'.txt'))
    $code=$LASTEXITCODE
    $results.Add([pscustomobject]@{Fixture=(Join-Path $Root $Name);Exit=$code})
    if($code){throw "Fixture failed: $Name ($code)"}
}
foreach($root in @($native,$x86)){
    Run-Fixture $root 'native-capture-test.exe'
    Run-Fixture $root 'frontend-scope-lifetime-test.exe'
    # The native graph defines OPENNT_ENVIRONMENT_ONLY. x86 retains the
    # entire existing RTL fixture; neither selects a new environment algorithm.
    Run-Fixture $root 'rtl-x86-fixture.exe'
}
Run-Fixture $native 'application-search-test.exe' @((Join-Path $log 'search-fixture'))
Run-Fixture $native 'run16-image-classification-test.exe' @(
    "$env:WINDIR/System32/notepad.exe", "$env:WINDIR/SysWOW64/cmd.exe",
    "$env:WINDIR/System32/kernel32.dll", "$env:WINDIR/System32/cmd.exe")
$import=Join-Path $repo 'tools/build/Stage-NativeWorkerImage.ps1'
foreach($case in @(
    @{Name='wrong-x86-image';Input=$PreviousLauncher;Pattern='Expected AMD64'},
    @{Name='dll-not-exe';Input=$Hook64;Pattern='Expected AMD64'},
    @{Name='outside-build';Input="$env:WINDIR/System32/cmd.exe";Pattern='must stay below build'}
)){
    $failure=$null
    try{& $import -InputFile $case.Input -OutputFile (Join-Path $log ($case.Name+'.exe'))}
    catch{$failure=$_.Exception.Message}
    if(!$failure -or $failure -notmatch $case.Pattern){throw "Wrong import rejection: $($case.Name): $failure"}
    $results.Add([pscustomobject]@{Fixture=$case.Name;Exit=0;Rejection=$failure})
}
& $import -InputFile (Join-Path $native 'run16.exe') -OutputFile (Join-Path $log 'accepted-run16.exe')
if((Get-FileHash (Join-Path $native 'run16.exe')).Hash -ne (Get-FileHash (Join-Path $log 'accepted-run16.exe')).Hash){throw 'Import hash mismatch'}
$results|ConvertTo-Json|Set-Content (Join-Path $log 'results.json')
'PASS native launcher capture/environment/scope/search/classification and exact native import/rejections'
