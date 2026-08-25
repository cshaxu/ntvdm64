[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BaseRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BaseRoot)) {
    $BaseRoot = Join-Path $repository 'artifacts\build\current\t185-v5-selection-engine-r1'
}
$base = [IO.Path]::GetFullPath($BaseRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }

function Hash([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing required input: $Path" }
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
}
function Copy-Verified([string]$Source, [string]$Destination) {
    $hash = Hash $Source
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    if ((Hash $Destination) -ne $hash) { throw "Copy hash mismatch: $Source" }
    $hash
}

$baseManifestPath = Join-Path $base 't182-s2-finite-sequence-engine-derivative.json'
$baseManifest = Get-Content -LiteralPath $baseManifestPath -Raw | ConvertFrom-Json
if ($baseManifest.adapterSources.Count -ne 67 -or $baseManifest.cliSources.Count -ne 5 -or
    (Hash (Join-Path $base 'runner-t182-finite-sequence.exe')) -ne
        'DE750579769772AFF1F5ECCB8AD1C4F8215A0FC4A8D21C438CDECFF8B9AE528A') {
    throw 'Base root is not the retained T185 current finite-sequence engine closure.'
}

& robocopy $base $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "Base copy failed: $LASTEXITCODE" }
$sourceHash = Copy-Verified -Source (Join-Path $repository 'src\bochs-core\cpu\exception.cc') `
    -Destination (Join-Path $build 'cpu\exception.cc')

$objects = @($baseManifest.adapterSources | ForEach-Object { 'adapter\' + ($_.name -replace '\.c$', '.obj') }) +
    @($baseManifest.cliSources | ForEach-Object { 'cli\' + ($_.name -replace '\.c$', '.obj') })
if ($objects.Count -ne 72) { throw 'Expected exactly 72 retained adapter/CLI objects.' }
foreach ($object in $objects) { [void](Hash (Join-Path $build $object)) }

$retained = @('main.o','cpu\libcpu.a','iodev\libiodev.a','memory\libmemory.a','gui\libgui.a',
    'machine\bx_ntvdm_machine_bop_v1.obj','machine\unexp_nt.c.obj','machine\illegalp.c.obj')
$record = [ordered]@{
    schema = 'runner.t187.s3.passive-listener-engine-derivative.v1'
    baseRoot = $base
    baseEngineSha256 = Hash (Join-Path $base 'runner-t182-finite-sequence.exe')
    buildRoot = $build
    bochsObjectTargets = @('cpu\exception.o')
    finalTargets = @('runner-t187-passive-listener.exe','runner-t187-passive-listener.map')
    retainedAdapterCliObjectCount = $objects.Count
    exceptionSourceSha256 = $sourceHash
    exceptionMacroSet = @('BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1',
        'BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1',
        'BX_NTVDM_ENABLE_STARTUP_TRANSACTION=0',
        'BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0',
        'BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1',
        'BX_NTVDM_ENABLE_MACHINE_COMPOSITION=1')
    retainedMachineInputs = @()
}
foreach ($path in $retained) { $record.retainedMachineInputs += [ordered]@{ path = $path; sha256 = Hash (Join-Path $build $path) } }

$make = @(
    '# Generated T187 S3 derivative: exactly one Bochs object target plus final link.',
    '!INCLUDE Makefile','',
    ('ADAPTER_OBJS = ' + ($objects -join ' ')),'',
    'cpu\exception.o: cpu\exception.cc',
    "`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=1 /DBX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1 /DBX_NTVDM_ENABLE_STARTUP_TRANSACTION=0 /DBX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=0 /DBX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1 /DBX_NTVDM_ENABLE_MACHINE_COMPOSITION=1 /Iadapter /Icli /Imachine /Tpcpu\exception.cc /Focpu\exception.o",'',
    'runner-t187-passive-listener.exe: cpu\exception.o',
    "`tlink /nologo /subsystem:console /incremental:no /opt:ref /map:runner-t187-passive-listener.map /out:`$@ `$(BX_OBJS) `$(SIMX86_OBJS) main.o cpu\exception.o iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a `$(DISASM_LIB) `$(FPU_LIB) `$(GUI_LINK_OPTS) `$(MCH_LINK_FLAGS) `$(SIMX86_LINK_FLAGS) `$(READLINE_LIB) `$(EXTRA_LINK_OPTS) `$(LIBS) machine\bx_ntvdm_machine_bop_v1.obj machine\unexp_nt.c.obj machine\illegalp.c.obj vcruntime.lib `$(ADAPTER_OBJS) kernel32.lib bcrypt.lib",''
)
$makePath = Join-Path $build 'runner-t187-passive-listener.mak'
[IO.File]::WriteAllText($makePath, ($make -join "`r`n"), [Text.UTF8Encoding]::new($false))
$makeText = Get-Content -LiteralPath $makePath -Raw
if ($makeText -notmatch '(?m)^cpu\\exception\.o: cpu\\exception\.cc\r?$' -or
    $makeText -match '(?m)^.*(?:main\.o|libcpu|libiodev).*:') {
    throw 'Generated makefile has an unauthorized build target.'
}
$record.makefileSha256 = Hash $makePath
$record.permittedBuild = 'nmake /f runner-t187-passive-listener.mak runner-t187-passive-listener.exe'
[IO.File]::WriteAllText((Join-Path $build 't187-s3-passive-listener-engine-derivative.json'),
    ($record | ConvertTo-Json -Depth 6), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared T187 S3 passive-listener derivative: $build"
