import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { spawnSync } from 'node:child_process';
import { dirname, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

const repository = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
const [executable, output] = process.argv.slice(2);
assert.ok(executable && output, 'usage: node original_runtime_layout_test.mjs <EXE> <fresh build output>');
const staged = resolve(output);
assert.ok(staged.toLowerCase().startsWith((resolve(repository, 'build') + sep).toLowerCase()));
assert.equal(existsSync(staged), false, 'Never overwrite sealed staging evidence');
const result = spawnSync(process.execPath,
  [resolve(repository, 'tools/build/Stage-OriginalSoftpcRuntime.mjs'),
    '--executable', resolve(executable), '--output', staged], { encoding: 'utf8' });
assert.equal(result.status, 0, result.stderr);
const manifest = JSON.parse(readFileSync(resolve(staged, 'runtime-manifest.json'), 'utf8'));
const names = new Set();
const dosFiles = ['NTIO.SYS', 'NTDOS.SYS', 'COMMAND.COM', 'LOADFIX.COM',
  'FASTOPEN.EXE', 'MEM.EXE', 'KB16.COM', 'KEYBOARD.SYS', 'GRAPHICS.COM',
  'GRAPHICS.PRO', 'CONFIG.NT', 'AUTOEXEC.NT', 'COUNTRY.SYS', 'HIMEM.SYS',
  'REDIR.EXE', 'DOSX.EXE'];
const hash = file => createHash('sha256').update(readFileSync(file)).digest('hex');
for (const asset of manifest.mediaAssets) {
  const name = asset.destination.toUpperCase();
  assert.equal(names.has(name), false, `Duplicate destination: ${name}`);
  names.add(name);
  assert.equal(hash(resolve(staged, asset.destination)), asset.sha256);
  assert.equal(hash(asset.source), asset.sha256, 'Original bytes must be unchanged');
}
for (const file of dosFiles) {
  assert.ok(names.has(`SYSTEM32/${file}`), `Original setup target 2 missing: ${file}`);
  assert.equal(existsSync(resolve(staged, file)), false, `Unexpected flat copy: ${file}`);
}
assert.equal(hash(resolve(staged, 'ntvdm.exe')), hash(resolve(executable)));
console.log('PASS original system32 deployment, unique destinations and immutable media hashes');
