[CmdletBinding()]
param(
    [Parameter(Mandatory)][string[]]$Image,
    [Parameter(Mandatory)][guid]$PackageId
)
$ErrorActionPreference='Stop'
if($PackageId -eq [guid]::Empty){throw 'Package identity must not be empty'}
$writer=Join-Path $PSScriptRoot '../../build/.ninja/build-tools/ntvdm-package-id.exe'
if(!(Test-Path -LiteralPath $writer -PathType Leaf)){throw 'Build native tools with tools/build/bootstrap.cmd first'}
foreach($item in $Image) {
    $path=(Resolve-Path -LiteralPath $item -ErrorAction Stop).Path
    & $writer $PackageId.ToString() $path
    if($LASTEXITCODE){throw "Package identity write failed ($LASTEXITCODE): $path"}
}
