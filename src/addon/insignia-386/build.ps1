[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$OutputRoot,
    [Parameter(Mandatory)][string]$Wlink,
    [string]$Nasm = 'nasm.exe'
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../../..").Path
$out = [IO.Path]::GetFullPath($OutputRoot)
if (!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $out)) {
    throw 'A fresh repository build/ output directory is required'
}
$linker = (Resolve-Path -LiteralPath $Wlink).Path
$assembler = (Get-Command $Nasm -CommandType Application -ErrorAction Stop | Select-Object -First 1).Source
New-Item -ItemType Directory -Path $out | Out-Null
Push-Location $out
try {
    & $assembler -f obj "$PSScriptRoot/insignia.asm" -o insignia.obj -l insignia.lst
    if ($LASTEXITCODE) { throw 'NASM failed' }
    # Both guests initially use static registration. Dynamic loading/unloading
    # is not supported by the initialization-only control-block allocation.
    $link = @'
format windows vxd static
option quiet
option map=insignia.map
name insignia.386
file insignia.obj
segment _LTEXT shared resident
segment _LDATA shared resident
export insignia_ddb.1
'@
    [IO.File]::WriteAllText("$out/insignia.lnk",$link.Replace("`r`n","`n"))
    & $linker '@insignia.lnk'
    if ($LASTEXITCODE) { throw 'Open Watcom link failed' }

    [ordered]@{
        source = (Get-FileHash "$PSScriptRoot/insignia.asm").Hash
        assembler = $assembler
        assemblerSha256 = (Get-FileHash $assembler).Hash
        linker = $linker
        linkerSha256 = (Get-FileHash $linker).Hash
        loadMode = 'static'
        artifacts = @('insignia.386') | ForEach-Object {
            [ordered]@{name=$_;bytes=(Get-Item "$out/$_").Length;sha256=(Get-FileHash "$out/$_").Hash}
        }
    } | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath "$out/build.json" -Encoding utf8
} finally { Pop-Location }
