import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
const root=process.cwd(), build=path.resolve('build/M0-T412/S1/console-membership');
fs.mkdirSync(build,{recursive:true});
const env=path.join(build,'msvc.cmd');
fs.writeFileSync(env, '@echo off\r\nset "membership_cwd=%CD%"\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%membership_cwd%"\r\n%*\r\n');
const log=fs.openSync(path.join(build,'build.log'),'w');
const compile=spawnSync('cmd.exe',['/d','/c',`call "${env}" cl.exe /nologo /MT /W4 /we4013 "${root}/tests/broker/console_membership.c" /Feconsole-membership.exe /Foconsole-membership.obj`],
    {cwd:build,windowsVerbatimArguments:true,windowsHide:true,stdio:['ignore',log,log],timeout:60000});
fs.closeSync(log);
assert.equal(compile.status,0,'See build.log');
const exe=path.join(build,'console-membership.exe'), bytes=fs.readFileSync(exe);
assert.equal(bytes.readUInt16LE(bytes.readUInt32LE(0x3c)+4),0x14c);
const run=spawnSync(exe,[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:45000});
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({status:run.status,stdout:run.stdout,stderr:run.stderr,error:run.error?.message},null,2));
console.log(run.stdout,run.stderr);
assert.equal(run.status,0,'Console membership test failed');
