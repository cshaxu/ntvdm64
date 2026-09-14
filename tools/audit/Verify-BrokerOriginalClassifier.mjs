import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
const root=process.cwd(),build=path.resolve('build/M0-T412/S2/original-classifier');
const ownerBuild=process.env.OPENNT_BROKER_OWNER_BUILD ? path.resolve(process.env.OPENNT_BROKER_OWNER_BUILD) : null;
fs.mkdirSync(build,{recursive:true});
for(const suffix of ['COM','pif','exe','bin']) fs.writeFileSync(path.join(build,`suffix.${suffix}`),Buffer.alloc(64,0x41));
const graph=fs.readFileSync('build/M0-T412/S1/text-cell-repair/product/build.ninja','utf8');
const flags=graph.match(/^cflags = (.*)$/m)[1].replaceAll('$:',':')+` /I "${root}/src/adapter-opennt-host/basesrv/include"`;
const rtlFlags=graph.match(/^build obj\/opennt-rtl\/error\.obj:.*\r?\n  rtl_cflags = (.*)$/m)[1].replaceAll('$:',':');
const source=fs.readFileSync('src/opennt-host/base/win32/client/vdm.c','utf8');
const upstream=fs.readFileSync('O:/repos.external/OpenNT/base/win32/client/vdm.c','utf8');
const hashes={};
for(const [name,pattern] of [['GetBinaryTypeW',/BOOL\r?\nWINAPI\r?\nGetBinaryTypeW\([\s\S]*?\r?\n}\r?\n/],['BaseIsDosApplication',/ULONG\r?\nBaseIsDosApplication\([\s\S]*?\r?\n}\r?\n/]]) {
    const original=upstream.match(pattern)[0].replace(/\r\n/g,'\n');
    assert.equal(source.match(pattern)[0].replace(/\r\n/g,'\n'),original,`${name} differs from original`);
    hashes[name]=createHash('sha256').update(original).digest('hex');
}
assert.deepEqual(hashes,{GetBinaryTypeW:'f7fd9766ea08e676a2fd457fb3fa508384630640032ce0615f752dc8a43f9fbd',BaseIsDosApplication:'9487407a2efc02764466cedbabb4e89424fd12d87d11601cce9fa5ad477f3711'});
const env=path.join(build,'msvc.cmd');
fs.writeFileSync(env,'@echo off\r\nset "classifier_cwd=%CD%"\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%classifier_cwd%"\r\n%*\r\n');
let commands=[
    `cl.exe ${flags} /we4013 /Fo"${build}/support-check.obj" "${root}/src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c"`,
    `cl.exe ${flags} /we4013 /Gy /DOPENNT_BASE_CLIENT_CLASSIFIER /Fo"${build}/client.obj" "${root}/src/opennt-host/base/win32/client/vdm.c"`,
    `cl.exe ${flags} /we4013 /Fo"${build}/fixture.obj" "${root}/tests/broker/original_classifier.c"`,
    `cl.exe ${flags} /we4013 /Fo"${build}/path.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_classifier_path.c"`,
    `cl.exe ${rtlFlags} /Fo"${build}/error.obj" "${root}/src/opennt-host/base/ntos/rtl/error.c"`,
    'link.exe /nologo /opt:ref /out:original-classifier.exe /map:original-classifier.map client.obj fixture.obj path.obj error.obj ntdll.lib kernel32.lib legacy_stdio_definitions.lib'
];
if(ownerBuild) commands=[
    `cl.exe ${flags} /we4013 /Fo"${build}/fixture.obj" "${root}/tests/broker/original_classifier.c"`,
    `cl.exe ${rtlFlags} /Fo"${build}/error.obj" "${root}/src/opennt-host/base/ntos/rtl/error.c"`,
    `link.exe /nologo /opt:ref /out:original-classifier.exe /map:original-classifier.map fixture.obj "${ownerBuild}/opennt-base-client.lib" "${ownerBuild}/opennt-base-bindings.lib" error.obj ntdll.lib kernel32.lib legacy_stdio_definitions.lib`
];
const log=fs.openSync(path.join(build,'build.log'),'w');
for(const command of commands) {
    const result=spawnSync('cmd.exe',['/d','/c',`call "${env}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
    if(result.status!==0) {fs.closeSync(log); throw Error('Build failed; see build.log');}
}
fs.closeSync(log);
const exe=path.join(build,'original-classifier.exe'),image=fs.readFileSync(exe);
assert.equal(image.readUInt16LE(image.readUInt32LE(0x3c)+4),0x14c);
const map=fs.readFileSync(path.join(build,'original-classifier.map'),'utf8');
for(const symbol of ['_OpenNtBaseGetBinaryTypeW@8','_BaseIsDosApplication'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(ownerBuild?'opennt-base-client:classifier.obj':'client.obj')),`Wrong provider: ${symbol}`);
const run=spawnSync(exe,[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:10000});
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({ownerBuild,hashes,status:run.status,stdout:run.stdout,stderr:run.stderr,error:run.error?.message},null,2));
console.log(run.stdout,run.stderr);
assert.equal(run.status,0,'Original classifier failed');
