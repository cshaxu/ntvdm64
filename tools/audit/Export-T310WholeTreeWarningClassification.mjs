/*
 * First-pass x86/x64 relevance classification for T310 S8 P4 compiler
 * diagnostics.  This does not decide a source fix from a warning spelling:
 * every output row retains the physical source location and architecture
 * evidence so the owning original package can read the declaration/caller.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const input = path.resolve(root, process.argv[3] ??
  'docs/etc/operations/m0-t310-s8-p4-whole-tree-compiler-warning-ledger.tsv');
const output = path.resolve(root, process.argv[4] ??
  'docs/etc/operations/m0-t310-s8-p4-whole-tree-warning-x86-x64-classification.tsv');

function splitTsv(line) { return line.split('\t'); }
function classificationFor(warning, architectures) {
  const x64Only = architectures.size === 1 && architectures.has('x64');
  if (new Set(['C4311', 'C4312', 'C4313']).has(warning)) {
    return ['must-repair-width-or-pointer-abi',
      'pointer/integer or variadic-width conversion; read the exact original value class and use a same-shaped native-width binding or the mapping manager'];
  }
  if (new Set(['C4013', 'C4057', 'C4113', 'C4133', 'C4047', 'C4152', 'C4431']).has(warning)) {
    return ['must-repair-call-abi',
      'undeclared, old-style, or incompatible call/table contract may truncate a pointer or pass the wrong calling convention; reconcile declaration, definition, table slot and selected callers'];
  }
  if (new Set(['C4244', 'C4267', 'C4305', 'C4306', 'C4310', 'C4366']).has(warning)) {
    return [x64Only ? 'must-read-x64-narrowing' : 'read-value-range-and-architecture-delta',
      'narrowing/widening is not automatically a defect; determine guest fixed-width value, private native value, or host identity before selecting original correction, overlay, or mapping-manager boundary'];
  }
  if (warning === 'C4131') {
    return ['must-read-old-style-call-contract',
      'K&R declaration may hide a modern parameter/return ABI mismatch; compare the selected declaration and callers before deciding whether it is a harmless source-style warning'];
  }
  if (new Set(['C4701', 'C4703', 'C4715', 'C4033']).has(warning)) {
    return ['must-repair-control-or-result-contract',
      'uninitialized or absent result can become architecture-visible state; repair the original failure/result contract, not the warning'];
  }
  if (warning === 'C4100') {
    return [x64Only ? 'read-x64-only-unused-formal' : 'nonwidth-unless-call-contract-evidence',
      'unused formal is not a width defect by itself.  Retain as evidence; inspect only when the selected declaration/caller shows a pointer, calling-convention, or omitted-result boundary'];
  }
  if (new Set(['C4005', 'C4101', 'C4102', 'C4127', 'C4142', 'C4189', 'C4210', 'C4211', 'C4245', 'C4389', 'C4456', 'C4457', 'C4459', 'C4554', 'C4706', 'C4996']).has(warning)) {
    return ['nonwidth-source-quality-not-current-blocker',
      'not an x86/x64 compatibility diagnosis on its own; retain the source row for later package review but do not create an adapter or modify a mirror solely for this warning'];
  }
  return ['unclassified-read-source',
    'warning class has no approved general disposition; read original source, declaration and selected caller before any change'];
}

const rows = fs.readFileSync(input, 'utf8').split(/\r?\n/).filter(Boolean).map(splitTsv);
const header = rows.shift();
const index = Object.fromEntries(header.map((name, position) => [name, position]));
for (const name of ['record_id', 'architecture', 'source_path', 'source_line', 'warning', 'message', 'review_cluster']) {
  if (!(name in index)) throw new Error(`missing required input column: ${name}`);
}
const grouped = new Map();
for (const row of rows) {
  const key = [row[index.source_path], row[index.source_line], row[index.warning], row[index.message], row[index.review_cluster]].join('\u001f');
  if (!grouped.has(key)) grouped.set(key, { row, architectures: new Set(), sourceRecords: [] });
  const group = grouped.get(key);
  group.architectures.add(row[index.architecture]);
  group.sourceRecords.push(row[index.record_id]);
}
const outputLines = [[
  'classification_id', 'source_path', 'source_line', 'warning', 'message',
  'architectures', 'review_cluster', 'x86_x64_disposition', 'required_next_read',
  'source_records'
].join('\t')];
let serial = 0;
const worklistDispositions = new Set([
  'must-repair-width-or-pointer-abi', 'must-repair-call-abi',
  'must-read-x64-narrowing', 'read-value-range-and-architecture-delta',
  'must-read-old-style-call-contract', 'must-repair-control-or-result-contract',
  'read-x64-only-unused-formal', 'unclassified-read-source'
]);
const classifiedRows = [];
for (const { row, architectures, sourceRecords } of [...grouped.values()].sort((a, b) => {
  const aa = a.row[index.review_cluster].localeCompare(b.row[index.review_cluster]);
  return aa || a.row[index.source_path].localeCompare(b.row[index.source_path]) ||
    Number(a.row[index.source_line]) - Number(b.row[index.source_line]) ||
    a.row[index.warning].localeCompare(b.row[index.warning]);
})) {
  const [disposition, nextRead] = classificationFor(row[index.warning], architectures);
  const classified = [
    `T310-S8-P4-CLASS-${String(++serial).padStart(6, '0')}`,
    row[index.source_path], row[index.source_line], row[index.warning], row[index.message],
    [...architectures].sort().join(','), row[index.review_cluster], disposition, nextRead, sourceRecords.join(',')
  ];
  classifiedRows.push(classified);
  outputLines.push(classified.map(value => String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ')).join('\t'));
}
fs.mkdirSync(path.dirname(output), { recursive: true });
fs.writeFileSync(output, `${outputLines.join('\r\n')}\r\n`, 'utf8');
const worklistOutput = output.replace(/\.tsv$/i, '-worklist.tsv');
const worklist = [outputLines[0], ...classifiedRows
  .filter(row => worklistDispositions.has(row[7]))
  .map(row => row.map(value => String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ')).join('\t'))];
fs.writeFileSync(worklistOutput, `${worklist.join('\r\n')}\r\n`, 'utf8');
const counts = new Map();
for (const line of outputLines.slice(1)) {
  const disposition = splitTsv(line)[7];
  counts.set(disposition, (counts.get(disposition) ?? 0) + 1);
}
console.log(JSON.stringify({ classified_source_diagnostics: serial, x86_x64_worklist_records: worklist.length - 1, by_disposition: Object.fromEntries(counts) }, null, 2));
