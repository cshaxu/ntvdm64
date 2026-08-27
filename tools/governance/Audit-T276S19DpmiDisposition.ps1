[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$root=(Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations=Join-Path $root 'docs/etc/operations'
$input=Join-Path $operations 'mvdm-file-recovery-ledger.tsv'
$files=@(Import-Csv $input -Delimiter "`t" | Where-Object {$_.target_path -like 'dpmi/*'} | Sort-Object target_path)
if($files.Count -ne 59){throw "Expected 59 DPMI paths, found $($files.Count)."}
$selected486=@('dpmi/486/dxboot.asm','dpmi/486/dxfunc.asm','dpmi/486/dxint31.asm','dpmi/486/dxintr.asm','dpmi/486/dxmain.asm','dpmi/486/dxstrt.asm','dpmi/486/dxutil.asm')
$selectedShared=@('dpmi/dxmmgr.asm','dpmi/dxend.asm','dpmi/dxmsg.asm','dpmi/dxbug.asm','dpmi/dxnetbio.asm','dpmi/dxdisk.asm','dpmi/dxini.asm','dpmi/dxoem.asm','dpmi/dxemm.asm','dpmi/dxendpm.asm','dpmi/dxfind.asm','dpmi/dxdma.asm','dpmi/dxemm2.asm','dpmi/ntnpxem.asm','dpmi/dxrom.asm')
$rows=foreach($f in $files){$p=$f.target_path;$role='';$boundary='';$summary='';$disp='';
 if($p -in @('dpmi/486/dosx.exe','dpmi/486/dosx.map','dpmi/486/dosx.sym')){$role='historical-dosx-guest-product-artifact';$boundary='mvdm-guest/dos artifact provenance';$summary='original DOSX guest product/debug artifact; load-only guest evidence, never host link input';$disp='guest-artifact-only'}
 elseif($p -in $selected486 -or $p -in $selectedShared){$role='historical-dosx-guest-provider-body';$boundary='mvdm-guest/dos; adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/monitor; adapter-bochs; session mapping manager';$summary='selected by original DOSX 486 object list; 16-bit DPMI provider/interrupt-reflector guest body, retained as a whole guest-product recovery unit';$disp='guest-provider package prerequisite; no host composition'}
 elseif($p -match '^dpmi/(dxboot|dxfunc|dxint31|dxintr|dxmain|dxstrt|dxutil|dxhpbios|dxvcpi|dxvcpibt|ntintr32)\.asm$'){$role='historical-dosx-alternate-or-gated-guest-body';$boundary='mvdm-guest/dos; adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/monitor; adapter-bochs';$summary='original alternate, optional or non-selected DOSX guest body; preserve source and build-gate evidence without host composition';$disp='guest-profile-gated source evidence'}
 elseif($p -match '^dpmi/.*\.(inc|mas)$'){$role='historical-dosx-guest-declaration-or-macro';$boundary='mvdm-guest/dos; adapter-mvdm-host-in; adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/monitor; adapter-bochs';$summary='original DOSX guest interface/macro/layout evidence; retain with the guest package and do not translate it into a host ABI';$disp='guest-declaration prerequisite'}
 else {$role='historical-dosx-build-or-localization-input';$boundary='tools/opennt; mvdm-guest/dos artifact provenance';$summary='original DOSX build/link/localization control input; evidence only until historical guest build is admitted';$disp='build-or-artifact-evidence-only'}
 [pscustomobject][ordered]@{file_id=$f.file_id;source_path=$p;file_kind=$f.file_kind;original_role_group=$role;provisional_component_owner='mvdm-guest/dos';required_boundary_owner=$boundary;lexical_external_families='DPMI protected-mode/LDT/exception/memory; BIOS/interrupt reflector; BOP/WOW/SoftPC forms';original_contract_summary=$summary;provisional_disposition=$disp;state='provisional-not-enabled';evidence="$p; dpmi/makefile; dpmi/dosxi.lnk"}}
$rows|ConvertTo-Csv -NoTypeInformation -Delimiter "`t"|Set-Content (Join-Path $operations 'm0-t276-s19-dpmi-file-disposition-ledger.tsv') -Encoding utf8
Write-Output "T276 S19 DPMI disposition generated: $($rows.Count) rows."
