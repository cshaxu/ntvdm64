param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$CompilerDriver
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'A fresh repository build output directory is required'
}
$driver = (Resolve-Path -LiteralPath $CompilerDriver).Path
New-Item -ItemType Directory $out | Out-Null
& $driver cl.exe /nologo /W3 /MT /O2 "/Fo$out/control.obj" "$repo/tests/mvdm-host/native_int_prefix_control.c" /link "/out:$out/control.exe" *> "$out/compile.log"
if ($LASTEXITCODE) { throw 'Native x86 prefix control build failed' }
& "$out/control.exe" | Tee-Object -FilePath "$out/result.txt"
if ($LASTEXITCODE) { throw 'Native x86 prefix control failed' }
