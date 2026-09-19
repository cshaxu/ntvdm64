[CmdletBinding()]
param(
    [string] $OutputRoot = (Join-Path $PSScriptRoot '..\..\build\M0-T420\S30\nt-com-mock-r1')
)

$ErrorActionPreference = 'Stop'

$repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$fixture = Join-Path $repo 'tests\mvdm-host\nt_com_mock_fixture.c'
$vsDevCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $fixture -PathType Leaf)) {
    throw "Missing original nt_com mock fixture: $fixture"
}
if (!(Test-Path -LiteralPath $vsDevCmd -PathType Leaf)) {
    throw "Missing x86 MSVC environment setup: $vsDevCmd"
}

$OutputRoot = [IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Force $OutputRoot | Out-Null
$exe = Join-Path $OutputRoot 'nt-com-mock-fixture.exe'
$log = Join-Path $OutputRoot 'nt-com-mock-fixture.log'

$include = @(
    'src',
    'src\opennt-abi\host-compat\include',
    'src\mvdm\softpc.new\base\inc',
    'src\mvdm\softpc.new\host\inc',
    'src\opennt-host\public\sdk\inc',
    'src\opennt-abi\source\public\sdk\inc',
    'src\opennt-abi\source\public\internal\base\inc',
    'src\opennt-abi\source\public\internal\windows\inc',
    'src\mvdm\inc',
    'src\ntvdm-exe\softpc\include'
) | ForEach-Object { '/I "' + (Join-Path $repo $_) + '"' }

$compile = @(
    'call "' + $vsDevCmd + '" -arch=x86 -host_arch=x86 >nul &&',
    'cl.exe /nologo /TC /MT /W4 /Gy /DWIN32 /DWINNT /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DC_VID /DSPC386 /DSIM32 /DV7VGA /DANSI /DPROD',
    '/FI "' + (Join-Path $repo 'src\opennt-abi\host-compat\include\nt.h') + '"',
    '/FI "' + (Join-Path $repo 'src\ntvdm-exe\softpc\include\mvdm_softpc_symbol_compat.h') + '"',
    ($include -join ' '),
    '"' + $fixture + '"',
    '/link /OPT:REF /OUT:"' + $exe + '" kernel32.lib user32.lib libcmt.lib libvcruntime.lib libucrt.lib'
) -join ' '

Push-Location $OutputRoot
try {
    cmd.exe /d /s /c $compile 2>&1 | Tee-Object -FilePath $log
    if ($LASTEXITCODE -ne 0) {
        throw "nt_com mock fixture compile failed ($LASTEXITCODE); see $log"
    }
    $output = & $exe 2>&1
    $output | Tee-Object -FilePath $log -Append
    if ($LASTEXITCODE -ne 0 -or $output -notcontains 'T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK') {
        throw "nt_com mock fixture failed ($LASTEXITCODE); see $log"
    }
}
finally {
    Pop-Location
}

Write-Output "T420 S30 original nt_com mock lifecycle passed: $exe"
