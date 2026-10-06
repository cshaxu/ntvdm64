// Test-only generated service variants. Production source remains unchanged.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
const [buildArg,serviceArg]=process.argv.slice(2);
assert(buildArg && serviceArg,'build root and verified native service archive');
const root=process.cwd(),build=path.resolve(buildArg),service=path.resolve(serviceArg);
assert(build.startsWith(path.join(root,'build')+path.sep));
fs.mkdirSync(build,{recursive:true});
const source=fs.readFileSync('src/ntsrv-exe/main.c','utf8');
const marker='error=OpenNtBaseServiceObserveNativeCreation(service,reporter,child,flags,&identity);';
assert.equal(source.split(marker).length,2,'unique observation boundary');
const wrapper=path.join(build,'msvc.cmd');
fs.writeFileSync(wrapper,'@echo off\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\n%*\r\n');
const logfile=fs.openSync(path.join(build,'build.log'),'w');
function run(command){const result=spawnSync('cmd.exe',['/d','/c',`call "${wrapper}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',logfile,logfile],timeout:60000});assert.equal(result.status,0,`fault build failed; ${build}/build.log`);}
try{
 for(const [name,replacement] of [['deny','error=ERROR_ACCESS_DENIED;'],['slow',`{ Sleep(2000); ${marker} }`]]){
  // Insert only inside the already authenticated observation call, never
  // in Direct launch/completion or worker control. No retry-to-success.
  fs.writeFileSync(path.join(build,`${name}.c`),source.replace(marker,replacement));
  run(`cl.exe /nologo /c /MT /W4 /we4013 /I "${service}/obj/basesrv" /I "${root}/src" ${name}.c /Fo${name}.obj`);
  run(`link.exe /nologo /opt:ref /out:${name}.exe ${name}.obj "${service}/obj/basesrv/stub.obj" "${service}/obj/run16/support.obj" "${service}/opennt-base-server.lib" "${service}/opennt-base-bindings.lib" "${service}/broker-transport.lib" "${service}/original-opennt-rtl-native.lib" "${service}/common-root.lib" "${service}/common-rpc.lib" "${service}/common-transport.lib" "${service}/common-codec.lib" "${service}/common-console.lib" rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib`);
 }
}finally{fs.closeSync(logfile);}
assert.equal(fs.readFileSync('src/ntsrv-exe/main.c','utf8'),source);
console.log('PASS test-only denied/slow observation services; production unchanged');
