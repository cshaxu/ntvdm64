[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $value = $Path.Replace('\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') {
        return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2)
    }
    return $value
}

function Get-OriginalSources([string]$Manifest) {
    $raw = Get-Content -LiteralPath $Manifest -Raw
    $match = [regex]::Match($raw, '(?s)SOURCES\s*=\s*(.*?)(?=\r?\n\s*(?:!INCLUDE|UMTYPE|i386_SOURCES|MIPS_SOURCES|ALPHA_SOURCES))')
    if (!$match.Success) { throw "Cannot isolate original SOURCES block: $Manifest" }
    $names = [regex]::Matches($match.Groups[1].Value, '\b([A-Za-z0-9_]+\.c)\b') |
        ForEach-Object { $_.Groups[1].Value } |
        Select-Object -Unique
    if ($names.Count -eq 0) { throw "No C sources selected by original manifest: $Manifest" }
    return @($names)
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T310/S2/softpc/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}

$ccpuRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/ccpu386'
$hostRoot = Join-Path $root 'src/mvdm-host/softpc.new/host/src'
$adapterWin32Root = Join-Path $root 'src/adapter-mvdm-host-out/win32/source'
$patchRoot = Join-Path $root 'src/mvdm-softpc-patch/x86/prod'
$patchBodyRoot = Join-Path $root 'src/mvdm-softpc-patch/patches/common'
$patchEvidenceRoot = Join-Path $root 'src/mvdm-softpc-patch/patches'
$ccpuManifest = Join-Path $ccpuRoot 'sources'
$hostManifest = Join-Path $hostRoot 'sources'
$ccpuNames = Get-OriginalSources $ccpuManifest
$hostNames = @('nt_cprgs.c', 'nt_cpu.c', 'nt_aorc.c', 'nt_reset.c', 'nt_error.c',
               'nt_msscs.c', 'sim32.c', 'nt_sas.c', 'nt_mem.c', 'nt_umb.c')
$adapterWin32Names = @('dialog_context.c')
$patchNames = @('PigReg_c.h', 'sas4gen.h', 'gdpvar.h')
$patchBodyNames = @('fmstubs.c')
$patchEvidenceNames = @('minnt/callconv.patch')
foreach ($name in $ccpuNames) {
    if (!(Test-Path -LiteralPath (Join-Path $ccpuRoot $name))) { throw "Original CCPU source missing: $name" }
}
foreach ($name in $hostNames) {
    if (!(Test-Path -LiteralPath (Join-Path $hostRoot $name))) { throw "Original SoftPC host root missing: $name" }
}
foreach ($name in $adapterWin32Names) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterWin32Root $name))) { throw "Required Win32 adapter source missing: $name" }
}
foreach ($name in $patchNames) {
    if (!(Test-Path -LiteralPath (Join-Path $patchRoot $name))) { throw "Registered SoftPC patch carrier missing: $name" }
}
foreach ($name in $patchBodyNames) {
    if (!(Test-Path -LiteralPath (Join-Path $patchBodyRoot $name))) { throw "Registered SoftPC patch body missing: $name" }
}
foreach ($name in $patchEvidenceNames) {
    if (!(Test-Path -LiteralPath (Join-Path $patchEvidenceRoot $name))) { throw "Registered SoftPC patch evidence missing: $name" }
}

New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj/ccpu'), (Join-Path $build 'obj/host'), (Join-Path $build 'obj/adapter-win32'), (Join-Path $build 'obj/patch') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$includeRoots = @(
    'src',
    # The source-shaped modern NT facade must precede the byte-exact OpenNT
    # declaration mirror: it selects its reached ntioapi subset before the
    # modern SDK's incompatible winternl declarations can collide with it.
    'src/adapter-mvdm-host-out/win32/include',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/public/ddk/inc',
    'src/mvdm-support/inc',
    # NTVDMx64's original patch script deletes the CCPU-local generated GDP
    # carrier, then supplies this selected x86 product carrier. Keep the
    # mirror source intact and express that historical selection in build
    # include order instead.
    'src/mvdm-softpc-patch/x86/prod',
    'src/mvdm-host/softpc.new/base/ccpu386',
    'src/mvdm-host/softpc.new/host/inc',
    # Original sas.h includes generated sas4gen.h. The selected mirror retains
    # the CVIDC generated carrier; the historical host/genPg output is absent.
    'src/mvdm-host/softpc.new/base/cvidc',
    'src/mvdm-host/softpc.new/base/inc',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/monitor/include',
    'src/session'
) | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }

