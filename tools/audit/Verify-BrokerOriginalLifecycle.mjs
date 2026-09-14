import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
const root=process.cwd(), build=path.resolve('build/M0-T412/S2/original-lifecycle');
fs.mkdirSync(build,{recursive:true});
const graph=fs.readFileSync('build/M0-T412/S1/text-cell-repair/product/build.ninja','utf8');
const flags=graph.match(/^cflags = (.*)$/m)[1].replaceAll('$:',':');
const ownerPath=path.resolve('src/opennt-host/base/win32/server/srvvdm.c');
const ownerBefore=fs.readFileSync(ownerPath);
const env=path.join(build,'msvc.cmd');
fs.writeFileSync(env,'@echo off\r\nset "lifecycle_cwd=%CD%"\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%lifecycle_cwd%"\r\n%*\r\n');
fs.writeFileSync(path.join(build,'ntdddfs.h'),'/* Unused umbrella dependency in test-only composition. */\n');
const rtl=fs.readFileSync('src/opennt-abi/source/public/sdk/inc/ntrtl.h','utf8');
const body=rtl.match(/__inline LUID\s+NTAPI\s+RtlConvertLongToLuid\([\s\S]*?\n\}/)?.[0];
assert(body,'Missing original inline');
fs.writeFileSync(path.join(build,'luid.c'),body.replace('__inline ','')+'\n');
const includes=`/D_CSRSRV_ /we4013 /FI "${root}/tests/broker/source_compile_probe.h" /I "${build}" /I "${root}/src/opennt-host/base/win32/inc" /I "${root}/src/opennt-host/base/win32/server"`;
const commands=[
    `cl.exe ${flags} ${includes} /Fo"${build}/srvvdm.obj" "${root}/src/opennt-host/base/win32/server/srvvdm.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/luid.obj" "${build}/luid.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/fixture.obj" "${root}/tests/broker/original_server_lifecycle.c"`,
    'link.exe /nologo /out:original-lifecycle.exe /map:original-lifecycle.map srvvdm.obj luid.obj fixture.obj ntdll.lib kernel32.lib user32.lib advapi32.lib'
];
const log=fs.openSync(path.join(build,'build.log'),'w');
for(const command of commands) {
    const result=spawnSync('cmd.exe',['/d','/c',`call "${env}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
    if(result.status!==0) {fs.closeSync(log); throw Error(`Build failed ${result.status}; see build.log`);}
}
fs.closeSync(log);
assert.deepEqual(fs.readFileSync(ownerPath),ownerBefore,'Original owner changed during test');
const image=fs.readFileSync(path.join(build,'original-lifecycle.exe'));
assert.equal(image.readUInt16LE(image.readUInt32LE(0x3c)+4),0x14c);
const map=fs.readFileSync(path.join(build,'original-lifecycle.map'),'utf8');
for(const symbol of ['BaseSrvCheckVDM','BaseSrvGetNextVDMCommand','BaseSrvSetReenterCount','BaseSrvExitDOSTask'])
    assert(map.split(/\r?\n/).some(line=>line.includes(`_${symbol}`)&&line.includes('srvvdm.obj')),`Original provider missing for ${symbol}`);
const result=spawnSync(path.join(build,'original-lifecycle.exe'),[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:15000});
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({status:result.status,stdout:result.stdout,stderr:result.stderr,error:result.error?.message},null,2));
console.log(result.stdout,result.stderr);
assert.equal(result.status,0,'Original lifecycle fixture failed');
