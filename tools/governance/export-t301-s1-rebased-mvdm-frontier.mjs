import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');

function readTsv(name) {
  const split = (line) => {
    const fields = []; let field = ''; let quoted = false;
    for (let index = 0; index < line.length; index += 1) {
      const character = line[index];
      if (character === '"') {
        if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted;
      } else if (character === '\t' && !quoted) { fields.push(field); field = ''; } else field += character;
    }
    fields.push(field); return fields;
  };
  const [header, ...body] = fs.readFileSync(path.join(operations, name), 'utf8').split(/\r?\n/).filter(Boolean).map(split);
  return body.map((fields) => Object.fromEntries(header.map((key, index) => [key, fields[index] || ''])));
}
function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
function key(definition) { return `${definition.source_root}|${definition.source_path}|${definition.source_sha256}|${definition.source_line}`; }
function write(name, rows, columns) { fs.writeFileSync(path.join(operations, name), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`); }

const oldBoundary = readTsv('mvdm-first-degree-rebaselined-boundary-ledger.tsv');
const confirmedBoundary = readTsv('mvdm-first-degree-rebaselined-full-mvdm-signature-gate-ledger.tsv')
  .filter((row) => row.next_disposition.startsWith('source call'));
const confirmedBoundaryIds = new Set(confirmedBoundary.map((row) => row.candidate_id));
const definitions = new Map(readTsv('mvdm-zero-degree-rebase-expansion-definition-ledger.tsv').map((row) => [key(row), row]));
const oldDefinitions = new Map(readTsv('mvdm-zero-degree-call-closure-ledger.tsv').map((row) => [key(row), row]));
const seed = new Set(confirmedBoundary.map((row) => row.selected_definition_identity));
const confirmedEdges = readTsv('mvdm-zero-degree-rebase-expansion-edge-gate-ledger.tsv')
  .filter((row) => row.next_disposition.startsWith('confirmed'));
const edgeKey = (edge) => `${edge.caller_identity}|${edge.call_source_line}|${edge.selected_definition_identity || edge.internal_candidate_identity}`;
const confirmedEdgeKeys = new Set(confirmedEdges.map(edgeKey));
const outgoing = new Map();
for (const edge of confirmedEdges) outgoing.set(edge.caller_identity, [...(outgoing.get(edge.caller_identity) || []), edge.selected_definition_identity]);
const admitted = new Set(seed); const pending = [...seed];
while (pending.length) {
  const caller = pending.shift();
  for (const target of outgoing.get(caller) || []) if (definitions.has(target) && !admitted.has(target)) { admitted.add(target); pending.push(target); }
}
const admittedRows = [...admitted].filter((identity) => !oldDefinitions.has(identity)).map((identity) => {
  const definition = definitions.get(identity); if (!definition) throw new Error(`missing expansion definition ${identity}`);
  return { ...definition, rebase_admission: seed.has(identity) ? 'confirmed original-boundary call' : 'confirmed expanded original-MVDM internal edge' };
}).sort((left, right) => left.source_path.localeCompare(right.source_path) || Number(left.source_line) - Number(right.source_line));
const newEdges = readTsv('mvdm-zero-degree-rebase-expansion-edge-ledger.tsv');
const newFirst = newEdges.filter((edge) => !edge.internal_candidate_identity || !confirmedEdgeKeys.has(edgeKey(edge)));
const retainedOld = oldBoundary.filter((row) => !confirmedBoundaryIds.has(row.candidate_id)).map((row) => ({ ...row, frontier_origin: 'retained T300 physical boundary call' }));
const appended = newFirst.map((edge, index) => ({
  candidate_id: `MVDM-FIRST-REBASE-NEW-${String(index + 1).padStart(6, '0')}`,
  caller_symbol: edge.caller_symbol,
  caller_source_path: edge.caller_source_path,
  caller_source_sha256: edge.caller_source_sha256,
  caller_source_line: edge.call_source_line,
  callee_spelling: edge.callee_spelling,
  call_form: 'direct-named-call',
  boundary: edge.internal_candidate_identity
    ? 'Unconfirmed original-MVDM internal candidate remains at first-degree pending source-shape/frontier proof.'
    : 'Direct exit from an admitted newly discovered original-MVDM body; first-degree classification required.',
  frontier_origin: edge.internal_candidate_identity ? 'unconfirmed expanded internal edge' : 'new direct exit from admitted MVDM body',
}));
const frontier = [...retainedOld, ...appended];
if (frontier.length !== oldBoundary.length - confirmedBoundaryIds.size + appended.length) throw new Error('rebased frontier cardinality mismatch');
write('mvdm-zero-degree-rebase-admitted-definition-ledger.tsv', admittedRows, ['symbol', 'source_root', 'source_path', 'source_sha256', 'source_line', 'linkage', 'source', 'rebase_admission']);
write('mvdm-first-degree-rebased-boundary-ledger.tsv', frontier, ['candidate_id', 'caller_symbol', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'callee_spelling', 'call_form', 'boundary', 'frontier_origin']);
console.log(`confirmed old boundary calls removed=${confirmedBoundaryIds.size}; admitted new zero-degree bodies=${admittedRows.length}; retained old first-degree calls=${retainedOld.length}; appended new first-degree calls=${appended.length}; rebased first-degree frontier=${frontier.length}`);
