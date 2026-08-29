/*
 * Apply one reviewed disposition to an exact current T310 whole-tree warning
 * cluster and regenerate the derived x86/x64 worklist.  This never suppresses
 * compiler output; it only records a completed source review.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const ledgerPath = path.resolve(root, process.argv[3]);
const sourcePath = process.argv[4];
const warning = process.argv[5];
const disposition = process.argv[6];
const nextRead = process.argv[7];
const worklistPath = path.resolve(root, process.argv[8]);
if (!ledgerPath || !sourcePath || !warning || !disposition || !nextRead || !worklistPath) {
  throw new Error('usage: root ledger source-path warning disposition next-read worklist');
}
const parsed = fs.readFileSync(ledgerPath, 'utf8').split(/\r?\n/).filter(Boolean).map(line => line.split('\t'));
const header = parsed.shift();
const index = Object.fromEntries(header.map((name, number) => [name, number]));
for (const name of ['source_path', 'warning', 'x86_x64_disposition', 'required_next_read']) {
  if (!(name in index)) throw new Error(`missing column: ${name}`);
}
let changed = 0;
for (const row of parsed) {
  if (row[index.source_path] !== sourcePath || row[index.warning] !== warning) continue;
  row[index.x86_x64_disposition] = disposition;
  row[index.required_next_read] = nextRead;
  changed += 1;
}
const worklistDispositions = new Set([
  'must-repair-width-or-pointer-abi', 'must-repair-call-abi',
  'must-read-x64-narrowing', 'read-value-range-and-architecture-delta',
  'must-read-old-style-call-contract', 'must-repair-control-or-result-contract',
  'read-x64-only-unused-formal', 'unclassified-read-source'
]);
const emit = rows => `${[header, ...rows].map(row => row.join('\t')).join('\r\n')}\r\n`;
fs.writeFileSync(ledgerPath, emit(parsed), 'utf8');
fs.writeFileSync(worklistPath, emit(parsed.filter(row => worklistDispositions.has(row[index.x86_x64_disposition]))), 'utf8');
console.log(JSON.stringify({ changed, worklist_records: parsed.filter(row => worklistDispositions.has(row[index.x86_x64_disposition])).length }, null, 2));
