param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$CompilerWrapper)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Require fresh build-owned evidence'}
$null=New-Item -ItemType Directory $root
foreach($variant in @('observed','ordinary')) {
    [string[]]$sources=if($variant -eq 'observed'){@("$repo/tests/app/ntvwm_palette_geometry_test.c")}else{
        @("$repo/tests/app/native_console_capture_test.c","$repo/src/ntvwm-exe/console_state.c")}
    $dir=Join-Path $root $variant
    $null=New-Item -ItemType Directory $dir
    & $CompilerWrapper cl.exe /nologo /MT /O2 /W4 /D_WIN32_WINNT=0x0A00 "/I$repo/src" "/Fo$dir\" "/Fe$dir/test.exe" @sources "$repo/src/common/console/members.c" /link user32.lib *> "$dir/build.txt"
    if($LASTEXITCODE){throw "Compile failed: $dir/build.txt"}
    $child=Start-Process "$dir/test.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/result.txt" -RedirectStandardError "$dir/stderr.txt"
    @{pid=$child.Id;start=$child.StartTime;deadline=$child.StartTime.AddSeconds(30)} | ConvertTo-Json | Set-Content "$dir/owner.json"
    if(!$child.WaitForExit(30000)){$child.Kill($true);throw 'Palette geometry test timeout'}
    if($child.ExitCode -ne 0){throw "Palette geometry test failed: $dir"}
    Get-Content "$dir/result.txt"
}
