[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$AnalysisRoot = '',
    [string]$ProjectionConfig = ''
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $scriptPath = $MyInvocation.MyCommand.Path
    if ([string]::IsNullOrWhiteSpace($scriptPath)) { throw 'RepositoryRoot was not supplied and script path is unavailable.' }
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $scriptPath)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($AnalysisRoot)) {
    $AnalysisRoot = Join-Path $repository 'artifacts\analysis\t95-s6-msvc-native-container-manifest-001-20260811-001'
}
$analysis = [IO.Path]::GetFullPath($AnalysisRoot)
if ([string]::IsNullOrWhiteSpace($ProjectionConfig)) {
    $ProjectionConfig = Join-Path $repository 'artifacts\analysis\t95-s6-msvc-x86-minimal-port-compile-001-20260811-001\bochs\config.h'
}
$projection = [IO.Path]::GetFullPath($ProjectionConfig)

if (Test-Path -LiteralPath $analysis) { throw "Refusing to overwrite existing analysis directory: $analysis" }

$bochs = Join-Path $repository 'refs\bochs'
$inputs = @(
    (Join-Path $bochs 'config.h'),
    (Join-Path $bochs 'Makefile'),
    (Join-Path $bochs 'vs2008\bochs.vcproj'),
    (Join-Path $bochs 'vs2008\bochs.sln'),
    (Join-Path $bochs 'bios\BIOS-bochs-latest'),
    (Join-Path $bochs 'bios\VGABIOS-lgpl-latest'),
    $projection
)
foreach ($input in $inputs) {
    if (-not (Test-Path -LiteralPath $input -PathType Leaf)) { throw "Required manifest input is missing: $input" }
}

function Get-ManifestInput([string]$Path) {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $Path).Hash
    return [ordered]@{
        path = [IO.Path]::GetFullPath($Path)
        sha256 = $hash
        bytes = (Get-Item -LiteralPath $Path).Length
    }
}

$profileText = @'
# T95 S6 prospective native-container profile.  This file is input evidence,
# not an authorization to compile, link, or execute Bochs.
config_interface: textconfig
display_library: nogui
cpu: count=1, ips=1000000, reset_on_triple_fault=0
megs: 4
romimage: file=ROM/BIOS-bochs-latest
vgaromimage: file=ROM/VGABIOS-lgpl-latest
plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0
'@

New-Item -ItemType Directory -Path $analysis -Force | Out-Null
$utf8 = [Text.UTF8Encoding]::new($false)
$profilePath = Join-Path $analysis 'native-container-profile.bochsrc'
[IO.File]::WriteAllText($profilePath, $profileText, $utf8)

$manifest = [ordered]@{
    schema = 'ntdos64.t95-s6.native-container-manifest.v1'
    purpose = 'Input and rejection-predicate record only; no build, link, or execution command is emitted.'
    repositoryRoot = $repository
    futureFreshBuildRoot = '<to-be-created-only-after-separate-admission>'
    toolchain = [ordered]@{
        island = 'Bochs native backend'
        compiler = 'BuildTools MSVC x86 (observed cl 19.43; link 14.43.34809)'
        crt = '/MT'
        architecture = 'Win32/x86'
        developerCommand = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat -arch=x86 -host_arch=x86'
    }
    inputs = [ordered]@{
        importedConfig = Get-ManifestInput (Join-Path $bochs 'config.h')
        cpu3Projection = Get-ManifestInput $projection
        originalMakefile = Get-ManifestInput (Join-Path $bochs 'Makefile')
        originalExecutableProject = Get-ManifestInput (Join-Path $bochs 'vs2008\bochs.vcproj')
        originalSolution = Get-ManifestInput (Join-Path $bochs 'vs2008\bochs.sln')
        biosRom = Get-ManifestInput (Join-Path $bochs 'bios\BIOS-bochs-latest')
        vgaRom = Get-ManifestInput (Join-Path $bochs 'bios\VGABIOS-lgpl-latest')
    }
    configuration = [ordered]@{
        sourceConfigImmutable = $true
        projectedConfigDestination = 'config.h in a later fresh root only'
        cpuLevel = 3
        requiredMacros = @('BX_WITH_NOGUI=1', 'BX_WITH_WIN32=0', 'BX_DEBUGGER=0', 'BX_INSTRUMENTATION=0', 'BX_SUPPORT_X86_64=0', 'BX_SUPPORT_FPU=0', 'BX_SUPPORT_PCI=0', 'BX_SUPPORT_SMP=0', 'BX_NETWORKING=0', 'BX_SUPPORT_SOUNDLOW=0', 'BX_SUPPORT_VMX=0', 'BX_SUPPORT_SVM=0')
    }
    originalLinkTemplate = @(
        'BX_OBJS + SIMX86_OBJS',
        'iodev/libiodev.a',
        'iodev/hdimage/libhdimage.a',
        'iodev/usb/libusb.a',
        'iodev/network/libnetwork.a',
        'iodev/sound/libsound.a',
        'cpu/libcpu.a',
        'cpu/cpudb/libcpudb.a',
        'memory/libmemory.a',
        'gui/libgui.a',
        'DISASM_LIB',
        'FPU_LIB',
        'GUI_LINK_OPTS + MCH_LINK_FLAGS + SIMX86_LINK_FLAGS + READLINE_LIB',
        'EXTRA_LINK_OPTS + LIBS'
    )
    profile = [ordered]@{
        path = $profilePath
        sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $profilePath).Hash
        RAMMiB = 4
        lifecycle = @('original main.cc', 'original SIM/parameter tree', 'original device-manager and PC-system lifecycle', 'original nogui backend')
        optionalPluginDenial = @('unmapped', 'biosdev', 'speaker', 'extfpuirq', 'parallel', 'serial')
    }
    exclusions = @(
        'No adapter object, header, library, C ABI, or runtime is copied or linked.',
        'No CLI object, header, library, argv, or environment handoff is copied or linked.',
        'No OpenNT object, guest artifact, BOP, DEM, DOS, or WOW semantic path is copied or linked.',
        'BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT remains absent/default-off.',
        'No hand-selected Bochs core-object vector or adapter-owned Bochs object assembly is permitted.',
        'No VS2008 solution, all target, historical UCRT artifact, or bochs.exe invocation is authorized.'
    )
    resetControlledStopEvidence = [ordered]@{
        positiveLogPredicates = @('Bochs benchmark mode is ON', 'cpu hardware reset', 'cpu loop quit, shutting down simulator')
        negativePredicates = @('No adapter/CLI/OpenNT object appears in the link map.', 'No guest, BOP, DEM, DOS, WOW, search, or CLI runtime claim is made.', 'No additional device is enabled by a project patch.')
        resultInterpretation = 'A future exit code is evidence only when the copied profile, link map, and native reset/controlled-stop logs all match this manifest.'
    }
    manifestGenerator = [IO.Path]::GetFullPath($MyInvocation.MyCommand.Path)
}

$manifestPath = Join-Path $analysis 'native-container-manifest.json'
[IO.File]::WriteAllText($manifestPath, ($manifest | ConvertTo-Json -Depth 8), $utf8)
Write-Host "Wrote T95 S6 native-container input manifest: $manifestPath"
