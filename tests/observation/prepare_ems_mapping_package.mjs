// Isolated test package and original-format PIF; never edits installed media.
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
const run = path.resolve(process.argv[2] || '');
const build = path.resolve('build') + path.sep;
if (!run.startsWith(build)) throw new Error('run root must be under build');
const root = path.join(run, 'ems-package-v2');
if (fs.existsSync(root)) throw new Error('refuse to overwrite package');
fs.mkdirSync(root, {recursive:true});
const manifest = [];
const hash = data => crypto.createHash('sha256').update(data).digest('hex');
function copy(source, relative) {
  const dest = path.join(root, relative);
  fs.mkdirSync(path.dirname(dest), {recursive:true});
  fs.copyFileSync(source, dest);
  manifest.push({source, relative, sha256:hash(fs.readFileSync(dest))});
}
copy(path.join(run,'original-softpc-process.exe'), 'ntvdm32.exe');
copy(path.join(run,'emsprobe.com'), 'EMSPROBE.COM');
for (const name of ['COMMAND.COM','NTIO.SYS','NTDOS.SYS','MEM.EXE'])
  copy(path.join('O:/ntvdm64',name),name);
for (const entry of fs.readdirSync('O:/ntvdm64/system32',{withFileTypes:true}))
  if (entry.isFile()) copy(path.join('O:/ntvdm64/system32',entry.name),path.join('system32',entry.name));
for (const name of ['config.nt','autoexec.nt'])
  copy(path.join('O:/ntvdm64/profiles/pure-dos',name),path.join('profiles/pure-dos',name));
// The original PIF reader may ignore startup-file overrides for an inherited
// console. Retain the package's default startup pair as well.
for (const name of ['config.nt','autoexec.nt'])
  copy(path.join('O:/ntvdm64',name),name);
const source = 'O:/ntvdm64/profiles/pure-dos/pure-dos.pif';
const pif = fs.readFileSync(source);
let found386=false, foundNt=false, at=369;
for(let count=0;at!==0xffff && count<16;count++) {
  if(at+22>pif.length) throw new Error('truncated PIF header');
  const signature=pif.subarray(at,at+16).toString('ascii').split('\0')[0];
  const data=pif.readUInt16LE(at+18), size=pif.readUInt16LE(at+20);
  if(data+size>pif.length) throw new Error('truncated PIF payload');
  if(signature==='WINDOWS 386 3.0') {
    pif.writeUInt16LE(2048,data+8); // original W386PIF30.PfMaxEMMK
    found386=true;
  }
  if(signature==='WINDOWS NT  3.1') {
    for(const [offset,name] of [[12,'config.nt'],[76,'autoexec.nt']]) {
      pif.fill(0,data+offset,data+offset+64);
      pif.write(`Q:\\profiles\\pure-dos\\${name}`,data+offset,'ascii');
    }
    foundNt=true;
  }
  at=pif.readUInt16LE(at+16);
}
if(!found386||!foundNt) throw new Error('required original PIF extensions missing');
fs.writeFileSync(path.join(root,'profiles/pure-dos/pure-dos.pif'),pif);
manifest.push({source,relative:'profiles/pure-dos/pure-dos.pif',sha256:hash(pif),
  changes:'EMS=2048 KiB; NT config/autoexec paths rebound to temporary Q: test package'});
fs.writeFileSync(path.join(run,'ems-package-manifest.json'),JSON.stringify(manifest,null,2));
console.log(root);
