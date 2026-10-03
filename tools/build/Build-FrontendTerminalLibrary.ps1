[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference = 'Stop'
$repository = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$owner = Join-Path $repository 'src/ntcon-exe'
$manifest = Get-Content -Raw -LiteralPath (Join-Path $owner 'libvterm-import.json') | ConvertFrom-Json
$archive = Join-Path $owner $manifest.archive
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $archive).Hash -ne $manifest.sha256) {
    throw 'Pinned libvterm archive changed'
}
$output = [IO.Path]::GetFullPath((Join-Path $repository $BuildRoot))
$prefix = [IO.Path]::GetFullPath((Join-Path $repository 'build')) + [IO.Path]::DirectorySeparatorChar
if (!$output.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase)) { throw 'BuildRoot must be beneath build/' }
if (Test-Path -LiteralPath $output) { throw 'Use a new terminal-library build root' }
[void](Get-Command cl.exe -ErrorAction Stop)
[void](New-Item -ItemType Directory -Path $output)
# Archive identity is fixed before extraction. Only the selected runtime closure
# and its original license are materialized, never upstream build/test scripts.
$root = 'libvterm-' + $manifest.version
$entries = @($manifest.compile | ForEach-Object { "$root/src/$_" }) +
    @($manifest.headers | ForEach-Object { "$root/$_" }) + @("$root/LICENSE")
& tar.exe -xzf $archive -C $output @entries
if ($LASTEXITCODE) { throw 'libvterm extraction failed' }
$source = Join-Path $output $root
$objects = @()
foreach ($file in $manifest.compile) {
    $object = Join-Path $output ([IO.Path]::GetFileNameWithoutExtension($file) + '.obj')
    $translationUnit = if ($file -eq 'screen.c') { Join-Path $owner $manifest.screenBinding } else { Join-Path $source "src/$file" }
    & cl.exe /nologo /c /MT /std:c11 /W3 /D_CRT_SECURE_NO_WARNINGS "/I$source/include" "/I$source/src" `
        "/Fo$object" $translationUnit *>> (Join-Path $output 'build.log')
    if ($LASTEXITCODE) { throw "libvterm compile failed: $file" }
    $objects += $object
}
& lib.exe /nologo "/OUT:$output/libvterm.lib" @objects *>> (Join-Path $output 'build.log')
if ($LASTEXITCODE) { throw 'libvterm archive failed' }
Write-Output "Built pinned libvterm with registered frontend screen binding: $output/libvterm.lib"
