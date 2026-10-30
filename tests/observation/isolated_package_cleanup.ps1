# Test-only ownership boundary. Never use this for publication or O:\winnt.
# Caller creates the scope before starting any case; exact isolated paths are
# the only cleanup authority, not image names or normal idle deadlines.
$script:IsolatedPackageCleanupRoot=$PSScriptRoot
function Get-IsolatedFileSha256([string]$LiteralPath) {
    $path=(Resolve-Path -LiteralPath $LiteralPath -ErrorAction Stop).Path
    $algorithm=[Security.Cryptography.SHA256]::Create()
    $stream=[IO.File]::OpenRead($path)
    try {return ([BitConverter]::ToString($algorithm.ComputeHash($stream))).Replace('-','')}
    finally {$stream.Dispose();$algorithm.Dispose()}
}
function Save-IsolatedPackageManifest([string]$Runtime, [string]$Destination) {
    $manifest = Join-Path $Runtime 'product-system32-manifest.json'
    $rows = Get-Content -LiteralPath $manifest -Raw | ConvertFrom-Json
    $rows = @($rows)
    $expected = @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe',
        'ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll')
    if (!$rows.Count -or $rows.Count -gt $expected.Count -or
            @($rows | Select-Object -ExpandProperty Name -Unique).Count -ne $rows.Count) {
        throw 'Expected unique staged component records'
    }
    foreach ($row in $rows) {
        if ($row.Name -notin $expected -or $row.RelativePath -ne ('system32\' + $row.Name) -or
                [IO.Path]::GetFileName($row.Name) -ne $row.Name) {
            throw 'Invalid staged component path'
        }
        if ((Get-IsolatedFileSha256 (Join-Path $Runtime $row.RelativePath)) -ne $row.Sha256) {
            throw "Staged component hash mismatch: $($row.Name)"
        }
    }
    Copy-Item -LiteralPath $manifest -Destination $Destination
}
function Get-IsolatedPackageIdentity([string]$Image) {
    if(-not ('NtvdmTestPackageIdentity' -as [type])) {
        Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class NtvdmTestPackageIdentity {
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode, SetLastError=true)] static extern IntPtr LoadLibraryExW(string p, IntPtr f, uint flags);
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode)] static extern IntPtr FindResourceW(IntPtr m, string n, IntPtr t);
    [DllImport("kernel32.dll")] static extern uint SizeofResource(IntPtr m, IntPtr r);
    [DllImport("kernel32.dll")] static extern IntPtr LoadResource(IntPtr m, IntPtr r);
    [DllImport("kernel32.dll")] static extern IntPtr LockResource(IntPtr r);
    [DllImport("kernel32.dll")] static extern bool FreeLibrary(IntPtr m);
    public static string Read(string p) {
        IntPtr m=LoadLibraryExW(p, IntPtr.Zero, 0x22);
        if(m==IntPtr.Zero)return null;
        try {
            IntPtr r=FindResourceW(m,"NTVDM_PACKAGE_ID",(IntPtr)10);
            if(r==IntPtr.Zero || SizeofResource(m,r)!=16)return null;
            IntPtr value=LockResource(LoadResource(m,r));
            if(value==IntPtr.Zero)return null;
            byte[] bytes=new byte[16]; Marshal.Copy(value,bytes,0,16);
            return BitConverter.ToString(bytes).Replace("-","");
        } finally { FreeLibrary(m); }
    }
}
'@
    }
    return [NtvdmTestPackageIdentity]::Read($Image)
}
function Get-PhysicalRuntimeStageRoot {
    return 'O:\tmp'
}
function Test-PhysicalRuntimeStage([string]$Path) {
    if (!(Test-Path -LiteralPath $Path -PathType Container)) { return $false }
    $root = (Resolve-Path -LiteralPath $Path).Path
    $stageRoot = (Get-PhysicalRuntimeStageRoot).TrimEnd('\') + '\'
    if (!$root.StartsWith($stageRoot, [StringComparison]::OrdinalIgnoreCase)) { return $false }
    return Test-Path -LiteralPath (Join-Path $root '.ntvdm64-test-stage.json') -PathType Leaf
}
function New-PhysicalRuntimeStage([string]$PackageRoot,
    [ValidateSet('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')]
    [string[]]$RuntimeImages=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe'),
    [string[]]$TopLevelNames=@(),
    [switch]$FreshPackageIdentity) {
    if(!$RuntimeImages.Count){throw 'At least one runtime image is required'}
    $source = (Resolve-Path -LiteralPath $PackageRoot).Path
    $build = (Resolve-Path "${script:IsolatedPackageCleanupRoot}/../../build").Path + '\'
    if (!$source.StartsWith($build, [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Physical test staging requires a repository-build package'
    }
    $stageRoot = Get-PhysicalRuntimeStageRoot
    if (!(Test-Path -LiteralPath $stageRoot -PathType Container)) {
        $null = New-Item -ItemType Directory -Path $stageRoot
    }
    # Some immutable 16-bit WOW fixtures fail before worker registration at
    # ordinary build-style physical paths.  Keep the stage real (never a
    # drive alias), but deliberately short.  The marker remains the cleanup
    # authority, so the compact leaf does not widen deletion scope.
    $stage = $null
    foreach($attempt in 1..8) {
        # Keep the complete physical root within the proven legacy-guest
        # budget (`O:\tmp\n123` is ten characters), not merely its 8.3 leaf.
        # This is test containment only; it never aliases or changes a user
        # installation path.
        $candidate = Join-Path $stageRoot ('n' + [Guid]::NewGuid().ToString('N').Substring(0,3))
        if(Test-Path -LiteralPath $candidate) { continue }
        # Creation, not the preceding observation, establishes ownership.
        # A concurrent test may claim the same three-hex leaf between those
        # operations; retain the compact-path constraint and try another leaf
        # rather than treating another task's stage as ours.
        try {
            $null = New-Item -ItemType Directory -Path $candidate -ErrorAction Stop
            $stage = $candidate
            break
        } catch [System.IO.IOException] {
            continue
        } catch [System.UnauthorizedAccessException] {
            continue
        }
    }
    if(!$stage) { throw 'Unable to allocate a short physical test stage' }
    try {
        # Copy the complete product package, but never historical build/log
        # evidence that happens to share the package root.  The product sees
        # its real physical path; no global drive alias is manufactured.
        $content = Get-ChildItem -LiteralPath $source -Force | Where-Object {
            $_.Name -notin @('builds','logs','logs2')
        }
        if($TopLevelNames.Count) {
            foreach($name in $TopLevelNames) {
                if([IO.Path]::GetFileName($name) -ne $name -or $name -in @('builds','logs','logs2')) {
                    throw 'Runtime stage top-level content must be a direct non-log package item.'
                }
            }
            $content=@($content | Where-Object { $_.Name -in $TopLevelNames })
            if($content.Count -ne @($TopLevelNames | Select-Object -Unique).Count) {
                throw 'Requested runtime stage content is absent from the package.'
            }
        }
        $content | ForEach-Object {
            # This helper's sole successful pipeline result is the stage
            # root below.  Do not leak Copy-Item results into the caller's
            # `$runtime` assignment, which would turn it into an array and
            # break the ownership check before any product case starts.
            $null = Copy-Item -LiteralPath $_.FullName -Destination $stage -Recurse -Force
        }
        if($FreshPackageIdentity) {
            # Copying a package deliberately preserves its UUID. Concurrent
            # tests that require a different broker must opt in to creating a
            # complete new test package, without touching the source package.
            $manifest=Join-Path $stage 'product-system32-manifest.json'
            $rows=Get-Content -LiteralPath $manifest -Raw | ConvertFrom-Json
            $rows=@($rows)
            $expected=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe',
                'ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll')
            if($rows.Count -ne $expected.Count -or
               @($rows | Select-Object -ExpandProperty Name -Unique).Count -ne $expected.Count) {
                throw 'Fresh identity requires the complete ten-image manifest'
            }
            $binary=Get-PackageBinaryRoot $stage
            $identity=[guid]::NewGuid()
            foreach($name in $expected) {
                $row=@($rows | Where-Object {$_.Name -eq $name})
                if($row.Count -ne 1 -or $row[0].RelativePath -ne ('system32\'+$name)) {
                    throw "Invalid package manifest entry: $name"
                }
                $path=Join-Path $binary $name
                if((Get-IsolatedFileSha256 $path) -ne $row[0].Sha256) {
                    throw "Package input differs from manifest: $name"
                }
                & "$script:IsolatedPackageCleanupRoot/../../tools/build/Set-PackageIdentity.ps1" -Image $path -PackageId $identity
                $row[0].PackageId=$identity.ToString()
                $row[0].Sha256=Get-IsolatedFileSha256 $path
            }
            $rows | ConvertTo-Json | Set-Content -LiteralPath $manifest -Encoding UTF8
        }
        $marker = [ordered]@{
            schema = 1
            source = $source
            createdUtc = [DateTime]::UtcNow.ToString('o')
            # Focused GUI/WOW tests can intentionally omit the text frontend.
            # This marker proves cleanup ownership, not full-package acceptance.
            binaries = $RuntimeImages |
                ForEach-Object {
                    $path = Join-Path (Get-PackageBinaryRoot $stage) $_
                    [ordered]@{ name = $_; sha256 = (Get-IsolatedFileSha256 $path) }
                }
        }
        $marker | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $stage '.ntvdm64-test-stage.json') -Encoding UTF8
        return (Resolve-Path -LiteralPath $stage).Path
    } catch {
        if (Test-Path -LiteralPath $stage) { Remove-Item -LiteralPath $stage -Recurse -Force }
        throw
    }
}
function Remove-PhysicalRuntimeStage([string]$Stage) {
    if (!(Test-PhysicalRuntimeStage $Stage)) { throw 'Refuse to remove a non-stage path' }
    $root = (Resolve-Path -LiteralPath $Stage).Path
    $marker = Get-Content -LiteralPath (Join-Path $root '.ntvdm64-test-stage.json') -Raw | ConvertFrom-Json
    foreach ($row in @($marker.binaries)) {
        $path = Join-Path (Get-PackageBinaryRoot $root) $row.name
        if (!(Test-Path -LiteralPath $path) -or
            (Get-IsolatedFileSha256 $path) -ne $row.sha256) {
            throw 'Test stage binary identity changed; preserve it for diagnosis'
        }
    }
    # A surviving broker keeps its image open; reject before recursive
    # removal so an active stage is not left partially deleted.
    $prefix=$root.TrimEnd('\')+'\'
    $active=@(Get-CimInstance Win32_Process | Where-Object {
        $_.ExecutablePath -and $_.ExecutablePath.StartsWith($prefix,[StringComparison]::OrdinalIgnoreCase)
    })
    if($active.Count){throw 'Test stage still has running processes; stop the owned scope before removal'}
    Remove-Item -LiteralPath $root -Recurse -Force
}
function Get-PackageBinaryRoot([string]$PackageRoot) {
    # Test-only compatibility for sealed flat baselines and build-link caches.
    # Production has one layout: <root>\system32. Never mix the two layouts.
    $system=Join-Path $PackageRoot 'system32'
    if(Test-Path -LiteralPath (Join-Path $system 'run16.exe')){
        foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')){
            if(Test-Path -LiteralPath (Join-Path $PackageRoot $name)){
                throw 'Mixed product binary layout'
            }
        }
        return $system
    }
    return $PackageRoot
}
function Assert-NoConflictingPackageBroker([string]$BrokerImage) {
    # A different verified UUID has a different endpoint. Unknown or equal
    # identity remains a conflict; never adopt or stop another test's broker.
    $ownIdentity=Get-IsolatedPackageIdentity $BrokerImage
    foreach($brokerRow in @(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'")){
        $brokerProcess=$null
        try {
            try {$brokerProcess=[Diagnostics.Process]::GetProcessById($brokerRow.ProcessId)}catch [ArgumentException]{continue}
            $null=$brokerProcess.Handle
            if(!$brokerProcess.HasExited){
                $otherIdentity=if($brokerRow.ExecutablePath) {Get-IsolatedPackageIdentity $brokerRow.ExecutablePath}else{$null}
                if(!$ownIdentity -or !$otherIdentity -or $ownIdentity -eq $otherIdentity){
                    throw "Existing live broker $($brokerRow.ProcessId) $($brokerRow.ExecutablePath); no test ownership"
                }
            }
        }finally{if($brokerProcess){$brokerProcess.Dispose()}}
    }
}
function New-IsolatedPackageScope([string]$PackageRoot,[string]$LaunchRoot='') {
    $physical=(Resolve-Path $PackageRoot).Path
    $build=(Resolve-Path "${script:IsolatedPackageCleanupRoot}/../../build").Path+'\'
    if(!$physical.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -and
       !(Test-PhysicalRuntimeStage $physical)){
        throw 'Test cleanup requires a repository-build package'
    }
    $binary=Get-PackageBinaryRoot $physical
    Assert-NoConflictingPackageBroker (Join-Path $binary 'ntsrv.exe')
    $paths=@()
    $launchBinary=if($LaunchRoot){Get-PackageBinaryRoot $LaunchRoot}else{''}
    foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe')){
        $file=Join-Path $binary $name
        $paths+=$file
        if($LaunchRoot){
            $alias=Join-Path $launchBinary $name
            if((Get-IsolatedFileSha256 $alias) -ne (Get-IsolatedFileSha256 $file)){throw 'Test alias identity mismatch'}
            $paths+=$alias
        }
    }
    $prior=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths})
    if($prior.Count){throw 'Existing package user; no test ownership'}
    [pscustomobject]@{Paths=$paths;Root=$physical}
}
function Stop-IsolatedPackageScope($Scope) {
    if(!$Scope -or !$Scope.Paths){throw 'Missing isolated test ownership scope'}
    $owned=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $Scope.Paths})
    Stop-IdentityCheckedProcesses $owned $Scope.Paths
    if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $Scope.Paths}).Count){
        throw 'Owned package resource remains after explicit cleanup'
    }
}
# The caller supplies only rows already selected by its ownership boundary.
# Pin every process before killing any; validate image AND creation identity.
function Stop-IdentityCheckedProcesses($Rows,[string[]]$Paths) {
    $processes=@()
    try {
        foreach($row in $Rows){
            if($row.ExecutablePath -notin $Paths){throw 'Unowned cleanup image'}
            try {$process=[Diagnostics.Process]::GetProcessById($row.ProcessId)}catch{continue}
            try {
                $null=$process.Handle
                if($process.HasExited){$process.Dispose();continue}
                if($process.MainModule.FileName -ne $row.ExecutablePath -or
                    $process.MainModule.FileName -notin $Paths -or
                    !$row.CreationDate -or
                    [Math]::Abs(($process.StartTime.ToUniversalTime()-$row.CreationDate.ToUniversalTime()).Ticks) -gt 10){
                    throw 'Cleanup PID no longer identifies the owned package image'
                }
            }catch{$process.Dispose();throw}
            $processes+=$process
        }
        foreach($process in $processes){if(!$process.HasExited){
            try {$process.Kill()}
            catch [ComponentModel.Win32Exception] {
                # Stopping an owned broker can concurrently end its workers.
                # Accept only a signaled pinned process handle, never an
                # access-denied error by itself or a disappeared WMI row.
                if(!$process.WaitForExit(1000)){throw}
            }
        }}
        foreach($process in $processes){
            if(!$process.WaitForExit(5000)){throw 'Owned test process did not stop'}
        }
    }finally{foreach($process in $processes){$process.Dispose()}}
}
