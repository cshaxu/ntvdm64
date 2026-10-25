[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$ProviderBuildRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$VsDevCmd = 'C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/Tools/VsDevCmd.bat'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$providerRoot = (Resolve-Path -LiteralPath $ProviderBuildRoot).Path
$provider = Join-Path $providerRoot 'wow32'
if (!(Test-Path -LiteralPath $provider -PathType Container)) {
    throw 'Require the configured WOW32 provider build tree.'
}
$out = [IO.Path]::GetFullPath($BuildRoot)
if (!$out.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $out)) { throw 'Require a fresh repository build output.' }
$null = New-Item -ItemType Directory -Path $out

function Invoke-Owned([string]$Name, [string]$Command, [int]$Seconds) {
    $stdout = Join-Path $out ($Name + '.stdout.txt')
    $stderr = Join-Path $out ($Name + '.stderr.txt')
    $line = '/d /s /c "call "' + $VsDevCmd + '" -arch=x86 -host_arch=x64 >nul && ' + $Command + '"'
    $process = Start-Process -FilePath $env:ComSpec -ArgumentList $line -WorkingDirectory $provider -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $null = $process.Handle
    @{ pid = $process.Id; command = $line; timeoutSeconds = $Seconds } | ConvertTo-Json | Set-Content (Join-Path $out ($Name + '.process.json'))
    if (!$process.WaitForExit($Seconds * 1000)) {
        & taskkill.exe /PID $process.Id /T /F | Out-Host
        throw "Owned process exceeded deadline: $Name"
    }
    $process.WaitForExit()
    $code = $process.ExitCode
    $process.Dispose()
    $code | Set-Content (Join-Path $out ($Name + '.exit.txt'))
    if ($code) { throw "$Name failed with $code" }
}

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
    $source = Join-Path $repo 'src/mvdm/wow32/wole2.c'
    $fixture = Join-Path $repo 'tests/adapter-mvdm-host-out/wow/wow_module_alias_fixture.c'
    $commands = @(& ninja.exe -C $provider -t commands wow32.dll)
    if ($LASTEXITCODE) { throw 'Cannot read the production WOW32 compile trace.' }
    $compile = @($commands | Where-Object { $_ -match '[/\\]wole2\.c$' })
    if ($compile.Count -ne 1 -or $compile[0] -notmatch '/DCPU_40_STYLE' -or
            $compile[0] -notmatch '/DCCPU' -or $compile[0] -notmatch '/MT' -or
            $compile[0] -notmatch '/O2' -or $compile[0] -notmatch '/Gz') {
        throw 'Require exactly one CCPU40 production wole2 compile command.'
    }
    $compile[0] | Set-Content (Join-Path $out 'production-command.txt')
    Get-FileHash -LiteralPath @($source, $fixture, $PSCommandPath) | Export-Csv -NoTypeInformation (Join-Path $out 'inputs.csv')

    $owner = $compile[0] -replace '/Fo\S+', ('/Fo"' + $out + '/wole2.obj"')
    Invoke-Owned 'owner' $owner 120
    $fixtureCommand = $owner -replace '/Fo"?[^"\s]*wole2\.obj"?', ('/Fo"' + $out + '/fixture.obj"')
    $fixtureCommand = $fixtureCommand -replace ('"?' + [regex]::Escape($source.Replace('\', '/')) + '"?$'), ('"' + $fixture + '"')
    if ($fixtureCommand -eq $owner) { throw 'Fixture compile substitution failed.' }
    Invoke-Owned 'fixture' $fixtureCommand 120
    Invoke-Owned 'link' ('link.exe /nologo /machine:x86 /subsystem:console /out:"' + $out + '/fixture.exe" /map:"' + $out + '/fixture.map" "' + $out + '/fixture.obj" "' + $out + '/wole2.obj" kernel32.lib ntdll.lib oldnames.lib') 120
    Invoke-OwnedExecutable 'run' (Join-Path $out 'fixture.exe') 30
    if ((Get-Content (Join-Path $out 'run.stdout.txt') -Raw) -ne "WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK`r`n") {
        throw 'Alias fixture contract markers absent.'
    }

    $text = Get-Content $source -Raw
    foreach ($required in @('STATUS_INFO_LENGTH_MISMATCH', 'ProcessBuffer = malloc_w(ProcessBufferSize)', 'RtlCopyMemory(lpNameBuffer, pname.Buffer, cch)')) {
        if (!$text.Contains($required)) { throw "Missing bounded alias source requirement: $required" }
    }
    foreach ($forbidden in @('LargeBuffer1[16*1024]', 'strcpy( lpNameBuffer, pname.Buffer )')) {
        if ($text.Contains($forbidden)) { throw "Retired unsafe alias source survived: $forbidden" }
    }
    Get-FileHash -LiteralPath @($source, $fixture, $PSCommandPath) | Export-Csv -NoTypeInformation (Join-Path $out 'outputs-inputs.csv')
    Get-FileHash (Join-Path $out 'fixture.exe'), (Join-Path $out 'wole2.obj') |
        Export-Csv -NoTypeInformation (Join-Path $out 'artifacts.csv')
    '0' | Set-Content (Join-Path $out 'exit.txt')
} catch {
    $_ | Out-String | Set-Content (Join-Path $out 'failure.txt')
    '1' | Set-Content (Join-Path $out 'exit.txt')
    exit 1
}
