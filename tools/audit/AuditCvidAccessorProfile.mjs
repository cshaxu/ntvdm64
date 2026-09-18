// Read-only product audit; generated research fixtures/results stay in build/.
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {spawnSync} from 'node:child_process';
const root=process.cwd(), upstream='O:/repos.external/OpenNT/base/mvdm/softpc.new';
const outputIndex=process.argv.indexOf('--output');
const out=path.resolve(outputIndex<0?'build/M0-T413/S1/accessor-audit':process.argv[outputIndex+1]);
const graphIndex=process.argv.indexOf('--build-graph');
const graphPath=graphIndex < 0 ? null : path.resolve(process.argv[graphIndex+1]);
fs.mkdirSync(out,{recursive:true});
const read=p=>fs.readFileSync(p,'utf8').replace(/\r\n/g,'\n');
if(process.argv.includes('--collect')){
 const files=fs.readdirSync(out).filter(n=>n.endsWith('.i'));
 if(files.length!==9)throw Error('Expected nine preprocessed short-vector consumers');
 const result=files.map(file=>({file,uses:read(path.join(out,file)).split('\n').filter(l=>!l.startsWith('#line')&&/\b(?:C_Video|Video)\s*\./.test(l))}));
 fs.writeFileSync(path.join(out,'short-consumers.json'),JSON.stringify(result,null,2));
 console.log(JSON.stringify(result,null,2));process.exit(0);
}
const mirror='src/mvdm/softpc.new';
const hash=p=>crypto.createHash('sha256').update(fs.readFileSync(p)).digest('hex');
function git(args){const r=spawnSync(process.env.AUDIT_GIT||'git.exe',args,{encoding:'utf8',maxBuffer:32*1024*1024});if(r.error)throw r.error;if(r.status>1)throw Error(r.stderr);return r.stdout;}
const census={};
for(const group of ['ccpu386','cvidc']){
 const args=['-c','core.autocrlf=true','diff','--no-index','--ignore-cr-at-eol'];
 const a=`${upstream}/base/${group}`,b=`${mirror}/base/${group}`;
 const num=git([...args,'--numstat','--',a,b]);
 const rows=num.trim().split('\n').filter(Boolean).map(l=>{const [add,del,file]=l.split('\t');return {file:file.split('/').pop(),add:+add,del:+del};});
 census[group]={rows,add:rows.reduce((s,x)=>s+x.add,0),del:rows.reduce((s,x)=>s+x.del,0)};
 fs.writeFileSync(path.join(out,`${group}.diff`),git([...args,'--',a,b]));
}
const source=`${mirror}/base/cpu/src/evid/vglob.c`,text=read(source);
const pairs=[...text.matchAll(/setVideo(\w+) IFN1\(([^,]+), value\)\s*\{\s*GLOBAL_VGAGlobals\.(\w+) = value;/g)].map(m=>({name:m[1],type:m[2].trim(),field:m[3]}));
if(pairs.length!==38)throw Error(`Expected 38 pairs, got ${pairs.length}`);
const generated=['sinit011.c','sinit012.c','sinit013.c'].map(n=>read(`${mirror}/base/cvidc/${n}`)).join('\n');
for(const p of pairs){
 const re=new RegExp('case\\s+S_\\d+_CiSetVideo'+p.name+'_\\w+_id\\s*:[\\s\\S]*?(?=case\\s|$)');
 const body=generated.match(re)?.[0];
 const offset=body?.match(/&\(r20\)\)\s*=\s*\(IS32\)\((\d+)\)/);
 if(!offset)throw Error(`No generated offset for ${p.name}`);
 p.offset=+offset[1];
}
const overlays=[];
const diagnosticRestore=['c_xfer.c','call.c','intx.c','iret.c','jmp.c','ret.c','sti.c'];
const report={baseline:git(['rev-parse','HEAD']).trim(),upstream,census,pairs,overlays,
 diagnosticRestore:census.ccpu386.rows.filter(x=>diagnosticRestore.includes(x.file)),
 originalAccessor:{source,lines:text.trimEnd().split('\n').length,sha256:hash(source),upstreamSha256:hash(`${upstream}/base/cpu/src/evid/vglob.c`),ccpuSha256:hash(`${mirror}/base/ccpu386/vglob.c`)}};
