[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$OutputRoot,
    [switch]$OriginalProviders, [string]$BuildCache='build/.ninja/msvc-x86-release/worker')
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
if (!$out.StartsWith(([IO.Path]::GetFullPath((Join-Path $root 'build')) + '\'),
    [StringComparison]::OrdinalIgnoreCase)) { throw 'Output must stay in build/.' }
if (Test-Path -LiteralPath $out) { throw 'Use fresh evidence directory.' }
New-Item -ItemType Directory -Path $out | Out-Null
$cache=Join-Path $root $BuildCache
if ([IO.Path]::GetFullPath($cache) -ne [IO.Path]::GetFullPath((Join-Path $root 'build/.ninja/msvc-x86-release/worker'))) {
    throw 'Use the canonical worker build cache.'
}
$target=if($OriginalProviders){'redirector-providers'}else{'redirector-capacity'}
$image=if($OriginalProviders){'redirector-netapi-provider-test.exe'}else{'redirector-oem-capacity-test.exe'}
& "$root/tools/build-fixtures.cmd" x86 $target 2>&1 |
    Tee-Object (Join-Path $out 'build.txt')
if($LASTEXITCODE){throw 'Redirector fixture build failed'}
$exe=Join-Path $out $image
Copy-Item (Join-Path $cache $image) $exe
Copy-Item (Join-Path $cache "$image.map") "$exe.map"
$selected=(Get-Content (Join-Path $cache 'source-manifest.json') -Raw|ConvertFrom-Json).sources
$inputs=@($selected|Where-Object {
    $_.object -like 'obj/tests/redirector/*' -and
    $(if($OriginalProviders){$_.object -notlike '*/capacity.obj'}
      else{$_.object -notmatch '/(providers|async|vrnetapi|vrnmpipe)\.obj$'})
}|ForEach-Object path)
if ($OriginalProviders) {
    $map = Get-Content -LiteralPath "$exe.map" -Raw
    foreach ($symbol in @('VrGetUserName','VrGetCDNames','VrGetComputerName',
        'VrGetDomainName','VrGetLogonServer','VrNetGetDCName','VrNetWkstaSetInfo',
        'VrReturnAssignMode','VrSetAssignMode')) {
        if ($map -notmatch "(?m)^.*_$symbol\s+.*\bvrnetapi\.obj\s*$") {
            throw "Original entrypoint not bound to this run's compiled unit: $symbol"
        }
    }
}
$info = [Diagnostics.ProcessStartInfo]::new()
$info.FileName = $exe; $info.UseShellExecute = $false; $info.CreateNoWindow = $true
$info.RedirectStandardOutput = $true; $info.RedirectStandardError = $true
$p = [Diagnostics.Process]::new(); $p.StartInfo = $info
try {
    if (!$p.Start()) { throw 'Fixture did not start.' }
    $stdout = $p.StandardOutput.ReadToEndAsync(); $stderr = $p.StandardError.ReadToEndAsync()
    if (!$p.WaitForExit(10000)) { $p.Kill(); $p.WaitForExit(); throw 'Timeout, not a pass.' }
    $text = $stdout.Result + $stderr.Result
    $text | Set-Content -LiteralPath (Join-Path $out 'result.txt') -Encoding UTF8
    [pscustomobject]@{exit_code=$p.ExitCode;original_providers=[bool]$OriginalProviders
        executable_sha256=(Get-FileHash $exe).Hash
        source_hashes=@($inputs | ForEach-Object {
            [pscustomobject]@{path=$_;sha256=(Get-FileHash $_).Hash}
        })
    } | ConvertTo-Json -Depth 4 | Set-Content (Join-Path $out 'result.json')
    Write-Output $text
    $count = if ($OriginalProviders) { 24 } else { 14 }
    if ($p.ExitCode -ne 0 -or $text -notmatch "assertions=$count failures=0") {
        throw 'OEM/CD assertions failed.'
    }
} finally { $p.Dispose() }
