import { createHash } from 'node:crypto';
import { copyFileSync, existsSync, readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { spawnSync } from 'node:child_process';

function usage() {
  throw new Error('usage: node tools/observation/ObserveSoftpcStartup.mjs --launcher <observer.exe> --product <product.exe> --stage <runtime-dir> --report <result.txt>');
}

function sha256(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

function normalizeManifest(manifest) {
  if ((manifest.format === 2 || manifest.format === 3) && manifest.product && Array.isArray(manifest.mediaAssets)) {
    return { product: manifest.product, mediaAssets: manifest.mediaAssets };
  }
  if (manifest.format === 1 && Array.isArray(manifest.assets) && manifest.assets.length !== 0) {
    return { product: manifest.assets[0], mediaAssets: manifest.assets.slice(1) };
  }
  throw new Error('unsupported runtime stage manifest');
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
const layout = normalizeManifest(JSON.parse(manifest));
if (layout.product.destination !== 'original-softpc-process.exe') {
  throw new Error('runtime stage product destination is not the fixed product name');
}
for (const asset of layout.mediaAssets) {
  const stagedAsset = resolve(options.stage, asset.destination);
  if (!existsSync(stagedAsset) || sha256(stagedAsset) !== asset.sha256) {
    throw new Error(`fixed runtime media identity mismatch: ${asset.destination}`);
  }
}
/* The product is deliberately the one mutable item in the fixed container.
 * App resolves MVDM system media and firmware relative to its own executable, so starting
 * the formal build output from outside this directory would silently sever
 * the product from the verified adjacent media. */
const stagedProduct = resolve(options.stage, layout.product.destination);
copyFileSync(options.product, stagedProduct);
const productSha256 = sha256(stagedProduct);
const fixedMediaManifestSha256 = createHash('sha256').update(JSON.stringify(
  layout.mediaAssets)).digest('hex');
const result = spawnSync(options.launcher, [stagedProduct, options.stage, options.report], {
  cwd: options.stage,
  encoding: 'utf8',
  windowsHide: false
});
writeFileSync(`${options.report}.json`, `${JSON.stringify({
  container: 'console-owning-nondebug',
  command: [options.launcher, stagedProduct, options.stage, options.report],
  stageManifestSha256: createHash('sha256').update(manifest).digest('hex'),
  fixedMediaManifestSha256,
  productSource: options.product,
  productStage: stagedProduct,
  productSha256,
  launcherExitCode: result.status,
  launcherSignal: result.signal,
  stdout: result.stdout,
  stderr: result.stderr
}, null, 2)}\n`);
if (result.error) throw result.error;
if (result.status !== 0) process.exit(result.status ?? 1);