$baseFlags = '/nologo /TC /c /MT /W4 /showIncludes /DWIN32 /DWINNT /Di386 /DNTVDM /DCPU_30_STYLE /DCPU_40_STYLE /DNEW_CPU /DCCPU /DSPC386 /DANSI /DPROD ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' +
    ($includeRoots -join ' ')

$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('build_root = ' + (NinjaPath $build))
$graph.Add('cflags = ' + $baseFlags)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' lib.exe /nologo /out:$out $in')
$graph.Add('rule forced_link_audit')
# This deliberately produces a non-runnable DLL.  /WHOLEARCHIVE makes the
# candidate's complete original membership visible to LINK; /FORCE keeps the
# unresolved physical forms in the adjacent log for source-first ownership.
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /dll /noentry /force:unresolved /force:multiple /out:$out /implib:$out.lib /wholearchive:original-ccpu386.lib /wholearchive:original-softpc-host-roots.lib /wholearchive:softpc-win32-bindings.lib kernel32.lib user32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib > $out.log 2>&1')

$ccpuObjects = foreach ($name in $ccpuNames) {
    $object = 'obj/ccpu/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $ccpuRoot $name)))
    $object
}
$hostObjects = foreach ($name in $hostNames) {
    $object = 'obj/host/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $hostRoot $name)))
    $object
}
$adapterWin32Objects = foreach ($name in $adapterWin32Names) {
    $object = 'obj/adapter-win32/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterWin32Root $name)))
    $object
}
$patchBodyObjects = @(foreach ($name in $patchBodyNames) {
    $object = 'obj/patch/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $patchBodyRoot $name)))
    $object
})
$graph.Add('build original-ccpu386.lib: lib ' + ($ccpuObjects -join ' '))
$graph.Add('build original-softpc-host-roots.lib: lib ' + ($hostObjects -join ' '))
$graph.Add('build softpc-win32-bindings.lib: lib ' + ($adapterWin32Objects -join ' '))
$graph.Add('build ntvdmx64-softpc-patch-evidence.lib: lib ' + ($patchBodyObjects -join ' '))
$graph.Add('build original-softpc-candidate: phony original-ccpu386.lib original-softpc-host-roots.lib softpc-win32-bindings.lib ntvdmx64-softpc-patch-evidence.lib')
$graph.Add('build original-softpc-forced-closure.dll: forced_link_audit original-ccpu386.lib original-softpc-host-roots.lib softpc-win32-bindings.lib')
$graph.Add('default original-softpc-candidate')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))

[ordered]@{
    schema = 'm0.t310.s2.original-softpc-candidate.v1'
    architecture = $Architecture
    toolchain = 'MSVC /MT via VsDevCmd'
    originalCcpuManifest = 'src/mvdm-host/softpc.new/base/ccpu386/sources'
    originalHostManifest = 'src/mvdm-host/softpc.new/host/src/sources'
    ccpuSourceCount = @($ccpuNames).Count
    ccpuSources = @($ccpuNames)
    hostRoots = @($hostNames)
    adapterWin32Sources = @($adapterWin32Names)
    patchInputs = @($patchNames | ForEach-Object {
        [ordered]@{
            path = 'src/mvdm-softpc-patch/x86/prod/' + $_
            sha256 = (Get-FileHash -LiteralPath (Join-Path $patchRoot $_) -Algorithm SHA256).Hash.ToLowerInvariant()
        }
    })
    patchBodies = @($patchBodyNames | ForEach-Object {
        [ordered]@{
            path = 'src/mvdm-softpc-patch/patches/common/' + $_
            sha256 = (Get-FileHash -LiteralPath (Join-Path $patchBodyRoot $_) -Algorithm SHA256).Hash.ToLowerInvariant()
            buildDisposition = 'compile-and-archive-debugbreak-evidence-only'
        }
    })
    patchEvidence = @($patchEvidenceNames | ForEach-Object {
        [ordered]@{
            path = 'src/mvdm-softpc-patch/patches/' + $_
            sha256 = (Get-FileHash -LiteralPath (Join-Path $patchEvidenceRoot $_) -Algorithm SHA256).Hash.ToLowerInvariant()
        }
    })
    forbiddenInputs = @('src.old', 'bochs-core', 'adapter-bochs')
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8

Write-Host "Generated T310 S2 original SoftPC candidate graph: $build (CCPU sources: $(@($ccpuNames).Count))"
