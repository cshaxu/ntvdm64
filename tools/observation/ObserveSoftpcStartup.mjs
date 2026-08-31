import { createHash } from 'node:crypto';
import { existsSync, readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { spawnSync } from 'node:child_process';

function usage() {
  throw new Error('usage: node tools/observation/ObserveSoftpcStartup.mjs --launcher <observer.exe> --product <product.exe> --stage <runtime-dir> --report <result.txt>');
}

const options = {};
for (let index = 2; index < process.argv.length; index += 2) {
  const key = process.argv[index];
  const value = process.argv[index + 1];
  if (!key?.startsWith('--') || value === undefined) usage();
  options[key.slice(2)] = value;
}
for (const key of ['launcher', 'product', 'stage', 'report']) {
  if (!options[key]) usage();
  options[key] = resolve(options[key]);
}
if (!existsSync(options.launcher) || !existsSync(options.product) || !existsSync(options.stage)) {
  throw new Error('launcher, product, and stage must already exist');
}
const manifestPath = resolve(options.stage, 'runtime-manifest.json');
if (!existsSync(manifestPath)) throw new Error(`missing fixed stage manifest: ${manifestPath}`);
const manifest = readFileSync(manifestPath);
const result = spawnSync(options.launcher, [options.product, options.stage, options.report], {
  cwd: options.stage,
  encoding: 'utf8',
  windowsHide: false
});
writeFileSync(`${options.report}.json`, `${JSON.stringify({
  container: 'console-owning-nondebug',
  command: [options.launcher, options.product, options.stage, options.report],
  stageManifestSha256: createHash('sha256').update(manifest).digest('hex'),
  launcherExitCode: result.status,
  launcherSignal: result.signal,
  stdout: result.stdout,
  stderr: result.stderr
}, null, 2)}\n`);
if (result.error) throw result.error;
if (result.status !== 0) process.exit(result.status ?? 1);
