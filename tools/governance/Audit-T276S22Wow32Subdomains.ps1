[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$wow = Join-Path $root 'src/mvdm-host/wow32'
$ops = Join-Path $root 'docs/etc/operations'

$groups = [ordered]@{
    'thunk-frame-and-structure' = @('mapembed.c','wow32.c','wowtbl.c','walias.c','wstruc.c','wcall16.c','wcall32.c','wres16.c','wres32.c','walloc16.c','wheap.c','wparam.c')
    'message-and-window-procedure' = @('wcntl32.c','wdde.c','wmsg16.c','wmdisp32.c','wmsgbm.c','wmsgcb.c','wmsgem.c','wmsglb.c','wmsgsbm.c','wmtbl32.c')
    'gdi-and-window-object' = @('wgdi31.c','wgman.c','wgdi.c','wgfont.c','wgmeta.c','wgpal.c','wgprnset.c','wgtext.c','wreldc.c','wcuricon.c','wsubcls.c','wdib.c','waccel.c')
    'kernel-task-and-dos-session' = @('wkbman.c','wkmem.c','wkgthunk.c','wkman.c','wkfileio.c','wkernel.c','wowhooks.c','wdos.c','wthman.c')
    'user-interface' = @('wuser31.c','wuman.c','wucaret.c','wuclass.c','wuclip.c','wucomm.c','wucursor.c','wudlg.c','wuhook.c','wulang.c','wumenu.c','wumsg.c','wuser.c','wutext.c','wutmr.c','wuwind.c','wusercli.c')
    'shell-ole-and-printing' = @('wow32fax.c','wshell.c','wcommdlg.c','wspool.c','wole2.c')
    'multimedia' = @('wsman.c','wmmedia.c','wmmedia1.c','wmmedia2.c','wmmstruc.c','wmmstru1.c','wmmstru2.c','wmmalias.c')
    'network' = @('wsdata.c','wsext.c','wsraw.c')
}

$active = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
$disabled = $false
foreach ($line in Get-Content (Join-Path $wow 'sources')) {
    if ($line -match '^!IF\s+0') { $disabled = $true; continue }
    if ($line -match '^!ENDIF') { $disabled = $false; continue }
    if (!$disabled -and $line -match '^\s*([\w]+\.c)\s*\\?') { [void]$active.Add($matches[1]) }
}
if ($active.Count -ne 77) { throw "Expected 77 active WOW32 C bodies, found $($active.Count)." }

$claimed = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
$result = foreach ($group in $groups.Keys) {
    foreach ($source in $groups[$group]) {
        if (!$active.Contains($source)) { throw "Subdomain manifest names non-active source '$source'." }
        if (!$claimed.Add($source)) { throw "Subdomain manifest assigns '$source' more than once." }
        $boundary = switch ($group) {
            'thunk-frame-and-structure' { 'adapter-wow; adapter-softpc; adapter-vdm-monitor; session' }
            'message-and-window-procedure' { 'adapter-wow; adapter-mvdm-host-out/win32; adapter-softpc; adapter-vdm-monitor; session' }
            'gdi-and-window-object' { 'adapter-wow; adapter-mvdm-host-out/win32; adapter-softpc; adapter-vdm-monitor; session' }
            'kernel-task-and-dos-session' { 'adapter-wow; adapter-mvdm-host-out/win32; adapter-softpc; adapter-vdm-monitor; session' }
            'user-interface' { 'adapter-wow; adapter-mvdm-host-out/win32; adapter-softpc; adapter-vdm-monitor; session' }
            'shell-ole-and-printing' { 'adapter-wow; adapter-mvdm-host-out/win32; adapter-softpc; adapter-vdm-monitor; session' }
            'multimedia' { 'adapter-wow; adapter-mvdm-host-out/win32; adapter-softpc; adapter-vdm-monitor; session' }
            'network' { 'adapter-wow; adapter-mvdm-host-out/win32; session' }
        }
        $mapping = switch ($group) {
            'network' { 'host-resource mapping for native socket/provider identity; copied guest buffers only' }
            'shell-ole-and-printing' { 'guest-memory lease for converted structures plus host-resource mapping for native identity' }
            default { 'guest-memory lease for VDM frame/guest structure access plus host-resource mapping for native identity' }
        }
        [pscustomobject][ordered]@{
            source_path = "wow32/$source"
            original_subdomain = $group
            original_selection = 'active original wow32.dll source'
            boundary_owners = $boundary
            mapping_requirement = $mapping
            provisional_disposition = 'whole WOW owner package prerequisite; no body enabled'
            source_evidence = "wow32/sources; wow32/$source source header and includes"
        }
    }
}
if ($claimed.Count -ne $active.Count) {
    $missing = @($active | Where-Object { !$claimed.Contains($_) }) -join ', '
    throw "Subdomain manifest omits active sources: $missing"
}

$result | Sort-Object source_path | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" |
    Set-Content (Join-Path $ops 'm0-t276-s22-wow32-active-subdomain-ledger.tsv') -Encoding utf8
Write-Output "T276 S22 WOW32 active subdomain ledger generated: $($result.Count) rows across $($groups.Count) subdomains."
