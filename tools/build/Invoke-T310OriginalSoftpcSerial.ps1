[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$BuildRoot,
    [Parameter(Mandatory = $true, Position = 0)]
    [string[]]$Targets
)

$ErrorActionPreference = 'Stop'
$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$graph = Join-Path $build 'build.ninja'
$runner = Join-Path $build 'run-ninja-parallel.cmd'
if (!(Test-Path -LiteralPath $graph) -or !(Test-Path -LiteralPath $runner)) {
    throw "Not a generated T310 graph: $build"
}

$runnerText = Get-Content -LiteralPath $runner -Raw
$vsMatch = [regex]::Match($runnerText, 'call "([^"]+VsDevCmd\.bat)"')
$ninjaMatch = [regex]::Match($runnerText, '"([^"]*ninja\.exe)"')
if (!$vsMatch.Success -or !$ninjaMatch.Success) {
    throw "Cannot recover the declared VS/Ninja tools from: $runner"
}
$vs = $vsMatch.Groups[1].Value
$ninja = $ninjaMatch.Groups[1].Value
if (!(Test-Path -LiteralPath $vs) -or !(Test-Path -LiteralPath $ninja)) {
    throw 'The generated graph toolchain is unavailable.'
}

$environment = & cmd.exe /d /s /c ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul && set')
$pathSet = $false
foreach ($line in $environment) {
    $index = $line.IndexOf('=')
    if ($index -le 0) { continue }
    $name = $line.Substring(0, $index)
    if ($name -ieq 'Path') {
        if ($pathSet) { continue }
        $pathSet = $true
    }
    Set-Item -Path ('env:' + $name) -Value $line.Substring($index + 1)
}

$commands = @(& $ninja -C $build -t commands @Targets)
if ($LASTEXITCODE -ne 0 -or $commands.Count -eq 0) {
    throw "Ninja could not expand the requested target command graph: $($Targets -join ', ')"
}

Push-Location -LiteralPath $build
try {
    for ($index = 0; $index -lt $commands.Count; $index++) {
        Write-Host ('[{0}/{1}] {2}' -f ($index + 1), $commands.Count, $commands[$index])
        & cmd.exe /d /s /c $commands[$index]
        if ($LASTEXITCODE -ne 0) {
            throw "Serial graph command $($index + 1) failed with exit code $LASTEXITCODE."
        }
    }
} finally {
    Pop-Location
}
