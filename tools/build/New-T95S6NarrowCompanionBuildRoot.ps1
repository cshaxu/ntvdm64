[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$R4Root = '',
    [string]$BuildRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($R4Root)) {
    $R4Root = Join-Path $repository 'artifacts\build\bochs-2.6-native-intact-msvc-r4'
}
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-adapter-msvc-r1'
}
$r4 = [IO.Path]::GetFullPath($R4Root)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (-not (Test-Path -LiteralPath (Join-Path $r4 'ntdos64-native-container.exe') -PathType Leaf)) {
    throw "Missing r4 source-built container: $r4"
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }

function Copy-DeclaredSources([string]$SourceDirectory, [string]$DestinationDirectory, [object[]]$Sources) {
    New-Item -ItemType Directory -Path $DestinationDirectory -Force | Out-Null
    Get-ChildItem -LiteralPath $SourceDirectory -Filter *.h | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $DestinationDirectory -ErrorAction Stop
    }
    $entries = @()
    foreach ($sourceName in $Sources) {
        $sourcePath = Join-Path $SourceDirectory $sourceName
        if (-not (Test-Path -LiteralPath $sourcePath -PathType Leaf)) { throw "Missing declared source: $sourcePath" }
        $destinationPath = Join-Path $DestinationDirectory $sourceName
        Copy-Item -LiteralPath $sourcePath -Destination $destinationPath -ErrorAction Stop
        $hash = (Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash
        if ((Get-FileHash -LiteralPath $destinationPath -Algorithm SHA256).Hash -ne $hash) {
            throw "Copy hash mismatch: $sourceName"
        }
        $entries += [ordered]@{ name = $sourceName; sha256 = $hash }
    }
    return $entries
}

& robocopy $r4 $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "r4 build-root copy failed with exit code $LASTEXITCODE" }

$currentMain = Join-Path $repository 'refs\bochs\main.cc'
$copiedMain = Join-Path $build 'main.cc'
Copy-Item -LiteralPath $currentMain -Destination $copiedMain -Force
$mainHash = (Get-FileHash -LiteralPath $currentMain -Algorithm SHA256).Hash
if ((Get-FileHash -LiteralPath $copiedMain -Algorithm SHA256).Hash -ne $mainHash) { throw 'main.cc copy hash mismatch' }
$staleMainObject = Join-Path $build 'main.o'
if (-not (Test-Path -LiteralPath $staleMainObject -PathType Leaf)) { throw "Expected stale r4 main object is absent: $staleMainObject" }
Remove-Item -LiteralPath $staleMainObject -Force
if (Test-Path -LiteralPath $staleMainObject) { throw 'Failed to remove stale build-local main.o' }

$adapterSources = @(
    'bx_ntvdm_startup_session_environment.c',
    'bx_ntvdm_startup_session.c',
    'bx_ntvdm_cpu_state_abi.c',
    'bx_ntvdm_guest_range.c',
    'bx_ntvdm_guest_write_abi.c',
    'bx_ntvdm_startup_plan_abi.c'
)
$cliSources = @('byob_profile.c', 'byob_identity.c', 'byob_image.c')
$adapterManifest = Copy-DeclaredSources (Join-Path $repository 'src\bx-vdm') (Join-Path $build 'adapter') $adapterSources
$cliManifest = Copy-DeclaredSources (Join-Path $repository 'src\cli') (Join-Path $build 'cli') $cliSources

$adapterObjects = @($adapterSources | ForEach-Object { 'adapter\' + ($_ -replace '\.c$', '.obj') })
$cliObjects = @($cliSources | ForEach-Object { 'cli\' + ($_ -replace '\.c$', '.obj') })
$allAdapterObjects = @($adapterObjects + $cliObjects)
$mak = @(
    '# Generated S6 narrow companion observation shim; r4 Makefile remains included unchanged.',
    '!INCLUDE Makefile',
    '',
    ('ADAPTER_OBJS = ' + ($allAdapterObjects -join ' ')),
    '',
    'main.o: main.cc',
    "`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_EXECUTION_PLAN=1 /Iadapter /Tpmain.cc /Fomain.o",
    ''
)
foreach ($object in $allAdapterObjects) {
    $source = $object -replace '\.obj$', '.c'
    $mak += "${object}: $source"
    $mak += "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Fo$object $source"
    $mak += ''
}
$mak += @(
    'ntdos64-native-adapter-observation.exe: main.o $(ADAPTER_OBJS) iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a $(DISASM_LIB) $(BX_OBJS) $(SIMX86_OBJS) $(FPU_LIB)',
    "`tlink /nologo /subsystem:console /incremental:no /opt:ref /verbose:lib /map:ntdos64-native-adapter-observation.map /out:`$@ `$(BX_OBJS) `$(SIMX86_OBJS) iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a `$(DISASM_LIB) `$(FPU_LIB) `$(GUI_LINK_OPTS) `$(MCH_LINK_FLAGS) `$(SIMX86_LINK_FLAGS) `$(READLINE_LIB) `$(EXTRA_LINK_OPTS) `$(LIBS) `$(ADAPTER_OBJS) kernel32.lib bcrypt.lib",
    ''
)
$shim = Join-Path $build 'ntdos64-native-adapter-observation.mak'
[IO.File]::WriteAllText($shim, ($mak -join "`r`n"), [Text.UTF8Encoding]::new($false))
$shimText = Get-Content -LiteralPath $shim -Raw
$expectedAdapterLine = 'ADAPTER_OBJS = ' + ($allAdapterObjects -join ' ')
if ($shimText.IndexOf($expectedAdapterLine, [StringComparison]::Ordinal) -lt 0) {
    throw 'Generated companion shim does not contain one complete ADAPTER_OBJS line.'
}

$record = [ordered]@{
    schema = 'ntdos64.t95-s6.narrow-companion-build-root.v1'
    r4Root = $r4
    r4ContainerSha256 = (Get-FileHash -LiteralPath (Join-Path $r4 'ntdos64-native-container.exe') -Algorithm SHA256).Hash
    buildRoot = $build
    mainSourceSha256 = $mainHash
    adapterSources = $adapterManifest
    cliSources = $cliManifest
    adapterObjects = $allAdapterObjects
    shimSha256 = (Get-FileHash -LiteralPath $shim -Algorithm SHA256).Hash
    permittedCommand = 'nmake /f ntdos64-native-adapter-observation.mak ntdos64-native-adapter-observation.exe'
    retryPolicy = 'none'
}
[IO.File]::WriteAllText((Join-Path $build 'narrow-companion-build-root.json'), ($record | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fresh narrow companion build root: $build"
