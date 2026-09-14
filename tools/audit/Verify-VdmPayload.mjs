import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
const root=process.cwd(), build=path.resolve('build/M0-T412/S3/payload');
const ownerBuild=path.resolve(process.env.OPENNT_BROKER_OWNER_BUILD || 'build/M0-T412/S3/product');
fs.mkdirSync(build,{recursive:true});
const command=path.join(build,'compile.cmd');
fs.writeFileSync(command,'@echo off\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\n'+
    `cl.exe /nologo /MT /W4 /I "${root}/src/broker" "${root}/tests/broker/vdm_payload.c" "${ownerBuild}/broker-transport.lib" /Fepayload.exe /link /map:payload.map\r\n`);
const compile=spawnSync('cmd.exe',['/d','/c',`call "${command}"`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,encoding:'utf8',timeout:60000});
fs.writeFileSync(path.join(build,'build.log'),compile.stdout+compile.stderr);
assert.equal(compile.status,0,'Build failed; see build.log');
const map=fs.readFileSync(path.join(build,'payload.map'),'utf8');
for (const symbol of ['_broker_vdm_payload_encode','_broker_vdm_payload_validate'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('broker-transport:vdm_payload.obj')),`Missing formal provider: ${symbol}`);
const exe=path.join(build,'payload.exe'), bytes=fs.readFileSync(exe);
assert.equal(bytes.readUInt16LE(bytes.readUInt32LE(0x3c)+4),0x14c);
const result=spawnSync(exe,[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:10000});
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({ownerBuild,status:result.status,stdout:result.stdout,stderr:result.stderr},null,2));
console.log(result.stdout,result.stderr);
assert.equal(result.status,0);
