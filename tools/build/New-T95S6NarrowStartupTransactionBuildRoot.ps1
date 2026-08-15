[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$R6Root = '',
    [string]$BuildRoot = '',
    [switch]$EnableBopCatalogListener,
    [switch]$EnableCpuResultBridge
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($R6Root)) {
    $R6Root = Join-Path $repository 'artifacts\build\bochs-2.6-native-adapter-msvc-r6'
}
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    throw 'BuildRoot is required; choose one new, uniquely named fresh root.'
}
$r6 = [IO.Path]::GetFullPath($R6Root)
$build = [IO.Path]::GetFullPath($BuildRoot)

if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build directory: $build"
}

$requiredR6Inputs = @(
    'ntdos64-native-adapter-observation.exe',
    'ntdos64-native-adapter-observation.mak',
    'main.cc',
    'main.o',
    'cpu\exception.cc',
    'cpu\exception.o',
    'cpu\event.cc',
    'cpu\event.o',
    'cpu\libcpu.a',
    'fpu\fpu.cc',
    'fpu\fpu.o',
    'fpu\libfpu.a',
    'fpu\Makefile',
    'Makefile',
    'cpu\Makefile'
)
foreach ($relativePath in $requiredR6Inputs) {
    $inputPath = Join-Path $r6 $relativePath
    if (-not (Test-Path -LiteralPath $inputPath -PathType Leaf)) {
        throw "Missing immutable r6 input: $inputPath"
    }
}

function Get-Sha256([string]$Path) {
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
}

function Copy-ClosureSources([string]$SourceDirectory, [string]$DestinationDirectory, [string[]]$Sources) {
    New-Item -ItemType Directory -Path $DestinationDirectory -Force | Out-Null
    $entries = @()
    foreach ($sourceName in $Sources) {
        $sourcePath = Join-Path $SourceDirectory $sourceName
        if (-not (Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
            throw "Missing declared closure source: $sourcePath"
        }
        $destinationPath = Join-Path $DestinationDirectory $sourceName
        Copy-Item -LiteralPath $sourcePath -Destination $destinationPath -ErrorAction Stop
        $sourceHash = Get-Sha256 $sourcePath
        if ((Get-Sha256 $destinationPath) -ne $sourceHash) {
            throw "Copy hash mismatch: $sourceName"
        }
        $entries += [ordered]@{ name = $sourceName; sha256 = $sourceHash }
    }

    $headerEntries = @()
    Get-ChildItem -LiteralPath $SourceDirectory -Filter *.h -File | Sort-Object Name | ForEach-Object {
        $destinationPath = Join-Path $DestinationDirectory $_.Name
        Copy-Item -LiteralPath $_.FullName -Destination $destinationPath -ErrorAction Stop
        $sourceHash = Get-Sha256 $_.FullName
        if ((Get-Sha256 $destinationPath) -ne $sourceHash) {
            throw "Header copy hash mismatch: $($_.Name)"
        }
        $headerEntries += [ordered]@{ name = $_.Name; sha256 = $sourceHash }
    }
    return [ordered]@{ sources = $entries; headers = $headerEntries }
}

& robocopy $r6 $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) {
    throw "r6 build-root copy failed with exit code $LASTEXITCODE"
}

$bochsReplacements = @(
    [ordered]@{ source = (Join-Path $repository 'refs\bochs\main.cc'); destination = (Join-Path $build 'main.cc'); staleObject = (Join-Path $build 'main.o') },
    [ordered]@{ source = (Join-Path $repository 'src\bx-core\cpu\exception.cc'); destination = (Join-Path $build 'cpu\exception.cc'); staleObject = (Join-Path $build 'cpu\exception.o') },
    [ordered]@{ source = (Join-Path $repository 'src\bx-core\cpu\event.cc'); destination = (Join-Path $build 'cpu\event.cc'); staleObject = (Join-Path $build 'cpu\event.o') },
    [ordered]@{ source = (Join-Path $repository 'src\bx-core\fpu\fpu.cc'); destination = (Join-Path $build 'fpu\fpu.cc'); staleObject = (Join-Path $build 'fpu\fpu.o') }
)
$bochsManifest = @()
foreach ($replacement in $bochsReplacements) {
    if (-not (Test-Path -LiteralPath $replacement.source -PathType Leaf)) {
        throw "Missing current Bochs source: $($replacement.source)"
    }
    Copy-Item -LiteralPath $replacement.source -Destination $replacement.destination -Force
    $sourceHash = Get-Sha256 $replacement.source
    if ((Get-Sha256 $replacement.destination) -ne $sourceHash) {
        throw "Bochs source copy hash mismatch: $($replacement.source)"
    }
    if (-not (Test-Path -LiteralPath $replacement.staleObject -PathType Leaf)) {
        throw "Expected copied stale object is absent: $($replacement.staleObject)"
    }
    Remove-Item -LiteralPath $replacement.staleObject -Force
    if (Test-Path -LiteralPath $replacement.staleObject) {
        throw "Failed to remove copied stale object: $($replacement.staleObject)"
    }
    $bochsManifest += [ordered]@{ path = $replacement.source.Substring($repository.Length + 1); sha256 = $sourceHash }
}

