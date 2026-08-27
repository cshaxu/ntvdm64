import crypto from 'node:crypto';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

const root = process.argv[2] || process.cwd();
const ops = path.join(root, 'docs', 'etc', 'operations');
const roots = ['O:\\repos.external\\OpenNT', 'O:\\repos.external\\OpenNT-4.5'].filter(fs.existsSync).sort((a,b)=>b.length-a.length);
const keywords = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'typedef', 'defined', '__asm', 'asm', 'catch', '__try']);
const parse = (file) => { const [h, ...d] = fs.readFileSync(file, 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean).map((line) => { const a=[]; let x='',q=false; for(let i=0;i<line.length;i+=1){const c=line[i];if(c==='"'){if(q&&line[i+1]==='"'){x+='"';i+=1;}else q=!q;}else if(c==='\t'&&!q){a.push(x);x='';}else x+=c;}a.push(x);return a; }); return d.map((v) => Object.fromEntries(h.map((k,i)=>[k,v[i]||'']))); };
const quote = (v) => `"${String(v||'').replaceAll('"','""')}"`;
const write = (name, rows, cols) => { const text=[cols.join('\t'),...rows.map(r=>cols.map(c=>quote(r[c])).join('\t'))].join('\r\n')+'\r\n'; const out=path.join(ops,name),tmp=`${out}.tmp`;fs.writeFileSync(tmp,text);fs.renameSync(tmp,out); };
const hash = (file) => crypto.createHash('sha256').update(fs.readFileSync(file)).digest('hex');
function mask(input) { const a=[...input];let s='c',e=false; for(let i=0;i<a.length;i+=1){const c=a[i],n=a[i+1]||'';if(s==='c'){if(c==='/'&&n==='/'){a[i]=a[i+1]=' ';i+=1;s='l';}else if(c==='/'&&n==='*'){a[i]=a[i+1]=' ';i+=1;s='b';}else if(c==='"'){a[i]=' ';s='s';e=false;}else if(c==="'"){a[i]=' ';s='q';e=false;}}else if(s==='l'){if(c==='\n')s='c';else if(c!=='\r')a[i]=' ';}else if(s==='b'){if(c==='*'&&n==='/'){a[i]=a[i+1]=' ';i+=1;s='c';}else if(c!=='\r'&&c!=='\n')a[i]=' ';}else {if(c==='\n')s='c';else {if(c!=='\r')a[i]=' ';if(e)e=false;else if(c==='\\')e=true;else if((s==='s'&&c==='"')||(s==='q'&&c==="'"))s='c';}}} return a.join('').split(/(?<=\n)/).map(l=>/^\s*#/.test(l)?l.replace(/[^\r\n]/g,' '):l).join(''); }
const line = (t,i) => t.slice(0,i).split('\n').length;
function close(t,i,l,r){let d=0;for(;i<t.length;i+=1){if(t[i]===l)d+=1;else if(t[i]===r&&--d===0)return i;}return -1;}
const next = (t,i) => {for(;i<t.length;i+=1)if(!/\s/.test(t[i]))return t[i];return '';};
const rel = (file) => { const base=roots.find(r=>file.toLowerCase().startsWith(r.toLowerCase())); return {root:base, relative:path.relative(base,file).replaceAll('\\','/')}; };
const pkg = (p) => { const s=p.split('/'); if(s[0]==='base'&&s[1]==='win32') return s.slice(0,3).join('/'); if(s[0]==='base'&&s[1]==='ntdll') return 'base/ntdll'; if(s[0]==='base'&&s[1]==='mvdm') return 'base/mvdm'; if(s[0]==='nt'&&s[1]==='private'&&s[2]==='mvdm') return 'nt/private/mvdm'; return s.slice(0,Math.max(1,Math.min(2,s.length-1))).join('/'); };
function filesFor(symbols, sourceRoot) { const list=path.join(os.tmpdir(),`t294-${process.pid}.regex`);fs.writeFileSync(list,symbols.map(s=>`\\b${s.replace(/[.*+?^${}()|[\]\\]/g,'\\$&')}\\s*\\(`).join('\n'));try{return execFileSync('rg.exe',['-l','-P','--no-messages','-f',list,'-g','*.c','-g','*.cc','-g','*.cpp','-g','*.cxx',sourceRoot],{encoding:'utf8',maxBuffer:128*1024*1024}).split(/\r?\n/).filter(Boolean);}catch(error){if(error.status===1)return[];throw error;}finally{fs.unlinkSync(list);} }
const candidates=parse(path.join(ops,'mvdm-host-first-degree-candidate-ledger.tsv'));
const frontier=parse(path.join(ops,'mvdm-host-first-degree-include-frontier-ledger.tsv'));
const frontierById=new Map(frontier.map(r=>[r.candidate_id,r]));
const semantic=parse(path.join(ops,'opennt-non-mvdm-stage1-semantic-symbol-ledger.tsv'));
const semanticBy=new Map(semantic.map(r=>[r.symbol,r]));
const zero=parse(path.join(ops,'mvdm-host-zero-degree-definition-ledger.tsv'));
const zeroSymbols=new Set(zero.map(r=>r.symbol));
const searchable=candidates.filter(c=>['declaration-guided-package-search','caller-package-limited-search'].includes(frontierById.get(c.candidate_id)?.frontier_status));
const symbols=[...new Set(searchable.map(r=>r.callee_spelling))].sort();
const pattern=new RegExp(`\\b(?:${symbols.sort((a,b)=>b.length-a.length).map(s=>s.replace(/[.*+?^${}()|[\]\\]/g,'\\$&')).join('|')})\\s*\\(`,'g');
const definitions=[]; const macroNames=new Set(); const scannedFiles=new Set();
const symbolsByPackage=new Map();
for(const candidate of searchable) for(const packageRoot of (frontierById.get(candidate.candidate_id).allowed_package_roots||'').split(';').filter(Boolean)){if(!symbolsByPackage.has(packageRoot))symbolsByPackage.set(packageRoot,new Set());symbolsByPackage.get(packageRoot).add(candidate.callee_spelling);}
for(const sourceRoot of roots) for(const [packageRoot,packageSymbols] of symbolsByPackage){const directory=path.join(sourceRoot,packageRoot);if(!fs.existsSync(directory))continue;for(const file of filesFor([...packageSymbols],directory)){if(scannedFiles.has(file))continue;scannedFiles.add(file);const raw=fs.readFileSync(file,'utf8'),m=mask(raw),info=rel(file);for(const x of raw.matchAll(/^\s*#\s*define\s+([A-Za-z_]\w*)\b/gm))macroNames.add(x[1]);pattern.lastIndex=0;for(const x of m.matchAll(pattern)){const symbol=x[0].match(/[A-Za-z_]\w*/)?.[0]||'';if(macroNames.has(symbol)||/^(IFN|IPT|IPF)\d*$/.test(symbol))continue;const open=m.indexOf('(',x.index+x[0].length-1),end=close(m,open,'(',')');if(end<0)continue;
  // OpenNT still contains K&R definitions: `fn(arg) TYPE arg; { ... }`.
  // Admit the first following body brace only when no statement terminator
  // intervenes; this preserves ordinary prototypes/calls as non-definitions.
  const tail=m.slice(end+1,end+1+2048), braceAt=tail.indexOf('{'), semiAt=tail.indexOf(';'), between=braceAt<0?'':tail.slice(0,braceAt);
  const linePrefix=m.slice(m.lastIndexOf('\n',x.index)+1,x.index);
  const trimmedPrefix=linePrefix.trim();
  // A call used as a condition can also be followed by a body brace.  A
  // definition prefix is either empty (implicit historic C return type) or a
  // declaration-like token sequence; expression punctuation is disqualifying.
  if(trimmedPrefix && /[()!<>=,.;+\-\/]/.test(trimmedPrefix))continue;
  const knrPrefix=/\b[A-Za-z_]\w*\s*$/.test(linePrefix)&&!/(?:\breturn|\bif|\bfor|\bwhile|\bswitch)\s*$/i.test(linePrefix);
  if(braceAt<0||(semiAt>=0&&semiAt<braceAt&&(!knrPrefix||/^\s*;/.test(between))))continue;
  const bodyOpen=end+1+braceAt,bodyEnd=close(m,bodyOpen,'{','}');if(bodyEnd<0)continue;const start=raw.lastIndexOf('\n',x.index)+1;definitions.push({symbol,source_root:info.root,source_path:info.relative,source_sha256:hash(file),source_line:String(line(m,x.index)),package_root:pkg(info.relative),signature_evidence:raw.slice(start,bodyOpen).replace(/\s+/g,' ').trim().slice(0,512),body:m.slice(bodyOpen+1,bodyEnd),body_offset:bodyOpen+1,masked:m});}}}
const canonical=new Map();
for(const d of definitions){
  // OpenNT keeps byte-identical source aliases under e.g. base/ and nt/private/.
  // Their identical function hash/line/signature is one source identity with
  // explicit aliases, not two invented product variants. Different bytes stay
  // separate, even when the spelling and path are the same.
  const key=`${d.symbol}:${d.source_sha256}:${d.source_line}:${d.signature_evidence}`;
  if(!canonical.has(key))canonical.set(key,{...d,provenance_roots:new Set(),source_aliases:new Set(),package_aliases:new Set()});
  const item=canonical.get(key);item.provenance_roots.add(d.source_root);item.source_aliases.add(d.source_path);item.package_aliases.add(d.package_root);
}
const defs=[...canonical.values()].map(d=>{d.source_aliases=[...d.source_aliases].sort().join(';');d.package_aliases=[...d.package_aliases].sort().join(';');d.source_path=d.source_aliases.split(';')[0];d.package_root=d.package_aliases.split(';')[0];return d;}).sort((a,b)=>a.symbol.localeCompare(b.symbol)||a.source_path.localeCompare(b.source_path)||Number(a.source_line)-Number(b.source_line));
defs.forEach((d,i)=>d.definition_id=`MVDM-FIRST-DEFINITION-${String(i+1).padStart(6,'0')}`);
const defsBy=new Map();for(const d of defs){if(!defsBy.has(d.symbol))defsBy.set(d.symbol,[]);defsBy.get(d.symbol).push(d);}
function definitionsForCandidate(candidate) {
  const frontierRow = frontierById.get(candidate.candidate_id);
  const allowed = new Set((frontierRow?.allowed_package_roots || '').split(';').filter(Boolean));
  if (!allowed.size) return [];
  return (defsBy.get(candidate.callee_spelling) || []).filter((definition) =>
    definition.package_aliases.split(';').some((packageRoot) => allowed.has(packageRoot))
  );
}
const directModernWin32=new Set(['GetShortPathNameA','GetShellWindow','GetTickCount']);
const monitorContracts=new Set(['ExitVDM','GetNextVDMCommand','VdmAllocateVirtualMemory','VdmFreeVirtualMemory','VdmCommitVirtualMemory','VdmDeCommitVirtualMemory','VdmRemoveVirtualMemory','VdmAddVirtualMemory','VdmInstallFaultHandler','VdmInstallHardwareIntHandler','VdmInstallSoftwareIntHandler','GetFastBopEntryAddress','SetVDMCurrentDirectories']);
const softpcContracts=new Set(['call_ica_hw_interrupt']);
const wowContracts=new Set(['FastWOWCallbackCall','SetWOWforceIncrAlloc','W32SetExceptionContext','WK32ICallProc32MakeCall','RegisterWowExec','RemoveFontResourceTracking','UnloadNetworkFonts','ShowStartGlass','SetRelAbs','GetRelAbs','GetHFONT','GdiQueryTable','GdiCleanCacheDC','AddFontResourceTracking','GetFreeSystemResources']);
const redirContracts=new Set(['GetLanmanSessionKey','AsyncDispositionRoutine']);
const vddContracts=new Set(['GetFaxDataMapName','VDDRetrieveNtHandle','VDDAssociateNtHandle','VDDAllocateDosHandle']);
function adapterFallback(candidate) {
  const symbol=candidate.callee_spelling, caller=candidate.caller_source_path;
  if(directModernWin32.has(symbol))return {outcome:'public-modern-win32-binding',owner:'adapter-mvdm-host-out/win32',disposition:'binding-only',basis:'public Win32 declaration/form retained; modern public Win32 binding is the intended non-invasive provider'};
  if(monitorContracts.has(symbol))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/monitor',disposition:'adapter-backed',basis:'historical MVDM monitor/control interface; retain original spelling and ABI while the monitor adapter supplies the modern non-kernel boundary'};
  if(wowContracts.has(symbol))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/wow',disposition:'adapter-backed',basis:'historical WOW lifecycle/callback/display interface; retain the caller contract and recover it only through the named WOW adapter'};
  if(redirContracts.has(symbol))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/redir',disposition:'adapter-backed',basis:'historical Redirector/async completion interface; retain the caller contract and recover it only through the named Redirector adapter'};
  if(vddContracts.has(symbol))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/vdd',disposition:'adapter-backed',basis:'historical VDD/FAX host interface; retain the caller contract and recover it only through the named VDD adapter'};
  if(softpcContracts.has(symbol))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/softpc',disposition:'adapter-backed',basis:'historical SoftPC interrupt/controller interface; retain its original calling shape and translate only at the named SoftPC adapter'};
  if(symbol.startsWith('host_'))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/softpc',disposition:'adapter-backed',basis:'historical SoftPC host interface; retain its original calling shape and translate only at the named SoftPC adapter'};
  if(caller.startsWith('softpc.new/host/'))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/softpc',disposition:'adapter-backed',basis:'historical SoftPC host-control interface; retain its original calling shape and translate only at the named SoftPC adapter'};
  if(caller.startsWith('dos/'))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/monitor',disposition:'adapter-backed',basis:'MVDM DOS caller reaches a host command/control interface; retain its original calling shape through the named monitor adapter'};
  if(caller.startsWith('xms.486/'))return {outcome:'same-shaped-adapter-required',owner:'adapter-mvdm-host-out/softpc + session mapping manager',disposition:'adapter-backed',basis:'XMS host-memory interface; preserve the 32-bit original contract and resolve it through the session mapping manager, never a host pointer'};
  return null;
}
function originalOwner(definitions) {
  const packages=new Set(definitions.flatMap((definition)=>definition.package_aliases.split(';')));
  if([...packages].every((packageRoot)=>packageRoot==='base/mvdm'||packageRoot==='nt/private/mvdm')) return 'mvdm-host (original MVDM mirror)';
  return 'admitted OpenNT package mirror pending component placement';
}
const second=[];const callNotes=[];
for(const d of defs){const calls=[];let opaque=false;for(const x of d.body.matchAll(/\b([A-Za-z_]\w*)\s*\(/g)){const symbol=x[1];if(keywords.has(symbol.toLowerCase()))continue;const at=d.body_offset+x.index,ln=line(d.masked,at);if(macroNames.has(symbol)){opaque=true;calls.push(`${symbol}@${ln}:macro`);callNotes.push({definition_id:d.definition_id,source_path:d.source_path,source_line:String(ln),callee_spelling:symbol,kind:'macro-expression',basis:'macro spelling; no function body expanded'});continue;}if(zeroSymbols.has(symbol)){calls.push(`${symbol}@${ln}:zero-back-edge`);continue;}if(defsBy.has(symbol)){calls.push(`${symbol}@${ln}:first-degree-internal`);continue;}const id=`MVDM-SECOND-CANDIDATE-${String(second.length+1).padStart(6,'0')}`;second.push({candidate_id:id,caller_definition_id:d.definition_id,caller_source_path:d.source_path,caller_source_sha256:d.source_sha256,caller_source_line:String(ln),caller_symbol:d.symbol,callee_spelling:symbol,call_form:'direct-named-call',boundary:'T294 records direct outgoing expression only; a later degree may inspect the callee body'});calls.push(`${symbol}@${ln}:second:${id}`);}d.leaf_status=calls.length===0?'leaf':'non-leaf';d.leaf_basis=calls.length===0?'no-direct-function-call':opaque?'macro-expression-present':'one-or-more-direct-call-expressions';d.direct_call_summary=calls.join(';');d.provenance_roots=[...d.provenance_roots].sort().join(';');}
const resolution=[];
for(const c of candidates){const sem=semanticBy.get(c.callee_spelling), prior=c.prior_semantic_disposition, frontierRow=frontierById.get(c.candidate_id), found=definitionsForCandidate(c);let outcome,owner,disposition,basis;
if(prior==='public-win32-or-crt-binding'){outcome='public-modern-win32-crt-leaf';owner='adapter-mvdm-host-out/win32';disposition='binding-only';basis=sem?.upstream_declaration_candidates||'prior public API/CRT classification; declaration evidence retained';}
else if(prior==='macro-or-function-pointer-contract'){outcome='macro-or-function-pointer-contract';owner='original calling owner';disposition='hard-boundary';basis='T293 source-form classification; no invented callable provider';}
else if(frontierRow?.frontier_status==='terminal-caller-not-host-runtime'){outcome='caller-not-host-runtime-source-only';owner='original package retained as source/contract evidence';disposition=frontierRow.caller_expected_final_disposition;basis=frontierRow.frontier_basis;}
else if(frontierRow?.frontier_status==='terminal-original-boundary'){outcome='declaration-only-hard-boundary';owner=prior==='kernel-vdm-monitor-contract'?'adapter-mvdm-host-out/monitor + adapter-bochs':prior==='wow-private-host-contract'?'adapter-mvdm-host-out/wow':prior==='netapi-redirector-private-contract'?'adapter-mvdm-host-out/redir':'external historical OpenNT/NT product shell (not a current host-runtime component)';disposition=prior==='not-host-runtime-source-only'?'not-host-runtime':'hard-boundary';basis=sem?.stage1_basis||'T293 source-form classification; deliberately not searched past a private, kernel, native-NT, or source-only boundary';}
else if(found.length){outcome=found.length===1?'exact-original-definition':'conditional-original-definition-variants';owner=prior==='kernel-vdm-monitor-contract'?'adapter-mvdm-host-out/monitor + adapter-bochs':prior==='wow-private-host-contract'?'adapter-mvdm-host-out/wow':prior==='netapi-redirector-private-contract'?'adapter-mvdm-host-out/redir':originalOwner(found);disposition=prior==='not-host-runtime-source-only'?'not-host-runtime':'adapter-backed';basis='exact C/C++ definition candidate(s) parsed from approved original OpenNT source; no runtime selection is made';}
else if(adapterFallback(c)){({outcome,owner,disposition,basis}=adapterFallback(c));}
else if(/kernel|native-nt|private-|wow-private|netapi/.test(prior)){outcome='declaration-only-hard-boundary';owner=prior==='kernel-vdm-monitor-contract'?'adapter-mvdm-host-out/monitor + adapter-bochs':'named adapter owner pending consolidation';disposition='hard-boundary';basis=sem?.stage1_basis||'private historical contract has no selected C/C++ body in approved roots';}
else {outcome='explicit-source-unavailable-or-declaration-only';owner='first-frontier package consolidation';disposition=prior==='not-host-runtime-source-only'?'not-host-runtime':'unresolved';basis='no exact C/C++ definition found in approved original trees; declaration/source boundary retained explicitly';}
resolution.push({candidate_id:c.candidate_id,caller_definition_id:c.caller_definition_id,caller_source_path:c.caller_source_path,caller_source_sha256:c.caller_source_sha256,caller_source_line:c.caller_source_line,caller_symbol:c.caller_symbol,callee_spelling:c.callee_spelling,prior_semantic_disposition:prior,resolution_outcome:outcome,definition_ids:found.map(d=>d.definition_id).join(';'),definition_source_identities:found.map(d=>`${d.source_aliases}:${d.source_line}@${d.source_sha256}`).join(';'),final_disposition:disposition,final_owner:owner,resolution_basis:basis});}
if(resolution.length!==candidates.length || new Set(resolution.map(r=>r.candidate_id)).size!==candidates.length) throw new Error('T294 result cardinality does not match the frozen first-degree input');
for(const row of resolution){
  const frontierRow=frontierById.get(row.candidate_id);
  if(!frontierRow) throw new Error(`Missing include frontier for ${row.candidate_id}`);
  if(row.definition_ids && !['declaration-guided-package-search','caller-package-limited-search'].includes(frontierRow.frontier_status)) throw new Error(`Terminal boundary unexpectedly received a source definition: ${row.candidate_id}`);
}
const sourceRows=[...new Map(defs.map(d=>[`${d.source_path}@${d.source_sha256}`,{source_path:d.source_path,source_sha256:d.source_sha256,package_root:d.package_root,provenance_roots:d.provenance_roots,definition_count:defs.filter(x=>x.source_path===d.source_path&&x.source_sha256===d.source_sha256).length}])).values()];
write('mvdm-host-first-degree-resolution-ledger.tsv',resolution,['candidate_id','caller_definition_id','caller_source_path','caller_source_sha256','caller_source_line','caller_symbol','callee_spelling','prior_semantic_disposition','resolution_outcome','definition_ids','definition_source_identities','final_disposition','final_owner','resolution_basis']);
write('mvdm-host-first-degree-definition-ledger.tsv',defs,['definition_id','symbol','source_path','source_aliases','source_sha256','source_line','package_root','package_aliases','provenance_roots','signature_evidence','leaf_status','leaf_basis','direct_call_summary']);
write('mvdm-host-first-degree-source-coverage-ledger.tsv',sourceRows,['source_path','source_sha256','package_root','provenance_roots','definition_count']);
write('mvdm-host-second-degree-candidate-ledger.tsv',second,['candidate_id','caller_definition_id','caller_source_path','caller_source_sha256','caller_source_line','caller_symbol','callee_spelling','call_form','boundary']);
write('mvdm-host-first-degree-call-resolution-ledger.tsv',callNotes,['definition_id','source_path','source_line','callee_spelling','kind','basis']);
const count=(a,k)=>[...a.reduce((m,r)=>m.set(r[k],(m.get(r[k])||0)+1),new Map()).entries()].sort(([a],[b])=>a.localeCompare(b));
console.log(`input candidates: ${candidates.length}; symbols: ${symbols.length}; definition identities: ${defs.length}; source files: ${sourceRows.length}; second-degree exits: ${second.length}`);
console.log(`outcomes: ${count(resolution,'resolution_outcome').map(([k,v])=>`${k}=${v}`).join('; ')}`);
console.log(`dispositions: ${count(resolution,'final_disposition').map(([k,v])=>`${k}=${v}`).join('; ')}`);
