// Real RPC negative peers. Mutations are test-only generated sources under build/.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {spawn, spawnSync} from 'node:child_process';
const root=process.cwd(), product=path.resolve(process.env.OPENNT_BROKER_PRODUCT_BUILD || 'build/.ninja/msvc-x64-release/service');
const build=path.resolve(process.env.OPENNT_VERSION_TEST_BUILD || 'build/version-negative');
const logs=path.resolve(process.env.OPENNT_VERSION_TEST_LOGS || path.join(build,'logs'));
const runtime=process.env.OPENNT_VERSION_TEST_RUNTIME;
if(!runtime)throw Error('OPENNT_VERSION_TEST_RUNTIME must name the deployed runtime directory');
const runtimeBinary=fs.existsSync(path.join(runtime,'system32','run16.exe'))?path.join(runtime,'system32'):runtime;
const manifestText=fs.readFileSync(path.join(runtime,'product-system32-manifest.json'),'utf8');
const manifest=JSON.parse(manifestText.replace(/^\uFEFF/,''));
const packageIds=[...new Set(manifest.map(row=>row.PackageId))];
assert.equal(packageIds.length,1,'Runtime package must have one identity');
const packageId=packageIds[0];
fs.mkdirSync(build,{recursive:true});fs.mkdirSync(logs,{recursive:true});
const source=fs.readFileSync('src/ntsrv-exe/main.c','utf8');
const header=fs.readFileSync('src/common/protocol/version.h','utf8');
const version=header.match(/#define APP_VERSION "(0\.0\.[0-9]+)"/)[1];
const protocol=Number(header.match(/#define APP_PROTOCOL_VERSION ([0-9]+)u/)[1]);
const idl=fs.readFileSync('src/common/protocol/service.idl','utf8');
assert(idl.includes(`version(${protocol}.0)`),'RPC major and protocol must agree');
assert.match(idl,/application_version\[32\]/);
assert.match(header,/#define APP_VERSION_BYTES 32u/);
const variants=['old-app','wrong-protocol','reply-app','reply-protocol','legacy-interface'];
const built=spawnSync('cmd.exe',['/d','/c','tools\\build-fixtures.cmd x64 version-negatives'],
    {cwd:root,windowsHide:true,encoding:'utf8',timeout:300000});
fs.writeFileSync(path.join(logs,'build.log'),(built.stdout||'')+(built.stderr||''));
assert.equal(built.status,0,`Native version fixtures failed: ${built.error?.message||''}`);
for (const name of variants) {
    // The mutated server must belong to the same complete test package as
    // its clients. It is a uniquely named fixture and never replaces ntsrv.
    const serverFixture=path.join(runtimeBinary,`version-negative-${name}.exe`);
    assert(!fs.existsSync(serverFixture),`Refuse to replace ${serverFixture}`);
    fs.copyFileSync(path.join(product,`diagnostics/version-${name}/ntsrv.exe`),serverFixture);
    const stamp=spawnSync(process.env.OPENNT_VERSION_PWSH || 'pwsh.exe',['-NoProfile','-ExecutionPolicy','Bypass','-File',
        path.join(root,'tools/build/Set-PackageIdentity.ps1'),'-Image',serverFixture,
        '-PackageId',packageId],{windowsHide:true,encoding:'utf8'});
    if(stamp.status!==0){fs.rmSync(serverFixture,{force:true});throw Error(`Cannot stamp ${name}: ${stamp.error?.message || stamp.stderr}`);}
    const server=spawn(serverFixture,[],{cwd:build,windowsHide:true,
        env:process.env,stdio:['ignore','pipe','pipe']});
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
            const r=spawnSync(path.join(runtimeBinary,exe),args,{cwd:runtime,windowsHide:true,encoding:'utf8',timeout:4000});
            fs.writeFileSync(path.join(logs,`${name}-${exe}.json`),JSON.stringify({status:r.status,error:r.error?.message,ms:Date.now()-start,stdout:r.stdout,stderr:r.stderr},null,2));
            assert.equal(r.status,1306,`${name} ${exe}: expected ERROR_REVISION_MISMATCH, not retry/timeout`);
            assert.match(r.stderr,/version mismatch/);
        }
    } finally {
        clearTimeout(timer);if(server.exitCode===null)server.kill();await done;
        fs.writeFileSync(path.join(logs,`${name}-server.log`),output);
        fs.rmSync(serverFixture,{force:true});
    }
    console.log(`PASS ${name}: launcher and worker reject before task delivery, no launcher retry`);
}
assert.equal(fs.readFileSync('src/ntsrv-exe/main.c','utf8'),source,'Production source must be unchanged');
