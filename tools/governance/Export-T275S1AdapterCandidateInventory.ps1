param(
    [Parameter(Mandatory = $true)]
    [string]$ReferenceRoot,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$families = [ordered]@{
    'adapter-mvdm-host-in' = 'bop-transport'
    'adapter-mvdm-host-out/softpc' = 'softpc-ccpu-sas'
    'adapter-mvdm-host-out/win32' = 'win32-ntdll'
}

$rows = foreach ($component in $families.Keys) {
    $inputRoot = Join-Path $ReferenceRoot $component
    if (-not (Test-Path -LiteralPath $inputRoot -PathType Container)) {
        throw "Missing recovery-reference root: $inputRoot"
    }
    $root = (Resolve-Path -LiteralPath $inputRoot).Path.TrimEnd([char]92)
    Get-ChildItem -LiteralPath $root -Recurse -File | Sort-Object FullName |
        ForEach-Object {
            $relative = $_.FullName.Substring($root.Length).TrimStart([char]92).Replace([char]92, [char]'/')
            $extension = $_.Extension.ToLowerInvariant()
            $kind = if ($extension -in @('.c', '.cc', '.cpp', '.cxx', '.asm', '.s')) {
                'implementation-candidate'
            } elseif ($extension -in @('.h', '.inc')) {
                'declaration-candidate'
            } else {
                'supporting-reference'
            }
            [pscustomobject][ordered]@{
                reference_component = $component
                reference_path = "$component/$relative"
                interface_family = $families[$component]
                candidate_kind = $kind
                sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
                initial_disposition = 'requires-original-caller-abi-failure-reuse-audit'
            }
        }
}

$directory = Split-Path -Parent $OutputPath
if (-not (Test-Path -LiteralPath $directory -PathType Container)) {
    throw "Output directory does not exist: $directory"
}
$rows | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $OutputPath -Encoding utf8
Write-Output "Wrote $($rows.Count) adapter recovery candidates."
