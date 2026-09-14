import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import assert from 'node:assert/strict';
const root=process.cwd(), build=path.resolve('build/M0-T412/S3/delivery-journal');
fs.mkdirSync(build,{recursive:true});
const command=path.join(build,'compile.cmd');
fs.writeFileSync(command,'@echo off\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\n'+
    `cl.exe /nologo /MT /W4 /I "${root}/src/broker" "${root}/tests/broker/delivery_journal.c" "${root}/src/broker/vdm_delivery.c" "${root}/src/broker/vdm_receipt.c" /Fejournal.exe\r\n`);
const compiled=spawnSync('cmd.exe',['/d','/c',`call "${command}"`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,encoding:'utf8',timeout:60000});
fs.writeFileSync(path.join(build,'build.log'),compiled.stdout+compiled.stderr);
assert.equal(compiled.status,0,'Build failed; see build.log');
const exe=path.join(build,'journal.exe'), bytes=fs.readFileSync(exe);
assert.equal(bytes.readUInt16LE(bytes.readUInt32LE(0x3c)+4),0x14c);
const result=spawnSync(exe,[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:10000});
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({status:result.status,stdout:result.stdout,stderr:result.stderr},null,2));
console.log(result.stdout,result.stderr);
assert.equal(result.status,0);
