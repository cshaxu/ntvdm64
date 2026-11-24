[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
 [ValidateSet('allocation','move')][string]$Case='allocation')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh worktree build output required'}
New-Item -ItemType Directory $out | Out-Null
$cache=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
& "$repo/tools/build-fixtures.cmd" x86 "xms-$Case" *> "$out/build.txt"
if($LASTEXITCODE){throw 'Native XMS build failed'}
$exe="$out/xms-boundary.exe"
Copy-Item "$cache/xms-$Case-fixture.exe" $exe
Copy-Item "$cache/xms-$Case-fixture.exe.map" "$exe.map"
$map=Get-Content "$exe.map" -Raw
$symbols=if($Case -eq 'move'){@('xmsMoveBlock')}else{@('xmsAllocBlock','xmsReallocBlock','xmsFreeBlock')}
foreach($symbol in $symbols){if($map -notmatch "_$symbol\s+.*provider.obj"){throw "Missing original owner $symbol"}}
$started=Get-Date
$p=Start-Process $exe -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/result.txt" -RedirectStandardError "$out/error.txt"
if(!$p.WaitForExit(10000)){$p.Kill();throw 'Fixture timeout'}
$result=Get-Content "$out/result.txt" -Raw
@{pid=$p.Id;started=$started.ToString('o');ended=(Get-Date).ToString('o');exit_code=$p.ExitCode;
  provider_sha256=(Get-FileHash (Join-Path $repo 'src/mvdm-host/xms.486/xmsblock.c')).Hash;
  executable_sha256=(Get-FileHash $exe).Hash;result=$result.Trim()} | ConvertTo-Json | Set-Content "$out/result.json"
$exitCode=$p.ExitCode
$p.Dispose()
if($exitCode -ne 0 -or $result -notmatch 'assertions=\d+ failures=0'){throw 'XMS fixture failed'}
Get-Content "$out/result.txt"
