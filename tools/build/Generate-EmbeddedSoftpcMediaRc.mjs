import { mkdirSync, writeFileSync } from 'node:fs';
import { dirname, resolve } from 'node:path';

if (process.argv.length !== 4) {
  throw new Error('usage: node Generate-EmbeddedSoftpcMediaRc.mjs <repository-root> <output-rc>');
}

const root = resolve(process.argv[2]);
const output = resolve(process.argv[3]);
const names = ['bios1.rom', 'bios2.rom', 'bios4.rom', 'v7vga.rom', 'profile.spc', 'cmos.ram'];
const quoteForRc = (path) => path.replace(/\\/g, '\\\\');
const lines = [
  '/* Generated build carrier: immutable original SoftPC media embedded in the selected product. */',
  '#include <windows.h>',
  ''
];
for (const name of names) {
  const source = resolve(root, 'src', 'mvdm-host', 'softpc.new', 'roms', name);
  lines.push(`"${name}" RCDATA "${quoteForRc(source)}"`);
}
mkdirSync(dirname(output), { recursive: true });
writeFileSync(output, `${lines.join('\n')}\n`, 'utf8');
