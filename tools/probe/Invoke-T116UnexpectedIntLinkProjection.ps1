[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot,
    [Parameter(Mandatory = $true)]
    [string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$output = [System.IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $output) {
    throw "Refusing to overwrite existing build directory: $output"
}

$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$clang = 'C:\Program Files\LLVM\bin\clang-cl.exe'
$unexp = Join-Path $root 'artifacts\build\current\t113-unexp-compile-projection-r3\CMakeFiles\ntdos64-opennt-system-provider-objects.dir\base\mvdm\softpc.new\base\system\unexp_nt.c.obj'
$component = Join-Path $root 'src\bx-ntvdm-machine-composition\bx_ntvdm_machine_bop_v1.c'
$main = Join-Path $root 'tests\bx-ntvdm-machine-composition\t116_unexpected_int_link_probe_main.c'
foreach ($path in @($vsDevCmd, $clang, $unexp, $component, $main)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Required input missing: $path" }
}

New-Item -ItemType Directory -Path $output | Out-Null
$componentObject = Join-Path $output 'machine_bop_v1.obj'
$mainObject = Join-Path $output 'link_probe_main.obj'
$exe = Join-Path $output 't116-unexpected-int-link-projection.exe'
$log = Join-Path $output 'build.log'
$map = Join-Path $output 'link.map'

$cmd = @(
    "call `"$vsDevCmd`" -arch=x86 -host_arch=x86",
    "`"$clang`" -m32 /nologo /MDd /GS /TC /c /Fo`"$componentObject`" /I`"$(Split-Path -Parent $component)`" `"$component`"",
    "`"$clang`" -m32 /nologo /MDd /GS /TC /c /Fo`"$mainObject`" `"$main`"",
    "`"$clang`" -m32 -fuse-ld=link /nologo /Fe`"$exe`" `"$mainObject`" `"$componentObject`" `"$unexp`" /link /MAP:`"$map`""
) -join ' && '

& cmd.exe /d /c $cmd 2>&1 | Tee-Object -FilePath $log
if ($LASTEXITCODE -ne 0) { throw "T116 link projection failed; see $log" }
foreach ($required in @('unexpected_int', '_inb', '_outb', '_c_sas_store')) {
    if (-not (Select-String -LiteralPath $map -SimpleMatch $required -Quiet)) {
        throw "Expected retained symbol is absent: $required"
    }
}

[pscustomobject]@{
    BuildRoot = $output
    Executable = $exe
    Log = $log
    Map = $map
    ExecutableSha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $exe).Hash
    OriginalObjectSha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $unexp).Hash
}
