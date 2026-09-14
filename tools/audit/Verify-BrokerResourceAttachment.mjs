import fs from 'node:fs';
import path from 'node:path';
import {spawn, spawnSync} from 'node:child_process';
const root = process.cwd();
const build = path.resolve('build/M0-T412/S3/rpc-resource');
fs.mkdirSync(build, {recursive: true});
const environment = path.join(build, 'msvc-mt.cmd');
fs.writeFileSync(environment, [
    '@echo off', 'set "RESOURCE_FIXTURE_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
    'call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
    'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%RESOURCE_FIXTURE_CWD%"', '%*', ''
].join('\r\n'));
const source = path.join(root, 'tests/broker/resource_attachment.c');
const idl = path.join(root, 'tests/broker/resource_attachment.idl');
const log = fs.openSync(path.join(build, 'build.log'), 'w');
const securityInclude = `/I "${root}/src/broker"`;
const ownerBuild=path.resolve('build/M0-T412/S3/product');
const ownerGraph=fs.readFileSync(path.join(ownerBuild,'build.ninja'),'utf8');
const registryFlags=ownerGraph.match(/^build obj\/opennt-base-bindings\/registry.obj:.*\r?\n  cflags = (.*)$/m)[1].replaceAll('$:',':');
const commands = [
    `cl.exe ${registryFlags} "${root}/tests/broker/resource_registration.c" /Foregistration.obj`,
    `cl.exe /nologo /MT /W4 /c "${root}/src/broker/rpc_security.c" /Fosecurity.obj`,
    `midl.exe /nologo /env win32 /target NT100 /prefix client Client_ /prefix server Server_ /out . "${idl}"`,
    `cl.exe /nologo /MT /W4 /DRESOURCE_SERVER /I . ${securityInclude} /c "${source}" /Foserver.obj`,
    'cl.exe /nologo /MT /W4 /I . /c resource_attachment_s.c /Foserver-stub.obj',
    `cl.exe /nologo /MT /W4 /I . ${securityInclude} /c "${source}" /Foclient.obj`,
    'cl.exe /nologo /MT /W4 /I . /c resource_attachment_c.c /Foclient-stub.obj',
    `link.exe /nologo /out:resource-server.exe /map:resource-server.map server.obj security.obj registration.obj "${ownerBuild}/opennt-base-bindings.lib" server-stub.obj client-stub.obj rpcrt4.lib advapi32.lib kernel32.lib`,
    'link.exe /nologo /out:resource-client.exe client.obj client-stub.obj rpcrt4.lib advapi32.lib kernel32.lib',
];
for (const command of commands) {
    const result = spawnSync('cmd.exe', ['/d', '/c', `call "${environment}" ${command}`],
        {cwd: build, windowsHide: true, windowsVerbatimArguments: true,
            stdio: ['ignore', log, log], timeout: 60000});
    if (result.status !== 0) { fs.closeSync(log); throw Error(`Build failed (${result.status}): ${command}; see ${build}/build.log`); }
}
fs.closeSync(log);
const serverMap=fs.readFileSync(path.join(build,'resource-server.map'),'utf8');
for (const symbol of ['_OpenNtBaseRegisterProcess','_OpenNtBaseRemoveProcess','_CsrLockProcessByClientId@8'])
    if (!serverMap.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('opennt-base-bindings:registry.obj')))
        throw Error(`Formal registered-process provider missing: ${symbol}`);
