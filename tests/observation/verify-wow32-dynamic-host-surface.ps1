param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'

function Get-X86Dumpbin {
    $root = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC'
    $toolset = Get-ChildItem -LiteralPath $root -Directory |
        Sort-Object Name -Descending |
        Select-Object -First 1
    if ($null -eq $toolset) { throw 'No x86 MSVC toolset is available.' }
    $dumpbin = Join-Path $toolset.FullName 'bin\Hostx86\x86\dumpbin.exe'
    if (-not (Test-Path -LiteralPath $dumpbin -PathType Leaf)) { throw "Missing x86 dumpbin: $dumpbin" }
    return $dumpbin
}

function Get-DumpbinText {
    param([string]$Dumpbin, [string]$Path)
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing host dependency: $Path" }
    return ((& $Dumpbin /exports $Path) -join "`n")
}

function Assert-NamedExports {
    param([string]$Name, [string]$Exports, [string[]]$Expected)
    $missing = @($Expected | Where-Object {
        -not [bool]($Exports -match ('(?m)\s' + [regex]::Escape($_) + '$'))
    })
    if ($missing.Count -ne 0) { throw "$Name misses original WOW32 exports: $($missing -join ', ')" }
    Write-Output "$Name named exports: $($Expected.Count)"
}

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$wow32 = Join-Path $root 'src\mvdm\wow32'
$dumpbin = Get-X86Dumpbin

$wsockOrdinals = @(Select-String -LiteralPath (Join-Path $wow32 'wsext.c') -Pattern '\(char \*\)\s*([0-9]+)' -AllMatches |
    ForEach-Object { $_.Matches | ForEach-Object { [int]$_.Groups[1].Value } })
if ($wsockOrdinals.Count -ne 49) { throw "Expected 49 original WSOCK32 ordinal entries, found $($wsockOrdinals.Count)." }
$wsockExports = Get-DumpbinText -Dumpbin $dumpbin -Path 'C:\Windows\SysWOW64\WSOCK32.DLL'
$missingOrdinals = @($wsockOrdinals | Where-Object {
    -not [bool]($wsockExports -match ('(?m)^\s*' + $_ + '\s+'))
})
if ($missingOrdinals.Count -ne 0) { throw "WSOCK32.DLL misses original WOW32 ordinal exports: $($missingOrdinals -join ', ')" }
Write-Output "WSOCK32 ordinal exports: $($wsockOrdinals.Count)"

$spoolNames = @('EXTDEVICEMODE', 'DEVICEMODE', 'DEVICECAPABILITIES', 'OpenPrinterA', 'StartDocPrinterA', 'StartPagePrinter', 'EndPagePrinter', 'EndDocPrinter', 'ClosePrinter', 'WritePrinter', 'DeletePrinter', 'GetPrinterDriverDirectoryA', 'AddPrinterA', 'AddPrinterDriverA', 'AddPortExA')
Assert-NamedExports -Name 'WINSPOOL.DRV' -Exports (Get-DumpbinText -Dumpbin $dumpbin -Path 'C:\Windows\SysWOW64\WINSPOOL.DRV') -Expected $spoolNames

$mediaFiles = @('wmmedia.c', 'wmmedia1.c', 'wmmedia2.c') | ForEach-Object { Join-Path $wow32 $_ }
$mediaNames = @(Select-String -LiteralPath $mediaFiles -Pattern 'GET_MULTIMEDIA_API\(\s*"([^"]+)"' -AllMatches |
    ForEach-Object { $_.Matches | ForEach-Object { $_.Groups[1].Value } } | Sort-Object -Unique)
if ($mediaNames.Count -ne 78) { throw "Expected 78 original WINMM lazy exports, found $($mediaNames.Count)." }
Assert-NamedExports -Name 'WINMM.DLL' -Exports (Get-DumpbinText -Dumpbin $dumpbin -Path 'C:\Windows\SysWOW64\WINMM.DLL') -Expected $mediaNames
Assert-NamedExports -Name 'OLETHK32.DLL' -Exports (Get-DumpbinText -Dumpbin $dumpbin -Path 'C:\Windows\SysWOW64\OLETHK32.DLL') -Expected @('ConvertObjDescriptor')

Write-Output 'WOW32_DYNAMIC_HOST_SURFACE_OK'
