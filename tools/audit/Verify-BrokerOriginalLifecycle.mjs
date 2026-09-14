import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
const root=process.cwd(), build=path.resolve('build/M0-T412/S3/original-lifecycle');
const ownerBuild=process.env.OPENNT_BROKER_OWNER_BUILD ? path.resolve(process.env.OPENNT_BROKER_OWNER_BUILD) : null;
fs.mkdirSync(build,{recursive:true});
const graph=fs.readFileSync('build/M0-T412/S1/text-cell-repair/product/build.ninja','utf8');
const flags=graph.match(/^cflags = (.*)$/m)[1].replaceAll('$:',':');
const rtlFlags=graph.match(/^build obj\/opennt-rtl\/error\.obj:.*\r?\n  rtl_cflags = (.*)$/m)[1].replaceAll('$:',':');
const ownerPath=path.resolve('src/opennt-host/base/win32/server/srvvdm.c');
const ownerBefore=fs.readFileSync(ownerPath);
const clientSource=fs.readFileSync('src/opennt-host/base/win32/client/vdm.c','utf8');
const configPattern=/BOOL\r?\nBaseGetVdmConfigInfo\([\s\S]*?\r?\n}/;
const configOriginal=fs.readFileSync('O:/repos.external/OpenNT/base/win32/client/vdm.c','utf8').match(configPattern)[0].replace(/\r\n/g,'\n');
assert.equal(createHash('sha256').update(configOriginal).digest('hex'),'2d57d7b0edaf2534e9a5654d3a2e30a6d91e2a24aea04fcc9e282af747a8c667');
assert.equal(clientSource.match(configPattern)[0].replace(/\r\n/g,'\n'),
    configOriginal.replace('pch = strstr(pSrc, "\\\\system32\\\\ntvdm");','pch = OpenNtBaseVdmImageEnd(pSrc);'));
const exitPattern=/BOOL\r?\nBaseCheckForVDM\([\s\S]*?\r?\n}/;
assert.equal(createHash('sha256').update(clientSource.match(exitPattern)[0].replace(/\r\n/g,'\n')).digest('hex'),
    'c6cf2dfdab281ee73c257c5f0a051eebe3439355f273d4a2a5eae5fa7e9d7b85');
assert.equal(clientSource.match(exitPattern)[0].replace(/\r\n/g,'\n'),
    fs.readFileSync('O:/repos.external/OpenNT/base/win32/client/vdm.c','utf8').match(exitPattern)[0].replace(/\r\n/g,'\n'));
