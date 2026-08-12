[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$AnalysisRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $scriptPath = $MyInvocation.MyCommand.Path
    if ([string]::IsNullOrWhiteSpace($scriptPath)) { throw 'RepositoryRoot was not supplied and script path is unavailable.' }
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $scriptPath)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($AnalysisRoot)) {
    $AnalysisRoot = Join-Path $repository 'artifacts\analysis\t95-s6-intact-native-tuple-manifest-001-20260811-001'
}
$analysis = [IO.Path]::GetFullPath($AnalysisRoot)
$bochs = Join-Path $repository 'src\bochs'
if (Test-Path -LiteralPath $analysis) { throw "Refusing to overwrite existing analysis directory: $analysis" }
foreach ($input in @((Join-Path $bochs 'config.h'), (Join-Path $bochs 'config.status'), (Join-Path $bochs 'Makefile'), (Join-Path $bochs 'Makefile.in'), (Join-Path $bochs 'vs2008\bochs.vcproj'))) {
    if (-not (Test-Path -LiteralPath $input -PathType Leaf)) { throw "Required intact-tuple input is missing: $input" }
}

function Get-Input([string]$Path) {
    return [ordered]@{
        path = [IO.Path]::GetFullPath($Path)
        sha256 = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
        bytes = (Get-Item -LiteralPath $Path).Length
    }
}

$makefiles = @(Get-ChildItem -LiteralPath $bochs -Recurse -File | Where-Object { $_.Name -eq 'Makefile' -or $_.Name -eq 'Makefile.in' } | Sort-Object FullName)
if ($makefiles.Count -eq 0) { throw 'The imported Bochs tree has no generated Makefile family.' }
$tupleMakefiles = foreach ($makefile in $makefiles) {
    $relative = $makefile.FullName.Substring($bochs.Length).TrimStart('\')
    [ordered]@{ relativePath = $relative.Replace('\', '/'); sha256 = (Get-FileHash -LiteralPath $makefile.FullName -Algorithm SHA256).Hash; bytes = $makefile.Length }
}

$profileText = @'
# T95 S6 intact-native runtime profile.  It controls enabled machine behavior,
# not the imported Bochs compile configuration or the guest/host service plane.
config_interface: textconfig
display_library: nogui
cpu: count=1, ips=1000000, reset_on_triple_fault=0
megs: 4
romimage: file=ROM/BIOS-bochs-latest
vgaromimage: file=ROM/VGABIOS-lgpl-latest
plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0, gameport=0
'@

New-Item -ItemType Directory -Path $analysis -Force | Out-Null
$utf8 = [Text.UTF8Encoding]::new($false)
$profilePath = Join-Path $analysis 'intact-native-runtime-profile.bochsrc'
[IO.File]::WriteAllText($profilePath, $profileText, $utf8)

$manifest = [ordered]@{
    schema = 'ntdos64.t95-s6.intact-native-tuple-manifest.v1'
    purpose = 'Matching original MSVC configuration tuple and minimal runtime-profile input only; this manifest authorizes no configure or build.'
    importedBochsRoot = $bochs
    tuple = [ordered]@{
        configHeader = Get-Input (Join-Path $bochs 'config.h')
        configStatus = Get-Input (Join-Path $bochs 'config.status')
        rootMakefile = Get-Input (Join-Path $bochs 'Makefile')
        rootMakefileTemplate = Get-Input (Join-Path $bochs 'Makefile.in')
        makefileFamily = @($tupleMakefiles)
        executableProject = Get-Input (Join-Path $bochs 'vs2008\bochs.vcproj')
    }
    nativeToolchain = [ordered]@{
        target = 'pentium-windows'
        architecture = 'Win32/x86'
        compiler = 'MSVC BuildTools x86'
        crt = '/MT (as recorded by imported generated Makefile)'
        make = 'nmake'
    }
    runtimeProfile = [ordered]@{
        path = $profilePath
        sha256 = (Get-FileHash -LiteralPath $profilePath -Algorithm SHA256).Hash
        allowedControlSurface = @('original Bochs config parser', 'nogui display selection', 'single CPU runtime selection', 'RAM size', 'declared BIOS/VGA ROM paths', 'original optional plugin_ctrl gate')
        deniedClaims = @('compiled support is not an enabled machine capability', 'no adapter/CLI/OpenNT input', 'no BOP/DEM/DOS/WOW semantics', 'no host filesystem capability', 'no guest runtime result')
    }
    futureTarget = [ordered]@{
        name = 'ntdos64-native-container.exe'
        construction = 'target-name-only transform of the original generated executable recipe after copying the complete tuple unchanged'
        prohibited = @('all', 'bochs.exe output', 'VS solution', 'configure', 'config projection', 'adapter', 'cli', 'opennt')
    }
    generator = [IO.Path]::GetFullPath($MyInvocation.MyCommand.Path)
}
[IO.File]::WriteAllText((Join-Path $analysis 'intact-native-tuple-manifest.json'), ($manifest | ConvertTo-Json -Depth 8), $utf8)
Write-Host "Wrote intact native tuple manifest: $analysis"
