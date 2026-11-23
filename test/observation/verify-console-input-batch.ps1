param([Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$LogPath)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {throw 'BuildRoot must be below repository build'}
$log=[IO.Path]::GetFullPath($LogPath)
if(!$log.StartsWith((Join-Path $root 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -and
   !$log.StartsWith('O:\winnt\Logs2\',[StringComparison]::OrdinalIgnoreCase) -and
   !$log.StartsWith('O:\winnt\logs\',[StringComparison]::OrdinalIgnoreCase)) {throw 'LogPath must use repository build or approved observation directory'}
New-Item -ItemType Directory -Force (Split-Path -Parent $log) | Out-Null
New-Item -ItemType Directory -Force $build | Out-Null
& "$root/tools/build-fixtures.cmd" x86 console-input-batch *> "$build/build.log"
if($LASTEXITCODE){throw 'Batch fixture build failed; see build.log'}
$exe="$root/build/.ninja/fixtures-x86/console-input-batch.exe"
& $exe *> $log
if($LASTEXITCODE){throw ('Batch fixture failed; see '+$log)}
Get-Content $log
