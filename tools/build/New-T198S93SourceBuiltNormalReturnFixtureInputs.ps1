param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$output = [IO.Path]::GetFullPath($OutputDirectory)
$commandPath = Join-Path $repository 'artifacts\inputs\t194-v6-normal-return-root-r1\COMMAND.COM'
$sharePath = Join-Path $repository 'artifacts\inputs\t194-v6-normal-return-root-r1\TARGET.EXE'
$commandHash = '908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43'
$shareHash = '69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC'

function Assert-Input([string]$Path, [int]$Bytes, [string]$Hash) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { throw "Missing locked input: $Path" }
    $data = [IO.File]::ReadAllBytes($Path)
    $actual = (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash
    if ($data.Length -ne $Bytes -or $actual -ne $Hash) {
        throw "Locked input identity mismatch: $Path bytes=$($data.Length) sha256=$actual"
    }
    return $data
}

function Write-Array([string]$Path, [string]$Symbol, [byte[]]$Bytes, [string]$SourceName) {
    $lines = [Collections.Generic.List[string]]::new()
    $lines.Add("/* Generated from locked source-built $SourceName; do not edit. */")
    $lines.Add('typedef unsigned char Bit8u;')
    $lines.Add("extern const Bit8u $Symbol[0x$($Bytes.Length.ToString('x'))] = {")
    for ($offset = 0; $offset -lt $Bytes.Length; $offset += 16) {
        $end = [Math]::Min($offset + 16, $Bytes.Length)
        $items = for ($i = $offset; $i -lt $end; ++$i) { '0x{0:x2}' -f $Bytes[$i] }
        $suffix = if ($end -lt $Bytes.Length) { ',' } else { '' }
        $lines.Add(('  ' + ($items -join ', ') + $suffix))
    }
    $lines.Add('};')
    [IO.File]::WriteAllLines($Path, $lines)
}

$command = Assert-Input $commandPath 50384 $commandHash
$share = Assert-Input $sharePath 882 $shareHash
New-Item -ItemType Directory -Force -Path $output | Out-Null
Write-Array (Join-Path $output 'command_bytes.cc') 't198_s93_command_bytes' $command 'COMMAND.COM'
Write-Array (Join-Path $output 'share_bytes.cc') 't198_s93_share_bytes' $share 'SHARE.EXE'

@{
    schema = 'runner.t198.s93.source-built-normal-return-inputs.v1'
    inputs = @(
        @{ role = 'command'; source = $commandPath; bytes = 50384; sha256 = $commandHash; symbol = 't198_s93_command_bytes' },
        @{ role = 'target'; source = $sharePath; bytes = 882; sha256 = $shareHash; symbol = 't198_s93_share_bytes' }
    )
    outputs = @('command_bytes.cc', 'share_bytes.cc')
    disposition = 'compile-only fixture input; not a guest runtime import'
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $output 'source-inputs.json') -Encoding utf8
Write-Host "Generated locked S93 COMMAND/SHARE fixture inputs: $output"
