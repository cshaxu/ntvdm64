[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)),
    [string]$BuildRoot = '',
    [switch]$Run,
    [switch]$ReenableFullBuild
)

$ErrorActionPreference = 'Stop'
if (-not $ReenableFullBuild) {
    throw 'Disabled by T95: this script drives the historical full bochs.exe graph. Define and approve a minimum mechanics object closure before re-enabling it.'
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$source = Join-Path $repository 'refs\bochs'
$compatibilityPatch = Join-Path $repository 'src\bx-vdm\patches\bochs-2.6-msvc-fmt-ll.patch'
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-minimal'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$bash = 'C:\msys64\usr\bin\bash.exe'
$devCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'

if (-not (Test-Path -LiteralPath $source -PathType Container)) { throw "Missing imported Bochs source: $source" }
if (-not (Test-Path -LiteralPath $compatibilityPatch -PathType Leaf)) { throw "Missing Bochs build compatibility patch: $compatibilityPatch" }
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
if (-not (Test-Path -LiteralPath $bash -PathType Leaf)) { throw "Missing MSYS bash: $bash" }
if (-not (Test-Path -LiteralPath $devCmd -PathType Leaf)) { throw "Missing Visual Studio developer command script: $devCmd" }

New-Item -ItemType Directory -Path $build -Force | Out-Null
& robocopy $source $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -gt 7) { throw "robocopy failed with exit code $LASTEXITCODE" }
$shimSource = Join-Path $build 'gui\siminterface.cc'
if (-not ([IO.File]::ReadAllText($shimSource).Contains('"0x"FMT_LL"x"'))) { throw "Build compatibility patch no longer applies: $shimSource" }
Get-ChildItem -LiteralPath $build -Recurse -File -Include '*.cc','*.h' | ForEach-Object {
    $content = [IO.File]::ReadAllText($_.FullName)
    $patched = $content.Replace('"FMT_LL', '" FMT_LL')
    if ($patched -ne $content) {
        [IO.File]::WriteAllText($_.FullName, $patched, [Text.UTF8Encoding]::new($false))
    }
}

$drive = $build.Substring(0, 1).ToLowerInvariant()
$msysBuild = '/' + $drive + $build.Substring(2).Replace('\', '/')
$patchDrive = $compatibilityPatch.Substring(0, 1).ToLowerInvariant()
$msysPatch = '/' + $patchDrive + $compatibilityPatch.Substring(2).Replace('\', '/')
$options = @(
    '--enable-cpu-level=3', '--disable-plugins', '--disable-debugger',
    '--disable-disasm', '--disable-gdb-stub', '--disable-iodebug',
    '--disable-smp', '--disable-x86-64', '--disable-long-phy-address',
    '--disable-ne2000', '--disable-pci', '--disable-usb', '--disable-usb-ohci',
    '--disable-usb-xhci', '--disable-pnic', '--disable-e1000', '--disable-sb16',
    '--disable-es1370', '--disable-gameport', '--disable-clgd54xx',
    '--disable-readline', '--without-x', '--with-nogui'
) -join ' '
$msvcCompiler = '/c/PROGRA~2/MICROS~4/2022/BUILDT~1/VC/Tools/MSVC/1443~1.348/bin/Hostx64/x64/cl.exe'
$configureCommand = "set -e; export PATH=/usr/bin:/ucrt64/bin:`$PATH; cd '$msysBuild'; mkdir -p tmp; export TMPDIR='$msysBuild/tmp' TMP='$msysBuild/tmp' TEMP='$msysBuild/tmp'; CC='$msvcCompiler' CXX='$msvcCompiler' ./configure --build=x86_64-pc-mingw32 --host=x86_64-pc-mingw32 --target=pentium-windows $options > configure.log 2>&1; find . -name Makefile -exec sed -i -e 's/{/(/g' -e 's/}/)/g' -e 's/^MAKE=make$/MAKE=nmake/' {} +"
$configureBatch = Join-Path $build 'configure-msvc.cmd'
[IO.File]::WriteAllText($configureBatch, "@echo off`r`ncall `"$devCmd`" -arch=x64 -host_arch=x64 >nul`r`n`"$bash`" -c `"$configureCommand`"`r`nexit /b %ERRORLEVEL%`r`n", [Text.ASCIIEncoding]::new())
& cmd.exe /d /s /c $configureBatch
if ($LASTEXITCODE -ne 0) { throw "Minimal Bochs configure failed; see $build\configure.log" }

$buildBatch = Join-Path $build 'build-msvc.cmd'
[IO.File]::WriteAllText($buildBatch, "@echo off`r`ncall `"$devCmd`" -arch=x64 -host_arch=x64 >nul`r`ncd /d `"$build`"`r`nnmake /nologo > build.log 2>&1`r`nexit /b %ERRORLEVEL%`r`n", [Text.ASCIIEncoding]::new())
& cmd.exe /d /s /c $buildBatch
if ($LASTEXITCODE -ne 0) { throw "Minimal Bochs build failed; see $build\configure.log and $build\build.log" }

$metadata = [ordered]@{
    schema = 'ntdos64.bochs-minimal-build.v1'
    source = $source
    buildRoot = $build
    compilerPath = 'C:\\msys64\\ucrt64\\bin'
    buildPatch = 'src/bx-vdm/patches/bochs-2.6-msvc-fmt-ll.patch'
    configureOptions = $options
    optionalFeatures = 'deny-by-default; only CPU level 3, FPU default, and nogui baseline admitted'
    executable = (Get-ChildItem -LiteralPath $build -Filter 'bochs.exe' -File -Recurse | Select-Object -First 1 -ExpandProperty FullName)
}
[IO.File]::WriteAllText((Join-Path $build 'ntdos64-minimal-build.json'), ($metadata | ConvertTo-Json), [Text.UTF8Encoding]::new($false))
Write-Host "Built minimal Bochs configuration at $build"
if ($Run) {
    throw 'Execution is intentionally not automated yet: T95 still requires a Bochs adapter-owned controlled-stop fixture.'
}
