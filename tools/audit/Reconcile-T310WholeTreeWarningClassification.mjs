/*
 * Reconcile a freshly generated T310 S8 P4 compiler classification with the
 * versioned review ledger.  Exact current source identity is authoritative:
 * resolved warnings disappear, while a warning still emitted today must not
 * inherit an old closed-* disposition.  Only enduring reviewed dispositions
 * (such as a kernel hard boundary or a proven non-width source form) carry
 * forward on an exact source-path/line/warning/message match.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const currentInput = path.resolve(root, process.argv[3] ??
  'build/M0-T310/S8/p1-machine-source/s8-p4-current-full-warning-classification.tsv');
const priorInput = path.resolve(root, process.argv[4] ??
  'docs/etc/operations/m0-t310-s8-p4-whole-tree-warning-x86-x64-classification.tsv');
const output = path.resolve(root, process.argv[5] ??
  'docs/etc/operations/m0-t310-s8-p4-whole-tree-warning-x86-x64-classification.tsv');
const worklistOutput = path.resolve(root, process.argv[6] ??
  'docs/etc/operations/m0-t310-s8-p4-whole-tree-warning-x86-x64-classification-worklist.tsv');

function rows(file) {
  const result = fs.readFileSync(file, 'utf8').split(/\r?\n/).filter(Boolean)
    .map(line => line.split('\t'));
  return { header: result.shift(), rows: result };
}
function indexOf(header) {
  return Object.fromEntries(header.map((name, index) => [name, index]));
}
function key(row, index) {
  return [row[index.source_path], row[index.source_line], row[index.warning], row[index.message]].join('\u001f');
}
function retained(disposition) {
  return disposition.startsWith('not-x86-x64-') ||
    disposition.includes('hard-boundary') ||
    disposition.includes('not-host-runtime') ||
    disposition.includes('original-unavailable');
}
function escape(value) {
  return String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ');
}

const current = rows(currentInput);
const prior = rows(priorInput);
const currentIndex = indexOf(current.header);
const priorIndex = indexOf(prior.header);
for (const name of ['source_path', 'source_line', 'warning', 'message', 'x86_x64_disposition', 'required_next_read']) {
  if (!(name in currentIndex) || !(name in priorIndex)) throw new Error(`missing column: ${name}`);
}

const priorByKey = new Map();
for (const row of prior.rows) priorByKey.set(key(row, priorIndex), row);
let carried = 0;
for (const row of current.rows) {
  const priorRow = priorByKey.get(key(row, currentIndex));
  if (priorRow === undefined) continue;
  const disposition = priorRow[priorIndex.x86_x64_disposition];
  if (!retained(disposition)) continue;
  row[currentIndex.x86_x64_disposition] = disposition;
  row[currentIndex.required_next_read] = priorRow[priorIndex.required_next_read];
  carried += 1;
}

const worklistDispositions = new Set([
  'must-repair-width-or-pointer-abi', 'must-repair-call-abi',
  'must-read-x64-narrowing', 'read-value-range-and-architecture-delta',
  'must-read-old-style-call-contract', 'must-repair-control-or-result-contract',
  'read-x64-only-unused-formal', 'unclassified-read-source'
]);
const serialize = list => `${[current.header, ...list].map(row => row.map(escape).join('\t')).join('\r\n')}\r\n`;
fs.writeFileSync(output, serialize(current.rows), 'utf8');
const worklist = current.rows.filter(row => worklistDispositions.has(row[currentIndex.x86_x64_disposition]));
fs.writeFileSync(worklistOutput, serialize(worklist), 'utf8');
console.log(JSON.stringify({ current_records: current.rows.length, carried_review_dispositions: carried, current_worklist_records: worklist.length }, null, 2));
