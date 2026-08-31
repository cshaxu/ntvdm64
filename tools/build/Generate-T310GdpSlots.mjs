/*
 * Derive an x86/x64-safe GDP macro overlay from the exact header selected by
 * the formal SoftPC graph.  It intentionally leaves the selected patch
 * header unchanged: field spellings, source ordering and declared types come
 * from that historical generated input, while storage is supplied privately
 * by mvdm-host-overlay.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const outputDirectory = path.resolve(root, process.argv[3] ??
  'build/M0-T310/S8/p1-machine-source/generated/gdp');
const source = path.resolve(root,
  'src/mvdm-softpc-patch/x86/prod/gdpvar.h');
const output = path.join(outputDirectory, 'mvdm_gdp_slots.h');

const lines = fs.readFileSync(source, 'utf8').split(/\r?\n/);
const directLvaluePattern = /^#define\s+(GLOBAL_[A-Za-z0-9_]+)\s+\(\*\((.+)\)\s*\(\(IUH\)Gdp\s*\+\s*([0-9+\s-]+)\)\)$/;
const directPointerPattern = /^#define\s+(GLOBAL_[A-Za-z0-9_]+)\s+\(\((.+)\)\s*\(\(IUH\)Gdp\s*\+\s*([0-9+\s-]+)\)\)$/;

function parseOffset(expression) {
  if (!/^-?\d+(\s*\+\s*-?\d+)*$/.test(expression)) return null;
  return expression.split('+').reduce((sum, term) => sum + Number(term.trim()), 0);
}

function lvalueType(castType) {
  const value = castType.trim().replace(/\*\s*$/, '').trim();
  if (value === castType.trim()) throw new Error(`GDP lvalue cast is not a pointer: ${castType}`);
  return value;
}

const records = [];
for (const [index, line] of lines.entries()) {
  const lvalue = line.match(directLvaluePattern);
  const pointer = line.match(directPointerPattern);
  const match = lvalue ?? pointer;
  if (!match) continue;
  const offset = parseOffset(match[3]);
  if (offset === null) throw new Error(`unsupported GDP offset at ${index + 1}`);
  records.push({
    name: match[1],
    type: lvalue ? lvalueType(match[2]) : match[2].trim(),
    offset,
    line: index + 1,
    form: lvalue ? 'lvalue' : 'pointer'
  });
}

if (records.length === 0) throw new Error('no selected GDP macros found');
const generated = [
  '/* Generated from src/mvdm-softpc-patch/x86/prod/gdpvar.h. */',
  '#ifndef MVDM_T310_GDP_SLOTS_H',
  '#define MVDM_T310_GDP_SLOTS_H',
  '',
  '#include "mvdm_gdp_state.h"',
  '',
  '/* Replace only direct fixed-base forms after the historical header has',
  ' * established every original spelling and helper macro.  The selected x86',
  ' * runtime deliberately retains the original contiguous 64 KiB GDP layout:',
  ' * CCPU-generated rules address that layout directly through jccc_gdp.',
  ' * Native-width slot storage is therefore an x64 compile/link carrier only.',
  ' */',
  '#if !defined(_M_IX86)'
];
for (const record of records) {
  generated.push(`#undef ${record.name}`);
  if (record.form === 'lvalue') {
    generated.push(`#define ${record.name} (*((` +
      `${record.type} *)mvdm_gdp_slot((const void *)Gdp, ${record.offset}u, ` +
      `sizeof(${record.type}))))`);
  } else {
    generated.push(`#define ${record.name} ((` +
      `${record.type})mvdm_gdp_slot((const void *)Gdp, ${record.offset}u, ` +
      `${record.name}Size))`);
  }
}
generated.push('#endif /* !_M_IX86 */', '', '#endif /* MVDM_T310_GDP_SLOTS_H */', '');
fs.mkdirSync(outputDirectory, { recursive: true });
fs.writeFileSync(output, generated.join('\r\n'), 'utf8');
console.log(JSON.stringify({ source, output, slots: records.length }, null, 2));
