import { copyFileSync, existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { dirname, isAbsolute, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const scriptRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const requiredAssets = [
  ['src/mvdm-guest/dos/v86/doskrnl/bios/NTIO.SYS', 'NTIO.SYS'],
  ['src/mvdm-guest/dos/v86/doskrnl/dos/NTDOS.SYS', 'NTDOS.SYS'],
  ['src/mvdm-guest/dos/v86/cmd/command/COMMAND.COM', 'COMMAND.COM'],
  /* Immutable original .COM workload used only by the bounded DOS EXEC
   * observation.  It is byte-identical to the source-built output, so the
   * staged media contract remains an original guest artifact, not a harness
   * program or a host-side loader input. */
  ['src/mvdm-guest/dos/v86/cmd/loadfix/LOADFIX.COM', 'LOADFIX.COM'],
  /* Small original MZ workload paired with LOADFIX.COM for the same bounded
   * DOS EXEC package.  FASTOPEN is the original no-op NT stub; it exercises
   * the guest MZ path without introducing a new host implementation. */
  ['src/mvdm-guest/dos/v86/cmd/fastopen/FASTOPEN.EXE', 'FASTOPEN.EXE'],
  /* T377's immutable device matrix.  The original programs resolve their
   * companions from the selected DOS system root; no guest payload is made
   * or patched by staging. */
  ['src/mvdm-guest/dos/v86/cmd/mem/mem.exe', 'MEM.EXE'],
  ['src/mvdm-guest/dos/v86/cmd/keyb/KB16.COM', 'KB16.COM'],
  ['src/mvdm-guest/dos/v86/dev/keyboard/KEYBOARD.SYS', 'KEYBOARD.SYS'],
  ['src/mvdm-guest/dos/v86/cmd/graphics/GRAPHICS.COM', 'GRAPHICS.COM'],
  ['src/mvdm-guest/dos/v86/cmd/graphics/graphics.pro', 'GRAPHICS.PRO'],
  ['src/mvdm-guest/bin86/config.nt', 'config.nt'],
  ['src/mvdm-guest/bin86/autoexec.nt', 'autoexec.nt'],
  ['src/mvdm-guest/dos/v86/cmd/command/COMMAND.COM', 'system32/COMMAND.COM'],
  ['src/mvdm-guest/dos/v86/dev/country/COUNTRY.SYS', 'system32/COUNTRY.SYS'],
  ['src/mvdm-guest/dos/v86/dev/himem/HIMEM.SYS', 'system32/HIMEM.SYS'],
  ['build/output/dos/REDIR.EXE', 'system32/REDIR.EXE'],
  ['build/output/dos/DOSX.EXE', 'system32/DOSX.EXE'],
];

function usage() {
  throw new Error('usage: node tools/build/Stage-OriginalSoftpcRuntime.mjs --executable <product.exe> --output <directory> [--redirector-dll <VDMREDIR.dll>] [--update --replace-product]');
}

function sha256(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

let executable = null;
let output = null;
let redirectorDll = null;
let update = false;
let replaceProduct = false;
for (let index = 2; index < process.argv.length; index += 1) {
  const option = process.argv[index];
  if (option === '--update') update = true;
  else if (option === '--replace-product') replaceProduct = true;
  else if (option === '--executable' || option === '--output' || option === '--redirector-dll') {
    const value = process.argv[index + 1];
    if (value === undefined) usage();
    if (option === '--executable') executable = value;
    else if (option === '--output') output = value;
    else redirectorDll = value;
    index += 1;
  }
  else usage();
}
if (executable === null || output === null) usage();
if (replaceProduct && !update) usage();

const executablePath = isAbsolute(executable) ? executable : resolve(process.cwd(), executable);
const outputPath = isAbsolute(output) ? output : resolve(process.cwd(), output);
const redirectorDllPath = redirectorDll === null ? null :
  (isAbsolute(redirectorDll) ? redirectorDll : resolve(process.cwd(), redirectorDll));
if (!existsSync(executablePath)) throw new Error(`product executable does not exist: ${executablePath}`);
if (redirectorDllPath !== null && !existsSync(redirectorDllPath)) {
  throw new Error(`redirector DLL does not exist: ${redirectorDllPath}`);
}
if (existsSync(outputPath) && !update) throw new Error(`refusing to overwrite runtime package: ${outputPath}`);
if (update && !existsSync(join(outputPath, 'runtime-manifest.json'))) {
  throw new Error(`refusing to update a directory without a runtime manifest: ${outputPath}`);
}

mkdirSync(join(outputPath, 'system32'), { recursive: true });
const manifest = [];
function stage(source, destination, replace = false) {
  if (!existsSync(source)) throw new Error(`required source asset does not exist: ${source}`);
  const target = join(outputPath, destination);
  if (existsSync(target)) {
    if (sha256(target) !== sha256(source) && !replace) {
      throw new Error(`refusing to replace different staged asset: ${target}`);
    }
    if (sha256(target) !== sha256(source) && replace) copyFileSync(source, target);
  } else {
    mkdirSync(dirname(target), { recursive: true });
    copyFileSync(source, target);
  }
  manifest.push({ source, destination, sha256: sha256(source) });
}

/* The executable is the sole replaceable fixed-container input: callers must
 * opt in after a formal link. Guest and firmware bytes never use this path. */
/* The original late-loaded owner DLLs import the parent as ntvdm.exe.  The
 * formal build output may retain a descriptive filename, but the staged,
 * launched product must retain this original module identity. */
stage(executablePath, 'ntvdm.exe', replaceProduct);
const runtimeCompanions = [];
if (redirectorDllPath !== null) {
  /* VDMREDIR is a parent-product companion, not mutable guest media.  The
   * original nt_bop loader resolves it beside the selected parent EXE. */
  stage(redirectorDllPath, 'VDMREDIR.DLL', replaceProduct);
  runtimeCompanions.push(manifest[manifest.length - 1]);
}
for (const [source, destination] of requiredAssets) stage(join(scriptRoot, source), destination);
writeFileSync(join(outputPath, 'runtime-manifest.json'), `${JSON.stringify({
  format: 4,
  /* The EXE is intentionally replaced by each formal product observation.
   * Everything in mediaAssets is immutable fixed-container input. */
  product: manifest[0],
  runtimeCompanions,
  mediaAssets: manifest.slice(1 + runtimeCompanions.length)
}, null, 2)}\n`);
console.log(outputPath);
