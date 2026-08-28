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
function write(name, rows, columns) { fs.writeFileSync(path.join(operations, name), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`); }

const frontier = readTsv('mvdm-first-degree-rebased-boundary-ledger.tsv');
const sourceMap = new Map(readTsv('mvdm-first-degree-rebased-global-source-map-ledger.tsv').map((row) => [row.candidate_id, row]));
const shapes = new Map(readTsv('mvdm-first-degree-rebaselined-declaration-shape-ledger.tsv').map((row) => [row.candidate_id, row]));
if (frontier.length !== sourceMap.size || frontier.length !== shapes.size) throw new Error(`current frontier/source-map/declaration cardinality mismatch: ${frontier.length}/${sourceMap.size}/${shapes.size}`);

function outcome(map, shape) {
  if (map.source_map_disposition === 'CRT-source-candidate-present') return ['public-crt-leaf', 'CRT implementation is a public leaf; do not select an OpenNT CRT body.'];
  if (map.source_map_disposition === 'host-boundary-source-candidate-present') return ['private-host-hard-boundary', 'Original source reaches CSR/CSRSS, kernel VDM, Win32k or an equivalent private host shell; retain declaration-only boundary.'];
  if (map.source_map_disposition === 'MVDM-variant-or-parser-gap-candidate-present') return ['mvdm-variant-or-parser-gap', 'The spelling has an original MVDM variant but no source-shape-confirmed binding in the lawful zero closure; preserve as a physical variant issue.'];
  if (map.source_map_disposition === 'non-MVDM-OpenNT-source-candidate-present') return ['original-non-mvdm-binding-required', 'One or more non-MVDM original bodies exist; select only after caller declaration/frontier and linkage proof.'];
  if (shape.declaration_shapes === 'macro-token') return ['macro-or-indirect', 'The physical caller include frontier exposes only a macro form; do not invent a function body.'];
  if (shape.declaration_shapes.includes('function-form-token')) return ['declaration-only-source-unresolved', 'A declaration form is visible, but no approved original function body is indexed; retain an explicit external/assembly/unavailable decision.'];
  return ['source-unavailable-or-declaration-only', 'No approved original function body or caller-frontier declaration token was found; retain explicit source-unavailable, assembly, macro or external-boundary disposition.'];
}

const rows = frontier.map((call) => {
  const map = sourceMap.get(call.candidate_id); const shape = shapes.get(call.candidate_id); const [disposition, rationale] = outcome(map, shape);
  return {
    candidate_id: call.candidate_id,
    caller_symbol: call.caller_symbol,
    caller_source_path: call.caller_source_path,
    caller_source_sha256: call.caller_source_sha256,
    caller_source_line: call.caller_source_line,
    callee_spelling: call.callee_spelling,
    source_map_disposition: map.source_map_disposition,
    global_original_definition_packages: map.global_original_definition_packages,
    declaration_shapes: shape.declaration_shapes,
    declaration_count: shape.declaration_count,
    static_disposition: disposition,
    rationale,
    next_action: disposition === 'original-non-mvdm-binding-required' ? 'perform declaration/frontier/linkage binding; then parse only the selected original first-degree body' : 'do not inspect a callee body in this disposition phase',
  };
});
const columns = ['candidate_id', 'caller_symbol', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'callee_spelling', 'source_map_disposition', 'global_original_definition_packages', 'declaration_shapes', 'declaration_count', 'static_disposition', 'rationale', 'next_action'];
write('mvdm-first-degree-disposition-ledger.tsv', rows, columns);
const counts = new Map(); for (const row of rows) counts.set(row.static_disposition, (counts.get(row.static_disposition) || 0) + 1);
console.log(`current physical outcomes=${rows.length}; ${[...counts].sort(([left], [right]) => left.localeCompare(right)).map(([name, count]) => `${name}=${count}`).join('; ')}`);