$staleCpuArchive = Join-Path $build 'cpu\libcpu.a'
Remove-Item -LiteralPath $staleCpuArchive -Force
if (Test-Path -LiteralPath $staleCpuArchive) {
    throw 'Failed to remove copied stale cpu/libcpu.a.'
}
$staleFpuArchive = Join-Path $build 'fpu\libfpu.a'
Remove-Item -LiteralPath $staleFpuArchive -Force
if (Test-Path -LiteralPath $staleFpuArchive) {
    throw 'Failed to remove copied stale fpu/libfpu.a.'
}

$adapterSources = @(
    'bx_ntvdm_startup_session_environment.c',
    'bx_ntvdm_startup_session.c',
    'bx_ntvdm_startup_transaction_bridge.c',
    'bx_ntvdm_cpu_delta_abi.c',
    'bx_ntvdm_cpu_result_v2.c',
    'bx_ntvdm_exception_abi.c',
    'bx_ntvdm_instruction_window_abi.c',
    'bx_ntvdm_cpu_state_abi.c',
    'bx_ntvdm_guest_range.c',
    'bx_ntvdm_guest_write_abi.c',
    'bx_ntvdm_session_resource_abi.c',
    'bx_ntvdm_host_service_catalog.c',
    'bx_ntvdm_host_transaction_abi.c',
    'bx_ntvdm_host_service_plane.c',
    'bx_ntvdm_host_drive_policy.c',
    'bx_ntvdm_namespace_root_set.c',
    'bx_ntvdm_startup_plan_abi.c'
)
if ($EnableBopCatalogListener) {
    $adapterSources += 'bx_ntvdm_bop_catalog_v1.c'
}
if ($EnableCpuResultBridge) {
    $adapterSources += @('bx_ntvdm_bios_memory_service.c',
        'bx_ntvdm_dem_boot_drive_service.c',
        'bx_ntvdm_cmd_current_dir_service.c',
        'bx_ntvdm_cmd_set_info_service.c',
        'bx_ntvdm_dem_debug_service.c',
        'bx_ntvdm_dem_drive_service.c',
        'bx_ntvdm_dem_dta_service.c',
        'bx_ntvdm_dem_hard_error_service.c',
        'bx_ntvdm_dem_dpb_service.c',
        'bx_ntvdm_guest_read_action_v1.c',
        'bx_ntvdm_multi_write_abi.c',
        'bx_ntvdm_multi_write_transaction.c',
        'bx_ntvdm_cpu_result_bridge_v1.c')
}
$cliSources = @('byob_profile.c', 'byob_identity.c', 'byob_image.c')
$adapterManifest = Copy-ClosureSources (Join-Path $repository 'src\bx-vdm') (Join-Path $build 'adapter') $adapterSources
$cliManifest = Copy-ClosureSources (Join-Path $repository 'src\cli') (Join-Path $build 'cli') $cliSources

