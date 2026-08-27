import crypto from 'node:crypto';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';

const repo=process.argv[2]||process.cwd(), ops=path.join(repo,'docs','etc','operations');
const roots=['O:\\repos.external\\OpenNT','O:\\repos.external\\OpenNT-4.5'].filter(fs.existsSync).sort((a,b)=>b.length-a.length);
const parse=(f)=>{const [h,...d]=fs.readFileSync(f,'utf8').replace(/^\uFEFF/,'').split(/\r?\n/).filter(Boolean).map(l=>{const a=[];let v='',q=false;for(let i=0;i<l.length;i+=1){const c=l[i];if(c==='"'){if(q&&l[i+1]==='"'){v+='"';i+=1;}else q=!q;}else if(c==='\t'&&!q){a.push(v);v='';}else v+=c;}a.push(v);return a;});return d.map(r=>Object.fromEntries(h.map((k,i)=>[k,r[i]||''])));};
const quote=(v)=>`"${String(v||'').replaceAll('"','""')}"`;
const write=(f,rows,cols)=>fs.writeFileSync(path.join(ops,f),[cols.join('\t'),...rows.map(r=>cols.map(c=>quote(r[c])).join('\t'))].join('\r\n')+'\r\n');
const hash=(f)=>crypto.createHash('sha256').update(fs.readFileSync(f)).digest('hex');
const includePattern=/^\s*#\s*include\s*[<"]([^>"]+)[>"]/gm;
const headerFiles=[];
for(const root of roots){const out=execFileSync('rg.exe',['--files','--no-messages','-g','*.h','-g','*.inc',root],{encoding:'utf8',maxBuffer:256*1024*1024});for(const file of out.split(/\r?\n/).filter(Boolean)){headerFiles.push({root,file,relative:path.relative(root,file).replaceAll('\\','/')});}}
const byName=new Map();for(const h of headerFiles){const name=path.posix.basename(h.relative).toLowerCase();if(!byName.has(name))byName.set(name,[]);byName.get(name).push(h);}
const mvdmByName=new Map();for(const [name,headers] of byName){const mvdm=headers.filter(h=>/^(base\/mvdm|nt\/private\/mvdm)\//i.test(h.relative));if(mvdm.length)mvdmByName.set(name,mvdm);}
const headerHash=new Map();
const identity=(h)=>{if(!headerHash.has(h.file))headerHash.set(h.file,hash(h.file));return `${h.relative}@${headerHash.get(h.file)}`;};
function packagePrefix(file){const root=roots.find(r=>file.toLowerCase().startsWith(r.toLowerCase()));if(!root)return '';const relative=path.relative(root,file).replaceAll('\\','/').toLowerCase();const match=relative.match(/^(base\/mvdm|nt\/private\/mvdm)\//);return match?match[1]:'';}
function uniqueContent(headers){const hashes=new Map();for(const header of headers){const value=hash(header.file);if(!hashes.has(value))hashes.set(value,[]);hashes.get(value).push(header);}return hashes.size===1?[...hashes.values()][0]:[];}
function resolveInclude(from, name){
  const local=path.resolve(path.dirname(from),name);
  if(fs.existsSync(local))return [{root:roots.find(r=>local.toLowerCase().startsWith(r.toLowerCase()))||'',file:local,relative:(roots.find(r=>local.toLowerCase().startsWith(r.toLowerCase()))?path.relative(roots.find(r=>local.toLowerCase().startsWith(r.toLowerCase())),local):local).replaceAll('\\','/')}];
  const normalized=name.replaceAll('\\','/').toLowerCase();
  // A qualified include is only admitted when its source identity is unique.
  if(normalized.includes('/'))return uniqueContent(headerFiles.filter(h=>h.relative.toLowerCase().endsWith('/'+normalized)||h.relative.toLowerCase()===normalized));
  // For bare names, honor the caller's MVDM package first.  We deliberately
  // do not search unrelated SDK/CRT/product headers by basename.
  const prefix=packagePrefix(from), samePackage=(byName.get(normalized)||[]).filter(h=>prefix&&h.relative.toLowerCase().startsWith(prefix+'/'));
  if(samePackage.length)return uniqueContent(samePackage);
  return uniqueContent(mvdmByName.get(normalized)||[]);
}
const fileLedger=parse(path.join(ops,'mvdm-file-recovery-ledger.tsv')), sourceByPath=new Map(fileLedger.filter(r=>r.target_path&&r.selected_source_path).map(r=>[r.target_path,r.selected_source_path])), dispositionByTargetPath=new Map(fileLedger.filter(r=>r.target_path).map(r=>[r.target_path,r.expected_final_disposition]));
const candidates=parse(path.join(ops,'mvdm-host-first-degree-candidate-ledger.tsv'));
const semanticBy=new Map(parse(path.join(ops,'opennt-non-mvdm-stage1-semantic-symbol-ledger.tsv')).map(r=>[r.symbol,r]));
const candidatesByCaller=new Map();for(const c of candidates){if(!candidatesByCaller.has(c.caller_source_path))candidatesByCaller.set(c.caller_source_path,[]);candidatesByCaller.get(c.caller_source_path).push(c);}
const directByCaller=new Map();
for(const [caller,rows] of candidatesByCaller){const source=sourceByPath.get(caller);if(!source||!fs.existsSync(source))throw new Error(`Missing selected caller source: ${caller}`);const headers=[...new Set([...fs.readFileSync(source,'utf8').matchAll(includePattern)].map(x=>x[1]))];directByCaller.set(caller,{source,headers});}
const resolvedHeadersByCaller=new Map();
const includeClosureCache=new Map();
function includeClosure(seed) {
  const seedKey=[...seed].map(identity).sort().join(';');
  if(includeClosureCache.has(seedKey)) return includeClosureCache.get(seedKey);
  const seen=new Map(), queue=[];
  for(const header of seed){const key=identity(header);if(!seen.has(key)){seen.set(key,header);queue.push(header);}}
  while(queue.length){
    const header=queue.shift();
    const raw=fs.readFileSync(header.file,'utf8');
    for(const match of raw.matchAll(includePattern)) for(const nested of resolveInclude(header.file,match[1])) {
      // Only original-tree headers are followed.  SDK/CRT includes terminate
      // the declaration search at their public binding boundary.
      if(!nested.root) continue;
      const key=identity(nested);
      if(!seen.has(key)){seen.set(key,nested);queue.push(nested);}
    }
  }
  const value=[...seen.values()]; includeClosureCache.set(seedKey,value); return value;
}
for(const [caller,info] of directByCaller){const direct=new Map();for(const name of info.headers)for(const header of resolveInclude(info.source,name))direct.set(identity(header),header);resolvedHeadersByCaller.set(caller,includeClosure([...direct.values()]));}
const terminal=(prior)=>prior==='public-win32-or-crt-binding'||prior==='not-host-runtime-source-only'||prior==='macro-or-function-pointer-contract'||/kernel|native-nt|private-|wow-private|netapi/.test(prior);
const declarationCache=new Map();
function declarationMatches(header,symbol){const k=`${header.file}:${symbol}`;if(declarationCache.has(k))return declarationCache.get(k);const raw=fs.readFileSync(header.file,'utf8');const re=new RegExp(`\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g,'\\$&')}\\s*\\(`,'g');const hits=[...raw.matchAll(re)].map(x=>raw.slice(0,x.index).split('\n').length);declarationCache.set(k,hits);return hits;}
function packageFor(h){const p=h.relative.toLowerCase();if(p.includes('/base/mvdm/')||p.startsWith('base/mvdm/'))return 'base/mvdm';if(p.includes('/nt/private/mvdm/')||p.startsWith('nt/private/mvdm/'))return 'nt/private/mvdm';if(p.includes('/base/win32/client/')||p.startsWith('base/win32/client/'))return 'base/win32/client';if(p.includes('/base/win32/server/')||p.startsWith('base/win32/server/'))return 'base/win32/server';if(p.includes('/base/ntdll/')||p.startsWith('base/ntdll/'))return 'base/ntdll';return ''}
const rows=[];
for(const c of candidates){const info=directByCaller.get(c.caller_source_path), sem=semanticBy.get(c.callee_spelling), callerDisposition=dispositionByTargetPath.get(c.caller_source_path)||'', callerNonRuntime=['not-host-runtime','tool-only','firmware-only'].includes(callerDisposition), isTerminal=terminal(c.prior_semantic_disposition)||callerNonRuntime;const headers=isTerminal?[]:resolvedHeadersByCaller.get(c.caller_source_path);const declarations=[];for(const h of headers)for(const ln of declarationMatches(h,c.callee_spelling))declarations.push(`${h.relative}:${ln}@${headerHash.get(h.file)}`);let status,rootsAllowed,basis;
if(callerNonRuntime){status='terminal-caller-not-host-runtime';rootsAllowed='';basis=`caller file final disposition is ${callerDisposition}; its private outgoing call is source evidence, not a current host-runtime recovery dependency`;}
else if(terminal(c.prior_semantic_disposition)){status=c.prior_semantic_disposition==='public-win32-or-crt-binding'?'terminal-public-api':'terminal-original-boundary';rootsAllowed='';basis=sem?.stage1_basis||'T293 prior source-form disposition';}
else {const packageRoots=[...new Set(headers.map(packageFor).filter(Boolean))];if(!packageRoots.length&&c.caller_source_path.includes('softpc.new/'))packageRoots.push('base/mvdm');if(!packageRoots.length&&c.caller_source_path.startsWith('dos/'))packageRoots.push('base/mvdm');status=declarations.length?'declaration-guided-package-search':'caller-package-limited-search';rootsAllowed=packageRoots.join(';');basis=declarations.length?'matching declaration in caller OpenNT include closure':'no declaration in OpenNT include closure; constrained to caller MVDM package only';}
rows.push({candidate_id:c.candidate_id,caller_definition_id:c.caller_definition_id,caller_source_path:c.caller_source_path,caller_source_sha256:c.caller_source_sha256,caller_source_line:c.caller_source_line,caller_symbol:c.caller_symbol,callee_spelling:c.callee_spelling,prior_semantic_disposition:c.prior_semantic_disposition,caller_expected_final_disposition:callerDisposition,direct_include_spellings:info.headers.join(';'),resolved_direct_headers:headers.map(identity).join(';'),declaration_candidates:declarations.join(';'),frontier_status:status,allowed_package_roots:rootsAllowed,frontier_basis:basis});}
write('mvdm-host-first-degree-include-frontier-ledger.tsv',rows,['candidate_id','caller_definition_id','caller_source_path','caller_source_sha256','caller_source_line','caller_symbol','callee_spelling','prior_semantic_disposition','caller_expected_final_disposition','direct_include_spellings','resolved_direct_headers','declaration_candidates','frontier_status','allowed_package_roots','frontier_basis']);
const count=(a,k)=>[...a.reduce((m,r)=>m.set(r[k],(m.get(r[k])||0)+1),new Map()).entries()].sort(([a],[b])=>a.localeCompare(b));
console.log(`candidate rows: ${rows.length}; caller files: ${directByCaller.size}; direct header paths: ${new Set(rows.flatMap(r=>r.resolved_direct_headers.split(';').filter(Boolean))).size}`);
console.log(`frontier: ${count(rows,'frontier_status').map(([k,v])=>`${k}=${v}`).join('; ')}`);
