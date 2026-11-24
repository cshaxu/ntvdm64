import fs from 'node:fs';
import path from 'node:path';
import {spawn, spawnSync} from 'node:child_process';
// Runtime/assertion runner only. Build first with the CMD/native target.
const [binaryRoot, outputRoot] = process.argv.slice(2);
if (!binaryRoot || !outputRoot) throw Error('Pass native resource-tests directory and fresh output root');
const binaries = path.resolve(binaryRoot), build = path.resolve(outputRoot);
if (!build.startsWith(path.resolve('build') + path.sep) || fs.existsSync(build))
    throw Error('Require a fresh build-owned report directory');
fs.mkdirSync(build, {recursive: true});
const serverMap=fs.readFileSync(path.join(binaries,'resource-server.map'),'utf8');
for (const [symbol,unit] of [['_BaseSrvIsFirstVDM','opennt-base-server:srvvdm.obj'],['_OpenNtBaseDispatchOperation','opennt-base-bindings:dispatch.obj']])
    if (!serverMap.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(unit)))
        throw Error(`Original service dispatch provider missing: ${symbol}`);
for (const [symbol,unit] of [['_broker_rpc_peer_process','rpc_security'],['_broker_vdm_receipt_accept','vdm_receipt']])
    if (!serverMap.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes(`broker-transport:${unit}.obj`)))
        throw Error(`Formal transport provider missing: ${symbol}`);
for (const symbol of ['_OpenNtBaseRegisterProcess','_OpenNtBaseRemoveProcess','_CsrLockProcessByClientId@8'])
    if (!serverMap.split(/\r?\n/).some(line=>line.includes(symbol)&&line.includes('opennt-base-bindings:registry.obj')))
        throw Error(`Formal registered-process provider missing: ${symbol}`);
for (const name of ['resource-server.exe', 'resource-client.exe']) {
    const image = fs.readFileSync(path.join(binaries, name));
    if (image.readUInt16LE(image.readUInt32LE(0x3c) + 4) !== 0x14c) throw Error('Not x86');
}
function launchServer(endpoint, downstream) {
const server = spawn(path.join(binaries, 'resource-server.exe'), [endpoint, ...(downstream ? [downstream] : [])],
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
    const result = spawnSync(path.join(binaries, 'resource-client.exe'),
        [endpoint, path.join(build, `${endpoint}.tmp`), ...(mode.endsWith('readonly') ? ['readonly'] :
            mode === 'unavailable' ? ['unavailable'] : mode === 'wrong-peer' ? ['denied', String(process.pid)] : mode.startsWith('wrong-') ? ['denied'] : mode === 'low-auth' ? ['low-auth'] : [])],
        {cwd: build, windowsHide: true, encoding: 'utf8', timeout: 15000});
    const record = {clientPid: result.pid, serverPid: relay.server.pid, targetPid: target?.server.pid,
        clientStatus: result.status, clientOutput: result.stdout,
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
        for (const instance of [relay,...(target?[target]:[])]) {
            const transcript=instance.transcript();
            if (!transcript.includes('ORIGINAL-CHECK copied=1 no-worker=1 abort=0'))
                throw Error('Original CheckVDM command copy and partial-launch abort were not observed');
            if ((transcript.match(/ORIGINAL-FIRST status=0 value=1/g)||[]).length!==1 ||
                (transcript.match(/ORIGINAL-FIRST status=0 value=0/g)||[]).length!==1)
                throw Error('Original first-VDM state was not preserved across authenticated requests');
            if (!transcript.includes('RECEIPT wrong-role=denied stream-alias=accepted'))
                throw Error('Receipt role/stream-alias checks missing');
            if (!transcript.includes('REVOKE generation=2 id=1 status=5') ||
                (transcript.match(/REVOKE generation=1 id=1 status=0/g)||[]).length!==2 ||
                !transcript.includes('REVOKE generation=1 id=2 status=0'))
                throw Error('Authenticated remote receipt revocation was not fully observed');
        }
        for (const instance of [relay,...(target?[target]:[])])
            if (!instance.transcript().includes('REGISTER status=0') ||
                !instance.transcript().includes('REGISTER retained-after-call=1 drained=1') ||
                !instance.transcript().includes('RECEIPT revoked=1 repeat-safe=1 drained=1'))
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
