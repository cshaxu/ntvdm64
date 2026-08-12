[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$AnalysisRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($AnalysisRoot)) {
    $AnalysisRoot = Join-Path $repository 'artifacts\analysis\t95-s6-startup-session-msvc-objects-001-20260811-001'
}
$analysis = [IO.Path]::GetFullPath($AnalysisRoot)
if (Test-Path -LiteralPath $analysis) { throw "Refusing to overwrite existing analysis directory: $analysis" }

$adapter = Join-Path $repository 'src\bx-ntvdm-adapter'
$cli = Join-Path $repository 'src\cli'
$sources = @(
    @{ Folder = 'adapter'; Name = 'bx_ntvdm_startup_session.c' },
    @{ Folder = 'adapter'; Name = 'bx_ntvdm_cpu_state_abi.c' },
    @{ Folder = 'adapter'; Name = 'bx_ntvdm_guest_range.c' },
    @{ Folder = 'adapter'; Name = 'bx_ntvdm_guest_write_abi.c' },
    @{ Folder = 'adapter'; Name = 'bx_ntvdm_startup_plan_abi.c' },
    @{ Folder = 'cli'; Name = 'byob_profile.c' },
    @{ Folder = 'cli'; Name = 'byob_identity.c' },
    @{ Folder = 'cli'; Name = 'byob_image.c' }
)
New-Item -ItemType Directory -Path (Join-Path $analysis 'adapter'), (Join-Path $analysis 'cli') -Force | Out-Null
foreach ($directory in @($adapter, $cli)) {
    $destinationName = if ($directory -eq $adapter) { 'adapter' } else { 'cli' }
    $destination = Join-Path $analysis $destinationName
    Get-ChildItem -LiteralPath $directory -Filter *.h | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $destination -ErrorAction Stop
    }
}
$manifest = @()
foreach ($source in $sources) {
    $originDirectory = if ($source.Folder -eq 'adapter') { $adapter } else { $cli }
    $origin = Join-Path $originDirectory $source.Name
    if (-not (Test-Path -LiteralPath $origin -PathType Leaf)) { throw "Missing declared source: $origin" }
    $destination = Join-Path (Join-Path $analysis $source.Folder) $source.Name
    Copy-Item -LiteralPath $origin -Destination $destination -ErrorAction Stop
    $hash = (Get-FileHash -LiteralPath $origin -Algorithm SHA256).Hash
    if ((Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash -ne $hash) { throw "Copy hash mismatch: $($source.Name)" }
    $manifest += [ordered]@{ relativePath = "$($source.Folder)/$($source.Name)"; sha256 = $hash }
}

$wrapperPath = Join-Path $analysis 'compile.cmd'
$logPath = Join-Path $analysis 'compile.log'
$lines = @(
    '@echo off',
    'setlocal',
    'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x86',
    'if errorlevel 1 exit /b 1',
    "cd /d `"$analysis`""
)
foreach ($source in $sources) {
    $object = $source.Name -replace '\.c$', '.obj'
    $lines += "cl.exe /nologo /c /MT /W3 /DWIN32 /Iadapter /Icli /Fo$($source.Folder)\$object $($source.Folder)\$($source.Name)"
    $lines += 'if errorlevel 1 exit /b 1'
}
$lines += 'exit /b 0'
[IO.File]::WriteAllText($wrapperPath, ($lines -join "`r`n"), [Text.UTF8Encoding]::new($false))
& $env:ComSpec /d /c "`"$wrapperPath`"" *> $logPath
$exitCode = $LASTEXITCODE
$objects = @()
foreach ($source in $sources) {
    $objectPath = Join-Path (Join-Path $analysis $source.Folder) ($source.Name -replace '\.c$', '.obj')
    if (Test-Path -LiteralPath $objectPath -PathType Leaf) {
        $objects += [ordered]@{ relativePath = "$($source.Folder)/$([IO.Path]::GetFileName($objectPath))"; sha256 = (Get-FileHash -LiteralPath $objectPath -Algorithm SHA256).Hash }
    }
}
$record = [ordered]@{
    schema = 'ntdos64.t95-s6.startup-session-msvc-object-audit.v1'
    analysisRoot = $analysis
    compilerCommand = 'BuildTools VsDevCmd x86 then cl /nologo /c /MT /W3 /DWIN32 /Iadapter /Icli'
    sourceManifest = $manifest
    exitCode = $exitCode
    objectManifest = $objects
    expectedObjectCount = $sources.Count
}
[IO.File]::WriteAllText((Join-Path $analysis 'object-audit.json'), ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
if ($exitCode -ne 0 -or $objects.Count -ne $sources.Count) { throw "Startup-session MSVC object audit failed; see $logPath" }
Write-Host "Startup-session MSVC object audit completed: $analysis"
