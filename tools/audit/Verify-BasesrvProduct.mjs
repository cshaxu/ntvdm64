import fs from 'node:fs';
import path from 'node:path';
import {spawn,spawnSync} from 'node:child_process';
const root=process.cwd(),product=path.resolve(process.env.OPENNT_BROKER_PRODUCT_BUILD || 'build/M0-T412/S3/product');
const build=path.resolve(process.env.OPENNT_BROKER_TEST_BUILD || 'build/M0-T412/S3/basesrv-product');
const logs=path.resolve(process.env.OPENNT_BROKER_TEST_LOGS || 'O:/winnt/logs/basesrv-product');
fs.mkdirSync(logs,{recursive:true});
fs.mkdirSync(build,{recursive:true});
const env=path.join(build,'msvc.cmd');
fs.writeFileSync(env,'@echo off\r\nset "service_test_cwd=%CD%"\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%service_test_cwd%"\r\n%*\r\n');
const flags=`/nologo /c /MT /W4 /I "${product}/obj/basesrv" /I "${root}/src"`;
const ownerGraph=fs.readFileSync(path.join(product,'build.ninja'),'utf8');
const sourceFlags=ownerGraph.match(/^build obj\/opennt-base-bindings\/registry.obj:.*\r?\n  cflags = (.*)$/m)[1].replaceAll('$:',':');
const baseClientFlags=`${sourceFlags} /I "${product}/obj/basesrv"`;
const log=fs.openSync(path.join(build,'build.log'),'w');
try {
    for (const command of [
        `cl.exe ${flags} "${root}/tests/broker/service_client.c" /Foclient.obj`,
        `cl.exe ${baseClientFlags} "${root}/tests/app/base_client_rpc_first_test.c" /Fobase-client-rpc-first.obj`,
        `cl.exe ${baseClientFlags} "${root}/src/adapter-opennt-host/basesrv/source/base_rpc_client.c" /Forpc-client.obj`,
        `cl.exe ${flags} "${product}/obj/basesrv/service_c.c" /Fostub.obj`,
        `cl.exe ${sourceFlags} "${root}/tests/broker/service_stream_source.c" /Fostream-source.obj`,
        `link.exe /nologo /opt:ref /map:client.map /out:client.exe client.obj stub.obj stream-source.obj "${product}/obj/run16/support.obj" "${product}/broker-transport.lib" "${product}/opennt-base-server.lib" "${product}/opennt-base-bindings.lib" "${product}/original-opennt-rtl-x86.lib" rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib`,
        `link.exe /nologo /opt:ref /map:base-client-rpc-first.map /out:base-client-rpc-first.exe base-client-rpc-first.obj rpc-client.obj stub.obj "${product}/obj/run16/support.obj" "${product}/opennt-base-client.lib" "${product}/broker-transport.lib" "${product}/opennt-base-bindings.lib" "${product}/opennt-base-server.lib" "${product}/original-opennt-rtl-x86.lib" rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib`
    ]) {
        const result=spawnSync('cmd.exe',['/d','/c',`call "${env}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
        if(result.status!==0) throw Error(`Client build failed ${result.status}; see ${build}/build.log`);
    }
} finally {fs.closeSync(log);}
const clientMap=fs.readFileSync(path.join(build,'client.map'),'utf8');
for(const [symbol,owner] of [['_BaseSrvDupStandardHandles','opennt-base-server:srvvdm.obj'],
    ['_BaseSrvCloseStandardHandles','opennt-base-server:srvvdm.obj'],
    ['_OpenNtBaseDuplicateStream','opennt-base-bindings:streams.obj']])
    if(!clientMap.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(owner)))
        throw Error(`Wrong source-to-RPC provider ${symbol}`);
const baseClientMap=fs.readFileSync(path.join(build,'base-client-rpc-first.map'),'utf8');
if(!baseClientMap.split(/\r?\n/).some(line=>line.includes('_OpenNtBaseClientCallServer@16')&&line.includes('rpc-client.obj')))
    throw Error('BaseClient RPC facade did not select the product provider');
for(const symbol of ['_Client_Check','_Client_Get','_Client_Update','_Client_ExitCode','_Client_Reenter','_Client_Reserve','_Client_Prepare','_Client_Release'])
    if(!baseClientMap.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('stub.obj')))
        throw Error(`BaseClient RPC route did not select generated stub ${symbol}`);
if(baseClientMap.includes('_CsrClientCallServer@16'))
    throw Error('Host ntdll CSR client entered the BaseClient RPC test');
const map=fs.readFileSync(path.join(product,'basesrv.exe.map'),'utf8');
for(const [symbol,owner] of [['_BaseSrvIsFirstVDM','opennt-base-server:srvvdm.obj'],['_BaseSrvGetVDMExitCode','opennt-base-server:srvvdm.obj'],['_BaseSrvSetReenterCount','opennt-base-server:srvvdm.obj'],['_Server_Check','entry.obj'],['_Server_Get','entry.obj'],['_Server_Update','entry.obj'],['_Server_ExitCode','entry.obj'],['_Server_Reenter','entry.obj'],['_Server_Reserve','entry.obj'],['_Server_Prepare','entry.obj'],['_Server_Release','entry.obj'],['_OpenNtBaseServiceFirst','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceCheck','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceGet','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceUpdate','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceExitCode','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceReenter','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceCreateReservation','opennt-base-bindings:service.obj'],['_OpenNtBaseServicePrepareWorker','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceReleaseReservation','opennt-base-bindings:service.obj'],['_OpenNtBaseServiceRetainPeer','opennt-base-bindings:service.obj'],['_OpenNtBaseRetainRegisteredProcess','opennt-base-bindings:registry.obj']])
    if(!map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(owner))) throw Error(`Wrong product provider ${symbol}`);
for(const [symbol,owner] of [['_OpenNtBaseBindResources','opennt-base-bindings:resources.obj'],['_OpenNtBaseDuplicateWait','opennt-base-bindings:waits.obj']])
    if(!map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(owner))) throw Error(`Missing typed wait binding ${symbol}`);
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
    const clientEnvironment={...process.env,BASESRV_TEST_PID:String(server.pid)};
    const duplicate=spawnSync(path.join(product,'basesrv.exe'),[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:5000});
    fs.writeFileSync(path.join(logs,'duplicate.log'),JSON.stringify({status:duplicate.status,error:duplicate.error?.message,stdout:duplicate.stdout,stderr:duplicate.stderr},null,2));
    if(duplicate.status!==1740) throw Error(`Duplicate endpoint owner not rejected: ${duplicate.status}`);
    const result=spawnSync(path.join(build,'client.exe'),[],{cwd:build,env:clientEnvironment,windowsHide:true,encoding:'utf8',timeout:15000});
    fs.writeFileSync(path.join(logs,'client.log'),(result.stdout||'')+(result.stderr||''));
    if(result.status!==0) throw Error(`Product RPC client failed ${result.status}`);
    console.log(result.stdout.trim());
    const baseClientFirst=spawnSync(path.join(build,'base-client-rpc-first.exe'),['--existing'],{cwd:build,windowsHide:true,encoding:'utf8',timeout:15000});
    fs.writeFileSync(path.join(logs,'base-client-first.log'),(baseClientFirst.stdout||'')+(baseClientFirst.stderr||''));
    if(baseClientFirst.status!==0) throw Error(`BaseClient RPC facade failed ${baseClientFirst.status}`);
    console.log(baseClientFirst.stdout.trim());
    const reservation=spawnSync(path.join(build,'base-client-rpc-first.exe'),['--reservation-parent'],{cwd:build,windowsHide:true,encoding:'utf8',timeout:20000});
    fs.writeFileSync(path.join(logs,'reservation.log'),(reservation.stdout||'')+(reservation.stderr||''));
    if(reservation.status!==0) throw Error(`Launcher/worker reservation route failed ${reservation.status}`);
    console.log(reservation.stdout.trim());
    const abandoned=spawnSync(path.join(build,'client.exe'),['--abandon'],{cwd:build,windowsHide:true,encoding:'utf8',timeout:15000});
    fs.writeFileSync(path.join(logs,'abandoned.log'),(abandoned.stdout||'')+(abandoned.stderr||''));
    if(abandoned.status!==0) throw Error(`Abandon client failed ${abandoned.status}`);
    await new Promise((resolve,reject)=>{
        const observed=()=>{
            if(output.includes('basesrv: connection rundown completed')) {
                clearTimeout(timer); server.stderr.off('data',observed); resolve();
            }
        };
        timer=setTimeout(()=>{server.stderr.off('data',observed);reject(Error('RPC context rundown not observed'));},10000);
        server.stderr.on('data',observed); observed();
    });
    const reconnected=spawnSync(path.join(build,'client.exe'),['--existing'],{cwd:build,env:clientEnvironment,windowsHide:true,encoding:'utf8',timeout:15000});
    fs.writeFileSync(path.join(logs,'after-rundown.log'),(reconnected.stdout||'')+(reconnected.stderr||''));
    if(reconnected.status!==0) throw Error(`Service failed after rundown ${reconnected.status}`);
    const baseClientExisting=spawnSync(path.join(build,'base-client-rpc-first.exe'),['--existing'],{cwd:build,windowsHide:true,encoding:'utf8',timeout:15000});
    if(baseClientExisting.status!==0) throw Error(`BaseClient RPC facade failed after rundown ${baseClientExisting.status}`);
    console.log('PASS: process-exit RPC rundown removes the registered connection; original service remains responsive.');
} finally {
    clearTimeout(timer);
    if(server.exitCode===null) server.kill();
    await done;
    fs.writeFileSync(path.join(logs,'server.log'),output);
}
console.log('PASS: protocol/app-version rejection and service boundary checks; owned server stopped. DOS execution is a separate gate.');
