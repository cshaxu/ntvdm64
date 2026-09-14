import fs from 'node:fs';
import path from 'node:path';
import {spawn,spawnSync} from 'node:child_process';
const root=process.cwd(),product=path.resolve('build/M0-T412/S3/product');
const build=path.resolve('build/M0-T412/S3/basesrv-product');
fs.mkdirSync(build,{recursive:true});
const env=path.join(build,'msvc.cmd');
fs.writeFileSync(env,'@echo off\r\nset "service_test_cwd=%CD%"\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%service_test_cwd%"\r\n%*\r\n');
const flags=`/nologo /c /MT /W4 /I "${product}/obj/basesrv" /I "${root}/src"`;
const log=fs.openSync(path.join(build,'build.log'),'w');
try {
    for (const command of [
        `cl.exe ${flags} "${root}/tests/broker/service_client.c" /Foclient.obj`,
        `cl.exe ${flags} "${product}/obj/basesrv/service_c.c" /Fostub.obj`,
        `link.exe /nologo /out:client.exe client.obj stub.obj "${product}/broker-transport.lib" rpcrt4.lib kernel32.lib advapi32.lib`
    ]) {
        const result=spawnSync('cmd.exe',['/d','/c',`call "${env}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
        if(result.status!==0) throw Error(`Client build failed ${result.status}; see ${build}/build.log`);
    }
} finally {fs.closeSync(log);}
const map=fs.readFileSync(path.join(product,'basesrv.exe.map'),'utf8');
for(const [symbol,owner] of [['_BaseSrvIsFirstVDM','opennt-base-server:srvvdm.obj'],['_OpenNtBaseServiceFirst','opennt-base-bindings:service.obj']])
    if(!map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(owner))) throw Error(`Wrong product provider ${symbol}`);
if(/fixture|registration\.obj|resource_attachment/i.test(map)) throw Error('Fixture entered product link');
const image=fs.readFileSync(path.join(product,'basesrv.exe'));
if(image.readUInt16LE(image.readUInt32LE(0x3c)+4)!==0x14c) throw Error('Not x86');
const server=spawn(path.join(product,'basesrv.exe'),[],{cwd:build,windowsHide:true,stdio:['ignore','pipe','pipe']});
let output='',timer;
const done=new Promise(resolve=>server.once('close',resolve));
server.stdout.on('data',data=>output+=data); server.stderr.on('data',data=>output+=data);
try {
    await new Promise((resolve,reject)=>{
        timer=setTimeout(()=>reject(Error('Owned product readiness timeout')),10000);
        server.once('error',reject);
        server.once('exit',code=>reject(Error(`Owned basesrv exited ${code}; existing broker is not test-owned`)));
        server.stdout.on('data',()=>{if(output.includes('basesrv: listening')) resolve();});
    });
    clearTimeout(timer);
    const duplicate=spawnSync(path.join(product,'basesrv.exe'),[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:5000});
    fs.writeFileSync(path.join(build,'duplicate.log'),JSON.stringify({status:duplicate.status,error:duplicate.error?.message,stdout:duplicate.stdout,stderr:duplicate.stderr},null,2));
    if(duplicate.status!==1740) throw Error(`Duplicate endpoint owner not rejected: ${duplicate.status}`);
    const result=spawnSync(path.join(build,'client.exe'),[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:15000});
    fs.writeFileSync(path.join(build,'client.log'),(result.stdout||'')+(result.stderr||''));
    if(result.status!==0) throw Error(`Product RPC client failed ${result.status}`);
    console.log(result.stdout.trim());
} finally {
    clearTimeout(timer);
    if(server.exitCode===null) server.kill();
    await done;
    fs.writeFileSync(path.join(build,'server.log'),output);
}
console.log('Product remains WIP: owned test server terminated; idle shutdown and DOS execution not asserted.');
