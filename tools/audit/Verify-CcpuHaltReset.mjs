// Real CCPU/SAS test with fixed and pre-fix c_main linked against the same
// formal libraries. All generated comparison material remains under build/.
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
const root = process.cwd();
const build = path.resolve('build/M0-T412/S1/event-profile/product');
const revision = process.argv[2];
if (!revision || !/^[a-f0-9]{7,40}$/.test(revision)) throw new Error('Pass audited pre-fix Git revision');
const relativeSource = 'src/mvdm-host/softpc.new/base/ccpu386/c_main.c';
const baseline = spawnSync('git', ['show', `${revision}:${relativeSource}`], { encoding: 'utf8' });
if (baseline.status !== 0) throw new Error('Cannot read baseline');
const old = baseline.stdout.replace(/\r\n/g, '\n');
const current = fs.readFileSync(relativeSource, 'utf8').replace(/\r\n/g, '\n');
const repaired = old.replace('\t if ( c_cpu_take_event(CPU_RESET_EXCEPTION_MASK) )',
  '\t /* DIVERGENCE(MVDM-HOST-DIV-214): atomically observe only, as in\n' +
  '\t  * the original HALT path; the normal RESET handler consumes it. */\n' +
  '\t if ( c_cpu_event_snapshot() & CPU_RESET_EXCEPTION_MASK )');
if (old === repaired || repaired !== current) throw new Error('Baseline differs beyond the audited repair');
fs.writeFileSync(path.join(build, 'generated/c_main-before-reset-fix.c'), old);
const sourceName = path.join(build, 'generated/c_main-before-reset-fix.c').replaceAll('\\', '/').replace(':', '$:');
let graph = fs.readFileSync(path.join(build, 'build.ninja'), 'utf8');
graph = graph.replace(/^build obj\/ccpu\/c_main.obj: cc .*$/m,
  `build obj/ccpu/c_main.obj: cc ${sourceName}`)
  .replaceAll('obj/ccpu/c_main.obj', 'obj/ccpu/c_main-before-reset-fix.obj')
  .replaceAll('original-ccpu386.lib', 'original-ccpu386-before-reset-fix.lib')
  .replaceAll('original-softpc-process', 'original-softpc-before-reset')
  .replaceAll('ccpu-halt-reset-test.exe', 'ccpu-halt-reset-before-fix.exe');
fs.writeFileSync(path.join(build, 'baseline.ninja'), graph);
const log = fs.openSync(path.join(build, 'baseline-build.log'), 'w');
const compile = spawnSync('cmd.exe', ['/d', '/c',
  'run-ninja-parallel.cmd -f baseline.ninja ccpu-halt-reset-before-fix.exe original-softpc-before-reset.exe'],
  { cwd: build, stdio: ['ignore', log, log], timeout: 120000 });
fs.closeSync(log);
if (compile.status !== 0) throw new Error(`Baseline compile failed: ${compile.error ?? compile.status}; see baseline-build.log`);
const results = [];
for (const [name, expected, marker] of [
  ['ccpu-halt-reset-before-fix.exe', 1, 'AX=dead producer=0'],
  ['ccpu-halt-reset-test.exe', 0, 'AX=beef producer=0']]) {
  const bytes = fs.readFileSync(path.join(build, name));
  if (bytes.readUInt16LE(bytes.readUInt32LE(0x3c) + 4) !== 0x14c) throw new Error('Not x86');
  const result = spawnSync(path.join(build, name), [],
    { cwd: build, encoding: 'utf8', windowsHide: true, timeout: 15000 });
  const row = { name, status: result.status, error: result.error?.message,
    stdout: result.stdout, stderr: result.stderr };
  results.push(row);
  fs.writeFileSync(path.join(build, 'halt-reset-results.json'), JSON.stringify({ revision, results }, null, 2));
  console.log(JSON.stringify(row));
  if (result.status !== expected || !result.stderr?.includes(marker)) throw new Error('Unexpected actual CCPU result');
}
console.log('PASS: original CCPU single-change HALT RESET A/B; not DOS/EDIT/WRITE application acceptance');
