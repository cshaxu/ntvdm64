param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$OutputRoot,
      [Parameter(Mandatory)][string]$CompilerDriver)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$build=(Resolve-Path $BuildRoot).Path
$driver=(Resolve-Path $CompilerDriver).Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith("$repo\build\",[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build output required'}
New-Item -ItemType Directory $out|Out-Null
Push-Location $out
try {
 & $driver cl /nologo /MT /TC "$PSScriptRoot/packstr.c" /Fe:packstr-test.exe /link "$build/obj/opennt-netlib/packstr.obj" kernel32.lib
 if($LASTEXITCODE){throw 'Original packstr fixture compile failed'}
 & ./packstr-test.exe
 if($LASTEXITCODE){throw "Original packstr assertion failed: $LASTEXITCODE"}
 '0'|Set-Content result.exit
} finally {Pop-Location}
