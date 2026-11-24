[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$provider = Join-Path $repo 'build/.ninja/msvc-x86-release/wow32'
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $out)) { throw 'Require a fresh repository build output.' }
$null = New-Item -ItemType Directory -Path $out

function Invoke-OwnedExecutable([string]$Name, [string]$File, [int]$Seconds) {
    $stdout = Join-Path $out ($Name + '.stdout.txt')
    $stderr = Join-Path $out ($Name + '.stderr.txt')
    $process = Start-Process -FilePath $File -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $null = $process.Handle
    @{ pid = $process.Id; file = $File; timeoutSeconds = $Seconds } | ConvertTo-Json | Set-Content (Join-Path $out ($Name + '.process.json'))
    if (!$process.WaitForExit($Seconds * 1000)) {
        & taskkill.exe /PID $process.Id /T /F | Out-Host
        throw "Owned executable exceeded deadline: $Name"
    }
    $code = $process.ExitCode
    $process.Dispose()
    $code | Set-Content (Join-Path $out ($Name + '.exit.txt'))
    if ($code) { throw "$Name failed with $code" }
}

try {
    $source = Join-Path $repo 'src/mvdm-host/wow32/wole2.c'
    $fixture = Join-Path $repo 'test/adapter-mvdm-host-out/wow/wow_module_alias_fixture.c'
    & "$repo/tools/build-fixtures.cmd" x86 wow-module-alias *> "$out/build.log"
    if ($LASTEXITCODE) { throw 'Native alias contract build failed' }
    Get-FileHash -LiteralPath @($source, $fixture, $PSCommandPath) |
        Export-Csv -NoTypeInformation (Join-Path $out 'inputs.csv')
    Copy-Item "$provider/module-alias.exe" "$out/fixture.exe"
    Copy-Item "$provider/module-alias.exe.map" "$out/fixture.map"
    Invoke-OwnedExecutable 'run' (Join-Path $out 'fixture.exe') 30
    if ((Get-Content (Join-Path $out 'run.stdout.txt') -Raw) -ne "WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK`r`n") {
        throw 'Alias fixture contract markers absent.'
    }

    Get-FileHash -LiteralPath @($source, $fixture, $PSCommandPath) | Export-Csv -NoTypeInformation (Join-Path $out 'outputs-inputs.csv')
    Get-FileHash (Join-Path $out 'fixture.exe'), (Join-Path $out 'fixture.map') |
        Export-Csv -NoTypeInformation (Join-Path $out 'artifacts.csv')
    '0' | Set-Content (Join-Path $out 'exit.txt')
} catch {
    $_ | Out-String | Set-Content (Join-Path $out 'failure.txt')
    '1' | Set-Content (Join-Path $out 'exit.txt')
    exit 1
}
