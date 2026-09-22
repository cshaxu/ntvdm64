param(
    [string]$OriginalRoot = 'O:/repos.external/OpenNT',
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
)
$ErrorActionPreference = 'Stop'
function Read-Pinned([string]$file, [string]$hash) {
    $path = Join-Path $OriginalRoot ('windows/core/ntuser/kernel/' + $file)
    if ((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $hash) {
        throw "Original source identity changed: $file"
    }
    return [IO.File]::ReadAllText($path).Replace("`r`n", "`n")
}
function Body([string]$text, [string]$name) {
    $pattern = '(?ms)^(?:void|ATOM|BOOL) ' + [regex]::Escape($name) + '\(.*?^\}'
    $matches = [regex]::Matches($text, $pattern)
    if ($matches.Count -ne 1) { throw "Expected one complete body: $name" }
    return $matches[0].Value
}
$inputSource = Read-Pinned 'input.c' 'B7656BCA55E892A42BA825BE0324211450AD298C40717E1AE5A889D084467529'
$classSource = Read-Pinned 'class.c' '82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A'
$checks = @(
    @{ File='input.c'; Name='AddAttachment'; Source=$inputSource },
    @{ File='input.c'; Name='Recalc2'; Source=$inputSource },
    @{ File='input.c'; Name='RecalcThreadAttachment'; Source=$inputSource },
    @{ File='class.c'; Name='_RegisterClassEx'; Source=$classSource },
    @{ File='class.c'; Name='_UnregisterClass'; Source=$classSource }
)
foreach ($check in $checks) {
    $path = Join-Path $RepositoryRoot ('src/opennt-host/windows/core/ntuser/kernel/' + $check.File)
    $mirror = [IO.File]::ReadAllText($path)
    if ($mirror -match '(?<!\r)\n') { throw "Non-original LF in mirror: $path" }
    $actual = Body ($mirror.Replace("`r`n", "`n")) $check.Name
    switch ($check.Name) {
        'Recalc2' {
            $actual = $actual.Replace("PQ pqAttach,`n    PTHREADINFO ptiCurrent)", 'PQ pqAttach)')
        }
        'RecalcThreadAttachment' {
            $actual = $actual.Replace('RecalcThreadAttachment(PTHREADINFO ptiCurrent)', 'RecalcThreadAttachment()')
            $actual = $actual.Replace('Recalc2(pti->pqAttach, ptiCurrent);', 'Recalc2(pti->pqAttach);')
        }
        '_RegisterClassEx' {
            $actual = $actual.Replace("LPDWORD pdwWOW,`n    wow_class_register_context *context )", 'LPDWORD pdwWOW )')
        }
        '_UnregisterClass' {
            $actual = $actual.Replace("PCLSMENUNAME pcmn,`n    wow_class_remove_context *context)", 'PCLSMENUNAME pcmn)')
            $actual = $actual.Replace("    /* DIVERGENCE(OPENNT-HOST-060): propagate native class-release failure. */`n    if (!DestroyClass(ppcls))`n        return FALSE;", '    DestroyClass(ppcls);')
        }
    }
    if ($actual -cne (Body $check.Source $check.Name)) {
        throw "Unregistered original-owner body change: $($check.Name)"
    }
    Write-Output "Original owner body verified: $($check.Name)"
}
$probePath = Join-Path $OriginalRoot 'base/ntos/ex/probe.c'
if ((Get-FileHash -LiteralPath $probePath -Algorithm SHA256).Hash -ne
    '0D1CAE414B609C624E6BDF04FA28EBD84FF38DBB9687A63D8BC9E840C2F2372B') {
    throw 'Original probe.c identity changed'
}
$originalProbe = [IO.File]::ReadAllText($probePath)
$mirrorProbe = [IO.File]::ReadAllText((Join-Path $RepositoryRoot 'src/opennt-host/base/ntos/ex/probe.c'))
if ($mirrorProbe -match '(?<!\r)\n') { throw 'Non-original LF in probe.c' }
$marker = '#undef ProbeForRead'
if (!$originalProbe.Contains($marker) -or !$mirrorProbe.Contains($marker) -or
    $originalProbe.Substring($originalProbe.IndexOf($marker)).TrimEnd() -cne
    $mirrorProbe.Substring($mirrorProbe.IndexOf($marker)).TrimEnd()) {
    throw 'Unregistered original-owner body change: ProbeForRead'
}
Write-Output 'Original owner body verified: ProbeForRead'
Write-Output 'Six owner bodies match pinned OpenNT after registered boundary adaptations.'

$userPath = Join-Path $OriginalRoot 'windows/core/ntuser/inc/user.h'
if ((Get-FileHash -LiteralPath $userPath -Algorithm SHA256).Hash -ne
    'ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1') {
    throw 'Original user.h identity changed'
}
$originalUser = [IO.File]::ReadAllText($userPath)
$mirrorUser = [IO.File]::ReadAllText((Join-Path $RepositoryRoot 'src/opennt-host/windows/core/ntuser/inc/user.h'))
if ($mirrorUser -match '(?<!\r)\n') { throw 'Non-original LF in user.h' }
foreach ($pattern in @(
    'typedef struct _LARGE_UNICODE_STRING \{[\s\S]*?\} LARGE_UNICODE_STRING, \*PLARGE_UNICODE_STRING;',
    'typedef struct _HEAD \{[\s\S]*?\} THROBJHEAD, \*PTHROBJHEAD;',
    'typedef struct tagWND \{[\s\S]*?\} WND;',
    'typedef struct tagDESKTOPINFO \{[\s\S]*?\} DESKTOPINFO, \*PDESKTOPINFO;'
)) {
    $expected = [regex]::Matches($originalUser, $pattern)
    $actual = [regex]::Matches($mirrorUser, $pattern)
    if ($expected.Count -ne 1 -or $actual.Count -ne 1 -or
        $expected[0].Value -cne $actual[0].Value) {
        throw "Original USER client layout changed: $pattern"
    }
}
Write-Output 'Original USER client layout bodies verified byte-for-byte (four spans).'
