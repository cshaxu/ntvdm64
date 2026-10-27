param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$InstallRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot,
    [ValidateRange(10000,600000)][int]$TimeoutMs = 60000,
    [ValidateSet('std-dosonly0','std-dosonly1','386-dosonly0','386-dosonly1')]
    [string[]]$Cases = @('std-dosonly0','std-dosonly1','386-dosonly0','386-dosonly1'),
    [switch]$AllowUserInstallation
)
$ErrorActionPreference = 'Stop'
if (@($Cases | Select-Object -Unique).Count -ne $Cases.Count) { throw 'Duplicate matrix cases' }
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$install = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$package = (Resolve-Path -LiteralPath $PackageRoot).Path
$observer = (Resolve-Path -LiteralPath $Observer).Path
$output = [IO.Path]::GetFullPath($OutputRoot)
if (!$install.StartsWith('O:\tmp\',[StringComparison]::OrdinalIgnoreCase) -and
    !($AllowUserInstallation -and $install -in @('O:\w31','O:\w31t'))) {
    throw 'A dedicated Windows installation copy under O:\tmp is required'
}
if ($install -match '[^\x20-\x7e]' -or $install -match '\s') {
    throw 'The isolated guest path must be ASCII and contain no whitespace'
}
if (!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)) {
    throw 'A fresh build-owned output directory is required'
}
if ($output -match '\s') { throw 'The observer output path must not contain whitespace' }
$patch = Join-Path $install 'PATCH'
$winIni = Join-Path $install 'WIN.INI'
$config = Join-Path $patch 'CONFIG.NT'
$autoexec = Join-Path $patch 'AUTOEXEC.NT'
$systemIni = Join-Path $install 'SYSTEM.INI'
$probe = Join-Path $patch 'W31CHECK.EXE'
$dosProbe = Join-Path $patch 'W31DOS.COM'
$dosResult = Join-Path $patch 'W31DOS.BIN'
$boot = Join-Path $patch 'W31BOOT.BIN'
$ready = Join-Path $patch 'W31READY.BIN'
foreach ($path in @($probe,$dosProbe,$dosResult,$boot,$ready)) {
    if (Test-Path -LiteralPath $path) { throw "Test input/output already exists: $path" }
}
foreach ($name in @('WINSTD.PIF','WIN386.PIF','WINSTD.CMD','WIN386.CMD')) {
    if (!(Test-Path -LiteralPath (Join-Path $patch $name) -PathType Leaf)) { throw "Missing $name" }
}
if ($Cases -contains '386-dosonly0' -or $Cases -contains '386-dosonly1') {
    $systemText = Get-Content -LiteralPath $systemIni -Raw
    if ($systemText -match '(?im)^\s*PermSwapDOSDrive\s*=' -or
        $systemText -match '(?im)^\s*32BitDiskAccess\s*=\s*(on|yes|true|1)\s*$') {
        throw 'Enhanced-mode test copy must not access a physical disk or permanent swap file'
    }
}
foreach ($case in @(@('WINSTD.PIF','/S'),@('WIN386.PIF','/3'))) {
    $pif = Join-Path $patch $case[0]
    $shown = & (Join-Path $repo 'assets/release/PIF.EXE') show $pif
    if ($LASTEXITCODE) { throw "Invalid test PIF: $pif" }
    $lines = @($shown)
    foreach ($required in @("PROGRAM=$install\WIN.COM", "DIRECTORY=$install",
            "ARGUMENTS=$($case[1])", "CONFIG=$patch\CONFIG.NT", "AUTOEXEC=$patch\AUTOEXEC.NT")) {
        if (-not ($lines | Where-Object { $_ -ieq $required })) { throw "PIF does not target isolated copy: $pif ($required)" }
    }
}
$nasm = (Get-Command nasm.exe -ErrorAction Stop).Source
New-Item -ItemType Directory -Path $output | Out-Null
& $nasm -f bin -o "$output/W31CHECK.EXE" "$repo/tests/observation/win31_startup_contract.asm"
if ($LASTEXITCODE) { throw 'Failed to assemble Win16 startup witness' }
$probeBytes = [IO.File]::ReadAllBytes("$output/W31CHECK.EXE")
foreach ($field in @(@('@W31_BOOT_PATH@',$boot),@('@W31_READY_PATH@',$ready))) {
    $image = [Text.Encoding]::ASCII.GetString($probeBytes)
    $offset = $image.IndexOf($field[0],[StringComparison]::Ordinal)
    if ($offset -lt 0 -or $image.IndexOf($field[0],$offset+1,[StringComparison]::Ordinal) -ge 0) {
        throw 'Probe path placeholder must occur exactly once'
    }
    $bytes = [Text.Encoding]::ASCII.GetBytes($field[1])
    if ($bytes.Length -ge 128) { throw 'Probe output path exceeds fixed guest field' }
    [Array]::Clear($probeBytes,$offset,128)
    [Array]::Copy($bytes,0,$probeBytes,$offset,$bytes.Length)
}
[IO.File]::WriteAllBytes("$output/W31CHECK.EXE",$probeBytes)
& $nasm -f bin -o "$output/W31DOS.COM" "$repo/tests/observation/win31_dosonly_contract.asm"
if ($LASTEXITCODE) { throw 'Failed to assemble DOSONLY witness' }
$dosBytes = [IO.File]::ReadAllBytes("$output/W31DOS.COM")
$dosImage = [Text.Encoding]::ASCII.GetString($dosBytes)
$dosOffset = $dosImage.IndexOf('@W31_DOS_PATH@',[StringComparison]::Ordinal)
if ($dosOffset -lt 0 -or $dosImage.IndexOf('@W31_DOS_PATH@',$dosOffset+1,[StringComparison]::Ordinal) -ge 0) {
    throw 'DOS probe path placeholder must occur exactly once'
}
$dosPathBytes = [Text.Encoding]::ASCII.GetBytes($dosResult)
if ($dosPathBytes.Length -ge 128) { throw 'DOS result path exceeds fixed guest field' }
[Array]::Clear($dosBytes,$dosOffset,128)
[Array]::Copy($dosPathBytes,0,$dosBytes,$dosOffset,$dosPathBytes.Length)
[IO.File]::WriteAllBytes("$output/W31DOS.COM",$dosBytes)
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$winBytes = [IO.File]::ReadAllBytes($winIni)
$configBytes = [IO.File]::ReadAllBytes($config)
$autoBytes = [IO.File]::ReadAllBytes($autoexec)
$encoding = [Text.Encoding]::GetEncoding(28591)
$ini = $encoding.GetString($winBytes)
if (@([regex]::Matches($ini,'(?im)^run=[^\r\n]*')).Count -ne 1) { throw 'Expected exactly one WIN.INI run entry' }
$baseConfig = $encoding.GetString($configBytes)
$baseConfig = [regex]::Replace($baseConfig,'(?im)^\s*dosonly\s*\r?\n','')
$results = [Collections.Generic.List[object]]::new()
$savedEnv = @{}
foreach ($name in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SHORT_HISTORY')) {
    $savedEnv[$name] = [Environment]::GetEnvironmentVariable($name,'Process')
}
try {
    Copy-Item -LiteralPath "$output/W31CHECK.EXE" -Destination $probe
    Copy-Item -LiteralPath "$output/W31DOS.COM" -Destination $dosProbe
    [IO.File]::WriteAllBytes($autoexec,$encoding.GetBytes($encoding.GetString($autoBytes)+"`r`n$dosProbe`r`n"))
    $ini = [regex]::Replace($ini,'(?im)^run=[^\r\n]*',"run=$probe")
    [IO.File]::WriteAllBytes($winIni,$encoding.GetBytes($ini))
    foreach ($case in @(
        @{Name='std-dosonly0'; Command='WINSTD.CMD'; Enhanced=$false; DosOnly=$false},
        @{Name='std-dosonly1'; Command='WINSTD.CMD'; Enhanced=$false; DosOnly=$true},
        @{Name='386-dosonly0'; Command='WIN386.CMD'; Enhanced=$true; DosOnly=$false},
        @{Name='386-dosonly1'; Command='WIN386.CMD'; Enhanced=$true; DosOnly=$true}
    ) | Where-Object {$_.Name -in $Cases}) {
        foreach ($path in @($boot,$ready,$dosResult)) { if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path} }
        $text = $baseConfig
        if ($case.DosOnly) { $text += "`r`ndosonly`r`n" }
        [IO.File]::WriteAllBytes($config,$encoding.GetBytes($text))
        $stage=$null; $scope=$null; $process=$null; $errorText=$null
        $report=Join-Path $output "$($case.Name).observer.txt"
        try {
            $stage=New-PhysicalRuntimeStage $package -FreshPackageIdentity
            $scope=New-IsolatedPackageScope $stage
            $pifName=if($case.Enhanced){'WIN386.PIF'}else{'WINSTD.PIF'}
            [ordered]@{case=$case.Name; package=$package; packageId=(Get-IsolatedPackageIdentity "$stage\system32\ntvdm.exe");
                ntvdmSha256=(Get-FileHash "$stage\system32\ntvdm.exe").Hash;
                pifSha256=(Get-FileHash (Join-Path $patch $pifName)).Hash;
                configSha256=(Get-FileHash $config).Hash; autoexecSha256=(Get-FileHash $autoexec).Hash;
                systemIniSha256=(Get-FileHash $systemIni).Hash; winIniSha256=(Get-FileHash $winIni).Hash;
                dosProbeSha256=(Get-FileHash $dosProbe).Hash; windowsProbeSha256=(Get-FileHash $probe).Hash} |
                ConvertTo-Json | Set-Content -LiteralPath (Join-Path $output "$($case.Name).inputs.json") -Encoding UTF8
            $env:PATH="$stage\system32;C:\Windows\System32;C:\Windows"
            $env:TEMP='O:\tmp'; $env:TMP='O:\tmp'
            $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'; $env:MVDM_OBSERVER_SHORT_HISTORY='1'
            $process=Start-Process $observer -ArgumentList @('C:\Windows\System32\cmd.exe',$install,$report,
                '/d','/c',(Join-Path $patch $case.Command),'--observation-timeout-ms',"$TimeoutMs") -WindowStyle Hidden -PassThru
            if (!$process.WaitForExit($TimeoutMs+15000)) { throw 'Observer did not complete its bounded observation' }
        } catch { $errorText=$_.ToString() }
        finally {
            if($process){if(!$process.HasExited){$process.Kill();[void]$process.WaitForExit(5000)};$process.Dispose()}
            if($scope){Stop-IsolatedPackageScope $scope}
            if($stage){Remove-PhysicalRuntimeStage $stage}
        }
        $started=Test-Path -LiteralPath $boot
        $found=Test-Path -LiteralPath $ready
        $flags=$null; $hwnd=$null; $observedDosOnly=$null; $valid=$false
        if(Test-Path -LiteralPath $dosResult) {
            $dosData=[IO.File]::ReadAllBytes($dosResult)
            if($dosData.Length -eq 5 -and [Text.Encoding]::ASCII.GetString($dosData,0,4) -eq 'D31C') {
                $observedDosOnly=$dosData[4]
                Copy-Item -LiteralPath $dosResult -Destination (Join-Path $output "$($case.Name).dosonly.bin")
            }
        }
        if($found) {
            $data=[IO.File]::ReadAllBytes($ready)
            if($data.Length -eq 10 -and [Text.Encoding]::ASCII.GetString($data,0,4) -eq 'W31C') {
                $flags=[BitConverter]::ToUInt32($data,4);$hwnd=[BitConverter]::ToUInt16($data,8)
                $valid=$hwnd -ne 0 -and ((($flags -band 0x20) -ne 0) -eq $case.Enhanced) -and
                    ($observedDosOnly -eq [int]$case.DosOnly)
                Copy-Item -LiteralPath $ready -Destination (Join-Path $output "$($case.Name).ready.bin")
            }
        }
        $observation=if(Test-Path -LiteralPath $report){Get-Content -LiteralPath $report -Raw}else{''}
        $exited=$observation -match '(?m)^result=exited\r?$' -and $observation -match '(?m)^exit=0x00000000\r?$'
        $pass=$started -and $valid -and $exited -and !$errorText
        $results.Add([pscustomobject]@{case=$case.Name;dosonly=$case.DosOnly;enhanced=$case.Enhanced;
            probeStarted=$started;progmanAndMode=$valid;flags=$flags;hwnd=$hwnd;observedDosOnly=$observedDosOnly;normalExit=$exited;
            passed=$pass;error=$errorText;observer=$report})
        Write-Host "$($case.Name): started=$started desktop-and-mode=$valid normal-exit=$exited pass=$pass"
    }
} finally {
    [IO.File]::WriteAllBytes($winIni,$winBytes)
    [IO.File]::WriteAllBytes($config,$configBytes)
    [IO.File]::WriteAllBytes($autoexec,$autoBytes)
    foreach ($path in @($probe,$dosProbe,$dosResult,$boot,$ready)) { if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path} }
    foreach ($name in $savedEnv.Keys) { [Environment]::SetEnvironmentVariable($name,$savedEnv[$name],'Process') }
    $results | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $output 'matrix-results.json') -Encoding UTF8
}
if($results.Count -ne $Cases.Count -or @($results | Where-Object {!$_.passed}).Count) { throw 'Win31 runtime matrix did not pass all selected cases' }
"PASS $($Cases.Count) selected Windows 3.1 mode/DOSONLY combinations"
