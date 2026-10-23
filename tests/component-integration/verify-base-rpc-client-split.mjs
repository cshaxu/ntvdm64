import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';

// Audit explicit link inputs, not build prerequisites or optimizer promises.
const [frontend, launcher] = process.argv.slice(2);
if (!frontend || !launcher) throw Error('Pass native frontend and launcher build roots');
function sources(root, target, extra = []) {
  const graph = new Map();
  for (const line of fs.readFileSync(path.join(root, 'build.ninja'), 'utf8').split(/\r?\n/)) {
    const m = line.match(/^build (.+?): (\S+)\s*(.*)$/);
    if (!m) continue;
    const inputs = m[3].split(/\s+\|\|?\s+/)[0].trim().split(/\s+/);
    for (const output of m[1].split(/\s+\|\s+/)[0].split(/\s+/)) graph.set(output, inputs);
  }
  assert(graph.has(target), `Missing ${target}`);
  const pending = [target, ...extra], seen = new Set(), result = [];
  while (pending.length) {
    const item = pending.pop();
    if (seen.has(item)) continue;
    seen.add(item);
    if (/\.(c|cpp)$/.test(item)) result.push(item.replaceAll('$:', ':'));
    else if (graph.has(item)) pending.push(...graph.get(item));
  }
  return result;
}
const forbidden = /\/nthook32-dll\/|\/base_rpc_client_launch\.c$|\/run16-exe\/native_launch\.c$/;
function checkFrontend(selected, map) {
  assert(selected.some(s => s.endsWith('/base_rpc_client_core.c')), 'Frontend missing core');
  assert(!selected.some(s => forbidden.test(s)), 'Frontend links execution/style provider');
  assert(!/OpenNtBaseClient(?:SetWindowStyle|WowStyle|DosWindowStyle|SeparateWowStyle|CallServer)\b/.test(map),
    'Frontend map contains launch/style symbol');
  assert(!/OpenNtBase(?:WriteCheckStyle|ReadCheckStyle|WriteGetStyle|ReadGetStyle)\b/.test(map),
    'Frontend map contains launch-style codec');
}
const frontendSources = sources(frontend, 'ntcon.exe');
const frontendMap = fs.readFileSync(path.join(frontend, 'ntcon.exe.map'), 'utf8');
checkFrontend(frontendSources, frontendMap);
for (const leak of ['base_rpc_client_launch.c', '../nthook32-dll/installer.cpp']) {
  assert.throws(() => checkFrontend([...frontendSources, 'O:/repo/src/' + leak.replace(/^\.\.\//, '')], frontendMap));
}
assert.throws(() => checkFrontend(frontendSources, frontendMap + '\n OpenNtBaseClientSetWindowStyle '));
assert.throws(() => checkFrontend(frontendSources, frontendMap + '\n OpenNtBaseReadGetStyle '));
const launcherSources = sources(launcher, 'run16.exe');
for (const file of ['base_rpc_client_core.c', 'base_rpc_client_launch.c'])
  assert(launcherSources.some(s => s.endsWith('/' + file)), `Launcher missing ${file}`);
const launcherMap = fs.readFileSync(path.join(launcher, 'run16.exe.map'), 'utf8');
assert(/OpenNtBaseClientCallServer\b/.test(launcherMap), 'Launcher execution facade not linked');
for (const file of ['src/ntsrv-exe/opennt/source/base_rpc_client_core.c',
  'src/ntsrv-exe/opennt/source/base_rpc_client_launch.c', 'tools/build/New-NativeWorkerNinja.ps1']) {
  assert(!fs.readFileSync(file, 'utf8').includes('OPENNT_BASE_RPC_FRONTEND_ONLY'), `Conditional remains: ${file}`);
}
console.log(JSON.stringify({result:'PASS', frontendSources:frontendSources.length,
  launcherSources:launcherSources.length, negativeControls:5,
  scope:'Explicit graph and final map ownership; not runtime propagation or security clearance'}, null, 2));
