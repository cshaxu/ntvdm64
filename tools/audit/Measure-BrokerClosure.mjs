// Whole-T endpoint accounting. Never equate imported original lines with hacks.
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
const baseline='08b33351b1c341bb433f27ab517ee9a4e0bc3e0c';
const build=path.resolve('build/M0-T412/S10/accounting');
fs.mkdirSync(build,{recursive:true});
function git(args){const r=spawnSync('git',args,{encoding:'utf8',maxBuffer:32*1024*1024});if(r.status>1)throw Error(r.stderr);return r.stdout;}
const rows=git(['diff','--numstat',baseline,'--','src']).trim().split('\n').filter(Boolean).map(line=>{
    const [added,deleted,file]=line.split('\t');return {file,added:Number(added),deleted:Number(deleted)};
});
for(const file of git(['ls-files','--others','--exclude-standard','src']).trim().split('\n').filter(Boolean)){
    const text=fs.readFileSync(file,'utf8');rows.push({file,added:text.split('\n').length-(text.endsWith('\n')?1:0),deleted:0});
}
const groups={};
for(const row of rows){
    if(row.file.endsWith('.md'))continue;
    const root=row.file.split('/')[1];
    const g=groups[root] ||= {files:0,added:0,deleted:0};
    ++g.files;g.added+=row.added;g.deleted+=row.deleted;
}
function distance(a,b){
    fs.writeFileSync(path.join(build,'original.txt'),a.replaceAll('\r\n','\n'));
    fs.writeFileSync(path.join(build,'copy.txt'),b.replaceAll('\r\n','\n'));
    const stat=git(['diff','--no-index','--numstat','--',path.join(build,'original.txt'),path.join(build,'copy.txt')]).trim();
    const [added='0',deleted='0']=stat.split('\t');return {added:Number(added),deleted:Number(deleted)};
}
const paired=[];
for(const row of rows){
    if(!/^src\/(mvdm-host|opennt-host)\//.test(row.file)||row.file.endsWith('.md'))continue;
    const before=spawnSync('git',['show',`${baseline}:${row.file}`],{encoding:'utf8',maxBuffer:16*1024*1024});
    if(before.status!==0)continue; // Newly imported subsets have no baseline pair.
    const relative=row.file.replace(/^src\/mvdm-host\//,'base/mvdm/').replace(/^src\/opennt-host\//,'');
    const original=path.join('O:/repos.external/OpenNT',relative);
    if(!fs.existsSync(original))throw Error(`Missing pinned OpenNT comparison: ${original}`);
    const source=fs.readFileSync(original,'utf8');
    paired.push({file:row.file,original,before:distance(source,before.stdout),after:distance(source,fs.readFileSync(row.file,'utf8'))});
}
const report={baseline,endpoint:git(['rev-parse','HEAD']).trim(),workingChanges:true,
    method:'git diff --numstat baseline vs working tree, plus untracked source. Physical text lines; excludes Markdown in grouped totals. Paired cohort contains only changed mirror paths present at both endpoints, against pinned OpenNT. Cropped original lines are omissions, not autonomous code.',groups,paired,rows};
fs.writeFileSync(process.env.OPENNT_ACCOUNTING_REPORT || 'O:/winnt/logs/m0-t412-s10-accounting.json',JSON.stringify(report,null,2));
console.log(JSON.stringify({groups,paired},null,2));