const capturePattern=/PCSR_CAPTURE_HEADER\r?\nCsrAllocateCaptureBuffer\([\s\S]*?ULONG\r?\nCsrAllocateMessagePointer\([\s\S]*?\r?\n}/;
const captureBody=fs.readFileSync('src/opennt-host/base/ntdll/csrutil.c','utf8').match(capturePattern)[0].replace(/\r\n/g,'\n');
assert.equal(captureBody,fs.readFileSync('O:/repos.external/OpenNT/base/ntdll/csrutil.c','utf8').match(capturePattern)[0].replace(/\r\n/g,'\n'));
assert.equal(createHash('sha256').update(captureBody).digest('hex'),'41d54b575c6895809c956136c3a35067dc3b87610a6226d8ec23fab431e11cab');
const clientBody=clientSource.match(/BOOL\r?\nAPIENTRY\r?\nGetNextVDMCommand\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
assert.equal(createHash('sha256').update(clientBody).digest('hex'),
    '47cf285cbc505d9d43a50af16e283ea51d1d9ef8d2ff093795086d0ca327addd','Original client body changed');
const lifecycleBody=clientSource.match(/VOID\r?\nAPIENTRY\r?\nExitVDM\([\s\S]*?VOID\r?\nAPIENTRY\r?\nRegisterWowExec\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
assert.equal(createHash('sha256').update(lifecycleBody).digest('hex'),
    '2936de23dcbbdec23cf9ea23416606a7a3faeb96473d407e4bfaee00f89ac3aa','Original client lifecycle group changed');
const updateBody=clientSource.match(/BOOL\r?\nBaseUpdateVDMEntry\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
const checkBody=clientSource.match(/BOOL\r?\nBaseCheckVDM\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
const checkOriginal=fs.readFileSync('O:/repos.external/OpenNT/base/win32/client/vdm.c','utf8').match(/BOOL\r?\nBaseCheckVDM\([\s\S]*?\r?\n}\r?\n/)[0].replace(/\r\n/g,'\n');
assert.equal(checkBody,checkOriginal,'Original launch body changed');
const tokenPattern=/NTSTATUS\r?\n_UserTestTokenForInteractive\([\s\S]*?\r?\n}/;
const tokenSource=fs.readFileSync('src/opennt-host/windows/core/ntuser/server/exports.c','utf8').match(tokenPattern)[0].replace(/\r\n/g,'\n');
assert.equal(createHash('sha256').update(tokenSource).digest('hex'),'7b6c9072220144e4b77aca973682838333994c3bc962eb802143e210e912a520','Pinned token helper changed');
assert.equal(tokenSource,fs.readFileSync('O:/repos.external/OpenNT/windows/core/ntuser/server/exports.c','utf8').match(tokenPattern)[0].replace(/\r\n/g,'\n'),'Original token helper changed');
assert.equal(createHash('sha256').update(checkBody).digest('hex'),'579840e2d5598051d157d9e92871027b543ecfcba8d299a745928bfd71c6386e','Pinned launch body changed');
assert.equal(createHash('sha256').update(updateBody).digest('hex'),
    'de56cfccd08922ae57df7332cc8df70f60db186f44f177336188c2d236e9fd26','Original update body changed');
const env=path.join(build,'msvc.cmd');
fs.writeFileSync(env,'@echo off\r\nset "lifecycle_cwd=%CD%"\r\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%lifecycle_cwd%"\r\n%*\r\n');
const includes=`/D_CSRSRV_ /DOPENNT_BASE_VDM_SERVER /we4013 /FI "${root}/src/adapter-opennt-host/basesrv/include/base_server.h" /I "${root}/src/adapter-opennt-host/basesrv/include" /I "${root}/src/opennt-host/base/win32/inc" /I "${root}/src/opennt-host/base/win32/server"`;
let commands=[
    `cl.exe ${flags} ${includes} /Fo"${build}/command.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_command.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/values.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_values.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/payload.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_payload.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/startup.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_startup.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/dispatch.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_dispatch.c"`,
    `cl.exe ${flags} ${includes} /DOPENNT_BASE_NATIVE_RESOURCES /Fo"${build}/resources.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_resource.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/registry.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_process.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/request.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_request.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/config.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_config.c"`,
    `cl.exe ${flags} /Fo"${build}/environment-check.obj" "${root}/src/opennt-host/base/win32/client/vdm.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/srvvdm.obj" "${root}/src/opennt-host/base/win32/server/srvvdm.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/fixture.obj" "${root}/tests/broker/original_server_lifecycle.c"`,
    `cl.exe ${flags} /Fo"${build}/process.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_client_process.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/interactive.obj" "${root}/src/adapter-opennt-host/basesrv/source/base_interactive.c"`,
    `cl.exe ${flags} ${includes} /Fo"${build}/exports.obj" "${root}/src/opennt-host/windows/core/ntuser/server/exports.c"`,
    `cl.exe ${flags} ${includes} /Gz /Fo"${build}/capture.obj" "${root}/src/opennt-host/base/ntdll/csrutil.c"`,
    `cl.exe ${flags} ${includes} /Gy /DOPENNT_BASE_CLIENT_VDM_COMMANDS /Fo"${build}/client.obj" "${root}/src/opennt-host/base/win32/client/vdm.c"`,
    `cl.exe ${rtlFlags} /Fo"${build}/error.obj" "${root}/src/opennt-host/base/ntos/rtl/error.c"`,
    'lib.exe /nologo /out:opennt-base-server.lib srvvdm.obj exports.obj',
    'lib.exe /nologo /out:opennt-base-client.lib client.obj capture.obj',
    'link.exe /nologo /opt:ref /out:original-lifecycle.exe /map:original-lifecycle.map fixture.obj opennt-base-server.lib opennt-base-client.lib request.obj config.obj process.obj interactive.obj error.obj ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib'
];
if(ownerBuild) {
    for(const name of ['opennt-base-client.lib','opennt-base-server.lib','opennt-base-bindings.lib'])
        assert(fs.existsSync(path.join(ownerBuild,name)),`Missing formal owner archive ${name}`);
    commands=[
        `cl.exe ${flags} ${includes} /Fo"${build}/fixture.obj" "${root}/tests/broker/original_server_lifecycle.c"`,
        `cl.exe ${rtlFlags} /Fo"${build}/error.obj" "${root}/src/opennt-host/base/ntos/rtl/error.c"`,
        `link.exe /nologo /opt:ref /out:original-lifecycle.exe /map:original-lifecycle.map fixture.obj "${ownerBuild}/opennt-base-server.lib" "${ownerBuild}/opennt-base-client.lib" "${ownerBuild}/opennt-base-bindings.lib" error.obj ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib`
    ];
}
commands.unshift(`cl.exe ${flags} /Gy /Fo"${build}/support.obj" "${root}/src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c"`);
const transportBuild=ownerBuild || path.resolve('build/M0-T412/S3/product');
assert(fs.existsSync(path.join(transportBuild,'broker-transport.lib')),'Build the formal transport archive first');
commands=commands.map(command=>command.startsWith('link.exe ')?command.replace(' fixture.obj ', ` fixture.obj support.obj "${transportBuild}/broker-transport.lib" ${ownerBuild?'':'registry.obj resources.obj dispatch.obj startup.obj payload.obj values.obj command.obj '}`):command);
const log=fs.openSync(path.join(build,'build.log'),'w');
for(const command of commands) {
    const result=spawnSync('cmd.exe',['/d','/c',`call "${env}" ${command}`],{cwd:build,windowsHide:true,windowsVerbatimArguments:true,stdio:['ignore',log,log],timeout:60000});
    if(result.status!==0) {fs.closeSync(log); throw Error(`Build failed ${result.status}; see build.log`);}
}
fs.closeSync(log);
const buildLog=fs.readFileSync(path.join(build,'build.log'),'utf8');
for(const retired of ['source_compile_probe.h','ntdddfs.h','luid.c'])
    assert(!buildLog.includes(retired),`Retired test build dependency: ${retired}`);
const inlinePattern=/__inline LUID\s+NTAPI\s+RtlConvertLongToLuid\([\s\S]*?\n\}/;
const normalizeInline=text=>text.match(inlinePattern)[0].split(/\r?\n/).map(line=>line.trimEnd()).join('\n');
assert.equal(normalizeInline(fs.readFileSync('src/opennt-host/public/sdk/inc/ntrtl.h','utf8')),
    normalizeInline(fs.readFileSync('O:/repos.external/OpenNT/public/sdk/inc/ntrtl.h','utf8')),'Original LUID inline changed');
assert.deepEqual(fs.readFileSync(ownerPath),ownerBefore,'Original owner changed during test');
const image=fs.readFileSync(path.join(build,'original-lifecycle.exe'));
assert.equal(image.readUInt16LE(image.readUInt32LE(0x3c)+4),0x14c);
const map=fs.readFileSync(path.join(build,'original-lifecycle.map'),'utf8');
for (const symbol of ['_OpenNtBaseEncodeCheckCommand','_OpenNtBaseDecodeCheckCommand',
    '_OpenNtBaseEncodeUpdateCommand','_OpenNtBaseDecodeUpdateCommand',
    '_OpenNtBaseEncodeGetCommand','_OpenNtBasePrepareGetCommand','_OpenNtBaseFinishGetCommand',
    '_OpenNtBaseApplyGetCommand','_OpenNtBaseReleaseGetCommand'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(ownerBuild?'opennt-base-bindings:command.obj':'command.obj')),`Command binding provider missing: ${symbol}`);
for (const symbol of ['_OpenNtBaseEncodeValues','_OpenNtBaseDecodeValues'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(ownerBuild?'opennt-base-bindings:values.obj':'values.obj')),`Scalar binding provider missing: ${symbol}`);
for (const symbol of ['_OpenNtBaseEncodeCheckPayload','_OpenNtBaseDecodeCheckPayload','_OpenNtBaseApplyGetPayload',
    '_OpenNtBaseEncodeGetRequest','_OpenNtBasePrepareGetPayload','_OpenNtBaseFinishGetPayload','_OpenNtBaseReleaseGetPayload'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(ownerBuild?'opennt-base-bindings:payload.obj':'payload.obj')),`Native payload provider missing: ${symbol}`);
assert(map.split(/\r?\n/).some(line=>line.includes('_OpenNtBaseDispatch')&&line.includes(ownerBuild?'opennt-base-bindings:dispatch.obj':'dispatch.obj')),'Production dispatch provider missing');
for (const symbol of ['_OpenNtBaseVdmOperation','_OpenNtBaseDispatchOperation'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(ownerBuild?'opennt-base-bindings:dispatch.obj':'dispatch.obj')),`Wire operation binding missing: ${symbol}`);
for (const symbol of ['_OpenNtBaseEncodeStartup','_OpenNtBaseDecodeStartup'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(ownerBuild?'opennt-base-bindings:startup.obj':'startup.obj')),`Startup provider missing: ${symbol}`);
for (const [symbol,unit] of [['_broker_vdm_delivery_rollback','vdm_delivery'],['_broker_vdm_receipt_accept','vdm_receipt']])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(`broker-transport:${unit}.obj`)),`Missing formal transport provider: ${symbol}`);
assert(map.includes('opennt-base-server:srvvdm.obj')&&map.includes('opennt-base-client:client.obj'),'Original owner libraries not selected');
assert(!map.includes('luid.obj'),'Generated inline replacement still linked');
for (const symbol of ['_NtCurrentPeb@0','_opennt_support_current_teb@0','_RtlProcessHeap@0'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('support.obj')),`Real process support missing: ${symbol}`);
for (const symbol of ['_OpenNtBaseDuplicateObject@28','_OpenNtBaseClose@4'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('resources.obj')),`Resource binding missing: ${symbol}`);
for (const symbol of ['_CsrLockProcessByClientId@8','_CsrUnlockProcess@4'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('registry.obj')),`Real registered-process binding missing: ${symbol}`);
for (const symbol of ['_OpenNtBaseServerRequestThread','_OpenNtBaseBindServerRequestThread'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('request.obj')),`Real request binding missing: ${symbol}`);
for(const symbol of ['_CsrAllocateCaptureBuffer@12','_CsrAllocateMessagePointer@12','_CsrFreeCaptureBuffer@4'])
    assert(map.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('opennt-base-client:capture.obj')),`Original capture provider missing: ${symbol}`);
assert(map.split(/\r?\n/).some(line=>line.includes('__UserTestTokenForInteractive')&&line.includes('opennt-base-server:exports.obj')),'Original token provider missing');
assert(map.split(/\r?\n/).some(line=>line.includes('_GetNextVDMCommand@4')&&line.includes('client.obj')),'Original client provider missing');
assert(map.split(/\r?\n/).some(line=>line.includes('_BaseUpdateVDMEntry')&&line.includes('client.obj')),'Original update provider missing');
assert(map.split(/\r?\n/).some(line=>line.includes('_BaseCheckVDM')&&line.includes('client.obj')),'Original launch provider missing');
for (const symbol of ['ExitVDM','SetVDMCurrentDirectories','GetVDMCurrentDirectories','CmdBatNotification','RegisterWowExec'])
    assert(map.split(/\r?\n/).some(line=>line.includes(`_${symbol}@`)&&line.includes('client.obj')),`Original client provider missing: ${symbol}`);
for(const symbol of ['BaseSrvCheckVDM','BaseSrvGetNextVDMCommand','BaseSrvSetReenterCount','BaseSrvExitDOSTask','BaseSrvUpdateVDMSequenceNumber','BaseSrvCleanupVDMResources'])
    assert(map.split(/\r?\n/).some(line=>line.includes(`_${symbol}`)&&line.includes('srvvdm.obj')),`Original provider missing for ${symbol}`);
assert(map.split(/\r?\n/).some(line=>line.includes('_BaseGetVdmConfigInfo')&&line.includes('client.obj')),'Original worker configuration provider missing');
assert(map.split(/\r?\n/).some(line=>line.includes('_BaseCheckForVDM')&&line.includes('client.obj')),'Original task-exit provider missing');
const result=spawnSync(path.join(build,'original-lifecycle.exe'),[],{cwd:build,windowsHide:true,encoding:'utf8',timeout:15000});
const processSupport='src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c';
fs.writeFileSync(path.join(build,'result.json'),JSON.stringify({ownerBuild,
    processSupport:{path:processSupport,sha256:createHash('sha256').update(fs.readFileSync(processSupport)).digest('hex')},
    consoleAssociation:'fixture-local only; not authenticated product Console identity',
    status:result.status,stdout:result.stdout,stderr:result.stderr,error:result.error?.message},null,2));
console.log(result.stdout,result.stderr);
assert.equal(result.status,0,'Original lifecycle fixture failed');
console.log('PASS: original BaseClient capture/copy, directories, exit, BAT/WOW registration, real wait/wake/retry with cleared exit code; captures drained');
