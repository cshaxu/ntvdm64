[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$NasmExecutable = 'nasm.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar,
        [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) {
    throw "Refusing to overwrite existing build root: $build"
}

$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
$source = Join-Path $repository 'tests\observation'
$outputs = [ordered]@{
    'VDMPRDR.COM' = 'vdmredir_pipe_open_read.asm'
    'VDMPTX.COM'  = 'vdmredir_pipe_transact.asm'
    'VDMPCAL.COM' = 'vdmredir_pipe_call.asm'
    'VDMPTMO.COM' = 'vdmredir_pipe_timeout.asm'
    'VDMMAIL.COM' = 'vdmredir_mailslot.asm'
    'VDMNETAP.COM'= 'vdmredir_netapi.asm'
    'VDMNEI.COM'  = 'vdmredir_net_enum_info.asm'
    'VDMUSE.COM'  = 'vdmredir_use_enum.asm'
    'VDMUSI.COM'  = 'vdmredir_use_info.asm'
    'VDMUSL.COM'  = 'vdmredir_use_lifecycle.asm'
    'VDMWKSTA.COM'= 'vdmredir_wksta.asm'
    'VDMMSG.COM'  = 'vdmredir_message.asm'
    'VDMSVC.COM'  = 'vdmredir_service.asm'
    'VDMWKSS.COM' = 'vdmredir_wksta_set.asm'
    'VDMASGN.COM' = 'vdmredir_assign_macro.asm'
    'VDMTRM1.COM' = 'vdmredir_terminate_owner.asm'
    'VDMTRM2.COM' = 'vdmredir_terminate_check.asm'
    'VDMNETB.COM' = 'vdmredir_netbios.asm'
    'VDMNETA.COM' = 'vdmredir_netbios_async.asm'
    'VDMDLC.COM'  = 'vdmredir_dlc_probe.asm'
    'VDMPASY.COM' = 'vdmredir_pipe_async.asm'
    'VDMPASW.COM' = 'vdmredir_pipe_async_write.asm'
}

New-Item -ItemType Directory -Path $build | Out-Null
foreach ($entry in $outputs.GetEnumerator()) {
    $input = Join-Path $source $entry.Value
    $output = Join-Path $build $entry.Key
    if (!(Test-Path -LiteralPath $input -PathType Leaf)) {
        throw "Required DOS probe source missing: $input"
    }
    & $nasm.Source -f bin -o $output $input
    if ($LASTEXITCODE -ne 0) {
        throw "NASM failed for $($entry.Value): $LASTEXITCODE"
    }
}

[ordered]@{
    schema = 'm0.t420.s20.vdmredir-guest-tests.v4'
    outputs = @($outputs.Keys | ForEach-Object {
        $file = Join-Path $build $_
        [ordered]@{ file = $_; sha256 = (Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant() }
    })
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8

Write-Host "VDMREDIR DOS guest tests completed: $build"
