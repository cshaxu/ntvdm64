param([string]$BuildRoot = 'build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$file = 'O:/repos.external/OpenNT/windows/core/ntuser/client/ntstubs.c'
if ((Get-FileHash $file).Hash -ne '7DF814A5496CEB8CEDAEE8B51D9F4D1966629F109F15EBF8622805EB6902EB01') {
    throw 'Unrecognized original creation source.'
}
$text = [IO.File]::ReadAllText($file)
$start = $text.IndexOf('HWND _CreateWindowEx(')
$end = $text.IndexOf('HKL _LoadKeyboardLayoutEx(', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original creation boundaries changed.' }
New-Item -ItemType Directory -Force $output | Out-Null
[IO.File]::WriteAllText((Join-Path $output 'original-create-entry.inc'),
    $text.Substring($start, $end - $start), [Text.Encoding]::ASCII)
$macros = 'O:/repos.external/OpenNT/windows/core/ntuser/inc/ntsend.h'
if ((Get-FileHash $macros).Hash -ne 'B708F1FEE46E93735C4C931D556E16DAA9679E6352D72620904D18AD21396ABF') {
    throw 'Original call macros changed.'
}
Copy-Item -LiteralPath $macros -Destination (Join-Path $output 'ntsend-fixture.h')
'WOW_CREATE_ENTRY_EXACT_ORIGINAL_BODY'
function Read-Pinned([string]$relative, [string]$hash) {
    $path = Join-Path 'O:/repos.external/OpenNT/windows/core/ntuser' $relative
    if ((Get-FileHash $path).Hash -ne $hash) { throw "Source changed: $path" }
    [IO.File]::ReadAllText($path)
}
function Slice([string]$value, [string]$first, [string]$next) {
    $a = $value.IndexOf($first)
    $b = $value.IndexOf($next, $a + $first.Length)
    if ($a -lt 0 -or $b -le $a) { throw 'Capture boundary changed.' }
    $value.Substring($a, $b - $a)
}
$types = Read-Pinned 'inc/user.h' 'ADCF0AC89C4779D9B7908610976F4A47D2385403EDC37496C8AB004B016CF6C1'
$capture = Read-Pinned 'client/rtlinit.c' 'D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C'
$init = Read-Pinned 'rtl/chartran.c' '0F9F0B7FDE3AAA475492A50F333745508698228C38FDBF9A21CBD4BFBEAD383B'
[IO.File]::WriteAllText((Join-Path $output 'original-large-types.inc'),
    (Slice $types 'typedef struct _LARGE_STRING {' ("/*`r`n * String macros")), [Text.Encoding]::ASCII)
$captureBody = Slice $capture 'BOOL RtlCaptureLargeAnsiString(' '//++'
$initStart = $init.IndexOf('VOID RtlInitLargeAnsiString(')
if ($initStart -lt 0) { throw 'String initializer missing.' }
[IO.File]::WriteAllText((Join-Path $output 'original-large-capture.inc'),
    ($captureBody + $init.Substring($initStart)), [Text.Encoding]::ASCII)
'WOW_CREATE_CAPTURE_EXACT_ORIGINAL_BODIES'
