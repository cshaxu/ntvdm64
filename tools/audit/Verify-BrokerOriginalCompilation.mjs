// Compile-only proof: unchanged original BaseSrv, no policy test doubles.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
const root=process.cwd();
const build=path.resolve('build/M0-T412/S1/base-owner');
fs.mkdirSync(build,{recursive:true});
const graphPath=path.resolve(process.argv[2] ?? 'build/M0-T412/S1/text-cell-repair/product/build.ninja');
const graph=fs.readFileSync(graphPath,'utf8');
const flags=graph.match(/^cflags = (.*)$/m)?.[1].replaceAll('$:',':');
assert(flags,'Generate the formal x86 product graph before this compile probe');
const source=path.resolve('src/opennt-host/base/win32/server/srvvdm.c');
const sourceBefore=fs.readFileSync(source);
const env=path.join(build,'msvc.cmd');
const vc='C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat';
fs.writeFileSync(env,`@echo off\r\nset "probe_cwd=%CD%"\r\ncall "${vc}" -arch=x86 -host_arch=x64 >nul\r\nif errorlevel 1 exit /b %errorlevel%\r\ncd /d "%probe_cwd%"\r\n%*\r\n`);
// srvvdm uses no DFS declarations: exclude this unrelated umbrella dependency
// only in this compile probe. Never install this empty header into the product.
fs.writeFileSync(path.join(build,'ntdddfs.h'),'/* Compile probe only: unused DFS umbrella dependency. */\n');
function command(args,logName) {
    const log=fs.openSync(path.join(build,logName),'w');
    const result=spawnSync('cmd.exe',['/d','/c',`call "${env}" ${args}`],{
        cwd:build,windowsVerbatimArguments:true,windowsHide:true,
        stdio:['ignore',log,log],timeout:60000});
    fs.closeSync(log);
    assert.equal(result.status,0,`${logName}: ${result.error?.message ?? result.status}`);
}
command(`cl.exe ${flags} /we4013 /FI "${root}/tests/broker/source_compile_probe.h" /I "${build}" /I "${root}/src/opennt-host/base/win32/inc" /Fo"${build}/srvvdm.obj" "${source}"`,'compile.log');
command(`dumpbin.exe /symbols "${build}/srvvdm.obj"`,'symbols.txt');
assert.deepEqual(fs.readFileSync(source),sourceBefore,'The original owner must not be edited');
const object=fs.readFileSync(path.join(build,'srvvdm.obj'));
assert.equal(object.readUInt16LE(0),0x14c,'Expected x86 COFF object');
const symbols=fs.readFileSync(path.join(build,'symbols.txt'),'utf8');
const unresolved=symbols.split(/\r?\n/).filter(line=>/\bUNDEF\b/.test(line));
assert(unresolved.some(line=>line.includes('ProbeAuthenticatedRequestThread')));
const result={scope:'compile-only; not linked or run',machine:'014c',
    sourceSha256:createHash('sha256').update(sourceBefore).digest('hex'),
    graph:graphPath,unresolved};
fs.writeFileSync(path.join(build,'compile-result.json'),JSON.stringify(result,null,2)+'\n');
console.log(`PASS unchanged original srvvdm.c compiles for x86; ${unresolved.length} external/common symbol records remain; no runtime claim`);
