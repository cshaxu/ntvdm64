[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$T130Root = '',
    [Parameter(Mandatory = $true)][string]$BuildRoot
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent $PSScriptRoot }
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($T130Root)) { $T130Root = Join-Path $repository 'artifacts\build\current\t130-machine-outcome-r1' }
$t130 = [IO.Path]::GetFullPath($T130Root)
$build = [IO.Path]::GetFullPath($BuildRoot)
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build directory: $build" }
function Hash([string]$path) { if(-not (Test-Path -LiteralPath $path -PathType Leaf)){throw "Missing: $path"}; (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
function Copy-Verified([string]$source,[string]$destination) { $hash=Hash $source; Copy-Item -LiteralPath $source -Destination $destination -Force; if((Hash $destination) -ne $hash){throw "Copy mismatch: $source"}; $hash }

foreach($path in @('main.o','cpu\exception.o','cpu\libcpu.a','iodev\libiodev.a','memory\libmemory.a','gui\libgui.a','ntdos64-t98-current-adapter.mak')){[void](Hash (Join-Path $t130 $path))}
& robocopy $t130 $build /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ /NFL /NDL /NJH /NJS | Out-Null
if($LASTEXITCODE -gt 7){throw "T130 copy failed: $LASTEXITCODE"}

# Retain the original archive separately. The admitted build can compile only
# cpu.o and may produce the replacement archive only from this base plus that
# same-named member.
[void](Copy-Verified (Join-Path $t130 'cpu\libcpu.a') (Join-Path $build 'cpu\libcpu.t130.a'))
$cpuSource=Join-Path $repository 'src\bochs\cpu\cpu.cc'
[void](Copy-Verified $cpuSource (Join-Path $build 'cpu\cpu.cc'))
$defines=@('/DBX_NTVDM_ENABLE_PREFETCH_PREDECESSOR_DIAGNOSTIC=1','/DBX_NTVDM_ENABLE_IRQ13_TRANSFER_DIAGNOSTIC=0','/DBX_NTVDM_ENABLE_IRET_TF_DIAGNOSTIC=0','/DBX_NTVDM_ENABLE_REAL_MODE_FAR_JUMP_DIAGNOSTIC=0','/DBX_NTVDM_ENABLE_STACK_TRANSFER_DIAGNOSTIC=0')
$make=@(
 '# Generated T148 S1: compile only the registered replacement CPU object.',
 '!INCLUDE Makefile','',
 'cpu\cpu.o: cpu\cpu.cc',
 ("`t`$(CXX) /c `$(BX_INCDIRS) `$(CXXFLAGS) " + ($defines -join ' ') + ' /Tpcpu\cpu.cc /Focpu\cpu.o')
)
$memberMake = Join-Path $build 't148-cpu-member-compile.mak'
[IO.File]::WriteAllText($memberMake,($make -join "`r`n"),[Text.UTF8Encoding]::new($false))

$baseShim = Join-Path $t130 'ntdos64-t98-current-adapter.mak'
$shimText = Get-Content -LiteralPath $baseShim -Raw
$shimText = $shimText.Replace('ntdos64-t98-current-adapter.exe', 'ntdos64-t148-single-member.exe')
$shimText = $shimText.Replace('ntdos64-t98-current-adapter.map', 'ntdos64-t148-single-member.map')
$shimText = [regex]::Replace($shimText, '(ntdos64-t148-single-member\.exe:\s+[^\r\n]*machine\\unexp_nt\.c\.obj)', '$1 cpu/libcpu.a')
$shim=Join-Path $build 'ntdos64-t148-single-member.mak'
[IO.File]::WriteAllText($shim,$shimText,[Text.UTF8Encoding]::new($false))
$manifest=[ordered]@{schema='ntdos64.t148.s1.single-member-cpu-archive.v1';mode='generator-only';t130ArchiveSha256=Hash (Join-Path $t130 'cpu\libcpu.a');cpuSourceSha256=Hash $cpuSource;defines=$defines;memberBuild='nmake /f t148-cpu-member-compile.mak cpu/cpu.o; Invoke-T148S1SingleMemberArchivePack.ps1';linkBuild='nmake /f ntdos64-t148-single-member.mak ntdos64-t148-single-member.exe';prohibited=@('full-cpu-archive-build','direct-cpu-object-link','iodev','memory','gui','adapter','cli','runtime')}
[IO.File]::WriteAllText((Join-Path $build 't148-s1-single-member-cpu-archive.json'),($manifest|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
Write-Host "Prepared T148 S1 single-member CPU archive derivative: $build"
