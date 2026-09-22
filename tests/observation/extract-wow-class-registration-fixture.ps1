param([string]$BuildRoot = 'build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith(($root + '\build\'), [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Fixture output must be under repository build.'
}
$source = 'O:/repos.external/OpenNT/windows/core/ntuser'
function Read-Pinned([string]$path, [string]$hash) {
    $file = Join-Path $source $path
    if ((Get-FileHash $file).Hash -ne $hash) { throw "Unrecognized original source: $file" }
    [IO.File]::ReadAllText($file)
}
function Slice([string]$text, [string]$start, [string]$end) {
    $a = $text.IndexOf($start)
    if ($a -lt 0) { throw "Missing start: $start" }
    $b = $text.IndexOf($end, $a + $start.Length)
    if ($b -le $a) { throw "Missing end: $end" }
    $text.Substring($a, $b - $a)
}
$client = Read-Pinned 'client/client.c' '787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472'
$text = Read-Pinned 'inc/ntcftxt.h' 'E7F3D42CE8939BBE0B14EA34BC693107334C19F8B2D7862EC15B75A501D11DAB'
$macros = Read-Pinned 'inc/ntsend.h' 'B708F1FEE46E93735C4C931D556E16DAA9679E6352D72620904D18AD21396ABF'
$capture = Read-Pinned 'client/rtlinit.c' 'D8841BD2F13E5825A84F670EC7CC60923977BF80618D737CBAC2208F94C6523C'
$lookup = Read-Pinned 'kernel/class.c' '82528443BC5F6C227E0E14FE8A9DBC928BA735F98CF3891744C20C8E1604939A'
$lookupStart = $lookup.IndexOf('PPCLS _InnerGetClassPtr(')
$lookupNext = $lookup.IndexOf('VOID UnlockAndFreeCPDs(', $lookupStart)
$lookupEnd = $lookup.LastIndexOf("`r`n}", $lookupNext)
if ($lookupStart -lt 0 -or $lookupEnd -le $lookupStart) { throw 'Original class lookup boundaries changed.' }
$lookupBody = $lookup.Substring($lookupStart, $lookupEnd + 3 - $lookupStart)
$lookupMirror = [IO.File]::ReadAllText((Join-Path $root 'src/opennt-host/windows/core/ntuser/kernel/class.c'))
$mirrorStart = $lookupMirror.IndexOf('PPCLS _InnerGetClassPtr(')
if ($mirrorStart -lt 0 -or $lookupMirror.Substring($mirrorStart).TrimEnd([char[]]"`r`n") -cne $lookupBody) {
    throw 'Original class lookup mirror bodies are not byte-exact.'
}
Write-Output 'WOW_ORIGINAL_CLASS_LOOKUP_BYTE_MATCH'
$mirror = [IO.File]::ReadAllText((Join-Path $root 'src/opennt-host/windows/core/ntuser/inc/ntcftxt.h'))
$expected = (Slice $text '#define StringDuplicate TEXT_FN(StringDuplicate)' 'DWORD  APIENTRY SetClassLong') +
    (Slice $text 'ATOM TEXT_FN(RegisterClassExWOW)(' 'UINT RegisterClipboardFormat(') +
    (Slice $text 'BOOL UnregisterClass(' 'SHORT VkKeyScan(')
# These selected functions have no comment delimiters inside string literals.
# Compare the complete retained token stream, exempting only the two registered
# failure-path frees, not arbitrary lines or all differences in this function.
$correction = "    LocalFree(pcmn->pszClientAnsiMenuName);`r`n    LocalFree(pcmn->pwszClientUnicodeMenuName);`r`n"
if ($mirror.IndexOf($correction) -lt 0 -or $mirror.IndexOf($correction) -ne $mirror.LastIndexOf($correction)) {
    throw 'Expected one registered menu-capture cleanup correction.'
}
function Tokens([string]$value) {
    (($value -replace '(?s)/\*.*?\*/', '') -replace '//[^\r\n]*', '') -replace '\s+', ''
}
if ((Tokens ($mirror.Replace($correction, ''))) -cne (Tokens $expected)) {
    throw 'Class mirror differs from its original subset beyond the registered two frees.'
}
Write-Output 'WOW_CLASS_MIRROR_SOURCE_MATCH correction=2'
New-Item -ItemType Directory -Force $output | Out-Null
$body = Slice $text '#define StringDuplicate TEXT_FN(StringDuplicate)' 'DWORD  APIENTRY SetClassLong'
$body += Slice $text 'ATOM TEXT_FN(RegisterClassExWOW)(' 'UINT RegisterClipboardFormat('
$body += Slice $client "ATOM`r`nWINAPI`r`nRegisterClassWOWA(" 'WORD WowGetDefWindowProcBits('
$body += Slice $text "BOOL UnregisterClass(" 'SHORT VkKeyScan('
[IO.File]::WriteAllText((Join-Path $output 'class-registration-wrapper.inc'),
    (Slice $client "ATOM`r`nWINAPI`r`nRegisterClassWOWA(" 'WORD WowGetDefWindowProcBits('), [Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $output 'class-registration.inc'), $body, [Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $output 'ntsend-fixture.h'), $macros, [Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $output 'class-capture.inc'),
    (Slice $capture 'BOOL RtlCaptureAnsiString(' 'BOOL RtlCaptureLargeAnsiString('), [Text.Encoding]::ASCII)
Get-FileHash (Join-Path $output 'class-registration.inc')
