// Run after generating the ordinary x86 graph at the supplied build root.
// No product deployment, desktop interaction or unrelated-process termination.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawn,spawnSync} from 'node:child_process';
const root=process.cwd(),build=path.resolve(process.argv[2]);
assert.ok(build.startsWith(path.join(root,'build')+path.sep));
const vs='C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat';
const ninja=process.argv[3];
assert.ok(ninja,'Pass native Ninja executable as third argument');
const logRoot=path.resolve(process.argv[4]||'O:/winnt/Logs2');
const prefix=process.argv[5]||`ntsrv-idle-${Date.now()}`;
assert.match(prefix,/^[A-Za-z0-9_-]+$/);
assert.ok(fs.statSync(logRoot).isDirectory(),'Use an existing approved runtime log directory');
const runtimeLog=name=>path.join(logRoot,`${prefix}-${name}`);
assert.ok(!fs.existsSync(runtimeLog('idle-results.json')),'Do not overwrite sealed run evidence');
let captureIndex=0;
function capture(file,args,options={},runtime=false) {
    const name=`capture-${++captureIndex}.log`;
    const target=runtime?runtimeLog(name):path.join(build,name),fd=fs.openSync(target,'w');
    let result;
    try {result=spawnSync(file,args,{cwd:build,windowsHide:true,timeout:30000,...options,stdio:['ignore',fd,fd]});}
    finally {fs.closeSync(fd);}
    return {...result,stdout:fs.readFileSync(target,'utf8'),stderr:''};
}
const setup=capture('cmd.exe',['/d','/s','/c',`"call "${vs}" -arch=x86 -host_arch=x64 >nul && set"`],
    {windowsVerbatimArguments:true});
