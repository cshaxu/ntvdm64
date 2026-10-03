param(
    [Parameter(Mandatory=$true)][string]$OutputRoot,
    [Parameter(Mandatory=$true)][string]$WorkerName,
    [Parameter(Mandatory=$true)][string]$FrontendName,
    [Parameter(Mandatory=$true)][string]$RecordName,
    [string]$PackageRoot = 'O:/winnt',
    [string]$PackageWorkerName = ''
)
$ErrorActionPreference = 'Stop'
if (!$PackageWorkerName) { $PackageWorkerName = $WorkerName }
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
$build = [IO.Path]::GetFullPath((Join-Path $root 'build')) + [IO.Path]::DirectorySeparatorChar
if (!$out.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)) { throw 'Audit output must be below repository build/.' }
New-Item -ItemType Directory -Force -Path $out | Out-Null
Push-Location $root
try {
    $rows = [Collections.Generic.List[object]]::new()
    $tokens = @($WorkerName,$FrontendName,$RecordName)
    $args = @('grep','-I','-n','-i')
    foreach ($token in $tokens) { $args += @('-e',$token) }
    $lines = & git -c core.quotepath=false @args
    if ($LASTEXITCODE -gt 1) { throw 'Tracked inventory failed.' }
    $untracked = @(& git -c core.quotepath=false ls-files --others --exclude-standard)
    foreach ($path in $untracked) {
        $bytes = [IO.File]::ReadAllBytes((Join-Path $root $path))
        if ($bytes -contains 0) { continue }
        $n = 0
        foreach ($line in [IO.File]::ReadAllLines((Join-Path $root $path))) {
            ++$n
            foreach ($token in $tokens) {
                if ($line.IndexOf($token,[StringComparison]::OrdinalIgnoreCase) -ge 0) {
                    $lines += ('{0}:{1}:{2}' -f $path,$n,$line); break
                }
            }
        }
    }
    foreach ($line in $lines) {
        $parts = $line -split ':',3
        if ($parts.Count -ne 3 -or $parts[1] -notmatch '^\d+$') { throw "Unparseable inventory row: $line" }
        foreach ($token in $tokens) {
            foreach ($match in [regex]::Matches($parts[2],[regex]::Escape($token),'IgnoreCase')) {
                $before = $parts[2].Substring(0,$match.Index)
                $after = $parts[2].Substring($match.Index+$match.Length)
                $class = 'worker-product'
                if ($token -eq $RecordName) { $class = 'project-record' }
                elseif ($token -eq $FrontendName) { $class = 'frontend-product' }
                elseif (($before -match '[a-zA-Z0-9]$') -or ($after -match '^[a-zA-Z0-9]')) { $class = 'substring-not-identity' }
                elseif ($after -match '^[/\\](client|server|inc|test|private\.c|output\.c|HandleKeyEvent|\{)(?![a-zA-Z0-9])' -or
                    $parts[0] -like 'artifacts/documentation-archive/*' -or
                    $parts[0] -eq 'src/ntcon-exe/window_keyboard.c' -or
                    $parts[0] -eq 'src/ntvdm-exe/win32/console_graphics.c' -or
                    $parts[0] -eq 'src/opennt-abi/host-compat/README.md') { $class = 'original-console-source' }
                elseif ($parts[0] -match '^docs/(proposals/proposal-(native-worker-frontend-renaming-001|worker-task-trace-observation-001|native-launch-hook-001)\.md|states/CURRENT\.md|etc/operations/t424-worker-frontend-renaming-plan\.md|etc/evidence/m0-t424-s2-native-worker-identity\.md)$' -and
                    $parts[2] -match '(?i)frontend|must never denote|and .+ is the renamed') { $class = 'reserved-frontend' }
                $rows.Add([pscustomobject]@{Path=$parts[0];Line=[int]$parts[1];Column=$match.Index+1;Token=$match.Value;Class=$class;Text=$parts[2]})
            }
        }
    }
    $rows | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $out 'occurrences.csv')
    $paths = @(& git -c core.quotepath=false ls-files) + $untracked
    $pathRows = foreach ($path in $paths) {
        foreach ($token in $tokens) {
            if ($path.IndexOf($token,[StringComparison]::OrdinalIgnoreCase) -ge 0) {
                [pscustomobject]@{Path=$path;Token=$token}
            }
        }
    }
    $pathRows | Export-Csv -NoTypeInformation -Encoding UTF8 (Join-Path $out 'paths.csv')
    $hashes = foreach ($name in @('run16.exe','ntsrv.exe','ntvdm.exe',($PackageWorkerName+'.exe'),($FrontendName+'.exe'),'ntmon.exe','wow32.dll','VDMREDIR.dll')) {
        $path = Join-Path $PackageRoot $name
        [pscustomobject]@{File=$name;Bytes=(Get-Item -LiteralPath $path).Length;SHA256=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash}
    }
    $hashes | ConvertTo-Json | Set-Content -Encoding UTF8 (Join-Path $out 'published-hashes.json')
    $summary = [pscustomobject]@{
        Revision=(& git rev-parse HEAD);TrackedFiles=@(& git ls-files).Count;UntrackedFiles=$untracked.Count
        Occurrences=$rows.Count;MatchingFiles=@($rows.Path | Sort-Object -Unique).Count;MatchingPaths=@($pathRows).Count
        Classes=@($rows | Group-Object Class | Select-Object Name,Count)
        Version=[IO.File]::ReadAllText((Join-Path $root 'src/common/protocol/version.h'))
        RpcIdentity=@([IO.File]::ReadAllLines((Join-Path $root 'src/common/protocol/service.idl')) | Select-Object -First 6)
    }
    $summary | ConvertTo-Json -Depth 5 | Set-Content -Encoding UTF8 (Join-Path $out 'summary.json')
    $summary | ConvertTo-Json -Depth 5
} finally { Pop-Location }
