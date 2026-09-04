import { createHash } from 'node:crypto';
import { copyFileSync, existsSync, readFileSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { spawnSync } from 'node:child_process';

function usage() {
  throw new Error('usage: node tools/observation/ObserveSoftpcStartup.mjs --launcher <observer.exe> --product <product.exe> --stage <runtime-dir> --report <result.txt> [--interactive | --interactive-script | --interactive-script-ver-only] [--product-command <declared-DOS-command>] [--observation-timeout-ms 10000..30000] [--minimal-host-environment] [--child-environment MVDM_SESSION_DISPOSE_REPORT_PATH=<absolute-path>|MVDM_COMMAND_CONTINUATION_REPORT_PATH=<absolute-path>|MVDM_DEM_OPEN_REPORT_PATH=<absolute-path>]');
}

function sha256(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

function normalizeManifest(manifest) {
  if ((manifest.format === 2 || manifest.format === 3 || manifest.format === 4) && manifest.product && Array.isArray(manifest.mediaAssets)) {
    return {
      product: manifest.product,
      runtimeCompanions: Array.isArray(manifest.runtimeCompanions) ? manifest.runtimeCompanions : [],
      mediaAssets: manifest.mediaAssets
    };
  }
  if (manifest.format === 1 && Array.isArray(manifest.assets) && manifest.assets.length !== 0) {
    return { product: manifest.assets[0], runtimeCompanions: [], mediaAssets: manifest.assets.slice(1) };
  }
  throw new Error('unsupported runtime stage manifest');
}

const options = {};
for (let index = 2; index < process.argv.length; index += 1) {
  const key = process.argv[index];
  if (key === '--interactive') {
    if (options.interactive !== undefined) usage();
    options.interactive = true;
    continue;
  }
  if (key === '--interactive-script') {
    if (options.interactiveScript !== undefined) usage();
    options.interactiveScript = true;
    continue;
  }
  if (key === '--interactive-script-ver-only') {
    if (options.interactiveScriptVerOnly !== undefined) usage();
    options.interactiveScriptVerOnly = true;
    continue;
  }
  if (key === '--minimal-host-environment') {
    if (options.minimalHostEnvironment !== undefined) usage();
    options.minimalHostEnvironment = true;
    continue;
  }
  const value = process.argv[index + 1];
  if (!key?.startsWith('--') || value === undefined) usage();
  options[key.slice(2)] = value;
  index += 1;
}
for (const key of Object.keys(options)) {
  if (!['launcher', 'product', 'stage', 'report', 'interactive', 'interactiveScript', 'interactiveScriptVerOnly', 'minimalHostEnvironment', 'product-command', 'observation-timeout-ms', 'child-environment'].includes(key)) {
    throw new Error(`unsupported observer option: --${key}`);
  }
}
if (options['observation-timeout-ms'] !== undefined &&
    (!/^(10000|[12][0-9]{4}|30000)$/.test(options['observation-timeout-ms']))) {
  throw new Error('observation timeout must be an integer from 10000 through 30000 ms');
}
if (options['product-command'] !== undefined &&
    (options['product-command'].length === 0 ||
     options['product-command'].length > 1024 ||
     /[\r\n]/.test(options['product-command']))) {
  throw new Error('product command must be one non-empty line no longer than 1024 bytes');
}
for (const key of ['launcher', 'product', 'stage', 'report']) {
  if (!options[key]) usage();
  options[key] = resolve(options[key]);
}
let childEnvironment = undefined;
if (options['child-environment'] !== undefined) {
  const names = [
    'MVDM_SESSION_DISPOSE_REPORT_PATH',
    'MVDM_COMMAND_CONTINUATION_REPORT_PATH',
    'MVDM_NATIVE_CHILD_REPORT_PATH',
    /* An already-existing default-off original DEM observer. This records
     * only the source-owned path/scalar call boundary for fixed media. */
    'MVDM_DEM_OPEN_REPORT_PATH'
  ];
  const separator = options['child-environment'].indexOf('=');
  const name = separator < 0 ? '' : options['child-environment'].slice(0, separator);
  const reportPath = separator < 0 ? '' : options['child-environment'].slice(separator + 1);
  if (!names.includes(name) || reportPath.length === 0) {
    throw new Error('only an approved absolute diagnostic report path is permitted');
  }
  if (!resolve(reportPath) || resolve(reportPath) !== reportPath) {
    throw new Error('child diagnostic report path must be absolute');
  }
  childEnvironment = { name, value: reportPath };
}
if (!existsSync(options.launcher) || !existsSync(options.product) || !existsSync(options.stage)) {
  throw new Error('launcher, product, and stage must already exist');
}
const manifestPath = resolve(options.stage, 'runtime-manifest.json');
if (!existsSync(manifestPath)) throw new Error(`missing fixed stage manifest: ${manifestPath}`);
const manifest = readFileSync(manifestPath);
const layout = normalizeManifest(JSON.parse(manifest));
if (layout.product.destination !== 'ntvdm.exe') {
  throw new Error('runtime stage product destination is not the fixed product name');
}
for (const asset of layout.mediaAssets) {
  const stagedAsset = resolve(options.stage, asset.destination);
  if (!existsSync(stagedAsset) || sha256(stagedAsset) !== asset.sha256) {
    throw new Error(`fixed runtime media identity mismatch: ${asset.destination}`);
  }
}
for (const companion of layout.runtimeCompanions) {
  const stagedCompanion = resolve(options.stage, companion.destination);
  if (!existsSync(stagedCompanion) || sha256(stagedCompanion) !== companion.sha256) {
    throw new Error(`runtime companion identity mismatch: ${companion.destination}`);
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
const runtimeCompanionsManifestSha256 = createHash('sha256').update(JSON.stringify(
  layout.runtimeCompanions)).digest('hex');
const launcherArguments = [stagedProduct, options.stage, options.report];
if (options['product-command'] !== undefined) {
  /* A declared command and a later Console row are independent product
   * contracts.  The command crosses only the existing app/BaseVDM boundary;
   * observer input remains gated on an original BIOS keyboard edge below. */
  launcherArguments.push('-f', '-o', '--command', options['product-command']);
} else if (options.interactive !== undefined || options.interactiveScript !== undefined || options.interactiveScriptVerOnly !== undefined) {
  /* S7's no-argument product contract still needs the original SoftPC
   * foreground admission switch.  Passing it explicitly prevents the C
   * observer's historical argc==4 fallback from manufacturing `/C EXIT`.
   * This is a launch-container selector only: it never supplies a DOS line,
   * guest byte, BOP record, or Console input event. */
  launcherArguments.push('-f');
}
if (options.interactiveScript !== undefined)
  launcherArguments.push('--observe-console-input');
else if (options.interactiveScriptVerOnly !== undefined)
  launcherArguments.push('--observe-console-input-ver-only');
if (options['observation-timeout-ms'] !== undefined)
  launcherArguments.push('--observation-timeout-ms', options['observation-timeout-ms']);
const result = spawnSync(options.launcher, launcherArguments, {
  cwd: options.stage,
  encoding: 'utf8',
  windowsHide: false,
  env: options.minimalHostEnvironment === undefined ?
    (childEnvironment === undefined ? process.env : {
      ...process.env,
      [childEnvironment.name]: childEnvironment.value
    }) : (() => {
      const systemRoot = process.env.SystemRoot || process.env.SYSTEMROOT || 'C:\\Windows';
      const environment = {
        SystemRoot: systemRoot,
        windir: systemRoot,
        ComSpec: `${systemRoot}\\System32\\cmd.exe`,
        PATH: `${systemRoot}\\System32`
      };
      if (childEnvironment !== undefined)
        environment[childEnvironment.name] = childEnvironment.value;
      return environment;
    })()
});
writeFileSync(`${options.report}.json`, `${JSON.stringify({
  container: 'console-owning-nondebug',
  command: [options.launcher, ...launcherArguments],
  minimalHostEnvironment: options.minimalHostEnvironment === true,
  childEnvironment: childEnvironment === undefined ? [] : [childEnvironment.name],
  stageManifestSha256: createHash('sha256').update(manifest).digest('hex'),
  fixedMediaManifestSha256,
  runtimeCompanionsManifestSha256,
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
