[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$CompilerDriver
)
# Exact original alias/free owners with native/guest service doubles. This is
# part of the full DDE group gate, not transport, guest or publication proof.
# Invoke under the active packet's owned hidden background supervisor. No
# Ninja/CMake tree or cross-task cache is created by this disposable C fixture.
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root = [IO.Path]::GetFullPath($BuildRoot).TrimEnd('\', '/')
$build = Join-Path $repo 'build'
if ([IO.Path]::GetDirectoryName($root) -ne $build -or
        [IO.Path]::GetFileName($root).StartsWith('.') -or
        (Test-Path -LiteralPath $root) -or
        ((Get-Item -LiteralPath $build -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)) {
    throw 'Require one fresh, direct, non-cache build child for disposable evidence'
}
$driver = (Resolve-Path -LiteralPath $CompilerDriver).Path
$fixture = Join-Path $PSScriptRoot 'wow_dde_alias_fixture.c'
$baseline = (& git -C $repo rev-parse '7835ba25e^{commit}').Trim()
if ($LASTEXITCODE) { throw 'Cannot resolve immutable pre-cohort baseline' }
$null = New-Item -ItemType Directory -Path $root
$steps = [Collections.Generic.List[object]]::new()
$selections = [Collections.Generic.List[object]]::new()

function Read-Input([string]$Path, [bool]$Old) {
    if ($Old) {
        $value = (& git -C $repo show "${baseline}:$Path") -join "`n"
        if ($LASTEXITCODE) { throw "Cannot read baseline $Path" }
        return $value
    }
    return [IO.File]::ReadAllText((Join-Path $repo $Path)).Replace("`r`n", "`n")
}

function Select-Body([string]$Text, [string]$Name, [string]$Variant) {
    $pattern = '(?m)^(?:static\s+)?(?:BOOL|VOID|HAND16|HANDLE|PHDDE)\s+(?:FASTCALL\s+)?' +
        [regex]::Escape($Name) + '\s*\([^;{}]*\)\s*\{'
    $found = [regex]::Matches($Text, $pattern)
    if ($found.Count -ne 1) { throw "Require one complete source body: $Name" }
    $match = $found[0]
    $offset = $match.Index + $match.Length
    $depth = 1
    while ($depth -and $offset -lt $Text.Length) {
        if ($Text[$offset] -eq '{') { ++$depth }
        elseif ($Text[$offset] -eq '}') { --$depth }
        ++$offset
    }
    if ($depth) { throw "Unbalanced source body: $Name" }
    $length = $offset - $match.Index
    $selections.Add([pscustomobject]@{
        variant = $Variant; name = $Name; offset = $match.Index; length = $length
    })
    return $Text.Substring($match.Index, $length)
}

function Invoke-Owned([string]$Name, [string]$File, [string[]]$Arguments,
        [int]$Seconds, [int]$Expected) {
    $start = @{ FilePath = $File; WorkingDirectory = $root; WindowStyle = 'Hidden';
        PassThru = $true; RedirectStandardOutput = "$root/$Name.stdout.log";
        RedirectStandardError = "$root/$Name.stderr.log" }
    if ($Arguments.Count) { $start.ArgumentList = $Arguments }
    $process = Start-Process @start
    $null = $process.Handle
    $row = [pscustomobject]@{
        name = $Name; pid = $process.Id; start = $process.StartTime.ToUniversalTime().ToString('o')
        file = $File; arguments = $Arguments; deadlineSeconds = $Seconds; exit = $null
    }
    $steps.Add($row)
    $steps | ConvertTo-Json -Depth 6 | Set-Content "$root/steps.json"
    if (!$process.WaitForExit($Seconds * 1000)) {
        & taskkill.exe /PID $process.Id /T /F
        $process.WaitForExit()
        throw "Owned deadline expired: $Name"
    }
    $process.WaitForExit()
    $row.exit = $process.ExitCode
    $steps | ConvertTo-Json -Depth 6 | Set-Content "$root/steps.json"
    if ($process.ExitCode -ne $Expected) {
        throw "$Name exit $($process.ExitCode), expected $Expected"
    }
}

try {
    $inputs = @('src/mvdm/wow32/wdde.c', 'src/mvdm/wow32/wdde.h', 'src/mvdm/wow32/wmdisp32.c', 'src/mvdm/wow32/wumsg.c') |
        ForEach-Object { Join-Path $repo $_ }
    Get-FileHash -LiteralPath @($inputs + @($fixture, $PSCommandPath, $driver)) |
        Export-Csv -NoTypeInformation "$root/source-inputs.csv"
    foreach ($variant in @('baseline', 'candidate')) {
        $old = $variant -eq 'baseline'
        $directory = Join-Path $root $variant
        $null = New-Item -ItemType Directory -Path $directory
        $header = Read-Input 'src/mvdm/wow32/wdde.h' $old
        $source = Read-Input 'src/mvdm/wow32/wdde.c' $old
        $first = $header.IndexOf('typedef struct _HDDE {', [StringComparison]::Ordinal)
        if ($first -lt 0) { throw 'Original DDE type start missing' }
        $last = $header.IndexOf('} DDEINFO, *PDDEINFO;', $first, [StringComparison]::Ordinal)
        if ($first -lt 0 -or $last -lt 0) { throw 'Original DDE type boundary missing' }
        $types = $header.Substring($first, $last + '} DDEINFO, *PDDEINFO;'.Length - $first)
        $flags = [regex]::Matches($header, '(?m)^#define DDE_(?!CONVERSION_)[^\n]+') |
            ForEach-Object Value
        if (@($flags).Count -ne 4) { throw 'Original DDE flag family changed' }
        if (!$old) {
            $conversionStart = $header.IndexOf('#define DDE_CONVERSION_MAX_PAIRS', [StringComparison]::Ordinal)
            if ($conversionStart -lt 0) { throw 'Conversion ownership types missing' }
            $conversionEnd = $header.IndexOf('} DDECONVERSION, *PDDECONVERSION;', $conversionStart, [StringComparison]::Ordinal)
            if ($conversionEnd -lt 0) { throw 'Conversion ownership types missing' }
            $types += "`n#define FIXTURE_CONVERSION 1`n" + $header.Substring($conversionStart,
                $conversionEnd + '} DDECONVERSION, *PDDECONVERSION;'.Length - $conversionStart)
        }
        [IO.File]::WriteAllText("$directory/dde_alias_types.h", $types + "`n" + ($flags -join "`n"))
        $markReturn = if ($source -match '(?m)^BOOL\s+W32MarkDDEHandle\s*\(') { 'BOOL' } else { 'VOID' }
        [IO.File]::WriteAllText("$directory/dde_alias_mark_prototype.h",
            "static $markReturn W32MarkDDEHandle(HAND16);`n")
        $bodies = foreach ($name in @('DDEAddhandle', 'DDEDeletehandle',
                'DDEFindPair16', 'DDEFindPair32', 'DDEFindNode16', 'DDEFindNode32',
                'DDEFindAckNode', 'W32MarkDDEHandle', 'W32UnMarkDDEHandle',
                'W32DDEFreeGlobalMem32', 'W32DdeFreeHandle16')) {
            Select-Body $source $name $variant
        }
        if (!$old) {
            foreach ($name in @('DDEBeginConversion', 'DDETrackhandle', 'DDERollback32')) {
                $bodies += Select-Body $source $name $variant
            }
            $bodies += Select-Body (Read-Input 'src/mvdm/wow32/wumsg.c' $false) 'W32CancelDDEPost' $variant
        }
        $bodies += Select-Body (Read-Input 'src/mvdm/wow32/wmdisp32.c' $old) 'WM32DDEAck' $variant
        [IO.File]::WriteAllText("$directory/dde_alias_exact_bodies.h", ($bodies -join "`n`n"))
        $command = "@echo off`r`ncall `"$driver`" cl.exe /nologo /TC /O2 /MT /Gz /W4 /WX " +
            "/wd4127 /wd4213 /wd4305 /wd4706 /I`"$directory`" `"$fixture`" " +
            "/Fo`"$directory/unit.obj`" /Fe`"$directory/unit.exe`"`r`nexit /b %errorlevel%`r`n"
        [IO.File]::WriteAllText("$directory/build.cmd", $command)
        Invoke-Owned "compile-$variant" 'cmd.exe' @('/d', '/c', "`"$directory/build.cmd`"") 180 0
        Invoke-Owned "body-$variant" "$directory/unit.exe" @() 30 $(if ($old) { 1 } else { 0 })
        $output = Get-Content -LiteralPath "$root/body-$variant.stdout.log" -Raw
        $failed = @([regex]::Matches($output, '(?m)^FAIL ([^\r\n]+)\r?$'))
        if ($old) {
            if ($failed.Count -ne 3 -or $failed[0].Groups[1].Value -ne 'empty-delete-false' -or
                    $failed[1].Groups[1].Value -ne 'mark-reject-unpublished' -or
                    $failed[2].Groups[1].Value -ne 'ack-retired-node-captured') {
                throw 'Baseline must demonstrate only the empty-list, mark-rejection and retired-ACK-node defects'
            }
        } elseif ($failed.Count) { throw 'Candidate has an alias/free ownership failure' }
        $result = [regex]::Match($output,
            'DDE_ALIAS cases=(\d+) errors=(\d+) nodes=0 guest_proof=0 transport_proof=0')
        if (!$result.Success -or [int]$result.Groups[1].Value -ne $(if ($old) { 82 } else { 101 }) -or
                [int]$result.Groups[2].Value -ne $(if ($old) { 3 } else { 0 })) {
            throw 'Incomplete exact-body ownership result'
        }
        Get-FileHash -LiteralPath "$directory/dde_alias_types.h", "$directory/dde_alias_exact_bodies.h",
            "$directory/unit.exe" | Export-Csv -NoTypeInformation "$directory/inputs.csv"
    }
    $selections | ConvertTo-Json | Set-Content "$root/selections.json"
    foreach ($entry in Import-Csv -LiteralPath "$root/source-inputs.csv") {
        if ((Get-FileHash -LiteralPath $entry.Path).Hash -ne $entry.Hash) {
            throw "Source changed during group: $($entry.Path)"
        }
    }
    @{ passed = $true; baseline = $baseline; baselineBodies = 12; candidateBodies = 16;
        scope = 'Original alias/free bodies with service doubles; no guest/transport proof'
    } | ConvertTo-Json | Set-Content "$root/result.json"
} catch {
    $_ | Out-String | Set-Content "$root/failure.txt"
    exit 1
}
