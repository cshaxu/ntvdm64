param([switch]$CleanupOnly)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
Set-Location $repo
$scratch=Join-Path $repo 'build/td-build-audit/checks'
$null=New-Item -ItemType Directory -Force -Path $scratch
$cleaner=Join-Path $repo 'tools/build/Clear-BuildOutput.ps1'
$passed=[Collections.Generic.List[string]]::new()
function Check([bool]$Condition,[string]$Name) {
    if(!$Condition){throw "FAIL: $Name"}
    $passed.Add($Name); Write-Output "PASS: $Name"
}
function Refuses([scriptblock]$Action,[string]$Name){
    $refused=$false
    try{& $Action | Out-Null}catch{$refused=$true}
    Check $refused $Name
}
function Write-Utf8([string]$Path,[string]$Value){
    [IO.File]::WriteAllText($Path,$Value,[Text.UTF8Encoding]::new($false))
}
Refuses {& $cleaner -CheckResidual} 'final residual check reports active disposable directories'
$safe='build/td-build-audit/checks/disposable'
$bad='build/td-build-audit/checks/sensitive'
$safePath=Join-Path $repo $safe; $badPath=Join-Path $repo $bad
$null=New-Item -ItemType Directory -Force -Path $safePath,$badPath
Write-Utf8 "$safePath/result.txt" 'generated test result'
Write-Utf8 "$badPath/settings.ini" 'user configuration fixture'
$manifest=Join-Path $scratch 'ownership.json'
$record=@{targets=@(
    @{path=$safe;owner='Td cleanup test';reason='generated fixture'},
    @{path=$bad;owner='Td cleanup test';reason='deliberate refusal fixture'}
);reviewedFiles=@()}
$initial=@(& $cleaner -Target @($safe,$bad) -Inventory)
foreach($targetRecord in $record.targets){$targetRecord.inventorySha256=($initial|Where-Object Path -eq $targetRecord.path|Select-Object -Last 1).InventorySha256}
Write-Utf8 $manifest ($record|ConvertTo-Json -Depth 5)
$preview=@(& $cleaner -Target $safe -OwnershipManifest $manifest)
Check ((Test-Path "$safePath/result.txt") -and !$preview[0].Applied) 'preview does not delete'
Refuses {& $cleaner -Target $safe -OwnershipManifest $manifest -Apply} 'Apply requires actual owner-exit attestation'
Refuses {& $cleaner -Target @($safe,$bad) -OwnershipManifest $manifest -Apply -TasksExited} 'all-target preflight rejects unreviewed configuration'
Check (Test-Path "$safePath/result.txt") 'later unsafe target leaves earlier safe target intact'
Write-Utf8 "$safePath/personal.txt" 'unreviewed content, despite a generated-looking extension'
Refuses {& $cleaner -Target $safe -OwnershipManifest $manifest -Apply -TasksExited} 'new generated-looking file cannot inherit directory ownership'
[IO.File]::Delete("$safePath/personal.txt")
$inventory=@(& $cleaner -Target $bad -Inventory)
Check (@($inventory | Where-Object Kind -eq 'ReviewRequired').Count -eq 1) 'inventory reports exact sensitive file without deleting'
foreach($target in @('build/../assets','C:/Windows','build/.ninja','build/.ninja/msvc-x86-release','build/.ccache','build/.ccache/tmp','build/*','build/.ninja.','build/.ccache ')){
    Refuses {& $cleaner -Target $target -OwnershipManifest $manifest} "refuses $target"
}
Refuses {& $cleaner -Target 'build/td-build-audit/checks' -OwnershipManifest $manifest} 'unknown ownership is refused'
Write-Utf8 "$safePath/.git" 'gitdir: not a real checkout; deliberate refusal test'
Refuses {& $cleaner -Target $safe -OwnershipManifest $manifest -Apply -TasksExited} 'Git marker is never ordinary output'
[IO.File]::Delete("$safePath/.git") # Remove only the test-owned marker, never Git data.
Write-Utf8 "$safePath/disk.img" 'test-authored media refusal fixture'
Refuses {& $cleaner -Target $safe -OwnershipManifest $manifest -Apply -TasksExited} 'unreviewed media is refused'
[IO.File]::Delete("$safePath/disk.img")
$link=Join-Path $safePath 'junction'
$null=New-Item -ItemType Junction -Path $link -Target $badPath
Refuses {& $cleaner -Target $safe -OwnershipManifest $manifest -Apply -TasksExited} 'junction traversal is refused'
[IO.Directory]::Delete($link) # Unlink the test-owned junction, without following it.
Check (Test-Path "$badPath/settings.ini") 'junction target remains intact'
$sha=[Security.Cryptography.SHA256]::Create()
$hash=([BitConverter]::ToString($sha.ComputeHash([IO.File]::ReadAllBytes("$badPath/settings.ini")))).Replace('-','')
$sha.Dispose()
$record.reviewedFiles=@(@{path="$bad/settings.ini";sha256=$hash;reason='test-authored data, not user configuration'})
Write-Utf8 $manifest ($record|ConvertTo-Json -Depth 5)
$deleted=@(& $cleaner -Target @($safe,$bad) -OwnershipManifest $manifest -Apply -TasksExited)
Check (!(Test-Path $safePath) -and !(Test-Path $badPath) -and $deleted.Count -eq 2) 'reviewed generated targets are deleted'