assert.equal(setup.status,0,`${setup.error||''} ${setup.stdout}`);
const toolEnv={...process.env};
for(const line of setup.stdout.split(/\r?\n/)) {
    const at=line.indexOf('=');if(at>0)toolEnv[line.slice(0,at)]=line.slice(at+1);
}
const log=fs.openSync(path.join(build,'idle-build.log'),'w');
function compile(command) {
    const r=spawnSync('cmd.exe',['/d','/s','/c',`"${command}"`],
        {cwd:build,env:toolEnv,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
    assert.equal(r.status,0,`Build failed: ${command}; ${r.error||''}; see idle-build.log`);
}
try {
    // Preserve dependency-driven object reuse. Run with the same admitted
    // toolchain and a file log; sandbox failures are not a reason to replay
    // every command and rebuild the entire source closure unconditionally.
    const buildResult=spawnSync(ninja,['-j4','ntsrv.exe','obj/worker/stub.obj'],
        {cwd:build,env:toolEnv,windowsHide:true,stdio:['ignore',log,log],timeout:120000});
    assert.equal(buildResult.status,0,`Ninja failed: ${buildResult.error||''}; see idle-build.log`);
    const flags=`/nologo /c /MT /W4 /Gy /I obj/basesrv /I "${root}/src"`;
    compile(`cl.exe ${flags} "${root}/tests/broker/ntsrv_idle_test.c" /Foidle-test.obj`);
    compile('link.exe /nologo /opt:ref /out:idle-test.exe idle-test.obj obj/basesrv/stub.obj obj/basesrv/console_query.obj obj/run16/support.obj opennt-base-server.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib');
    compile(`cl.exe ${flags} "${root}/tests/broker/ntsrv_idle_client.c" /Foidle-client.obj`);
    compile('link.exe /nologo /opt:ref /out:idle-client.exe idle-client.obj obj/worker/stub.obj broker-transport.lib rpcrt4.lib advapi32.lib kernel32.lib');
} finally { fs.closeSync(log); }
const unit=capture(path.join(build,'idle-test.exe'),[],{},true);
fs.writeFileSync(runtimeLog('idle-unit.log'),unit.stdout+unit.stderr);
assert.equal(unit.status,0,unit.stdout+unit.stderr);
const fatal=capture(path.join(build,'idle-test.exe'),['fatal'],{},true);
assert.equal(fatal.status,8,'SetWaitableTimer failure must terminate with its error');
assert.match(fatal.stdout,/fatal SetWaitableTimer/);
for(const [mode,error,api] of [['create-fatal',8,'CreateWaitableTimer'],['stop-fatal',5,'RpcMgmtStopServerListening']]) {
    const failed=capture(path.join(build,'idle-test.exe'),[mode],{},true);
    assert.equal(failed.status,error,failed.stdout);
    assert.ok(failed.stdout.includes(`fatal ${api}`),failed.stdout);
}
const pe=fs.readFileSync(path.join(build,'ntsrv.exe')),offset=pe.readUInt32LE(0x3c);
assert.equal(pe.readUInt16LE(offset+4),0x14c,'x86');
assert.equal(pe.readUInt16LE(offset+24+68),2,'Windows subsystem, no automatically allocated Console');
console.log(unit.stdout.trim());
console.log('PASS: fatal timer failure; x86 Windows-subsystem broker');
const results=[];
const sleep=ms=>new Promise(resolve=>setTimeout(resolve,ms));
function child(file,args=[]) {
    const target=runtimeLog(`capture-${++captureIndex}.log`),fd=fs.openSync(target,'w');
    const p=spawn(path.join(build,file),args,{cwd:build,windowsHide:true,stdio:['ignore',fd,fd]});
    fs.closeSync(fd);
    Object.defineProperty(p,'text',{get:()=>fs.readFileSync(target,'utf8')});
    p.done=new Promise((resolve,reject)=>{p.once('error',reject);p.once('close',code=>resolve(code));});
    return p;
}
async function marker(p,text,ms=5000) {
    const until=Date.now()+ms;
    while(!p.text.includes(text)) {
        if(p.exitCode!==null || Date.now()>=until) throw Error(`Missing ${text}: ${p.text}`);
        await sleep(25);
    }
}
async function finish(p,ms=16000) {
    let timer;
    try { return await Promise.race([p.done,new Promise((_,reject)=>{timer=setTimeout(()=>reject(Error(`Timeout: ${p.text}`)),ms);})]); }
    finally {clearTimeout(timer);}
}
async function scenario(name,body) {
    const start=Date.now(),owned=[];
    const server=child('ntsrv.exe');owned.push(server);
    const client=(mode,ms=0)=>{const p=child('idle-client.exe',[mode,String(ms),String(server.pid)]);owned.push(p);return p;};
    try {
        // An existing main-session broker is never adopted or terminated.
        await marker(server,'basesrv: listening');
        await body(server,client,start);
        assert.equal(await finish(server),0,server.text);
        results.push({name,elapsed_ms:Date.now()-start,passed:true});
        console.log(`PASS: ${name} (${Date.now()-start} ms)`);
    } finally {
        for(const p of owned) {if(p.exitCode===null)p.kill();await p.done;}
        fs.writeFileSync(runtimeLog(`${name}.log`),owned.map(p=>`${p.pid}: ${p.text}`).join('\n'));
        fs.writeFileSync(runtimeLog('idle-results.json'),JSON.stringify(results,null,2));
    }
}
await scenario('manual-empty',async(server,client,start)=>{
    await finish(server);assert.ok(Date.now()-start>=9800 && Date.now()-start<15000);
});
await scenario('observer-does-not-retain',async(server,client,start)=>{
    const observer=client('observe',18000);
    assert.equal(await finish(observer),0,observer.text);
    assert.match(observer.text,/OBSERVED EMPTY/);assert.match(observer.text,/OBSERVED BROKER EXIT/);
    assert.ok(Date.now()-start<15000);
});
await scenario('connected-then-disconnect',async(server,client)=>{
    const active=client('hold',11500);await marker(active,'CONNECTED');
    assert.match(active.text,/WOW STARTUP UNAUTHORIZED REJECTED/);
    await sleep(10500);assert.equal(server.exitCode,null,'Live ordinary client must retain broker');
    assert.equal(await finish(active),0,active.text);
    const disconnected=Date.now();await finish(server);
    assert.ok(Date.now()-disconnected>=9700);
});
await scenario('failed-connect-restarts-grace',async(server,client,start)=>{
    await sleep(6500);
    const rejected=client('badpeer');assert.equal(await finish(rejected),0,rejected.text);
    const failed=Date.now();await finish(server);
    assert.ok(Date.now()-failed>=9700 && Date.now()-start<22000);
});
await scenario('version-failure-restarts-grace',async(server,client)=>{
    await sleep(1500);
    const rejected=client('reject');assert.equal(await finish(rejected),0,rejected.text);
    const failed=Date.now();await finish(server);assert.ok(Date.now()-failed>=9700);
});
await scenario('client-crash-rundown',async(server,client)=>{
    const active=client('crash',100);assert.equal(await finish(active),73,active.text);
    await marker(server,'rundown completed');
    const rundown=Date.now();await finish(server);assert.ok(Date.now()-rundown>=9600);
});
await scenario('late-connect-cancels-grace',async(server,client)=>{
    await sleep(8000);
    const active=client('hold',3500);await marker(active,'CONNECTED');
    await sleep(2500);assert.equal(server.exitCode,null);
    assert.equal(await finish(active),0,active.text);
    const disconnected=Date.now();await finish(server);assert.ok(Date.now()-disconnected>=9700);
});
console.log('PASS: all broker idle RPC scenarios (no guest/runtime package changed)');
