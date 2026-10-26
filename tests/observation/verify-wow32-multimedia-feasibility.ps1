[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot,
    [Parameter(Mandatory = $true)]
    [string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$root = [IO.Path]::GetFullPath($BuildRoot)
if (-not $root.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path -LiteralPath $root)) {
    throw 'Require a fresh build-owned evidence directory.'
}
$null = New-Item -ItemType Directory -Path $root
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $vs -PathType Leaf)) { throw "Missing $vs" }

function Invoke-Owned {
    param([string]$Name, [string]$Arguments, [int]$Expected = 0)
    $out = Join-Path $root "$Name.stdout.txt"
    $err = Join-Path $root "$Name.stderr.txt"
    $process = Start-Process -FilePath cmd.exe -ArgumentList $Arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput $out -RedirectStandardError $err
    $null = $process.Handle
    @{ pid = $process.Id; start = $process.StartTime.ToUniversalTime().ToString('o'); executable = 'cmd.exe'; arguments = $Arguments } |
        ConvertTo-Json | Set-Content -LiteralPath (Join-Path $root "$Name.process.json")
    $process.WaitForExit()
    $process.Refresh()
    $process.ExitCode | Set-Content -LiteralPath (Join-Path $root "$Name.exit.txt")
    if ($process.ExitCode -ne $Expected) { throw "$Name failed: $($process.ExitCode)" }
}

try {
    $fixture = Join-Path $repo 'tests\adapter-mvdm-host-out\wow\wow_multimedia_inventory_fixture.c'
    $object = Join-Path $root 'wow_multimedia_inventory_fixture.obj'
    $exe = Join-Path $root 'wow_multimedia_inventory_fixture.exe'
    $compile = Join-Path $root 'compile.cmd'
    $link = Join-Path $root 'link.cmd'
    @('@echo off', ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
      'if errorlevel 1 exit /b %errorlevel%',
      ('cl.exe /nologo /W4 /WX /TC /MT /O2 /c /Fo"' + $object + '" "' + $fixture + '"')) |
        Set-Content -LiteralPath $compile -Encoding ascii
    @('@echo off', ('call "' + $vs + '" -arch=x86 -host_arch=x64 >nul'),
      'if errorlevel 1 exit /b %errorlevel%',
      ('link.exe /nologo /MACHINE:X86 /OUT:"' + $exe + '" "' + $object + '" winmm.lib')) |
        Set-Content -LiteralPath $link -Encoding ascii
    Invoke-Owned -Name compile -Arguments ('/d /c ' + $compile)
    Invoke-Owned -Name link -Arguments ('/d /c ' + $link)
    Invoke-Owned -Name run -Arguments ('/d /s /c """' + $exe + '"""')
    $output = Get-Content -LiteralPath (Join-Path $root 'run.stdout.txt') -Raw
    if ($output -notmatch '^WOW_MULTIMEDIA_FEASIBILITY .*timer_min=\d+ timer_max=\d+') {
        throw 'Missing public WINMM feasibility witness.'
    }
    @($fixture, $PSCommandPath) | Get-FileHash | Select-Object Path, Hash |
        ConvertTo-Json | Set-Content -LiteralPath (Join-Path $root 'inputs.json')
    '0' | Set-Content -LiteralPath (Join-Path $root 'exit.txt')
}
catch {
    $_ | Out-String | Set-Content -LiteralPath (Join-Path $root 'failure.txt')
    '1' | Set-Content -LiteralPath (Join-Path $root 'exit.txt')
    throw
}