for (const name of ['resource-server.exe', 'resource-client.exe']) {
    const image = fs.readFileSync(path.join(build, name));
    if (image.readUInt16LE(image.readUInt32LE(0x3c) + 4) !== 0x14c) throw Error('Not x86');
}
function launchServer(endpoint, downstream) {
const server = spawn(path.join(build, 'resource-server.exe'), [endpoint, ...(downstream ? [downstream] : [])],
    {cwd: build, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe']});
const serverDone = new Promise(resolve => server.once('close', code => resolve(code)));
let stdout = '', stderr = '';
server.stdout.on('data', data => stdout += data);
server.stderr.on('data', data => stderr += data);
const ready = new Promise((resolve, reject) => {
        const timeout = setTimeout(() => reject(Error('Server readiness timeout')), 10000);
        server.once('error', error => {clearTimeout(timeout); reject(error);});
        server.once('exit', code => {clearTimeout(timeout); reject(Error(`Server exited ${code}: ${stderr}`));});
        server.stdout.on('data', () => {if (stdout.includes('READY')) {clearTimeout(timeout); resolve();}});
    });
return {server, serverDone, ready, transcript() { return stdout + stderr; }, async cleanup(name) {
    if (server.exitCode === null) server.kill();
    await serverDone;
    fs.writeFileSync(path.join(build, `${name}-server.log`), stdout + stderr);
    if (name.startsWith('wrong-') && stdout.includes('REGISTER status='))
        throw Error('Rejected caller reached process registration');
    if (name === 'wrong-peer' && !stdout.includes('PEER status=5 matched=0'))
        throw Error('Wrong process was not rejected by the peer-process binding');
}};
}
async function finished(child) {
    let stopTimer;
    try {
        const code = await Promise.race([child.serverDone,
            new Promise((_, reject) => {stopTimer = setTimeout(() => reject(Error('Server shutdown timeout')), 5000);})]);
        if (code !== 0) throw Error(`Server exit ${code}`);
        return code;
    } finally {clearTimeout(stopTimer);}
}
async function runCase(mode) {
const endpoint = `ntvdm-resource-fixture-${process.pid}-${Date.now()}-${mode}`;
let target, relay;
try {
    if (mode.startsWith('relay')) {
        target = launchServer(`${endpoint}-target`);
        await target.ready;
    }
    relay = launchServer(endpoint, target ? `${endpoint}-target` :
        mode === 'unavailable' ? `${endpoint}-absent` : mode === 'wrong-scope' ? 'deny-scope' : mode === 'wrong-session' ? 'deny-session' : undefined);
    await relay.ready;
    const result = spawnSync(path.join(build, 'resource-client.exe'),
        [endpoint, path.join(build, `${endpoint}.tmp`), ...(mode.endsWith('readonly') ? ['readonly'] :
            mode === 'unavailable' ? ['unavailable'] : mode === 'wrong-peer' ? ['denied', String(process.pid)] : mode.startsWith('wrong-') ? ['denied'] : mode === 'low-auth' ? ['low-auth'] : [])],
        {cwd: build, windowsHide: true, encoding: 'utf8', timeout: 15000});
    const record = {clientStatus: result.status, clientOutput: result.stdout,
        clientError: result.stderr, spawnError: result.error?.message};
    fs.writeFileSync(path.join(build, `${mode}-result.json`), JSON.stringify(record, null, 2));
    if (result.status !== 0) throw Error('Resource attachment fixture failed');
    if (!mode.startsWith('wrong-')) record.serverStatus = await finished(relay);
    else {
        if (relay.server.exitCode !== null) throw Error('Denied call unexpectedly stopped server');
        record.serverStatus = 'still listening; owned fixture terminated in cleanup';
    }
    if (mode === 'low-auth') {
        if (!relay.transcript().includes('AUTH level=6 service=10')) throw Error('Requested integrity was not observed as WINNT packet privacy');
        record.observedAuthentication = 'requested integrity; server observed WINNT packet privacy';
    }
    if (target) record.targetStatus = await finished(target);
    if (!mode.startsWith('wrong-')) {
        for (const instance of [relay,...(target?[target]:[])])
            if (!instance.transcript().includes('REGISTER status=0') ||
                !instance.transcript().includes('REGISTER retained-after-call=1 drained=1'))
                throw Error('Registered process did not survive RPC attachment lifetime and drain');
    }
    console.log(JSON.stringify(record));
    fs.writeFileSync(path.join(build, `${mode}-result.json`), JSON.stringify(record, null, 2));
} finally {
    // Only this fixture's owned child, never process-name enumeration.
    await relay?.cleanup(mode);
    await target?.cleanup(`${mode}-target`);
}
}
for (const mode of ['shared', 'readonly', 'relay', 'relay-readonly', 'unavailable', 'wrong-scope', 'wrong-session', 'low-auth', 'wrong-peer']) await runCase(mode);
