import fs from 'node:fs';
import path from 'node:path';

function fail(message) {
  throw new Error(message);
}

function readText(filename) {
  return fs.readFileSync(filename, 'utf8');
}

function writeText(filename, content) {
  fs.writeFileSync(filename, content, 'utf8');
}

function definitionsFrom(source) {
  const definitions = new Map();
  const pattern = /^\s*(?:GLOBAL\s+)?VOID\s*(?:\r?\n\s*)?([A-Za-z_][A-Za-z0-9_]*)\s+IFN([0-9])\((.*?)\)/gms;
  for (const match of source.matchAll(pattern)) {
    definitions.set(
      match[1],
      `VOID ${match[1]} IPT${match[2]}(${match[3].replace(/\s+/g, ' ').trim()});`
    );
  }
  return definitions;
}

function importDefinitionsFrom(source) {
  const definitions = new Map();
  const pattern = /^\s*(?:GLOBAL\s+)?VOID\s*(?:\r?\n\s*)?([A-Za-z_][A-Za-z0-9_]*)\s+IFN([0-9])\((.*?)\)/gms;
  for (const match of source.matchAll(pattern)) {
    definitions.set(
      match[1],
      `IMPORT VOID ${match[1]} IPT${match[2]}(${match[3].replace(/\s+/g, ' ').trim()});`
    );
  }
  return definitions;
}

if (process.argv.length !== 4) {
  fail('usage: node Generate-T310BaseVideoTypedSources.mjs <repository-root> <output-directory>');
}

const repositoryRoot = path.resolve(process.argv[2]);
const output = path.resolve(process.argv[3]);
const videoRoot = path.join(repositoryRoot, 'src/mvdm-host/softpc.new/base/video');
fs.mkdirSync(output, { recursive: true });

for (const name of ['egawrtm0.c', 'egwrtm12.c']) {
  const sourcePath = path.join(videoRoot, name);
  const source = readText(sourcePath);
  const definitions = definitionsFrom(source);
  if (definitions.size === 0) fail(`No original IFN definitions discovered: ${sourcePath}`);

  const rewritten = source.replace(
    /^([ \t]*VOID[ \t]+)([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*\)\s*;/gm,
    (match, unusedPrefix, symbol) => definitions.get(symbol) ?? match
  );
  writeText(
    path.join(output, name),
    `/* Generated source-shaped declaration carrier.  Derived from exact original ${name}.\r\n` +
      ' * DIVERGENCE: table-target declarations receive the later original IFN signature\r\n' +
      ' * before their initializer; no algorithm, table order, or function body changes. */\r\n' +
      rewritten
  );
}

let gfx = readText(path.join(videoRoot, 'gfx_updt.c'));
for (const [from, to] of [
  ['(boolean(*)())cga_mark_byte', 't310_cga_mark_byte'],
  ['(boolean(*)())cga_mark_word', 't310_cga_mark_word'],
  ['(boolean(*)())ega_mark_byte', 't310_ega_mark_byte'],
  ['(boolean(*)())ega_mark_word', 't310_ega_mark_word'],
  ['(boolean(*)())ega_mark_string', 't310_ega_mark_string'],
  ['(boolean(*)())ega_mark_wfill', 't310_ega_mark_wfill']
]) {
  gfx = gfx.split(from).join(to);
}
gfx = gfx.split('update_alg.mark_fill = (boolean(*)())cga_mark_string;').join('update_alg.mark_fill = t310_cga_mark_fill;');
gfx = gfx.split('update_alg.mark_wfill = (boolean(*)())cga_mark_string;').join('update_alg.mark_wfill = t310_cga_mark_wfill;');
gfx = gfx.split('update_alg.mark_string = (boolean(*)())cga_mark_string;').join('update_alg.mark_string = t310_cga_mark_string;');

const wrappers = `/* DIVERGENCE: original update_alg stores VOID marking procedures through
 * boolean callback slots.  All reached callers discard the boolean result.
 * These typed carriers preserve the original marking call and define that
 * historically unspecified result as FALSE. */
static boolean t310_cga_mark_byte IFN1(int, address) { cga_mark_byte(address); return FALSE; }
static boolean t310_cga_mark_word IFN1(int, address) { cga_mark_word(address); return FALSE; }
static boolean t310_cga_mark_fill IFN2(int, first, int, last) { cga_mark_string(first, last); return FALSE; }
static boolean t310_cga_mark_wfill IFN3(int, first, int, last, int, colour) { UNUSED(colour); cga_mark_string(first, last); return FALSE; }
static boolean t310_cga_mark_string IFN2(int, first, int, last) { cga_mark_string(first, last); return FALSE; }
static boolean t310_ega_mark_byte IFN1(int, address) { ega_mark_byte(address); return FALSE; }
static boolean t310_ega_mark_word IFN1(int, address) { ega_mark_word(address); return FALSE; }
static boolean t310_ega_mark_fill IFN2(int, first, int, last) { ega_mark_string(first, last); return FALSE; }
static boolean t310_ega_mark_wfill IFN3(int, first, int, last, int, colour) { ega_mark_wfill(first, last, colour); return FALSE; }
static boolean t310_ega_mark_string IFN2(int, first, int, last) { ega_mark_string(first, last); return FALSE; }

`;
const marker = 'GLOBAL VOID\nset_mark_funcs IFN0()';
const normalizedGfx = gfx.replace(/\r\n/g, '\n');
if (!normalizedGfx.includes(marker)) fail('Unable to find original set_mark_funcs boundary');
gfx = normalizedGfx.replace(marker, wrappers + marker);
writeText(
  path.join(output, 'gfx_updt.c'),
  '/* Generated source-shaped callback carrier.  Derived from exact original gfx_updt.c.\r\n' +
    ' * DIVERGENCE: only the historically untyped marking callback casts are replaced.\r\n */\r\n' +
    gfx
);

const writeDefinitions = new Map();
for (const provider of ['egawrtm0.c', 'egwrtm12.c']) {
  for (const [name, declaration] of importDefinitionsFrom(readText(path.join(videoRoot, provider)))) {
    writeDefinitions.set(name, declaration);
  }
}
let egaWrit = readText(path.join(videoRoot, 'ega_writ.c'));
egaWrit = egaWrit.replace(
  /^[ \t]*IMPORT\s+VOID\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*\)\s*;/gm,
  (match, symbol) => writeDefinitions.get(symbol) ?? match
);
writeText(
  path.join(output, 'ega_writ.c'),
  '/* Generated source-shaped declaration carrier.  Derived from exact original ega_writ.c.\r\n' +
    ' * DIVERGENCE: CHAIN2 BIOS writer imports receive the exact original provider IFN signatures.\r\n */\r\n' +
    egaWrit
);

const vgaMode = readText(path.join(videoRoot, 'vga_mode.c'))
  .replace('extern void jazz_text_update();', 'extern void jazz_text_update IPT0();');
writeText(
  path.join(output, 'vga_mode.c'),
  '/* Generated source-shaped declaration carrier.  Derived from exact original vga_mode.c.\r\n' +
    ' * DIVERGENCE: preserve the original zero-argument display-update ABI at its external declaration.\r\n */\r\n' +
    vgaMode
);

process.stdout.write(JSON.stringify({ generated: ['egawrtm0.c', 'egwrtm12.c', 'gfx_updt.c', 'ega_writ.c', 'vga_mode.c'] }) + '\n');
