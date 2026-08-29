/*
 * Per-file, per-owner review index for M0 T310 S8 P4.
 *
 * This joins the source-first ABI candidate census with the unsuppressed
 * compiler cross-check. It makes exhaustive source review tractable without
 * treating diagnostic counts as a safety or implementation decision.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const abiInput = path.resolve(root, process.argv[3] ?? 'docs/etc/operations/m0-t310-s8-p4-whole-tree-static-abi-inventory.tsv');
const warningInput = path.resolve(root, process.argv[4] ?? 'docs/etc/operations/m0-t310-s8-p4-whole-tree-compiler-warning-ledger.tsv');
const output = path.resolve(root, process.argv[5] ?? 'docs/etc/operations/m0-t310-s8-p4-whole-tree-contract-review-index.tsv');

function readTsv(file) {
  const [header, ...records] = fs.readFileSync(file, 'utf8').replace(/^\uFEFF/, '').trimEnd().split(/\r?\n/);
  const fields = header.split('\t');
  return records.filter(Boolean).map(record => {
    const values = record.split('\t');
    return Object.fromEntries(fields.map((field, index) => [field, values[index] ?? '']));
  });
}

const entries = new Map();
function get(cluster, sourcePath) {
  const key = `${cluster}\u001f${sourcePath}`;
  if (!entries.has(key)) {
    entries.set(key, {
      review_cluster: cluster,
      source_path: sourcePath,
      profiles: new Set(),
      candidateKinds: new Map(),
      symbols: new Set(),
      warnings: new Map(),
      candidateCount: 0,
      diagnosticOccurrences: 0,
      diagnosticUnique: 0
    });
  }
  return entries.get(key);
}

for (const row of readTsv(abiInput)) {
  const entry = get(row.review_cluster, row.source_path);
  entry.profiles.add(row.profile_class);
  entry.candidateCount += 1;
  entry.candidateKinds.set(row.candidate_kind, (entry.candidateKinds.get(row.candidate_kind) ?? 0) + 1);
  if (row.symbol && row.symbol !== '-') entry.symbols.add(row.symbol);
}
for (const row of readTsv(warningInput)) {
  const entry = get(row.review_cluster, row.source_path);
  entry.diagnosticUnique += 1;
  entry.diagnosticOccurrences += Number(row.occurrence_count || '0');
  entry.warnings.set(row.warning, (entry.warnings.get(row.warning) ?? 0) + Number(row.occurrence_count || '0'));
}

function joinCounts(values) {
  return [...values.entries()].sort((left, right) => left[0].localeCompare(right[0]))
    .map(([name, count]) => `${name}:${count}`).join(';');
}
const header = [
  'review_id', 'review_cluster', 'profile_class', 'source_path',
  'static_candidate_count', 'candidate_kinds', 'candidate_symbols',
  'compiler_diagnostic_occurrence_count', 'compiler_unique_diagnostic_count',
  'compiler_warning_counts', 'source_review_state', 'required_source_review'
];
const lines = [header.join('\t')];
const ordered = [...entries.values()].sort((left, right) => left.review_cluster.localeCompare(right.review_cluster) || left.source_path.localeCompare(right.source_path));
ordered.forEach((entry, index) => {
  const profile = entry.profiles.size === 0 ? 'compiler-crosscheck-only' : [...entry.profiles].sort().join(';');
  const symbols = [...entry.symbols].sort().join(';') || '-';
  lines.push([
    `T310-S8-P4-REVIEW-${String(index + 1).padStart(4, '0')}`,
    entry.review_cluster, profile, entry.source_path,
    entry.candidateCount, joinCounts(entry.candidateKinds), symbols,
    entry.diagnosticOccurrences, entry.diagnosticUnique, joinCounts(entry.warnings),
    'pending-original-contract-review',
    'read original declarations, assignments/initializers and selected callers; classify as minimal mirror correction, overlay, checked guest/physical mapping, or not selected'
  ].map(value => String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ')).join('\t'));
});
fs.mkdirSync(path.dirname(output), { recursive: true });
fs.writeFileSync(output, `${lines.join('\r\n')}\r\n`, 'utf8');
console.log(JSON.stringify({ review_rows: ordered.length, source_files: new Set(ordered.map(entry => entry.source_path)).size }, null, 2));
