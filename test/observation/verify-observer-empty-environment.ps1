param([Parameter(Mandatory)][string]$BuildRoot,
    [string]$Environment='build/M0-T427/S4/r049/msvc-x86.cmd')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-root required'}
$name='MVDM_TEST_EMPTY_ENV_PROBE'
if([Environment]::GetEnvironmentVariables('Process').Contains($name)){throw 'Diagnostic variable already owned'}
$null=New-Item -ItemType Directory -Path $root
$compiler=(Resolve-Path $Environment).Path;$output=Join-Path $root 'empty-environment-probe.exe'
& $compiler cl.exe /nologo /MT /W4 /WX "/Fo$root/probe.obj" "/Fe$output" "$PSScriptRoot/observer_empty_environment_probe.c"
if($LASTEXITCODE){throw 'Native diagnostic compilation failed'}
try {
    [Environment]::SetEnvironmentVariable($name,$null,'Process')
    $empty=& $output empty;if($LASTEXITCODE){throw 'Native empty-variable contract failed'}
    Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue
    $absent=& $output absent;if($LASTEXITCODE){throw 'Native absent-variable contract failed'}
    @($empty,$absent)|Set-Content (Join-Path $root 'results.txt')
    @($empty,$absent)
}finally{Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
