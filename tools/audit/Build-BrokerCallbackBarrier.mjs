// Test-only callback barrier: production source and published EXEs stay intact.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
const root=process.cwd();
const product=path.resolve(process.env.OPENNT_BROKER_PRODUCT_BUILD || 'build/M0-T412/S10/final');
const build=path.resolve('build/M0-T412/S10/callback-barrier');
fs.mkdirSync(build,{recursive:true});
const source=fs.readFileSync('src/app/basesrv_entry.c','utf8');
const marker='    AcquireSRWLockExclusive(&idle_lock);\n    /* Cancellation';
const normalized=source.replaceAll('\r\n','\n');
assert.equal(normalized.split(marker).length,2);
const barrier=`    AcquireSRWLockExclusive(&idle_lock);
    {
        HANDLE ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,L"Local\\\\MvdmS10TimerReady");
        HANDLE release=OpenEventW(SYNCHRONIZE,FALSE,L"Local\\\\MvdmS10TimerRelease");
        if (!ready || !release) ExitProcess(90);
        SetEvent(ready);
        if (WaitForSingleObject(release,15000)!=WAIT_OBJECT_0) ExitProcess(91);
        CloseHandle(release); CloseHandle(ready);
    }
    /* Cancellation`;
fs.writeFileSync(path.join(build,'barrier.c'),normalized.replace(marker,barrier));
const cmd=path.join(build,'compile.cmd');
fs.writeFileSync(cmd,`@echo off\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncl.exe /nologo /c /MT /W4 /we4013 /I "${product}/obj/basesrv" /I "${root}/src" barrier.c /Fobarrier.obj\r\nif errorlevel 1 exit /b %errorlevel%\r\nlink.exe /nologo /opt:ref /out:barrier.exe barrier.obj "${product}/obj/basesrv/stub.obj" "${product}/obj/basesrv/console_query.obj" "${product}/obj/run16/support.obj" "${product}/opennt-base-server.lib" "${product}/opennt-base-bindings.lib" "${product}/broker-transport.lib" "${product}/original-opennt-rtl-x86.lib" rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib\r\n`);
const log=fs.openSync(path.join(build,'build.log'),'w');
try {
    const r=spawnSync('cmd.exe',['/d','/c',`call "${cmd}"`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
    assert.equal(r.status,0,'Barrier compilation failed; see build.log');
} finally {fs.closeSync(log);}
assert.equal(fs.readFileSync('src/app/basesrv_entry.c','utf8'),source);
console.log(`PASS: test-only barrier compiled from production callback: ${build}`);
