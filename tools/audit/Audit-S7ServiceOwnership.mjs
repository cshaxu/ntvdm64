import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';

// Bounded S7 inventory, not an automatic provenance classifier. Original
// ownership still requires block review, including adaptations inside mirrors.
const destination=process.argv[2];
const build=path.resolve('build')+path.sep;
if(!destination || !path.resolve(destination).toLowerCase().startsWith(build.toLowerCase()))
  throw Error('Supply a fresh JSON report path below repository build/');
if(fs.existsSync(destination))throw Error('Do not overwrite retained evidence');
const hash=value=>crypto.createHash('sha256').update(value).digest('hex');
const normalize=value=>value.toString('utf8').replaceAll('\r\n','\n');
function source(file) {
  const bytes=fs.readFileSync(file),raw=normalize(bytes);
  const blank=s=>s.replace(/[^\n]/g,' ');
  const clean=raw.replace(/\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'/g,blank)
    .replace(/^\s*#[^\n]*/gm,blank);
  const functions=[];let depth=0,start=0,open=0,name='';
  for(let i=0;i<clean.length;i++) {
    if(clean[i]==='{') {
      if(!depth){name=clean.slice(start,i).match(/\b([A-Za-z_]\w*)\s*\([^{};]*\)\s*$/)?.[1]??'';open=i;}
      depth++;
    } else if(clean[i]==='}') {
      if(--depth<0)throw Error('Unbalanced source '+file);
      if(!depth){
        if(name)functions.push({name,line:raw.slice(0,open).split('\n').length,
          end:raw.slice(0,i).split('\n').length,bodySha256:hash(raw.slice(open,i+1)),
          calls:[...new Set([...clean.slice(open,i+1).matchAll(/\b([A-Za-z_]\w*)\s*\(/g)].map(m=>m[1]))]});
        start=i+1;name='';
      }
    } else if(clean[i]===';' && !depth)start=i+1;
  }
  if(depth)throw Error('Unbalanced source '+file);
  return {path:file,bytes:bytes.length,sha256:hash(bytes),normalizedSha256:hash(raw),functions};
}
const originals=['base/win32/server/srvvdm.c','base/win32/server/srvinit.c',
  'base/win32/client/vdm.c'].map(relative=>source('O:/repos.external/OpenNT/'+relative));
const current=source('src/ntsrv-exe/opennt/source/base_service.c');
const symbols=new Set(current.functions.map(f=>f.name));
for(const f of current.functions) {
  f.originalBodyMatches=originals.flatMap(s=>s.functions.filter(o=>o.bodySha256===f.bodySha256)
    .map(o=>({path:s.path,name:o.name,line:o.line,end:o.end})));
  f.originalNamedCalls=originals.flatMap(s=>s.functions.filter(o=>f.calls.includes(o.name))
    .map(o=>({path:s.path,name:o.name,line:o.line})));
  f.internalCalls=f.calls.filter(name=>symbols.has(name));
  f.review='pending manual function/block attribution; matches are evidence, not classification';
}
const comparisons=originals.map(original=>{
  const relative=original.path.replace('O:/repos.external/OpenNT/','');
  const mirror='src/opennt-host/'+relative;
  if(!fs.existsSync(mirror))return {original:original.path,mirror,selectedMirrorPresent:false};
  const selected=source(mirror);
  return {original:original.path,originalSha256:original.sha256,mirror,
    mirrorSha256:selected.sha256,byteEqual:selected.sha256===original.sha256,
    normalizedEqual:selected.normalizedSha256===original.normalizedSha256,
    originalBytes:original.bytes,mirrorBytes:selected.bytes,
    scope:'Only these three comparison inputs; not the complete selected-mirror sweep'};
});
const protocolRoot='src/common/protocol';
const protocols=fs.readdirSync(protocolRoot).sort().filter(name=>/\.(h|idl|acf)$/.test(name))
  .map(name=>{const file=protocolRoot+'/'+name,bytes=fs.readFileSync(file);
    return {path:file,bytes:bytes.length,sha256:hash(bytes)};});
fs.writeFileSync(destination,JSON.stringify({scope:'T424 S7 service/protocol inventory',
  limitation:'Lexical functions, not preprocessor-selected bodies or provenance proof; manual ledger and full mirror sweep required',
  current,originals,comparisons,protocols},null,2)+'\n');
console.log(`PASS inventory: ${current.functions.length} current functions, ${protocols.length} protocol inputs; manual attribution remains open`);
