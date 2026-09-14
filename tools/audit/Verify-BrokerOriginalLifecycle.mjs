import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
const root=process.cwd(), build=path.resolve('build/M0-T412/S2/original-lifecycle');
fs.mkdirSync(build,{recursive:true});
const graph=fs.readFileSync('build/M0-T412/S1/text-cell-repair/product/build.ninja','utf8');
const flags=graph.match(/^cflags = (.*)$/m)[1].replaceAll('$:',':');
const rtlFlags=graph.match(/^build obj\/opennt-rtl\/error\.obj:.*\r?\n  rtl_cflags = (.*)$/m)[1].replaceAll('$:',':');
const ownerPath=path.resolve('src/opennt-host/base/win32/server/srvvdm.c');
const ownerBefore=fs.readFileSync(ownerPath);
const clientSource=fs.readFileSync('src/opennt-host/base/win32/client/vdm.c','utf8');
const clientBody=clientSource.match(/BOOL\r?\nAPIENTRY\r?\nGetNextVDMCommand\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
assert.equal(createHash('sha256').update(clientBody).digest('hex'),
    '47cf285cbc505d9d43a50af16e283ea51d1d9ef8d2ff093795086d0ca327addd','Original client body changed');
const lifecycleBody=clientSource.match(/VOID\r?\nAPIENTRY\r?\nExitVDM\([\s\S]*?VOID\r?\nAPIENTRY\r?\nRegisterWowExec\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
assert.equal(createHash('sha256').update(lifecycleBody).digest('hex'),
    '2936de23dcbbdec23cf9ea23416606a7a3faeb96473d407e4bfaee00f89ac3aa','Original client lifecycle group changed');
const updateBody=clientSource.match(/BOOL\r?\nBaseUpdateVDMEntry\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
const checkBody=clientSource.match(/BOOL\r?\nBaseCheckVDM\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
const checkOriginal=fs.readFileSync('O:/repos.external/OpenNT/base/win32/client/vdm.c','utf8').match(/BOOL\r?\nBaseCheckVDM\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
assert.equal(checkBody,checkOriginal,'Original launch body changed');
assert.equal(createHash('sha256').update(checkBody).digest('hex'),'579840e2d5598051d157d9e92871027b543ecfcba8d299a745928bfd71c6386e','Pinned launch body changed');
assert.equal(createHash('sha256').update(updateBody).digest('hex'),
    'de56cfccd08922ae57df7332cc8df70f60db186f44f177336188c2d236e9fd26','Original update body changed');
const env=path.join(build,'msvc.cmd');
fs.writeFileSync(env,'@echo off\r\nset "lifecycle_cwd=%CD%"\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%lifecycle_cwd%"\r\n%*\r\n');
const includes=`/D_CSRSRV_ /DOPENNT_BASE_VDM_SERVER /we4013 /FI "${root}/src/adapter-opennt-host/basesrv/include/base_server.h" /I "${root}/src/adapter-opennt-host/basesrv/include" /I "${root}/src/opennt-host/base/win32/inc" /I "${root}/src/opennt-host/base/win32/server"`;
const commands=[
    `cl.exe ${flags} /Fo"${build}/environment-check.obj" "${root}/src/opennt-host/base/win32/client/vdm.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/srvvdm.obj" "${root}/src/opennt-host/base/win32/server/srvvdm.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/fixture.obj" "${root}/tests/broker/original_server_lifecycle.c"`,
    `cl.exe ${flags} /Fo"${build}/process.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_client_process.c"`,
    `cl.exe ${flags} ${includes} /Gy /DOPENNT_BASE_CLIENT_VDM_COMMANDS /Fo"${build}/client.obj" "${root}/src/opennt-host/base/win32/client/vdm.c"`,
    `cl.exe ${rtlFlags} /Fo"${build}/error.obj" "${root}/src/opennt-host/base/ntos/rtl/error.c"`,
    'lib.exe /nologo /out:opennt-base-server.lib srvvdm.obj',
    'lib.exe /nologo /out:opennt-base-client.lib client.obj',
    'link.exe /nologo /opt:ref /out:original-lifecycle.exe /map:original-lifecycle.map fixture.obj opennt-base-server.lib opennt-base-client.lib process.obj error.obj ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib'
];
const log=fs.openSync(path.join(build,'build.log'),'w');
for(const command of commands) {
    const result=spawnSync('cmd.exe',['/d','/c',`call "${env}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
    if(result.status!==0) {fs.closeSync(log); throw Error(`Build failed ${result.status}; see build.log`);}
}
fs.closeSync(log);
const buildLog=fs.readFileSync(path.join(build,'build.log'),'utf8');
for(const retired of ['source_compile_probe.h','ntdddfs.h','luid.c'])
    assert(!buildLog.includes(retired),`Retired test build dependency: ${retired}`);
const inlinePattern=/__inline LUID\s+NTAPI\s+RtlConvertLongToLuid\([\s\S]*?\n\}/;
const normalizeInline=text=>text.match(inlinePattern)[0].split(/\r?\n/).map(line=>line.trimEnd()).join('\n');
assert.equal(normalizeInline(fs.readFileSync('src/opennt-host/public/sdk/inc/ntrtl.h','utf8')),
    normalizeInline(fs.readFileSync('O:/repos.external/OpenNT/public/sdk/inc/ntrtl.h','utf8')),'Original LUID inline changed');
assert.deepEqual(fs.readFileSync(ownerPath),ownerBefore,'Original owner changed during test');
const image=fs.readFileSync(path.join(build,'original-lifecycle.exe'));
assert.equal(image.readUInt16LE(image.readUInt32LE(0x3c)+4),0x14c);
const map=fs.readFileSync(path.join(build,'original-lifecycle.map'),'utf8');
assert(map.includes('opennt-base-server:srvvdm.obj')&&map.includes('opennt-base-client:client.obj'),'Original owner libraries not selected');
assert(!map.includes('luid.obj'),'Generated inline replacement still linked');
assert(map.split(/\r?\n/).some(line=>line.includes('_GetNextVDMCommand@4')&&line.includes('client.obj')),'Original client provider missing');
assert(map.split(/\r?\n/).some(line=>line.includes('_BaseUpdateVDMEntry')&&line.includes('client.obj')),'Original update provider missing');
assert(map.split(/\r?\n/).some(line=>line.includes('_BaseCheckVDM')&&line.includes('client.obj')),'Original launch provider missing');
for (const symbol of ['ExitVDM','SetVDMCurrentDirectories','GetVDMCurrentDirectories','CmdBatNotification','RegisterWowExec'])
    assert(map.split(/\r?\n/).some(line=>line.includes(`_${symbol}@`)&&line.includes('client.obj')),`Original client provider missing: ${symbol}`);
for(const symbol of ['BaseSrvCheckVDM','BaseSrvGetNextVDMCommand','BaseSrvSetReenterCount','BaseSrvExitDOSTask'])
    assert(map.split(/\r?\n/).some(line=>line.includes(`_${symbol}`)&&line.includes('srvvdm.obj')),`Original provider missing for ${symbol}`);
const result=spawnSync(path.join(build,'original-lifecycle.exe'),[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:15000});
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({status:result.status,stdout:result.stdout,stderr:result.stderr,error:result.error?.message},null,2));
console.log(result.stdout,result.stderr);
assert.equal(result.status,0,'Original lifecycle fixture failed');
console.log('PASS: original BaseClient capture/copy, directories, exit, BAT/WOW registration, real wait/wake/retry with cleared exit code; captures drained');
