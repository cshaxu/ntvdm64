/*
 * Inventory the GDP generated header actually selected by the formal SoftPC
 * build before any x64 overlay is designed.  The selected NTVDMx64 x86/prod
 * carrier is a retained patch input and shares the CVIDC layout family; it
 * takes precedence over the unselected ccpu386-local generated header.  The
 * original EDL generator is absent from all local source editions, so this
 * tool treats the selected emitted macro table as the authoritative
 * source-layout record.  It intentionally does not rewrite it.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const input = path.resolve(root, process.argv[3] ??
  'src/mvdm-softpc-patch/x86/prod/gdpvar.h');
const output = path.resolve(root, process.argv[4] ??
  'docs/etc/operations/m0-t310-s8-p4-gdp-generated-layout-inventory.tsv');

const lines = fs.readFileSync(input, 'utf8').split(/\r?\n/);
const rows = [];
const offsets = new Map();
const macroPattern = /^#define\s+(GLOBAL_[A-Za-z0-9_]+)\s+\(\*\((.+)\)\s*\(\(IUH\)Gdp\s*\+\s*(-?\d+)\)\)$/;
for (let number = 0; number < lines.length; number += 1) {
  const match = lines[number].match(macroPattern);
  if (!match) continue;
  const [, name, type, offsetText] = match;
  const offset = Number(offsetText);
  const record = { name, type: type.trim(), offset, line: number + 1 };
  rows.push(record);
  const group = offsets.get(offset) ?? [];
  group.push(record);
  offsets.set(offset, group);
}
const outputLines = [[
  'record_id', 'macro', 'original_line', 'original_offset', 'declared_type',
  'same_offset_macros', 'x64_overlay_disposition'
].join('\t')];
for (const [index, row] of rows.entries()) {
  const aliases = offsets.get(row.offset).map(value => value.name).join(',');
  const disposition = aliases.includes(',')
    ? 'retain-original-offset-alias-group; require one typed slot group'
    : 'candidate-generated-typed-slot';
  outputLines.push([
    `T310-S8-P4-GDP-${String(index + 1).padStart(4, '0')}`,
    row.name, row.line, row.offset, row.type, aliases, disposition
  ].join('\t'));
}
fs.mkdirSync(path.dirname(output), { recursive: true });
fs.writeFileSync(output, `${outputLines.join('\r\n')}\r\n`, 'utf8');
console.log(JSON.stringify({
  input,
  output,
  generated_macros: rows.length,
  original_offset_groups: offsets.size,
  aliased_offset_groups: [...offsets.values()].filter(group => group.length > 1).length
}, null, 2));
