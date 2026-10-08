param(
    [string]$Apply = 'tools/win31-launch/APPLY.CMD'
)

$ErrorActionPreference = 'Stop'
$Apply = (Resolve-Path -LiteralPath $Apply).Path
$text = Get-Content -LiteralPath $Apply -Raw

foreach ($required in @(
    'SYSTEM\MOUSE.DRV SYSTEM.INI SYSTEM\KRNL386.EXE SYSTEM\WIN386.EXE',
    '"%TARGET%\SYSTEM\MOUSE.DRV"',
    '"%PATCH%\MOUSE.DRV.ORIG"',
    '"%RELEASE%\MOUSE31.DRV" "%TARGET%\SYSTEM\MOUSE.DRV"'
)) {
    if (!$text.Contains($required)) {
        throw "APPLY.CMD does not use the installed SYSTEM\\MOUSE.DRV contract: $required"
    }
}

if ($text -match '(?im)^for /f .*"%TARGET%\\MOUSE\.DRV"' -or
    $text -match '(?im)^copy /y "%TARGET%\\MOUSE\.DRV"') {
    throw 'APPLY.CMD retains a root-level MOUSE.DRV install path'
}

'PASS win31-launch consistently uses SYSTEM\MOUSE.DRV for the installed driver'
