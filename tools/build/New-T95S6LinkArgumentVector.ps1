$ErrorActionPreference = 'Stop'

function Assert-T95S6AbsolutePath([string]$Path, [string]$Role) {
    if ([string]::IsNullOrWhiteSpace($Path)) { throw "$Role path is empty." }
    if (-not [System.IO.Path]::IsPathRooted($Path)) { throw "$Role path must be absolute: $Path" }
    return [System.IO.Path]::GetFullPath($Path)
}

function Assert-T95S6ObjectPath([string]$Path) {
    $fullPath = Assert-T95S6AbsolutePath $Path 'Object'
    if (-not $fullPath.EndsWith('.obj', [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Object path must end in .obj: $fullPath"
    }
    if (-not (Test-Path -LiteralPath $fullPath -PathType Leaf)) {
        throw "Object path does not exist: $fullPath"
    }
    return $fullPath
}

function Test-T95S6LinkArgumentVector {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$ArgumentVector,

        [Parameter(Mandatory = $true)]
        [string]$ExpectedOutPath,

        [Parameter(Mandatory = $true)]
        [string]$ExpectedMapPath,

        [Parameter(Mandatory = $true)]
        [string[]]$ExpectedObjectPath
    )

    $expectedOut = Assert-T95S6AbsolutePath $ExpectedOutPath 'Output'
    $expectedMap = Assert-T95S6AbsolutePath $ExpectedMapPath 'Map'
    $expectedObjects = @($ExpectedObjectPath | ForEach-Object { Assert-T95S6ObjectPath $_ })
    if ($expectedObjects.Count -eq 0) { throw 'At least one object path is required.' }
    if (@($expectedObjects | Select-Object -Unique).Count -ne $expectedObjects.Count) {
        throw 'Expected object paths contain a duplicate.'
    }
    foreach ($argument in $ArgumentVector) {
        if ([string]::IsNullOrWhiteSpace($argument)) { throw 'Argument vector contains an empty token.' }
    }

    $required = @('/nologo', "/OUT:$expectedOut", '/SUBSYSTEM:CONSOLE', '/INCREMENTAL:NO', '/OPT:REF', "/MAP:$expectedMap", '/VERBOSE:LIB')
    foreach ($token in $required) {
        if (@($ArgumentVector | Where-Object { $_ -ceq $token }).Count -ne 1) {
            throw "Required link token is not present exactly once: $token"
        }
    }
    foreach ($token in $ArgumentVector) {
        if ($token.StartsWith('/')) {
            if ($required -notcontains $token) { throw "Forbidden, malformed, duplicate, or unexpected option: $token" }
        }
    }
    if ($ArgumentVector.Count -ne (7 + $expectedObjects.Count)) {
        throw "Argument vector count is not exact: expected $(7 + $expectedObjects.Count), got $($ArgumentVector.Count)."
    }

    $actualObjects = @($ArgumentVector | Where-Object { -not $_.StartsWith('/') })
    if ($actualObjects.Count -ne $expectedObjects.Count) { throw 'Argument vector object token count is not exact.' }
    if (@($actualObjects | Select-Object -Unique).Count -ne $actualObjects.Count) { throw 'Argument vector contains a duplicate object token.' }
    for ($index = 0; $index -lt $expectedObjects.Count; $index++) {
        $actual = Assert-T95S6ObjectPath $actualObjects[$index]
        if ($actual -cne $expectedObjects[$index]) {
            throw "Object token mismatch at index ${index}: expected '$($expectedObjects[$index])', got '$actual'."
        }
    }
    return $true
}

function New-T95S6LinkArgumentVector {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory = $true)]
        [string]$OutPath,

        [Parameter(Mandatory = $true)]
        [string]$MapPath,

        [Parameter(Mandatory = $true)]
        [string[]]$ObjectPath
    )

    $out = Assert-T95S6AbsolutePath $OutPath 'Output'
    $map = Assert-T95S6AbsolutePath $MapPath 'Map'
    $objects = @($ObjectPath | ForEach-Object { Assert-T95S6ObjectPath $_ })
    if ($objects.Count -eq 0) { throw 'At least one object path is required.' }
    if (@($objects | Select-Object -Unique).Count -ne $objects.Count) { throw 'Object paths contain a duplicate.' }
    $arguments = @('/nologo', "/OUT:$out", '/SUBSYSTEM:CONSOLE', '/INCREMENTAL:NO', '/OPT:REF', "/MAP:$map", '/VERBOSE:LIB') + $objects
    [void](Test-T95S6LinkArgumentVector -ArgumentVector $arguments -ExpectedOutPath $out -ExpectedMapPath $map -ExpectedObjectPath $objects)
    return $arguments
}
