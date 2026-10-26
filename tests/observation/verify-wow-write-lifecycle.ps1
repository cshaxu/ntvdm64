[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$CompilerDriver,
    [Parameter(Mandatory)][string]$BuildRoot,
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Use a fresh repository build evidence directory'
}
$CompilerDriver=(Resolve-Path -LiteralPath $CompilerDriver).Path
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot\isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null
$saved=@{}
foreach($name in @('TEMP','TMP','PATH',
    'MVDM_WOW_WINDOW_TRACE_PATH','MVDM_WOW_REGISTRATION_TRACE_PATH',
    'MVDM_WOW_DISPATCH_TRACE_PATH')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
try {
    & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root\probe.obj" "/Fe$root\probe.exe" "$PSScriptRoot\wow_write_lifecycle_probe.c" /link user32.lib *> "$root\compile.txt"
    if($LASTEXITCODE){throw "WRITE lifecycle probe build failed: $LASTEXITCODE"}
    $runtime=New-PhysicalRuntimeStage $PackageRoot -RuntimeImages $RuntimeImages
    $scope=New-IsolatedPackageScope $runtime
    $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
    $env:MVDM_WOW_WINDOW_TRACE_PATH="$root\windows.txt"
    $env:MVDM_WOW_REGISTRATION_TRACE_PATH="$root\registration.txt"
    $env:MVDM_WOW_DISPATCH_TRACE_PATH="$root\dispatch.txt"
    @('WRITE.EXE','system32\run16.exe','system32\ntvdm.exe','system32\WOW32.DLL') | ForEach-Object {
        [pscustomobject]@{File=$_;SHA256=(Get-IsolatedFileSha256 (Join-Path $runtime $_))}
    } | ConvertTo-Json | Set-Content "$root\inputs.json"
    & "$root\probe.exe" $runtime *> "$root\probe.txt"
    if($LASTEXITCODE){throw "WRITE restart lifecycle probe failed: $LASTEXITCODE"}
    'PASS WRITE normal close followed by clean restart and second normal close' | Set-Content "$root\result.txt"
} finally {
    if($scope){Stop-IsolatedPackageScope $scope};if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
