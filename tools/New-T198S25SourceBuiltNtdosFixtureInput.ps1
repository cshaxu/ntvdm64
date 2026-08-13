param(
    [string]$RepositoryRoot = '',
    [string]$OutputPath = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$input = Join-Path $repository 'artifacts\toolchain-runs\ntdos-tools16-historical-v1\base\mvdm\dos\v86\doskrnl\dos\NTDOS.SYS'
if ([string]::IsNullOrWhiteSpace($OutputPath)) {
    $OutputPath = Join-Path $repository 'artifacts\build\t198-s25-native-ntdos-r1\ntdos_bytes.cc'
}
if (-not (Test-Path -LiteralPath $input -PathType Leaf)) { throw "Missing locked input: $input" }
$bytes = [IO.File]::ReadAllBytes($input)
$hash = (Get-FileHash -LiteralPath $input -Algorithm SHA256).Hash
if ($bytes.Length -ne 27858 -or $hash -ne '957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84') {
    throw "Locked NTDOS identity mismatch: bytes=$($bytes.Length) sha256=$hash"
}
$lines = [Collections.Generic.List[string]]::new()
$lines.Add('/* Generated from the locked source-built NTDOS.SYS; do not edit. */')
$lines.Add('typedef unsigned char Bit8u;')
$lines.Add('extern const Bit8u t198_s25_ntdos_bytes[0x6cd2] = {')
for ($offset = 0; $offset -lt $bytes.Length; $offset += 16) {
    $end = [Math]::Min($offset + 16, $bytes.Length)
    $items = for ($i = $offset; $i -lt $end; ++$i) { '0x{0:x2}' -f $bytes[$i] }
    $suffix = if ($end -lt $bytes.Length) { ',' } else { '' }
    $lines.Add(('  ' + ($items -join ', ') + $suffix))
}
$lines.Add('};')
$directory = Split-Path -Parent $OutputPath
New-Item -ItemType Directory -Force -Path $directory | Out-Null
[IO.File]::WriteAllLines($OutputPath, $lines)
Write-Host "Generated exact NTDOS fixture input: $OutputPath"
