import { createHash } from 'node:crypto';
import { readdirSync, readFileSync, statSync, writeFileSync } from 'node:fs';
import { dirname, extname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const repositoryRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..');
const outputPath = join(repositoryRoot, 'docs', 'etc', 'operations',
  'm0-t348-s1-guest-artifact-matrix.tsv');

const stagedGuestPaths = new Map([
  ['dos/v86/doskrnl/bios/NTIO.SYS', 'mvdm/NTIO.SYS'],
  ['dos/v86/doskrnl/dos/NTDOS.SYS', 'mvdm/NTDOS.SYS'],
  ['dos/v86/cmd/command/COMMAND.COM', 'mvdm/COMMAND.COM;mvdm/system32/COMMAND.COM'],
  ['bin86/config.nt', 'mvdm/config.nt'],
  ['bin86/autoexec.nt', 'mvdm/autoexec.nt'],
  ['dos/v86/dev/country/COUNTRY.SYS', 'mvdm/system32/COUNTRY.SYS'],
  ['dos/v86/dev/himem/HIMEM.SYS', 'mvdm/system32/HIMEM.SYS']
]);
const stagedOutputPaths = new Map([
  ['dos/REDIR.EXE', 'mvdm/system32/REDIR.EXE'],
  ['dos/DOSX.EXE', 'mvdm/system32/DOSX.EXE']
]);
const stagedFirmwarePaths = new Map([
  ['softpc.new/roms/bios1.rom', 'mvdm/softpc/bios1.rom'],
  ['softpc.new/roms/bios2.rom', 'mvdm/softpc/bios2.rom'],
  ['softpc.new/roms/bios4.rom', 'mvdm/softpc/bios4.rom'],
  ['softpc.new/roms/v7vga.rom', 'mvdm/softpc/v7vga.rom'],
  ['softpc.new/roms/profile.spc', 'mvdm/softpc/profile.spc'],
  ['softpc.new/roms/cmos.ram', 'mvdm/softpc/cmos.ram']
]);
const binaryExtensions = new Set(['.com', '.dll', '.drv', '.exe', '.fon', '.rom',
  '.sys']);
const buildExtensions = new Set(['.lib', '.lnk', '.map', '.obj', '.pdb', '.res',
  '.sym']);

function walk(root) {
  const paths = [];
  for (const entry of readdirSync(root, { withFileTypes: true })) {
    const path = join(root, entry.name);
    if (entry.isDirectory()) paths.push(...walk(path));
    else if (entry.isFile()) paths.push(path);
  }
  return paths.sort((left, right) => left.localeCompare(right));
}

function digest(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

function fileClass(path) {
  const extension = extname(path).toLowerCase();
  if (binaryExtensions.has(extension)) return 'binary-product';
  if (buildExtensions.has(extension)) return 'build-intermediate';
  return 'source-resource-or-control';
}

function tsv(value) {
  return String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ');
}

const rows = [];
function addRoot(rootName, rootPath, series, dispositionForPath, reference) {
  for (const path of walk(rootPath)) {
    const relativePath = relative(rootPath, path).replaceAll('\\', '/');
    if (rootName === 'mvdm-guest' && relativePath === 'README.md') continue;
    const disposition = dispositionForPath(relativePath);
    rows.push([
      rootName,
      relativePath,
      statSync(path).size,
      digest(path),
      fileClass(path),
      series,
      disposition.kind,
      disposition.destination,
      reference
    ]);
  }
}

addRoot('mvdm-guest', join(repositoryRoot, 'src', 'mvdm-guest'), 'selected-original-mirror',
  (relativePath) => stagedGuestPaths.has(relativePath)
    ? { kind: 'runtime-media', destination: stagedGuestPaths.get(relativePath) }
    : relativePath.startsWith('wow16/') || relativePath.startsWith('font16/')
      ? { kind: 'runtime-deferred-wow16', destination: '' }
      : { kind: 'mirror-not-current-stage', destination: '' },
  'm0-t274/m0-t297 guest carry manifests');
addRoot('build-output', join(repositoryRoot, 'build', 'output', 'dos'), 'source-built-cross-check',
  (relativePath) => stagedOutputPaths.has(`dos/${relativePath}`)
    ? { kind: 'runtime-media', destination: stagedOutputPaths.get(`dos/${relativePath}`) }
    : { kind: 'source-built-cross-check', destination: '' },
  'build/output/dos');
addRoot('build-output', join(repositoryRoot, 'build', 'output', 'wow16'), 'source-built-cross-check',
  () => ({ kind: 'source-built-cross-check', destination: '' }), 'build/output/wow16');
addRoot('mvdm-softpc-firmware', join(repositoryRoot, 'src', 'mvdm-softpc-firmware', 'softpc.new', 'roms'),
  'selected-original-firmware', (relativePath) => stagedFirmwarePaths.has(`softpc.new/roms/${relativePath}`)
    ? { kind: 'runtime-firmware-adjacent', destination: stagedFirmwarePaths.get(`softpc.new/roms/${relativePath}`) }
    : { kind: 'firmware-not-current-stage', destination: '' }, 'Stage-OriginalSoftpcRuntime.mjs');

rows.sort((left, right) => left[0].localeCompare(right[0]) || left[1].localeCompare(right[1]));
const header = ['root', 'relative_path', 'size_bytes', 'sha256', 'file_class',
  'source_series', 'final_disposition', 'staged_destination', 'provenance_reference'];
writeFileSync(outputPath, `${[header, ...rows].map((row) => row.map(tsv).join('\t')).join('\n')}\n`);
console.log(`${rows.length} rows -> ${relative(repositoryRoot, outputPath).replaceAll('\\', '/')}`);
