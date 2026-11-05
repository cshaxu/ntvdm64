param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {throw 'Fresh build-owned fixture required'}
$package=(Resolve-Path -LiteralPath $PackageRoot).Path

New-Item -ItemType Directory -Path $out | Out-Null
foreach($name in @('SETUP.EXE','DOSX.EX_','KRNL386.EX_','WIN386.EX_')) {Copy-Item -LiteralPath (Join-Path $package $name) -Destination $out}
Copy-Item -LiteralPath (Join-Path $package 'PATCH') -Destination $out -Recurse
$args=@{MediaRoot=$out}
$apply=Join-Path $repo 'tools/build/win31-setup/apply-setup.ps1'
& $apply @args
& $apply @args
$unknown=Join-Path $out 'PATCH/USER-DATA.TXT'
[IO.File]::WriteAllText($unknown,'keep')
$refused=$false
try { & $apply @args } catch { $refused=$_.Exception.Message -like '*Preserve unknown existing PATCH payload*' }
if(!$refused -or [IO.File]::ReadAllText($unknown) -ne 'keep') {throw 'Unknown payload was not preserved'}
'PASS repeated preparation and unknown PATCH payload preservation'
