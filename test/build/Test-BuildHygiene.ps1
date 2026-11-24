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
    # Production compilation/no-op checks live in verify-generated-input-stability.ps1.
    # Exercise the shared native policy directly; do not maintain another build recipe.
    $cmake=(Get-Command cmake.exe -ErrorAction Stop).Source
    $ccache=(Get-Command ccache.exe -ErrorAction Stop).Source
    $policy=Join-Path $repo 'tools/build/BuildTree.cmake'
    $temporaryConfig='build/.ninja/hygiene-policy-check'
    $tree=Join-Path $repo $temporaryConfig
    if(Test-Path -LiteralPath $tree){throw "Test policy directory already exists: $tree"}
    $foreign=Join-Path $scratch 'foreign-cache'
    $savedDir=$env:CCACHE_DIR; $savedTemp=$env:CCACHE_TEMPDIR; $savedMax=$env:CCACHE_MAXSIZE
    try{
        $refusal=@(& $cmake "-DNTVDM_BINARY_DIR=$scratch/outside-ninja" -P $policy 2>&1)
        Check ($LASTEXITCODE -ne 0 -and ($refusal -join ' ') -match 'strictly below') 'native policy refuses outside-Ninja configuration'
        $env:CCACHE_DIR=$foreign; $env:CCACHE_TEMPDIR="$foreign/tmp"; $env:CCACHE_MAXSIZE='1M'
        & $cmake "-DNTVDM_BINARY_DIR=$tree" -DNTVDM_USE_CCACHE=ON "-DNTVDM_CCACHE_EXECUTABLE=$ccache" -P $policy
        Check ($LASTEXITCODE -eq 0) 'native policy accepts an unconfigured inside-Ninja test prefix'
        $commandPath=Join-Path $tree 'compiler-command.txt'
        $command=[IO.File]::ReadAllText($commandPath).Replace('\','/')
        $root=$repo.Replace('\','/')
        Check ($command.Contains("CCACHE_DIR=$root/build/.ccache") -and $command.Contains("CCACHE_TEMPDIR=$root/build/.ccache/tmp") -and $command.Contains('CCACHE_MAXSIZE=5G') -and !$command.Contains($foreign.Replace('\','/'))) 'native policy fixes cache settings despite hostile caller environment'
        $before=Get-Item -LiteralPath $commandPath
        $ticks=$before.LastWriteTimeUtc.Ticks; $hash=(Get-FileHash -LiteralPath $commandPath).Hash
        & $cmake "-DNTVDM_BINARY_DIR=$tree" -DNTVDM_USE_CCACHE=ON "-DNTVDM_CCACHE_EXECUTABLE=$ccache" -P $policy
        Check ($LASTEXITCODE -eq 0 -and (Get-Item -LiteralPath $commandPath).LastWriteTimeUtc.Ticks -eq $ticks -and (Get-FileHash -LiteralPath $commandPath).Hash -eq $hash) 'unchanged native cache command preserves bytes and timestamp'
        & $cmake "-DNTVDM_BINARY_DIR=$tree" -DNTVDM_USE_CCACHE=OFF -P $policy
        Check ($LASTEXITCODE -eq 0 -and [IO.File]::ReadAllText($commandPath) -eq 'cl.exe') 'cache-off removes the optional launcher'
    }finally{
        $env:CCACHE_DIR=$savedDir; $env:CCACHE_TEMPDIR=$savedTemp; $env:CCACHE_MAXSIZE=$savedMax
        if(Test-Path -LiteralPath $tree){
            $inventory=@(& $cleaner -Target $temporaryConfig -Inventory)
            $owner=@{targets=@(@{path=$temporaryConfig;owner='Build hygiene test';reason='test-owned unconfigured native policy output';inventorySha256=$inventory[-1].InventorySha256});reviewedFiles=@()}
            & $cleaner -Target $temporaryConfig -OwnershipManifest $owner -Apply -TasksExited | Out-Null
        }
    }
    Check (!(Test-Path -LiteralPath $tree)) 'unconfigured policy prefix is removed, not hidden in cache'
}
Write-Utf8 (Join-Path $scratch 'result.json') (@{passed=@($passed);count=$passed.Count;utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json -Depth 4)
"PASS: $($passed.Count) build-hygiene assertions"
