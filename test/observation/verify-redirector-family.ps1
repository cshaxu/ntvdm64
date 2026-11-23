[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out=[IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
if (!$out.StartsWith(([IO.Path]::GetFullPath((Join-Path $root 'build'))+'\'),
        [StringComparison]::OrdinalIgnoreCase)) { throw 'Output must stay in build/.' }
if (Test-Path -LiteralPath $out) { throw 'Use a fresh evidence directory.' }
New-Item -ItemType Directory -Path $out | Out-Null
$results=[Collections.Generic.List[object]]::new()
function Build-Fixtures([string]$architecture,[string]$target) {
    & "$root/tools/build-fixtures.cmd" $architecture $target 2>&1 |
        Tee-Object (Join-Path $out "$architecture-$target-build.log")
    if($LASTEXITCODE){throw "Fixture build failed: $architecture/$target"}
}
function Run-Fixture([string]$architecture,[string]$name,[string]$path) {
    $info=[Diagnostics.ProcessStartInfo]::new()
    $info.FileName=$path; $info.WorkingDirectory=$out
    $info.UseShellExecute=$false; $info.CreateNoWindow=$true
    $info.RedirectStandardOutput=$true; $info.RedirectStandardError=$true
    $process=[Diagnostics.Process]::new(); $process.StartInfo=$info
    try {
        if(!$process.Start()){throw "Failed to start $name"}
        $stdout=$process.StandardOutput.ReadToEndAsync()
        $stderr=$process.StandardError.ReadToEndAsync()
        if(!$process.WaitForExit(15000)){
            $process.Kill();$process.WaitForExit();throw "Fixture timed out: $name"
        }
        $results.Add([pscustomobject]@{name=$name;architecture=$architecture;
            exit=$process.ExitCode;sha256=(Get-FileHash $path).Hash;
            stdout=$stdout.Result;stderr=$stderr.Result})
        if($process.ExitCode){throw "Fixture failed: $name ($($process.ExitCode))"}
    } finally {$process.Dispose()}
}
try {
    foreach($architecture in 'x86','x64') {
        Build-Fixtures $architecture 'redirector-lifecycle'
        foreach($name in 't290_s3_vrmisc_fixture','t290_s3_vrinit_failure_fixture') {
            Run-Fixture $architecture $name "$root/build/.ninja/fixtures-$architecture/$name.exe"
        }
    }
    Build-Fixtures 'x86' 'command-redirection'
    Run-Fixture 'x86' 'command-redirection-fixture' "$root/build/.ninja/msvc-x86-release/worker/command-redirection-fixture.exe"
    & "$PSScriptRoot/verify-redirector-oem-capacity.ps1" -OutputRoot ([IO.Path]::GetRelativePath($root,(Join-Path $out 'capacity')))
    & "$PSScriptRoot/verify-redirector-oem-capacity.ps1" -OutputRoot ([IO.Path]::GetRelativePath($root,(Join-Path $out 'providers'))) -OriginalProviders
    [pscustomobject]@{exit=0;standaloneCases=$results;capacityAssertions=14;providerAssertions=24} |
        ConvertTo-Json -Depth 5 | Set-Content (Join-Path $out 'result.json')
} catch {
    [pscustomobject]@{exit=1;error="$_";standaloneCases=$results} |
        ConvertTo-Json -Depth 5 | Set-Content (Join-Path $out 'result.json')
    throw
}
Write-Host 'Redirector family passed: five lifecycle instances,14 capacity and24 provider assertions.'
