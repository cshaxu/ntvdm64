import fs from 'node:fs';
import path from 'node:path';
import {spawn, spawnSync} from 'node:child_process';
const root = process.cwd();
const build = path.resolve('build/M0-T412/S1/rpc-resource');
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
const commands = [
    `midl.exe /nologo /env win32 /target NT100 /out . "${idl}"`,
    `cl.exe /nologo /MT /W4 /DRESOURCE_SERVER /I . /c "${source}" /Foserver.obj`,
    'cl.exe /nologo /MT /W4 /I . /c resource_attachment_s.c /Foserver-stub.obj',
    'link.exe /nologo /out:resource-server.exe server.obj server-stub.obj rpcrt4.lib advapi32.lib kernel32.lib',
    `cl.exe /nologo /MT /W4 /I . /c "${source}" /Foclient.obj`,
    'cl.exe /nologo /MT /W4 /I . /c resource_attachment_c.c /Foclient-stub.obj',
    'link.exe /nologo /out:resource-client.exe client.obj client-stub.obj rpcrt4.lib advapi32.lib kernel32.lib',
];
for (const command of commands) {
    const result = spawnSync('cmd.exe', ['/d', '/c', `call "${environment}" ${command}`],
        {cwd: build, windowsHide: true, windowsVerbatimArguments: true,
            stdio: ['ignore', log, log], timeout: 60000});
    if (result.status !== 0) { fs.closeSync(log); throw Error(`Build failed (${result.status}): ${command}; see ${build}/build.log`); }
}
fs.closeSync(log);
for (const name of ['resource-server.exe', 'resource-client.exe']) {
    const image = fs.readFileSync(path.join(build, name));
    if (image.readUInt16LE(image.readUInt32LE(0x3c) + 4) !== 0x14c) throw Error('Not x86');
}
async function runCase(mode) {
const endpoint = `ntvdm-resource-fixture-${process.pid}-${Date.now()}-${mode}`;
const server = spawn(path.join(build, 'resource-server.exe'), [endpoint],
    {cwd: build, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe']});
const serverDone = new Promise(resolve => server.once('exit', code => resolve(code)));
let stdout = '', stderr = '';
server.stdout.on('data', data => stdout += data);
server.stderr.on('data', data => stderr += data);
try {
    await new Promise((resolve, reject) => {
        const timeout = setTimeout(() => reject(Error('Server readiness timeout')), 10000);
        server.once('error', error => {clearTimeout(timeout); reject(error);});
        server.once('exit', code => {clearTimeout(timeout); reject(Error(`Server exited ${code}: ${stderr}`));});
        server.stdout.on('data', () => {if (stdout.includes('READY')) {clearTimeout(timeout); resolve();}});
    });
    const result = spawnSync(path.join(build, 'resource-client.exe'),
        [endpoint, path.join(build, `${endpoint}.tmp`), ...(mode === 'readonly' ? ['readonly'] : [])],
        {cwd: build, windowsHide: true, encoding: 'utf8', timeout: 15000});
    const record = {clientStatus: result.status, clientOutput: result.stdout,
        clientError: result.stderr, spawnError: result.error?.message};
    fs.writeFileSync(path.join(build, `${mode}-result.json`), JSON.stringify(record, null, 2));
    if (result.status !== 0) throw Error('Resource attachment fixture failed');
    let stopTimer;
    try {
        record.serverStatus = await Promise.race([serverDone,
            new Promise((_, reject) => {stopTimer = setTimeout(() => reject(Error('Server shutdown timeout')), 5000);})]);
    } finally {clearTimeout(stopTimer);}
    console.log(JSON.stringify(record));
    fs.writeFileSync(path.join(build, `${mode}-result.json`), JSON.stringify(record, null, 2));
    if (record.serverStatus !== 0) throw Error('Server failed to exit normally');
} finally {
    // Only this fixture's owned child, never process-name enumeration.
    if (server.exitCode === null) server.kill();
    fs.writeFileSync(path.join(build, `${mode}-server.log`), stdout + stderr);
}
}
for (const mode of ['shared', 'readonly']) await runCase(mode);
