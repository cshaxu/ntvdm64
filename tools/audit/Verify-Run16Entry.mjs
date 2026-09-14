import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=process.cwd(), build=path.resolve('build/M0-T412/S3/entry-tests');
const exe=path.resolve('build/M0-T412/S3/product/run16.exe');
fs.mkdirSync(build,{recursive:true});
const image=fs.readFileSync(exe), pe=image.readUInt32LE(0x3c);
assert.equal(image.readUInt16LE(pe+4),0x14c);
assert.equal(image.readUInt16LE(pe+24+68),3,'run16 must be a console executable');
const map=fs.readFileSync(`${exe}.map`,'utf8');
assert(map.split(/\r?\n/).some(line=>line.includes('_OpenNtBaseGetBinaryTypeW@8')&&line.includes('opennt-base-client:classifier.obj')));
assert(!map.includes('base_vdm_local.obj')&&!map.includes('machine_shell.obj'));
const runner=path.join(build,'compile.cmd');
fs.writeFileSync(runner,'@echo off\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncl.exe /nologo /MT /W4 "'+root+'/tests/app/run16_native_child.c" /Fochild.obj /Fe"native child.exe"\r\n');
const compiled=spawnSync('cmd.exe',['/d','/c',`call "${runner}"`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,encoding:'utf8',timeout:60000});
fs.writeFileSync(path.join(build,'build.log'),compiled.stdout+compiled.stderr);
assert.equal(compiled.status,0,'native child build failed');
const records=[];
function run(raw, expected) {
    const r=spawnSync(exe,raw?[raw]:[],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,
        input:Buffer.from('stdin payload'),timeout:10000,
        env:{...process.env,RUN16_TEST_VALUE:'value spaces 123',RUN16_TEST_CWD:build}});
    assert.equal(r.status,expected,`${raw}: ${r.stderr?.toString()}`);
    records.push({command:raw,status:r.status,stdout:r.stdout.toString(expected===23?'utf16le':'utf8'),stderr:r.stderr.toString()});
    return r;
}
for(const name of [path.join(build,'native child.exe'),'native child.exe']) {
    const raw=`"${name}" alpha "two words" "" slash\\\\tail`;
    const r=run(raw,23);
    assert.equal(r.stdout.toString('utf16le'),raw,'command tail was changed');
    assert.equal(r.stderr.toString(),'stdin payload','standard streams were not inherited');
}
assert.match(run('',87).stderr.toString(),/Usage:/);
run('missing-run16-test.exe',2);
assert.match(run('O:\\ntvdm64\\MEM.EXE',50).stderr.toString(),/not integrated/);
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({sha256:createHash('sha256').update(image).digest('hex'),records},null,2));
console.log('PASS: real run16 console entry, original classification, raw command tail, cwd/environment/streams, native exit; VDM remains explicitly unsupported');
