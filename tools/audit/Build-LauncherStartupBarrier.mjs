// Test-only deterministic launch cut points. No hook enters a product binary.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
const root=process.cwd();
const product=path.resolve('build/M0-T412/S10/process-loss');
const build=path.resolve('build/M0-T412/S10/startup-barrier');
fs.mkdirSync(build,{recursive:true});
const original=fs.readFileSync('src/app/run16_entry.c','utf8');
const graph=fs.readFileSync(path.join(product,'build.ninja'),'utf8');
const flags=graph.match(/^build obj\/run16\/entry.obj:.*\r?\n  cflags = (.*)$/m)[1].replaceAll('$:',':');
const barrier=`
    {
        HANDLE ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,L"Local\\\\MvdmLaunchReady");
        HANDLE release=OpenEventW(SYNCHRONIZE,FALSE,L"Local\\\\MvdmLaunchRelease");
        if (!ready || !release) ExitProcess(90);
        SetEvent(ready);
        if (WaitForSingleObject(release,20000)!=WAIT_OBJECT_0) ExitProcess(91);
        CloseHandle(ready); CloseHandle(release);
    }
`;
const cmd=path.join(build,'compile.cmd');
fs.writeFileSync(cmd,'@echo off\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\n%*\r\n');
const log=fs.openSync(path.join(build,'build.log'),'w');
function compile(command){
    const r=spawnSync('cmd.exe',['/d','/c',`call "${cmd}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
    assert.equal(r.status,0,'See startup-barrier/build.log');
}
try {
    for(const [name,marker,after] of [
        ['before','    result = OpenNtBaseClientPrepareWorker(reservation, worker.hProcess);',false],
        ['after','    CloseHandle(startup_job);startup_job=NULL;',true]]) {
        assert.equal(original.split(marker).length,2);
        fs.writeFileSync(path.join(build,`${name}.c`),original.replace(marker,after?marker+barrier:barrier+marker));
        compile(`cl.exe ${flags} ${name}.c /Fo${name}.obj`);
        const objects=['obj/run16/console_probe.obj','obj/run16/support.obj','obj/run16/rpc_client.obj','obj/run16/stub.obj','opennt-base-client.lib','opennt-base-bindings.lib','broker-transport.lib','original-opennt-rtl-x86.lib'].map(p=>`"${product}/${p}"`).join(' ');
        compile(`link.exe /nologo /subsystem:console /entry:wWinMainCRTStartup /opt:ref /out:${name}.exe ${name}.obj ${objects} rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib`);
    }
    for(const name of ['run16.exe','basesrv.exe','ntvdm.exe'])fs.copyFileSync(path.join(product,name),path.join(build,name));
} finally {fs.closeSync(log);}
assert.equal(fs.readFileSync('src/app/run16_entry.c','utf8'),original);
console.log('PASS: two build-only launcher cut points; production source unchanged');
