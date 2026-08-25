[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$MachineRoot = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($MachineRoot)) {
    $MachineRoot = Join-Path $repository 'artifacts\build\current\t177-s4-native-post-machine-composition-r1'
}
$machine = [IO.Path]::GetFullPath($MachineRoot)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }

function Get-Sha256([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing required input: $Path" }
    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
}
function Copy-Verified([string]$Source, [string]$Destination) {
    $hash = Get-Sha256 $Source
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    if ((Get-Sha256 $Destination) -ne $hash) { throw "Copy hash mismatch: $Source" }
    return $hash
}

$expectedBinary = '8758F4335CB32B4FB97688ED3860E8B0C9E86D8155E6AA316E54F346177E8BFA'
if ((Get-Sha256 (Join-Path $machine 'ntdos64-t98-current-adapter.exe')) -ne $expectedBinary) {
    throw 'Machine root is not the retained T177 native-POST/machine-composition image.'
}
$sourceManifestPath = Join-Path $machine 't98-s1-current-adapter-engine-derivative.json'
$sourceManifest = Get-Content -LiteralPath $sourceManifestPath -Raw | ConvertFrom-Json
if ($sourceManifest.adapterSources.Count -ne 67 -or $sourceManifest.cliSources.Count -ne 4) {
    throw 'Retained manifest does not have the expected 67 adapter plus four CLI base closure.'
}
$adapterSources = @($sourceManifest.adapterSources | ForEach-Object { [string]$_.name })
$cliSources = @($sourceManifest.cliSources | ForEach-Object { [string]$_.name }) + 'byob_launch_plan.c'
if (($adapterSources | Sort-Object -Unique).Count -ne 67 -or
    ($cliSources | Sort-Object -Unique).Count -ne 5) {
    throw 'T182 source closure is not exactly 67 adapter plus five CLI units.'
}

& robocopy $machine $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "Machine copy failed: $LASTEXITCODE" }
New-Item -ItemType Directory -Path (Join-Path $build 'adapter'), (Join-Path $build 'cli') -Force | Out-Null

$manifest = [ordered]@{
    schema = 'ntdos64.t182.s2.finite-sequence-engine-derivative.v1'
    mode = 'generator-only-no-compiler-linker-archive-or-executable-invocation'
    machineRoot = $machine
    machineBinarySha256 = $expectedBinary
    buildRoot = $build
    bochsReplacementCount = 0
    adapterSources = @()
    cliSources = @()
    retainedMachineInputs = @()
}
foreach ($path in @('main.o','cpu\exception.o','cpu\libcpu.a','iodev\libiodev.a',
        'memory\libmemory.a','gui\libgui.a','machine\bx_ntvdm_machine_bop_v1.obj',
        'machine\unexp_nt.c.obj','machine\illegalp.c.obj')) {
    $manifest.retainedMachineInputs += [ordered]@{ path = $path; sha256 = Get-Sha256 (Join-Path $build $path) }
}
foreach ($header in Get-ChildItem -LiteralPath (Join-Path $repository 'src\bx-vdm') -Filter *.h -File) {
    [void](Copy-Verified $header.FullName (Join-Path $build ('adapter\' + $header.Name)))
}
foreach ($header in Get-ChildItem -LiteralPath (Join-Path $repository 'src\cli') -Filter *.h -File) {
    [void](Copy-Verified $header.FullName (Join-Path $build ('cli\' + $header.Name)))
}
foreach ($name in $adapterSources) {
    $source = Join-Path $repository ('src\bx-vdm\' + $name)
    $manifest.adapterSources += [ordered]@{ name = $name; sha256 = Copy-Verified $source (Join-Path $build ('adapter\' + $name)) }
}
foreach ($name in $cliSources) {
    $source = Join-Path $repository ('src\cli\' + $name)
    $manifest.cliSources += [ordered]@{ name = $name; sha256 = Copy-Verified $source (Join-Path $build ('cli\' + $name)) }
}

$objects = @(
    $adapterSources | ForEach-Object { 'adapter\' + ($_ -replace '\.c$', '.obj') }
) + @(
    $cliSources | ForEach-Object { 'cli\' + ($_ -replace '\.c$', '.obj') }
)
$make = @(
    '# Generated T182 S2 derivative.  It compiles only the current adapter/CLI closure.',
    '!INCLUDE Makefile','',
    ('ADAPTER_OBJS = ' + ($objects -join ' ')),'',
    '# No Bochs object or archive is a target/prerequisite in this derivative.',''
)
foreach ($object in $objects) {
    $source = $object -replace '\.obj$', '.c'
    $make += "${object}: $source"
    $make += "`tcl.exe /nologo /c /MT /W3 /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS /Iadapter /Icli /Fo$object $source"
    $make += ''
}
$make += @(
    ('ntdos64-t182-finite-sequence.exe: $(ADAPTER_OBJS)'),
    "`tlink /nologo /subsystem:console /incremental:no /opt:ref /map:ntdos64-t182-finite-sequence.map /out:`$@ `$(BX_OBJS) `$(SIMX86_OBJS) main.o cpu\exception.o iodev/libiodev.a iodev/hdimage/libhdimage.a iodev/usb/libusb.a iodev/network/libnetwork.a iodev/sound/libsound.a cpu/libcpu.a cpu/cpudb/libcpudb.a memory/libmemory.a gui/libgui.a `$(DISASM_LIB) `$(FPU_LIB) `$(GUI_LINK_OPTS) `$(MCH_LINK_FLAGS) `$(SIMX86_LINK_FLAGS) `$(READLINE_LIB) `$(EXTRA_LINK_OPTS) `$(LIBS) machine\bx_ntvdm_machine_bop_v1.obj machine\unexp_nt.c.obj machine\illegalp.c.obj vcruntime.lib `$(ADAPTER_OBJS) kernel32.lib bcrypt.lib",''
)
$shim = Join-Path $build 'ntdos64-t182-finite-sequence.mak'
[IO.File]::WriteAllText($shim, ($make -join "`r`n"), [Text.UTF8Encoding]::new($false))
$shimText = Get-Content -LiteralPath $shim -Raw
if (($shimText | Select-String -AllMatches -Pattern '(^|\r?\n)(main\.o|cpu\\exception\.o):').Matches.Count -ne 0) {
    throw 'T182 makefile unexpectedly rebuilds a Bochs object.'
}
if ($shimText -match '(?m)^ntdos64-t182-finite-sequence\.exe:.*(?:libcpu|libiodev|libmemory|libgui)') {
    throw 'T182 makefile unexpectedly makes a Bochs archive a prerequisite.'
}
$objectRuleCount = [regex]::Matches($shimText, '(?m)^.*\.obj: .*\.c\r?$').Count
if ($shimText -notmatch [regex]::Escape('cli\byob_launch_plan_v2.obj') -or
    $objectRuleCount -ne 72) {
    throw 'T182 makefile does not contain the exact 72-object current closure.'
}
$manifest.shimSha256 = Get-Sha256 $shim
$manifest.permittedBuild = 'nmake /f ntdos64-t182-finite-sequence.mak ntdos64-t182-finite-sequence.exe'
[IO.File]::WriteAllText((Join-Path $build 't182-s2-finite-sequence-engine-derivative.json'),
    ($manifest | ConvertTo-Json -Depth 7), [Text.UTF8Encoding]::new($false))
Write-Host "Prepared fresh T182 S2 finite-sequence derivative: $build"