$adapterObjects = @($adapterSources | ForEach-Object { 'adapter\' + ($_ -replace '\.c$', '.obj') })
$cliObjects = @($cliSources | ForEach-Object { 'cli\' + ($_ -replace '\.c$', '.obj') })
$closureObjects = @($adapterObjects + $cliObjects)
if ($closureObjects.Count -ne (20 + $(if ($EnableBopCatalogListener) { 1 } else { 0 }) + $(if ($EnableCpuResultBridge) { 13 } else { 0 }))) {
    throw "Unexpected narrow startup-transaction object count: $($closureObjects.Count)"
}
foreach ($requiredObject in @('adapter\bx_ntvdm_cpu_delta_abi.obj', 'adapter\bx_ntvdm_cpu_result_v2.obj')) {
    if ($closureObjects -notcontains $requiredObject) {
        throw "Missing r1-proven ABI provider from closure: $requiredObject"
    }
}
$rejectedTokens = @(
    'adapter\\bx_ntvdm_adapter_runtime\.obj',
      'adapter\\bx_ntvdm_host_session\.obj',
      'adapter\\bx_ntvdm_host_namespace\.obj',
      'adapter\\bx_ntvdm_guest_read(?!_action_v1\.(obj|c))',
    'adapter\\bx_ntvdm_guest_gather',
    'adapter\\bx_ntvdm_observation',
    'adapter\\bx_ntvdm_multi_write(?!_(abi|transaction)\.(obj|c))',
    'opennt\\',
    'softpc'
)

$cpuArchiveObjects = @(
    'init.o', 'cpu.o', 'event.o', 'icache.o', 'resolver.o', 'fetchdecode.o', 'access.o', 'access32.o', 'shift16.o', 'logical16.o', 'ctrl_xfer32.o', 'ctrl_xfer16.o', 'mmx.o', '3dnow.o', 'fpu_emu.o', 'sse.o', 'sse_move.o', 'sse_pfp.o', 'sse_rcp.o', 'sse_string.o', 'xsave.o', 'aes.o', 'svm.o', 'vmx.o', 'vmcs.o', 'vmexit.o', 'vmfunc.o', 'soft_int.o', 'apic.o', 'bcd.o', 'mult16.o', 'tasking.o', 'shift32.o', 'shift8.o', 'arith8.o', 'stack.o', 'stack16.o', 'protect_ctrl.o', 'mult8.o', 'load.o', 'data_xfer8.o', 'vm8086.o', 'logical8.o', 'logical32.o', 'arith16.o', 'segment_ctrl.o', 'data_xfer16.o', 'data_xfer32.o', 'exception.o', 'generic_cpuid.o', 'proc_ctrl.o', 'crregs.o', 'msr.o', 'smm.o', 'flag_ctrl_pro.o', 'stack32.o', 'debugstuff.o', 'flag_ctrl.o', 'mult32.o', 'arith32.o', 'jmp_far.o', 'call_far.o', 'ret_far.o', 'iret.o', 'ctrl_xfer_pro.o', 'segment_ctrl_pro.o', 'io.o', 'crc32.o', 'bit.o', 'bit16.o', 'bit32.o', 'bmi32.o', 'string.o', 'paging.o',
    'access64.o', 'arith64.o', 'ctrl_xfer64.o', 'data_xfer64.o', 'fetchdecode64.o', 'logical64.o', 'mult64.o', 'shift64.o', 'bit64.o', 'stack64.o', 'avx.o', 'avx_pfp.o', 'avx_fma.o', 'avx2.o', 'gather.o', 'bmi64.o', 'tbm32.o', 'tbm64.o', 'xop.o'
)

$rootLocalOptInMacros = @(
    'BX_NTVDM_ENABLE_EXECUTION_PLAN=1',
    'BX_NTVDM_ENABLE_STARTUP_TRANSACTION=1'
)
$exceptionOptInFlags = @(
    '/DBX_NTVDM_ENABLE_EXECUTION_PLAN=1',
    '/DBX_NTVDM_ENABLE_STARTUP_TRANSACTION=1'
)
if ($EnableBopCatalogListener) {
    $rootLocalOptInMacros += 'BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1'
    $exceptionOptInFlags += '/DBX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1'
}
if ($EnableCpuResultBridge) {
    $rootLocalOptInMacros += 'BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=1'
    $exceptionOptInFlags += '/DBX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=1'
}

$mak = @(
    '# Generated M0 T95 S6 narrow startup-transaction closure shim.',
    '# It is a future build input only; this generator invokes no build tool.',
    '!INCLUDE Makefile',
    '',
    ('ADAPTER_OBJS = ' + ($closureObjects -join ' ')),
    '',
    'main.o: main.cc',
    "`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) /DBX_NTVDM_ENABLE_EXECUTION_PLAN=1 /DBX_NTVDM_ENABLE_STARTUP_TRANSACTION=1 /Iadapter /Icli /Tpmain.cc /Fomain.o",
    '',
    'cpu\\exception.o: cpu\\exception.cc',
    ("`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) " + ($exceptionOptInFlags -join ' ') + ' /Iadapter /Icli /Tpcpu\\exception.cc /Focpu\\exception.o'),
    '',
    'cpu\\libcpu.a: cpu\\exception.o',
    "`tcd cpu && lib /nologo /subsystem:console /verbose /out:libcpu.a $($cpuArchiveObjects -join ' ')",
    ''
)
foreach ($object in $closureObjects) {
    $source = $object -replace '\.obj$', '.c'
    $includeFlags = if ($object.StartsWith('adapter\\', [StringComparison]::Ordinal)) { '/Iadapter /Icli' } else { '/Icli' }
    $mak += "${object}: $source"
    $mak += "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS $includeFlags /Fo$object $source"
    $mak += ''
}
$mak += @(
    'ntdos64-native-startup-transaction.exe: main.o cpu\\exception.o cpu\\libcpu.a $(ADAPTER_OBJS) iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a $(DISASM_LIB) $(BX_OBJS) $(SIMX86_OBJS) $(FPU_LIB)',
    "`tlink /nologo /subsystem:console /incremental:no /opt:ref /verbose:lib /map:ntdos64-native-startup-transaction.map /out:`$@ `$(BX_OBJS) `$(SIMX86_OBJS) iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a `$(DISASM_LIB) `$(FPU_LIB) `$(GUI_LINK_OPTS) `$(MCH_LINK_FLAGS) `$(SIMX86_LINK_FLAGS) `$(READLINE_LIB) `$(EXTRA_LINK_OPTS) `$(LIBS) `$(ADAPTER_OBJS) kernel32.lib bcrypt.lib",
    ''
)
$shim = Join-Path $build 'ntdos64-native-startup-transaction.mak'
[IO.File]::WriteAllText($shim, ($mak -join "`r`n"), [Text.UTF8Encoding]::new($false))
$shimText = Get-Content -LiteralPath $shim -Raw
if ($shimText -notmatch [regex]::Escape('ADAPTER_OBJS = ' + ($closureObjects -join ' '))) {
    throw 'Generated shim does not contain the complete frozen object list.'
}
if (($shimText | Select-String -AllMatches -Pattern 'BX_NTVDM_ENABLE_EXECUTION_PLAN=1').Matches.Count -ne 2 -or
    ($shimText | Select-String -AllMatches -Pattern 'BX_NTVDM_ENABLE_STARTUP_TRANSACTION=1').Matches.Count -ne 2) {
    throw 'Generated shim does not contain exactly two opt-in macro occurrences.'
}
if (($shimText | Select-String -AllMatches -Pattern 'BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1').Matches.Count -ne $(if ($EnableBopCatalogListener) { 1 } else { 0 })) {
    throw 'Generated shim does not contain the expected BOP catalogue listener macro occurrences.'
}
if (($shimText | Select-String -AllMatches -Pattern 'BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE=1').Matches.Count -ne $(if ($EnableCpuResultBridge) { 1 } else { 0 })) {
    throw 'Generated shim does not contain the expected CPU-result bridge macro occurrences.'
}
if ($shimText -match 'BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT') {
    throw 'Generated shim unexpectedly enables the broad exception interceptor.'
}
foreach ($token in $rejectedTokens) {
    if ($shimText -match $token) {
        throw "Generated shim contains rejected token: $token"
    }
}

$r6Inputs = @{}
foreach ($relativePath in $requiredR6Inputs) {
    $r6Inputs[$relativePath] = Get-Sha256 (Join-Path $r6 $relativePath)
}
$record = [ordered]@{
    schema = 'ntdos64.t95-s6.narrow-startup-transaction-build-root.v1'
    mode = 'generator-only-no-compiler-linker-archive-or-executable-invocation'
    r6Root = $r6
    r6InputsSha256 = $r6Inputs
    buildRoot = $build
    bochsReplacements = $bochsManifest
    adapter = $adapterManifest
    cli = $cliManifest
    closureObjects = $closureObjects
    rootLocalOptInMacros = $rootLocalOptInMacros
    bopCatalogListenerEnabled = [bool]$EnableBopCatalogListener
    cpuResultBridgeEnabled = [bool]$EnableCpuResultBridge
    absentMacro = 'BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT'
    rejectedTokens = $rejectedTokens
    shimSha256 = Get-Sha256 $shim
    buildCommand = $null
    retryPolicy = 'none'
}
[IO.File]::WriteAllText((Join-Path $build 'narrow-startup-transaction-build-root.json'), ($record | ConvertTo-Json -Depth 7), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fresh narrow startup-transaction build root: $build"
