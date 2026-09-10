[CmdletBinding()]
param([string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$docsRoot = (Resolve-Path -LiteralPath (Join-Path $RepositoryRoot 'docs')).Path
$broken = [System.Collections.Generic.List[string]]::new()
$linkPattern = '(?<!\!)\[[^\]]*\]\((?<target>[^)\s]+)(?:\s+"[^"]*")?\)'

Get-ChildItem -LiteralPath $docsRoot -Recurse -File -Filter '*.md' | ForEach-Object {
    $source = $_.FullName
    $lineNumber = 0
    foreach ($line in [System.IO.File]::ReadLines($source)) {
        $lineNumber++
        foreach ($match in [regex]::Matches($line, $linkPattern)) {
            $target = $match.Groups['target'].Value
            if ($target -match '^(#|[a-z][a-z0-9+.-]*:)' -or $target.StartsWith('//')) { continue }
            $targetPath = ($target -split '[#?]', 2)[0]
            if ([string]::IsNullOrWhiteSpace($targetPath)) { continue }
            $candidate = Join-Path (Split-Path -Parent $source) $targetPath
            if (-not (Test-Path -LiteralPath $candidate)) {
                $relativeSource = $source.Substring($RepositoryRoot.Length + 1).Replace('\', '/')
                $broken.Add("$relativeSource`:$lineNumber -> $target")
            }
        }
    }
}

if ($broken.Count -gt 0) {
    $broken | ForEach-Object { Write-Error $_ }
    throw "Documentation relative-link verification failed: $($broken.Count) broken link(s)."
}

Write-Output 'Documentation relative-link verification passed.'
