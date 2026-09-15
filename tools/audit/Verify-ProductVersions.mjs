// Real RPC negative peers. Mutations are test-only generated sources under build/.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawn, spawnSync} from 'node:child_process';
const root=process.cwd(), product=path.resolve(process.env.OPENNT_BROKER_PRODUCT_BUILD || 'build/M0-T412/S9/version-agreement');
const build=path.resolve(process.env.OPENNT_VERSION_TEST_BUILD || 'build/M0-T412/S9/version-negative');
const logs=path.resolve(process.env.OPENNT_VERSION_TEST_LOGS || 'O:/winnt/logs/m0-t412-s9-version-negative');
fs.mkdirSync(build,{recursive:true});fs.mkdirSync(logs,{recursive:true});
const source=fs.readFileSync('src/app/basesrv_entry.c','utf8');
const header=fs.readFileSync('src/app/version.h','utf8');
const version=header.match(/#define APP_VERSION "(0\.0\.[0-9]+)"/)[1];
const protocol=Number(header.match(/#define APP_PROTOCOL_VERSION ([0-9]+)u/)[1]);
const active=fs.readFileSync('docs/states/CURRENT.md','utf8').match(/\*\*Active: M[0-9]+ T([0-9]+) S[0-9]+\./);
if(active)assert.equal(version,`0.0.${active[1]}`,'Application version must match admitted T');
const idl=fs.readFileSync('src/broker/service.idl','utf8');
assert(idl.includes(`version(${protocol}.0)`),'RPC major and protocol must agree');
assert.match(idl,/application_version\[32\]/);
assert.match(header,/#define APP_VERSION_BYTES 32u/);
const cmd=path.join(build,'msvc.cmd');
fs.writeFileSync(cmd,'@echo off\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\n%*\r\n');
const compileLog=fs.openSync(path.join(build,'build.log'),'w');
function compile(command) {
    const r=spawnSync('cmd.exe',['/d','/c',`call "${cmd}" ${command}`],
        {cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',compileLog,compileLog],timeout:60000});
    assert.equal(r.status,0,`Build failed: ${command}; see ${build}/build.log`);
}
const variants=[
    ['old-app','expected[APP_VERSION_BYTES]=APP_VERSION','expected[APP_VERSION_BYTES]="0.0.0"'],
    ['wrong-protocol','protocol!=APP_PROTOCOL_VERSION','protocol!=(APP_PROTOCOL_VERSION+1)'],
    ['reply-app','memcpy(server_version,expected,sizeof(expected));',"memcpy(server_version,expected,sizeof(expected)); server_version[0]='9';"],
    ['reply-protocol','*server_protocol=APP_PROTOCOL_VERSION;','*server_protocol=APP_PROTOCOL_VERSION+1;'],
    ['legacy-interface',null,null]
];
try {
    fs.writeFileSync(path.join(build,'legacy.idl'),idl.replace(`version(${protocol}.0)`,'version(1.0)'));
    compile('midl.exe /nologo /env win32 /target NT100 /prefix client Client_ /prefix server Server_ /h legacy.h /cstub legacy_c.c /sstub legacy_s.c legacy.idl');
    compile(`cl.exe /nologo /c /MT /W4 /I "${product}/obj/basesrv" /I "${root}/src" legacy_s.c /Folegacy-stub.obj`);
    for (const [name,from,to] of variants) {
        if(from) assert.equal(source.split(from).length,2,`Unique mutation: ${name}`);
        const body=from?source.replace(from,to):source.replace('"service.h"','"legacy.h"').replaceAll(`Server_vdm_service_v${protocol}_0_s_ifspec`,'Server_vdm_service_v1_0_s_ifspec');
        fs.writeFileSync(path.join(build,`${name}.c`),body);
        compile(`cl.exe /nologo /c /MT /W4 /we4013 /I "${product}/obj/basesrv" /I "${root}/src" ${name}.c /Fo${name}.obj`);
        const stub=name==='legacy-interface'?'legacy-stub.obj':`"${product}/obj/basesrv/stub.obj"`;
        compile(`link.exe /nologo /opt:ref /out:${name}.exe ${name}.obj ${stub} "${product}/obj/basesrv/console_query.obj" "${product}/obj/run16/support.obj" "${product}/opennt-base-server.lib" "${product}/opennt-base-bindings.lib" "${product}/broker-transport.lib" "${product}/original-opennt-rtl-x86.lib" rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib`);
    }
} finally {fs.closeSync(compileLog);}
for (const [name] of variants) {
    const trace=path.join(logs,`${name}.trace.log`);
    assert(!fs.existsSync(trace),'Use a fresh observation directory');
    const server=spawn(path.join(build,`${name}.exe`),[],{cwd:build,windowsHide:true,
        env:{...process.env,MVDM_BASESRV_TRACE_PATH:trace},stdio:['ignore','pipe','pipe']});
    let output='',timer;
    const done=new Promise(resolve=>server.once('close',resolve));
    server.stdout.on('data',d=>output+=d);server.stderr.on('data',d=>output+=d);
    try {
        await new Promise((resolve,reject)=>{
            timer=setTimeout(()=>reject(Error('Owned server readiness timeout')),10000);
            server.once('error',reject);
            server.once('exit',code=>reject(Error(`Owned server exited ${code}; do not use an existing broker`)));
            server.stdout.on('data',()=>{if(output.includes('basesrv: listening'))resolve();});
        });
        clearTimeout(timer);
        for(const [exe,args] of [['run16.exe',['MEM.EXE']],['ntvdm.exe',[]]]) {
            const start=Date.now();
            const r=spawnSync(`O:/winnt/${exe}`,args,{cwd:'O:/winnt',windowsHide:true,encoding:'utf8',timeout:4000});
            fs.writeFileSync(path.join(logs,`${name}-${exe}.json`),JSON.stringify({status:r.status,error:r.error?.message,ms:Date.now()-start,stdout:r.stdout,stderr:r.stderr},null,2));
            assert.equal(r.status,1306,`${name} ${exe}: expected ERROR_REVISION_MISMATCH, not retry/timeout`);
            assert.match(r.stderr,/version mismatch/);
        }
    } finally {
        clearTimeout(timer);if(server.exitCode===null)server.kill();await done;
        fs.writeFileSync(path.join(logs,`${name}-server.log`),output);
    }
    const events=fs.existsSync(trace)?fs.readFileSync(trace,'utf8'):'';
    assert(!/phase=(?:check|get|prepare|reserve|update)\b/.test(events),'No task admitted by rejected peer');
    if(name.startsWith('reply-')) assert.match(events,/disconnect/,'Rejected successful context must be disconnected');
    console.log(`PASS ${name}: launcher and worker reject before task delivery, no launcher retry`);
}
assert.equal(fs.readFileSync('src/app/basesrv_entry.c','utf8'),source,'Production source must be unchanged');
