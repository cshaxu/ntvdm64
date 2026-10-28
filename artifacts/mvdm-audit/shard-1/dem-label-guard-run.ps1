[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path '.').Path
$target = [IO.Path]::GetFullPath($OutputRoot)
$parent = [IO.Path]::GetFullPath((Join-Path $repo 'build/t473-s10'))
if (!$target.StartsWith($parent + [IO.Path]::DirectorySeparatorChar,
        [StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $target)) {
    throw 'OutputRoot must be a new exact directory inside the S10 temporary root.'
}
$source = Join-Path $repo 'artifacts/mvdm-audit/shard-1/dem-label-guard-probe.c'
$object = Join-Path $repo 'build/.ninja/msvc-x86-release/worker/obj/dem/demlabel.obj'
if ((Get-FileHash -LiteralPath $object -Algorithm SHA256).Hash -ne
        'C6405450FFB6674BF4E91C1BE6B3AC02809B22E84E91A034873E61557E09D1C6') {
    throw 'Selected object differs from the reviewed image-bound input.'
}
$compiler = 'C:\Users\neko\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw32\bin'
New-Item -ItemType Directory -Path $target | Out-Null
$testObject = Join-Path $target 'demlabel.obj'
$testExe = Join-Path $target 'dem-label-guard.exe'
& (Join-Path $compiler 'objcopy.exe') --remove-section=.drectve $object $testObject
if ($LASTEXITCODE -ne 0) { throw "objcopy failed: $LASTEXITCODE" }
& (Join-Path $compiler 'gcc.exe') -O2 -o $testExe $source $testObject
if ($LASTEXITCODE -ne 0) { throw "compiler failed: $LASTEXITCODE" }
$output = @(& $testExe)
if ($LASTEXITCODE -ne 0 -or $output -notcontains
        'PASS bounded label: selected object read exactly one byte beyond NUL into guarded page, before volume call') {
    throw "Guard contract failed (exit $LASTEXITCODE): $output"
}
$output
