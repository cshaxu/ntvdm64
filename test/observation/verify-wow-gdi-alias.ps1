param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path,
    [string]$OutputRoot = ''
)
$ErrorActionPreference = 'Stop'
if (!$OutputRoot) { $OutputRoot = Join-Path $RepositoryRoot 'build/test-wow-gdi-alias' }
New-Item -ItemType Directory -Force $OutputRoot | Out-Null
$implementation = Join-Path $RepositoryRoot 'src/compat/wow_gdi_alias.c'
$include = Join-Path $RepositoryRoot 'src'
Push-Location $OutputRoot
try {
    foreach ($name in @('wow_gdi_alias_fixture', 'wow_gdi_class_brush_boundary')) {
        $source = Join-Path $RepositoryRoot "test/compat/$name.c"
        & cl.exe /nologo /TC /MT /W4 /WX /I $include $source $implementation /Fe:$name.exe /link gdi32.lib | Out-Host
        if ($LASTEXITCODE) { throw "fixture compile failed: $name $LASTEXITCODE" }
        & "./$name.exe" | Out-Host
        if ($LASTEXITCODE) { throw "fixture failed: $name $LASTEXITCODE" }
    }
} finally { Pop-Location }
