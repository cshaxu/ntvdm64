param(
    [Parameter(Mandatory)][string]$ProviderBuildRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$VsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Run this driver as an owned hidden background process. Native fixtures do
# not replace real Win16 acceptance through the production registrar.
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$provider = (Resolve-Path $ProviderBuildRoot).Path
# The formal CMake root now contains one Ninja subgraph per component.  Keep
# accepting the older component-root argument, but select the WOW32 graph when
# the caller supplies the stable configuration root.
if (Test-Path (Join-Path $provider 'wow32/build.ninja')) {
    $provider = (Resolve-Path (Join-Path $provider 'wow32')).Path
}
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path $out)) { throw 'Fresh repository build output required' }
$null = New-Item -ItemType Directory $out
function Invoke-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds) {
    $start = [DateTime]::UtcNow
    $p = Start-Process $file -ArgumentList $arguments -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.stdout.log" -RedirectStandardError "$out/$name.stderr.log"
    $null = $p.Handle
    @{pid=$p.Id;start=$start.ToString('o');deadline=$start.AddSeconds($seconds).ToString('o');file=$file;arguments=$arguments;workingDirectory=$out} | ConvertTo-Json | Set-Content "$out/$name.process.json"
    if (!$p.WaitForExit($seconds*1000)) {
        & taskkill.exe /PID $p.Id /T /F *> "$out/$name.timeout.log"
        throw "$name timeout"
    }
    $p.Refresh()
    $p.ExitCode | Set-Content "$out/$name.exit"
    if ($p.ExitCode) { throw "$name failed: $($p.ExitCode)" }
}
function Build([string]$name,[string]$command) {
    Invoke-Owned $name 'cmd.exe' ('/d /s /c "call "'+$VsDevCmd+'" -arch=x86 -host_arch=x64 >nul && '+$command+'"') 300
}
try {
    $ninja = (Get-Command ninja.exe -ErrorAction Stop).Source
    $objects = @('obj/wow32-dll/wow_bitmap_bindings.obj','obj/opennt-user/clres.obj',
        'obj/opennt-user/cldib.obj','obj/adapter-win32/chartran.obj',
        'obj/wow32-dll/ntuser_rtl_compat.obj')
    Build 'objects' ('"'+$ninja+'" -C "'+$provider+'" '+($objects -join ' '))
    $compile = & $ninja -C $provider -t commands obj/wow32-dll/wow_bitmap_bindings.obj
    if ($LASTEXITCODE -or @($compile).Count -ne 1) { throw 'Ambiguous production compile command' }
    # The production clres object also contains WOWFindResourceExWCover,
    # whose independent loader closure is outside this bitmap fixture.
    # Reuse its existing bitmap-fixture selection rather than stub the loader.
    $resourceCompile = & $ninja -C $provider -t commands obj/opennt-user/clres.obj
    if ($LASTEXITCODE -or @($resourceCompile).Count -ne 1) { throw 'Ambiguous resource compile command' }
    Build 'resource-owner' (($resourceCompile -replace '/Fo\S+', ('/Fo"'+$out+'/clres.obj"'))+' /DWOW_BITMAP_FIXTURE_BINDINGS /FI "'+$repo+'/src/wow32-dll/include/wow_bitmap_bindings.h"')
    & "$repo/tests/observation/new-wow-bitmap-test-resource.ps1" -BuildRoot $out
    Build 'resource' ('rc.exe /nologo /i "'+$out+'" /fo "'+$out+'/fixture.res" "'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_bitmap_binding_fixture.rc"')
    foreach ($case in @('binding','binding_failure')) {
        $source = "$repo/tests/adapter-mvdm-host-out/wow/wow_bitmap_${case}_fixture.c"
        $command = $compile -replace '/Fo\S+\s+\S+wow_bitmap_bindings\.c$', ('/Fo"'+$out+'/'+$case+'.obj" "'+$source+'"')
        if ($command -eq $compile) { throw 'Compile substitution failed' }
        Build "$case-compile" $command
        $linkObjects = '"'+$out+'/'+$case+'.obj"'
        if ($case -eq 'binding') {
            $linkObjects += ' "'+$out+'/fixture.res" '+(($objects | ForEach-Object {
                if ($_ -eq 'obj/opennt-user/clres.obj') {'"'+$out+'/clres.obj"'}
                else {'"'+$provider+'/'+$_+'"'}
            }) -join ' ')
        }
        Build "$case-link" ('link.exe /nologo /machine:x86 /subsystem:console /opt:ref /out:"'+$out+'/'+$case+'.exe" '+$linkObjects+' kernel32.lib user32.lib gdi32.lib ntdll.lib legacy_stdio_definitions.lib')
        Invoke-Owned "$case-run" "$out/$case.exe" ' ' 60
        $marker = if ($case -eq 'binding') {'WOW_BITMAP_BINDING_OK'} else {'WOW_BITMAP_BINDING_FAILURE_OK'}
        if (!(Select-String -LiteralPath "$out/$case-run.stdout.log" -SimpleMatch $marker)) { throw "Missing $marker" }
    }
    Invoke-Owned 'registration' (Get-Process -Id $PID).Path ('-NoProfile -ExecutionPolicy Bypass -File "'+$repo+'/tests/observation/verify-wow32-registration-surface.ps1" -RepositoryRoot "'+$repo+'"') 60
    0 | Set-Content "$out/suite.exit"
} catch {
    $_ | Out-String | Set-Content "$out/failure.txt"
    1 | Set-Content "$out/suite.exit"
    exit 1
}