fs.writeFileSync(path.join(out,'audit.json'),JSON.stringify(report,null,2));
const regs=Array.from({length:32},(_,i)=>'r'+i);
let fixture=`#include "insignia.h"\n#include "host_def.h"\n#include "Evid_c.h"\n#include "gdpvar.h"\n#include <stdio.h>\n#include <string.h>\n#include <stddef.h>\nIHP Gdp;\nIUH ${regs.join(',')};\nstatic unsigned char arena[65536],expected[65536];\n#define CHECK(x) do{if(!(x)){printf("FAIL %d\\n",__LINE__);return 1;}}while(0)\n`;
for(const p of pairs)fixture+=`extern ${p.type} getVideo${p.name}(void);\nextern void setVideo${p.name}(${p.type});\n`;
fixture+='int main(void){ Gdp=(IHP)arena;\n';
fixture+=regs.map((r,i)=>`${r}=${i+100};`).join('')+'\n';
for(const p of pairs){
 fixture+=`{ ${p.type} value=(${p.type})0x5a; memset(arena,0xa5,sizeof(arena));memcpy(expected,arena,sizeof(arena));\n`;
 fixture+=`CHECK((unsigned char*)&GLOBAL_VGAGlobals.${p.field}-arena==${p.offset});\nmemcpy(expected+${p.offset},&value,sizeof(value));\nsetVideo${p.name}(value);CHECK(getVideo${p.name}()==value);CHECK(!memcmp(arena,expected,sizeof(arena)));}\n`;
}
fixture+=regs.map((r,i)=>`CHECK(${r}==${i+100});`).join('')+'\nprintf("PASS 38 original accessor pairs: offsets, values, isolated field writes, scratch preservation\\n");return 0;}\n';
fs.writeFileSync(path.join(out,'original-accessor-fixture.c'),fixture);
// This audit must compile against the caller's current formal x86 graph. A
// historical graph would silently reintroduce stale include roots and turn a
// source-layout change into a false C-VID failure.
if (!graphPath || !fs.existsSync(graphPath)) {
 throw Error('Require --build-graph <current-formal-x86-build.ninja>');
}
const graph=read(graphPath);
const flags=graph.match(/^cvidc_rule_cflags = (.+)$/m)[1].replaceAll('$:',':').replace('/showIncludes','');
const q=s=>'"'+path.resolve(s)+'"';
const cmd=['@echo off','call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul','if errorlevel 1 exit /b %errorlevel%',
 `cl.exe ${flags} /Fo${q(out+'/vglob.obj')} ${q(source)}`,'if errorlevel 1 exit /b %errorlevel%',
 `cl.exe ${flags} /Fo${q(out+'/fixture.obj')} ${q(out+'/original-accessor-fixture.c')}`,'if errorlevel 1 exit /b %errorlevel%',
 `link.exe /nologo /out:${q(out+'/original-accessor-fixture.exe')} ${q(out+'/vglob.obj')} ${q(out+'/fixture.obj')} kernel32.lib libcmt.lib libvcruntime.lib libucrt.lib legacy_stdio_definitions.lib`,'if errorlevel 1 exit /b %errorlevel%',q(out+'/original-accessor-fixture.exe')];
fs.writeFileSync(path.join(out,'build-fixture.cmd'),cmd.join('\r\n')+'\r\n');
const shortConsumers=['base/keymouse/mouse_io.c','base/debug/trace.c','base/ccpu386/sascdef.c',...['nt_ega','nt_vga','nt_graph','nt_mouse','nt_fulsc','nt_det'].map(n=>'host/src/'+n+'.c')];
const pre=cmd.slice(0,3);
for(const file of shortConsumers){
 const key=file.startsWith('host/')?'host_cflags':'cflags';
 const f=graph.match(new RegExp('^'+key+' = (.+)$','m'))[1].replaceAll('$:',':').replace('/showIncludes','').replace(' /c ',' /P ');
 pre.push(`cl.exe ${f} /Fi${q(out+'/'+path.basename(file)+'.i')} ${q(mirror+'/'+file)}`,'if errorlevel 1 exit /b %errorlevel%');
}
fs.writeFileSync(path.join(out,'preprocess-consumers.cmd'),pre.join('\r\n')+'\r\n');
console.log(JSON.stringify({census,overlays,accessors:pairs.length,original:report.originalAccessor},null,2));
