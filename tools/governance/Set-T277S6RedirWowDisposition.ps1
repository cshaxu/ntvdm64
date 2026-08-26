[CmdletBinding()]
param([string]$RepositoryRoot = '')
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference='Stop'; $ops=Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations'
$work=@(Import-Csv (Join-Path $ops 'm0-t277-s2-source-form-worklist.tsv') -Delimiter "`t" | Where-Object {$_.caller_package -in @('vdmredir','wow32')})
if($work.Count -ne 17){throw "Expected 17 Redirector/WOW work items; found $($work.Count)."}
$groups=[ordered]@{
  'Redirector-CRT'=@('getenv')
  'Redirector-guest-address-lease'=@('_inlineConvertAddress')
  'Redirector-debug-output'=@('DbgOut')
  'WOW-bounded-machine-run'=@('host_simulate')
  'WOW-debugger-notification'=@('DBGNotifyNewTask','DBGNotifyRemoteThreadAddress','DBGNotifyDebugged')
  'WOW-media-debug'=@('wow32MciDebugOutput','wow32MciSetDebugLevel')
  'WOW-16-to-32-dispatch'=@('W32Dispatch')
  'WOW-serial-endpoint'=@('GetCommShadowMSR','GetCommHandle')
}
$map=@{};foreach($family in $groups.Keys){foreach($symbol in $groups[$family]){if($map.ContainsKey($symbol)){throw "Duplicate group: $symbol"};$map[$symbol]=$family}}
$actual=@($work.symbol|Sort-Object -Unique);$missing=@($actual|?{-not $map.ContainsKey($_)});$extra=@($map.Keys|?{$_ -notin $actual});if($missing.Count -or $extra.Count){throw "Group mismatch Missing=[$($missing -join ',')], Extra=[$($extra -join ',')]"}
function Form($family){switch($family){
 'Redirector-CRT'{'original Redirector CRT getenv callers in vdmredir/vrdebug.c and vrdll.c; standard.h is the declaration carrier'}
 'Redirector-guest-address-lease'{'vdmredir/vrdisp.c:171 defines LPVOID _inlineConvertAddress(WORD,WORD,WORD,BOOLEAN)'}
 'Redirector-debug-output'{'vdmredir/vrdlcdbg.c:583 defines original variadic VOID DbgOut(LPSTR,...)'}
 'WOW-bounded-machine-run'{'wow32/wcall16.c:780 and wow32/wkman.c use host_simulate with saved/restored guest IP; S3 supplies the sole machine boundary'}
 'WOW-debugger-notification'{'wow32/wkman.c:1138/1663 and wow32/wow32.c:1704 issue original debugger notifications with WOW state'}
 'WOW-media-debug'{'wow32/wmmstru1.c:1535 and :1555 define original MCI debug output/level helpers'}
 'WOW-16-to-32-dispatch'{'wow32/wow32.c:953 defines original W32Dispatch over WOW16 thunk tables and guest stack state'}
 'WOW-serial-endpoint'{'wow32/wucomm.c:2224 and :2247 define original COM shadow status and HANDLE retrieval forms'}
 default{throw "Unknown family $family"}}}
function Disposition($family){switch($family){
 'Redirector-CRT'{return @('native CRT binding','original getenv call and caller-side absent-value path','none','preserve original CRT failure/null behavior','no Redirector protocol enablement')}
 'Redirector-guest-address-lease'{return @('adapter-redir -> adapter-softpc -> session guest_memory','original segment:offset:size conversion contract','scoped session guest_memory lease or copied buffer; never return a durable raw PVOID','invalid span/expired lease yields original source-derived error path','no Redirector-private pointer mapper')}
 'Redirector-debug-output'{return @('adapter-redir -> adapter-debugger','original variadic debug-output call shape','formatted copied diagnostic record only','debug output unavailable is non-functional diagnostic omission, never protocol success','no debugger product body link')}
 'WOW-bounded-machine-run'{return @('adapter-softpc -> adapter-bochs','original host_simulate ordering around saved/restored guest IP','S3 typed bounded run only; no raw CPU pointer','return/stop result follows WOW caller path; no CCPU loop','Bochs remains sole executor')}
 'WOW-debugger-notification'{return @('adapter-wow -> adapter-debugger','original debugger notification parameters and ordering','copied fixed-width session diagnostic record; no raw VP/PVOID/HANDLE','debugger absent follows original no-debug path','no NT4 remote-debug product shell')}
 'WOW-media-debug'{return @('adapter-wow -> adapter-debugger','original VOID MCI debug output/level helper forms','copied UTF-8/ANSI diagnostic data only','debug facility absent is explicit no-debug behavior','no multimedia product enablement')}
 'WOW-16-to-32-dispatch'{return @('adapter-wow -> adapter-softpc -> session guest_memory','original W32Dispatch entry and WOW thunk ordering','scoped guest_memory leases and 32-bit compatibility object values only','unavailable thunk returns original WOW failure; no direct host API substitution','no raw pointer/handle or table reconstruction outside original WOW package')}
 'WOW-serial-endpoint'{return @('adapter-wow -> adapter-win32; session host_resource mapping','original BYTE/HANDLE COM shadow/query forms','session host_resource maps native endpoint identity to old 32-bit compatible object values','missing endpoint preserves original invalid/no-port result','no native HANDLE crosses old WOW ABI')}
 default{throw "Unknown family $family"}}}
$rows=foreach($item in $work|Sort-Object caller_package,caller_path,caller_line){$f=$map[$item.symbol];$d=Disposition $f;[pscustomobject]@{work_item_id=$item.work_item_id;caller_package=$item.caller_package;caller_path=$item.caller_path;caller_line=$item.caller_line;symbol=$item.symbol;family=$f;original_provider_form=(Form $f);owner=$d[0];retained_interface_shape=$d[1];mapping_rule=$d[2];failure_disposition=$d[3];implementation_constraint=$d[4];audit_state='source-form-family-reviewed; implementation-not-enabled';evidence=$item.evidence}}
$rows|Export-Csv (Join-Path $ops 'm0-t277-s6-redir-wow-disposition-ledger.tsv') -Delimiter "`t" -NoTypeInformation -Encoding utf8
Write-Host "Wrote $($rows.Count) T277 S6 Redirector/WOW dispositions in $($groups.Count) families."