if(!$CleanupOnly){
    $cmake=(Get-Command cmake.exe -ErrorAction Stop).Source
    $ccache=(Get-Command ccache.exe -ErrorAction Stop).Source
    $ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
    $vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
    function Run-Dev([string]$Architecture,[string]$Command,[string]$Name){
        $batch=Join-Path $scratch "$Name.cmd"
        Write-Utf8 $batch ("@echo off`r`ncall `"$vs`" -arch=$Architecture -host_arch=x64 >nul`r`nif errorlevel 1 exit /b %errorlevel%`r`n$Command`r`nexit /b %errorlevel%`r`n")
        $lines=@(& cmd.exe /d /c $batch 2>&1)
        $exit=$LASTEXITCODE
        Write-Utf8 (Join-Path $scratch "$Name.log") ($lines -join "`r`n")
        if($exit){throw "$Name exit $exit; $($lines | Select-Object -Last 10)"}
        return $lines
    }
    foreach($architecture in @('x86','x64')){
        $preset="msvc-$architecture-release"
        $tree=Join-Path $repo "build/.ninja/$preset"
        $null=Run-Dev $architecture "`"$cmake`" --preset $preset" "$architecture-configure-off"
        if($architecture -eq 'x86'){
            $imports='ntvwm.exe ntcon.exe ntmon.exe run16.exe ntsrv.exe'
            $null=Run-Dev $architecture "`"$ninja`" -C `"$tree/worker`" $imports" 'native-imports'
            foreach($image in ($imports -split ' ')){
                if((Get-FileHash "$repo/assets/release/$image").Hash -ne (Get-FileHash "$tree/worker/$image").Hash){throw 'Native import changed verified release input'}
            }
            Check $true 'native image imports preserve verified identities through existing PE checks'
            $unchanged=Run-Dev $architecture "`"$ninja`" -C `"$tree/worker`" $imports" 'native-imports-incremental'
            Check (($unchanged -join ' ') -match 'no work to do') 'unchanged native imports do not restage'
        }
        $null=Run-Dev $architecture "`"$cmake`" --build `"$tree`" --target W16Style" "$architecture-build-off"
        $before=@(Get-ChildItem "$tree/CMakeFiles" -Filter '*.obj' -Recurse | ForEach-Object {"$($_.FullName):$($_.LastWriteTimeUtc.Ticks)"})
        $noWork=Run-Dev $architecture "`"$cmake`" --build `"$tree`" --target W16Style" "$architecture-incremental-off"
        $after=@(Get-ChildItem "$tree/CMakeFiles" -Filter '*.obj' -Recurse | ForEach-Object {"$($_.FullName):$($_.LastWriteTimeUtc.Ticks)"})
        Check ($before.Count -gt 0 -and !(Compare-Object $before $after) -and ($noWork -join ' ') -match 'no work to do') "$architecture unchanged incremental does not compile"
        $null=Run-Dev $architecture "`"$cmake`" --preset $preset-ccache" "$architecture-configure-cache"
        $foreign=Join-Path $scratch 'foreign-cache'
        $savedDir=$env:CCACHE_DIR; $savedTemp=$env:CCACHE_TEMPDIR; $savedMax=$env:CCACHE_MAXSIZE
        try{
            $env:CCACHE_DIR=$foreign; $env:CCACHE_TEMPDIR="$foreign/tmp"; $env:CCACHE_MAXSIZE='1M'
            $null=Run-Dev $architecture "`"$cmake`" --build `"$tree`" --target W16Style --verbose" "$architecture-build-cache-hostile-env"
        }finally{$env:CCACHE_DIR=$savedDir;$env:CCACHE_TEMPDIR=$savedTemp;$env:CCACHE_MAXSIZE=$savedMax}
        Check (!(Test-Path $foreign)) "$architecture ignores caller's external cache location"
        $log=Get-Content (Join-Path $scratch "$architecture-build-cache-hostile-env.log") -Raw
        Check ($log -match 'CCACHE_DIR=.*build/.ccache' -and $log -match 'CCACHE_TEMPDIR=.*build/.ccache/tmp' -and $log -match 'CCACHE_MAXSIZE=5G') "$architecture generated compile command fixes all cache settings"
        $manifestTree=if($architecture -eq 'x86'){"$tree/worker"}else{"$tree/monitor"}
        $commands=Get-Content "$manifestTree/build.ninja" -Raw
        Check ($commands -match 'CCACHE_DIR=.*build/.ccache' -and $commands -match 'CCACHE_TEMPDIR=.*build/.ccache/tmp' -and $commands -match 'CCACHE_MAXSIZE=5G') "$architecture original manifest graph fixes cache routing too"
        $library=if($architecture -eq 'x86'){'common-root.lib'}else{'common-rpc.lib'}
        $null=Run-Dev $architecture "`"$ninja`" -C `"$manifestTree`" $library" "$architecture-original-manifest-library"
        $null=Run-Dev $architecture "`"$ninja`" -C `"$tree`" -t clean ntw16s W16Style" "$architecture-cache-rebuild-clean"
        $null=Run-Dev $architecture "`"$cmake`" --build `"$tree`" --target W16Style" "$architecture-cache-hit-rebuild"
        $null=Run-Dev $architecture "`"$cmake`" --preset $preset" "$architecture-disable-cache"
        $cache=Get-Content "$tree/CMakeCache.txt" -Raw
        Check ($cache -match '(?m)^CMAKE_C_COMPILER_LAUNCHER:STRING=\r?$' -and $cache -match '(?m)^CMAKE_CXX_COMPILER_LAUNCHER:STRING=\r?$') "$architecture cache-off clears old launchers in same tree"
    }
    $stats=@(& $cmake -E env "CCACHE_DIR=$repo/build/.ccache" "CCACHE_TEMPDIR=$repo/build/.ccache/tmp" "CCACHE_MAXSIZE=5G" $ccache --print-stats)
    if($LASTEXITCODE){throw 'Repository ccache stats failed'}
    Write-Utf8 (Join-Path $scratch 'ccache-stats.txt') ($stats -join "`n")
    $hits=0
    foreach($line in $stats){if($line -match '^(direct_cache_hit|preprocessed_cache_hit)\s+(\d+)'){$hits += [int]$Matches[2]}}
    Check ($hits -gt 0) 'actual repository-local ccache hits observed'
    $refusal=@(& $cmake -S $repo -B "$scratch/outside-ninja" -G Ninja 2>&1)
    Check ($LASTEXITCODE -ne 0 -and ($refusal -join ' ') -match 'strictly below') 'real CMake refuses outside-Ninja configuration'
    & $cmake "-DNTVDM_BINARY_DIR=$repo/build/.ninja/reasonable-extra-config" -P "$repo/tools/build/BuildTree.cmake"
    Check ($LASTEXITCODE -eq 0) 'real configuration allows another stable inside-Ninja tree'
    $temporaryConfig='build/.ninja/reasonable-extra-config'
    $configurationInventory=@(& $cleaner -Target $temporaryConfig -Inventory)
    $configurationOwner=@{targets=@(@{path=$temporaryConfig;owner='Td configuration test';reason='unconfigured test-only prefix, no Ninja graph or CMake cache';inventorySha256=$configurationInventory[-1].InventorySha256});reviewedFiles=@()}
    & $cleaner -Target $temporaryConfig -OwnershipManifest $configurationOwner -Apply -TasksExited | Out-Null
    Check (!(Test-Path (Join-Path $repo $temporaryConfig))) 'unconfigured test prefix is removed, not hidden in cache'
    & "$repo/tests/observation/verify-title-marker.ps1" -BuildRoot "$scratch/title-marker"
    if($LASTEXITCODE -or (Get-Content "$scratch/title-marker/exit-code.txt" -Raw).Trim() -ne '0'){throw 'Migrated style fixture entrypoint failed'}
    Check $true 'migrated dual-width fixture entrypoint builds in cache and logs outside it'
}
Write-Utf8 (Join-Path $scratch 'result.json') (@{passed=@($passed);count=$passed.Count;utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json -Depth 4)
"PASS: $($passed.Count) build-hygiene assertions"
