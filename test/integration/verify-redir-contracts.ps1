param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 redir-contracts *> "$out/build.log"
if($LASTEXITCODE){throw 'Redirector contracts failed to build'}
$cache="$repo/build/.ninja/fixtures-x86"
foreach($name in 'dlc-event','redir-queue','redir-loader'){
 $arguments=@()
 if($name -eq 'redir-loader'){$arguments+= "$cache/redir-loader-provider.dll"}
 & "$cache/$name.exe" @arguments *> "$out/$name.txt"
 if($LASTEXITCODE){throw "Contract failed: $name"}
 Get-Content "$out/$name.txt"
}
'Queue case characterizes the known allocation-failure exception; it does not prove defect-free queue behavior.'
