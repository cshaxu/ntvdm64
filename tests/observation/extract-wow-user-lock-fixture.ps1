param([string]$BuildRoot='build/M0-T420/S40/class-registration-r1')
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
if (!$output.StartsWith($root+'\build\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Generated fixture must remain under build'
}
$path='O:/repos.external/OpenNT/windows/core/ntuser/kernel/ssend.c'
if ((Get-FileHash $path).Hash -ne 'E4B9600381683902A05FF8BA3E7E0F6876246A97960482F8B0BF6E51DA601844') {
    throw 'Original callback owner changed'
}
$source=[IO.File]::ReadAllText($path)
$start=$source.IndexOf('#define MAKECALL(api)')
$end=$source.IndexOf('#define CHECKRETURN()', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Original macro boundaries changed' }
New-Item -ItemType Directory -Force $output | Out-Null
[IO.File]::WriteAllText((Join-Path $output 'original-user-callout.inc'),
    $source.Substring($start,$end-$start),[Text.Encoding]::ASCII)
'WOW_ORIGINAL_USER_CALLOUT_MACROS'
