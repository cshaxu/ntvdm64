const fs=require('node:fs'),path=require('node:path'),cp=require('node:child_process');
const root=path.resolve(__dirname,'../..'),out=path.resolve(root,process.argv[2]);
const rel=path.relative(path.join(root,'build'),out);if(!rel||rel.startsWith('..')||path.isAbsolute(rel))throw Error('Expected a build subdirectory');
const source=fs.readFileSync(process.argv[3]||path.join(root,'src/mvdm-host/softpc.new/base/ccpu386/ccpusas4.c'),'utf8');
const names=['bios_read_word','bios_read_double'];let bodies=names.map(n=>{const m=[...source.matchAll(new RegExp('^LOCAL IU(?:16|32)\\s*\\n'+n+' IFN1\\(LIN_ADDR, linAddr\\)\\s*\\{[\\s\\S]*?^\\}','gm'))];if(m.length!==1)throw Error('Ambiguous source '+n);return m[0][0];});
fs.mkdirSync(out,{recursive:true});fs.writeFileSync(path.join(out,'original_bios_width.h'),bodies.join('\n'));
const exe=path.join(out,'bios-width.exe');const build=cp.spawnSync('cl.exe',['/nologo','/MT','/std:c11','/W3','/I'+out,'/Fe:'+exe,'/Fo:'+path.join(out,'bios-width.obj'),path.join(__dirname,'ccpu_bios_width_test.c')],{cwd:out,encoding:'utf8'});fs.writeFileSync(path.join(out,'build.log'),(build.stdout||'')+(build.stderr||''));if(build.error||build.status!==0)throw Error(build.error||build.stdout||build.stderr);
const run=cp.spawnSync(exe,[],{cwd:out,encoding:'utf8'});fs.writeFileSync(path.join(out,'test.log'),(run.stdout||'')+(run.stderr||''));process.stdout.write(run.stdout||'');if(run.error||run.status!==0){process.stderr.write(run.stderr||'');process.exit(1);}
