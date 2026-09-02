import { copyFileSync, existsSync, mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { dirname, isAbsolute, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const scriptRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const requiredAssets = [
  ['src/mvdm-guest/dos/v86/doskrnl/bios/NTIO.SYS', 'NTIO.SYS'],
  ['src/mvdm-guest/dos/v86/doskrnl/dos/NTDOS.SYS', 'NTDOS.SYS'],
  ['src/mvdm-guest/dos/v86/cmd/command/COMMAND.COM', 'COMMAND.COM'],
  ['src/mvdm-guest/bin86/config.nt', 'config.nt'],
  ['src/mvdm-guest/bin86/autoexec.nt', 'autoexec.nt'],
  ['src/mvdm-guest/dos/v86/cmd/command/COMMAND.COM', 'system32/COMMAND.COM'],
  ['src/mvdm-guest/dos/v86/dev/country/COUNTRY.SYS', 'system32/COUNTRY.SYS'],
  ['src/mvdm-guest/dos/v86/dev/himem/HIMEM.SYS', 'system32/HIMEM.SYS'],
  ['build/output/dos/REDIR.EXE', 'system32/REDIR.EXE'],
  ['build/output/dos/DOSX.EXE', 'system32/DOSX.EXE'],
  ['src/mvdm-softpc-firmware/softpc.new/roms/bios1.rom', 'softpc/bios1.rom'],
  ['src/mvdm-softpc-firmware/softpc.new/roms/bios2.rom', 'softpc/bios2.rom'],
  ['src/mvdm-softpc-firmware/softpc.new/roms/bios4.rom', 'softpc/bios4.rom'],
  ['src/mvdm-softpc-firmware/softpc.new/roms/v7vga.rom', 'softpc/v7vga.rom'],
  ['src/mvdm-softpc-firmware/softpc.new/roms/profile.spc', 'softpc/profile.spc'],
  ['src/mvdm-softpc-firmware/softpc.new/roms/cmos.ram', 'softpc/cmos.ram']
];

function usage() {
  throw new Error('usage: node tools/build/Stage-OriginalSoftpcRuntime.mjs --executable <product.exe> --output <new-directory>');
}

function sha256(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

let executable = null;
let output = null;
for (let index = 2; index < process.argv.length; index += 2) {
  const option = process.argv[index];
  const value = process.argv[index + 1];
  if (value === undefined) usage();
  if (option === '--executable') executable = value;
  else if (option === '--output') output = value;
  else usage();
}
if (executable === null || output === null) usage();

const executablePath = isAbsolute(executable) ? executable : resolve(process.cwd(), executable);
const outputPath = isAbsolute(output) ? output : resolve(process.cwd(), output);
if (!existsSync(executablePath)) throw new Error(`product executable does not exist: ${executablePath}`);
if (existsSync(outputPath)) throw new Error(`refusing to overwrite runtime package: ${outputPath}`);

mkdirSync(join(outputPath, 'system32'), { recursive: true });
mkdirSync(join(outputPath, 'softpc'), { recursive: true });
const manifest = [];
function stage(source, destination) {
  if (!existsSync(source)) throw new Error(`required source asset does not exist: ${source}`);
  const target = join(outputPath, destination);
  copyFileSync(source, target);
  manifest.push({ source, destination, sha256: sha256(source) });
}

stage(executablePath, 'original-softpc-process.exe');
for (const [source, destination] of requiredAssets) stage(join(scriptRoot, source), destination);
writeFileSync(join(outputPath, 'runtime-manifest.json'), `${JSON.stringify({
  format: 3,
  /* The EXE is intentionally replaced by each formal product observation.
   * Everything in mediaAssets is immutable fixed-container input. */
  product: manifest[0],
  mediaAssets: manifest.slice(1)
}, null, 2)}\n`);
console.log(outputPath);
