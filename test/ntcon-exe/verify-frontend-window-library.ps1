param([string]$BuildRoot = 'build/M0-T423/S6/library')
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$output = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
$buildPrefix = (Join-Path $repo 'build') + [IO.Path]::DirectorySeparatorChar
if (!$output.StartsWith($buildPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build output must remain below repository build/'
}

$owner = Join-Path $repo 'src/ntcon-exe'
$manifest = Get-Content (Join-Path $owner 'nxvm-import.json') -Raw | ConvertFrom-Json
if ((Get-FileHash (Join-Path $repo 'src/lib-nxvm/LICENSE.nxvm') -Algorithm SHA256).Hash -ne $manifest.licenseSha256) {
    throw 'Imported license differs from pinned nxvm'
}
foreach ($row in $manifest.files) {
    if ($row.path -notmatch '^lib-nxvm/lib/(types|base|kvm-base|kvm-window)/[A-Za-z0-9_./-]+$' -or
        $row.path.Contains('..')) { throw "Invalid imported path: $($row.path)" }
    $file = Join-Path (Join-Path $repo 'src') $row.path
    if ((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash -ne $row.sha256) {
        throw "Imported file differs from pinned nxvm: $($row.path)"
    }
}
New-Item -ItemType Directory -Force -Path $output | Out-Null
$line='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 frontend-library"'
$p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$output/build.stdout" -RedirectStandardError "$output/build.stderr"
try {
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$output/build.owner.json"
    if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'Frontend producer deadline'}
    $p.WaitForExit();$p.ExitCode|Set-Content "$output/build.exit"
    if($p.ExitCode){throw 'Frontend native build failed'}
} finally {$p.Dispose()}
$cache=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
foreach($name in @('frontend-window-library-test','frontend-window-keyboard-test')) {
    Copy-Item "$cache/$name.exe" "$output/$name.exe" -Force
    $p=Start-Process "$output/$name.exe" -WorkingDirectory $output -WindowStyle Hidden -PassThru -RedirectStandardOutput "$output/$name.stdout" -RedirectStandardError "$output/$name.stderr"
    try {
        $null=$p.Handle
        @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image="$output/$name.exe";timeoutSeconds=30}|ConvertTo-Json|Set-Content "$output/$name.owner.json"
        if(!$p.WaitForExit(30000)){$p.Kill();$p.WaitForExit();throw 'Frontend assertion deadline'}
        $p.WaitForExit();$p.ExitCode|Set-Content "$output/$name.exit"
        if($p.ExitCode){throw "$name assertions failed"}
        Get-Content "$output/$name.stdout"
    } finally {$p.Dispose()}
}
Write-Output 'Actual Window input/return remains gated by Verify-CommandExitStatus Window17.'
Write-Output "PASS byte-exact imported files: $($manifest.files.Count); selected library x86 units: $(@($manifest.files | Where-Object compile).Count)"
