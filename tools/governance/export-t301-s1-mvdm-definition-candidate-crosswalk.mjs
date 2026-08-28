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

const boundary = new Map(readTsv('mvdm-first-degree-rebaselined-boundary-ledger.tsv').map((row) => [row.candidate_id, row]));
const declarations = readTsv('mvdm-first-degree-rebaselined-declaration-shape-ledger.tsv')
  .filter((row) => row.next_disposition.startsWith('function-declaration-candidate'));
const definitions = readTsv('mvdm-first-degree-rebaselined-mvdm-definition-form-candidate-ledger.tsv');
const definitionsByName = new Map();
for (const definition of definitions) definitionsByName.set(definition.symbol, [...(definitionsByName.get(definition.symbol) || []), definition]);

const rows = declarations.map((declaration) => {
  const call = boundary.get(declaration.candidate_id);
  if (!call) throw new Error(`missing physical boundary identity ${declaration.candidate_id}`);
  const candidates = definitionsByName.get(declaration.callee_spelling) || [];
  const identities = candidates.map((candidate) => `${candidate.source_root}|${candidate.source_path}|${candidate.source_sha256}|${candidate.source_line}`).join(';');
  return {
    candidate_id: declaration.candidate_id,
    caller_symbol: call.caller_symbol,
    caller_source_path: call.caller_source_path,
    caller_source_sha256: call.caller_source_sha256,
    caller_source_line: call.caller_source_line,
    callee_spelling: declaration.callee_spelling,
    declaration_shapes: declaration.declaration_shapes,
    original_mvdm_definition_candidate_count: String(candidates.length),
    original_mvdm_definition_identities: identities,
    next_disposition: candidates.length
      ? 'candidate original-MVDM body exists; require per-call include/linkage/variant binding before zero-degree reclassification'
      : 'no original-MVDM body candidate; continue T301 approved-source-union resolution',
  };
});

if (rows.length !== declarations.length) throw new Error('candidate crosswalk cardinality mismatch');
const columns = Object.keys(rows[0]);
const output = path.join(operations, 'mvdm-first-degree-rebaselined-definition-candidate-crosswalk-ledger.tsv');
fs.writeFileSync(output, `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
const withCandidate = rows.filter((row) => row.original_mvdm_definition_candidate_count !== '0');
console.log(`declaration-shaped physical calls=${rows.length}; calls with original-MVDM body candidates=${withCandidate.length}; calls without an original-MVDM body candidate=${rows.length - withCandidate.length}`);
