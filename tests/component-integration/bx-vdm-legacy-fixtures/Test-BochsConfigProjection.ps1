$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$tool = Join-Path $repositoryRoot 'tools\build\Project-BochsConfig.ps1'
$testRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("t95-s6-config-projection-" + [guid]::NewGuid().ToString('N'))

function Assert-Throws([scriptblock]$Action, [string]$ExpectedText) {
    try {
        & $Action
    } catch {
        if ($_.Exception.Message -notmatch [regex]::Escape($ExpectedText)) {
            throw "Expected failure containing '$ExpectedText', got '$($_.Exception.Message)'."
        }
        return
    }
    throw "Expected failure containing '$ExpectedText'."
}

try {
    New-Item -ItemType Directory -Path $testRoot | Out-Null
    $source = Join-Path $testRoot 'input-config.h'
    @(
        '#ifndef _BX_CONFIG_H_', '#define _BX_CONFIG_H_ 1',
        '#define BX_CPU_LEVEL 6', '#define BX_WITH_WIN32 1', '#define BX_EMPTY',
        'typedef unsigned long Bit32u;', '#endif'
    ) | Set-Content -LiteralPath $source -Encoding ascii
    $hash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToUpperInvariant()
    $manifest = [ordered]@{
        schemaVersion = 1
        sourceConfigSha256 = $hash
        replacements = @(
            [ordered]@{ name = 'BX_CPU_LEVEL'; oldValue = '6'; newValue = '3'; classification = 'source-forced' },
            [ordered]@{ name = 'BX_WITH_WIN32'; oldValue = '1'; newValue = '0'; classification = 'deny' }
        )
        additions = @([ordered]@{ name = 'BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT'; expectedSource = '<absent>'; newValue = '0'; classification = 'default-off' })
        derivedAssertions = @()
    }
    $manifestPath = Join-Path $testRoot 'manifest.json'
    $manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $manifestPath -Encoding utf8

    $rootA = Join-Path $testRoot 'projection-a'
    $rootB = Join-Path $testRoot 'projection-b'
    & $tool -ManifestPath $manifestPath -AnalysisRoot $rootA -InputConfigPath $source
    & $tool -ManifestPath $manifestPath -AnalysisRoot $rootB -InputConfigPath $source
    $projected = Get-Content -LiteralPath (Join-Path $rootA 'config.h') -Raw
    if ($projected -notmatch '#define BX_CPU_LEVEL 3' -or $projected -notmatch '#define BX_WITH_WIN32 0') { throw 'Named replacements were not projected.' }
    if ($projected -notmatch '#define BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT 0') { throw 'Default-off addition was not projected.' }
    if ($projected -notmatch 'typedef unsigned long Bit32u;' -or $projected -notmatch '#define BX_EMPTY') { throw 'Unchanged mechanics or an empty define were not retained.' }
    $hashA = (Get-FileHash -LiteralPath (Join-Path $rootA 'config.h') -Algorithm SHA256).Hash
    $hashB = (Get-FileHash -LiteralPath (Join-Path $rootB 'config.h') -Algorithm SHA256).Hash
    if ($hashA -cne $hashB) { throw 'Projection output is not deterministic.' }
    $report = Get-Content -LiteralPath (Join-Path $rootA 'projection-report.json') -Raw | ConvertFrom-Json
    if ($report.replacements.Count -ne 3) { throw 'Projection report omitted a named change.' }
    if (@($report.replacements | Where-Object { $_.name -ceq 'BX_CPU_LEVEL' }).Count -ne 1) { throw 'One exact #define was not represented as one record.' }

    $v1ManifestPath = Join-Path $repositoryRoot 'tools\build\t95-s6-msvc-config-projection-manifest.json'
    $v2ManifestPath = Join-Path $repositoryRoot 'tools\build\t95-s6-msvc-config-projection-manifest-v2.json'
    if ((Get-FileHash -LiteralPath $v1ManifestPath -Algorithm SHA256).Hash.ToUpperInvariant() -cne '6815D8BD419ED9522C98F9DF55D242FB7037F42125C4504EC5766AE732FB9E28') { throw 'Projection v1 manifest is not immutable.' }
    $v1 = Get-Content -LiteralPath $v1ManifestPath -Raw | ConvertFrom-Json
    $v2 = Get-Content -LiteralPath $v2ManifestPath -Raw | ConvertFrom-Json
    if (($v1.replacements | ConvertTo-Json -Depth 5 -Compress) -cne ($v2.replacements | ConvertTo-Json -Depth 5 -Compress)) { throw 'Projection v2 changed a v1 replacement.' }
    if (($v1.derivedAssertions | ConvertTo-Json -Depth 5 -Compress) -cne ($v2.derivedAssertions | ConvertTo-Json -Depth 5 -Compress)) { throw 'Projection v2 changed a v1 derived assertion.' }
    if (@($v2.additions).Count -ne (@($v1.additions).Count + 1)) { throw 'Projection v2 did not add exactly one addition.' }
    $fmtAddition = @($v2.additions | Where-Object { $_.name -ceq 'FMT_LL' })
    if ($fmtAddition.Count -ne 1 -or $fmtAddition[0].expectedSource -cne '<absent>' -or $fmtAddition[0].newValue -cne '"I64"') { throw 'Projection v2 FMT_LL addition is not exact.' }

    $v2Synthetic = [ordered]@{
        schemaVersion = 1
        sourceConfigSha256 = $hash
        replacements = $manifest.replacements
        additions = @(
            [ordered]@{ name = 'BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT'; expectedSource = '<absent>'; newValue = '0'; classification = 'default-off' },
            [ordered]@{ name = 'FMT_LL'; expectedSource = '<absent>'; newValue = '"I64"'; classification = 'MSVC format-token platform omission' }
        )
        derivedAssertions = @()
    }
    $v2SyntheticPath = Join-Path $testRoot 'v2-synthetic.json'
    $v2Synthetic | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $v2SyntheticPath -Encoding utf8
    $v2Root = Join-Path $testRoot 'projection-v2'
    & $tool -ManifestPath $v2SyntheticPath -AnalysisRoot $v2Root -InputConfigPath $source
    $v2Projected = Get-Content -LiteralPath (Join-Path $v2Root 'config.h') -Raw
    if ($v2Projected -notmatch '#define FMT_LL "I64"') { throw 'Projection v2 did not add FMT_LL exactly.' }
    $v2Report = Get-Content -LiteralPath (Join-Path $v2Root 'projection-report.json') -Raw | ConvertFrom-Json
    if (@($v2Report.replacements | Where-Object { $_.name -ceq 'FMT_LL' }).Count -ne 1) { throw 'Projection v2 report omitted FMT_LL.' }

    $duplicateSource = Join-Path $testRoot 'duplicate-config.h'
    @((Get-Content -LiteralPath $source), '#define BX_CPU_LEVEL 6') | Set-Content -LiteralPath $duplicateSource -Encoding ascii
    $duplicateSourceHash = (Get-FileHash -LiteralPath $duplicateSource -Algorithm SHA256).Hash.ToUpperInvariant()
    $duplicateSourceManifest = [ordered]@{
        schemaVersion = 1
        sourceConfigSha256 = $duplicateSourceHash
        replacements = @([ordered]@{ name = 'BX_CPU_LEVEL'; oldValue = '6'; newValue = '3'; classification = 'source-forced' })
        additions = @()
        derivedAssertions = @()
    }
    $duplicateSourceManifestPath = Join-Path $testRoot 'duplicate-source.json'
    $duplicateSourceManifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $duplicateSourceManifestPath -Encoding utf8
    $duplicateSourceRoot = Join-Path $testRoot 'projection-duplicate-source'
    Assert-Throws { & $tool -ManifestPath $duplicateSourceManifestPath -AnalysisRoot $duplicateSourceRoot -InputConfigPath $duplicateSource } 'requires exactly one source definition; found 2'
    if (Test-Path -LiteralPath $duplicateSourceRoot) { throw 'Duplicate source definition failure created an analysis root.' }

    $missing = $manifest.PSObject.Copy()
    $missing.replacements = @([ordered]@{ name = 'BX_MISSING'; oldValue = '0'; newValue = '1'; classification = 'deny' })
    $missingPath = Join-Path $testRoot 'missing.json'
    $missing | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $missingPath -Encoding utf8
    $missingRoot = Join-Path $testRoot 'projection-missing'
    Assert-Throws { & $tool -ManifestPath $missingPath -AnalysisRoot $missingRoot -InputConfigPath $source } 'requires exactly one source definition'
    if (Test-Path -LiteralPath $missingRoot) { throw 'Missing replacement failure created an analysis root.' }

    $duplicate = $manifest.PSObject.Copy()
    $duplicate.replacements = @(
        [ordered]@{ name = 'BX_CPU_LEVEL'; oldValue = '6'; newValue = '3'; classification = 'source-forced' },
        [ordered]@{ name = 'BX_CPU_LEVEL'; oldValue = '6'; newValue = '3'; classification = 'source-forced' }
    )
    $duplicatePath = Join-Path $testRoot 'duplicate.json'
    $duplicate | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $duplicatePath -Encoding utf8
    Assert-Throws { & $tool -ManifestPath $duplicatePath -AnalysisRoot (Join-Path $testRoot 'projection-duplicate') -InputConfigPath $source } "Duplicate manifest macro 'BX_CPU_LEVEL'"

    $unexpected = $manifest.PSObject.Copy()
    $unexpected.replacements = @([ordered]@{ name = 'BX_CPU_LEVEL'; oldValue = '6'; newValue = '3'; classification = 'source-forced'; unexpected = 'reject' })
    $unexpectedPath = Join-Path $testRoot 'unexpected.json'
    $unexpected | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $unexpectedPath -Encoding utf8
    Assert-Throws { & $tool -ManifestPath $unexpectedPath -AnalysisRoot (Join-Path $testRoot 'projection-unexpected') -InputConfigPath $source } "unexpected property 'unexpected'"

    Write-Host 'Bochs config projection completeness and fail-closed verification passed.'
} finally {
    if (Test-Path -LiteralPath $testRoot) { Remove-Item -LiteralPath $testRoot -Recurse -Force }
}
