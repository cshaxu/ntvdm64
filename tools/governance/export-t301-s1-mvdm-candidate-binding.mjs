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
function sourcePackage(definition) {
  const suffix = definition.source_path.split('/')[0];
  return definition.source_root.includes('OpenNT-4.5') ? `nt/private/mvdm/${suffix}` : `base/mvdm/${suffix}`;
}
function fileIdentity(row) { return `${row.source_path}\u0000${row.source_sha256}`; }

const boundaryById = new Map(readTsv('mvdm-first-degree-rebaselined-boundary-ledger.tsv').map((row) => [row.candidate_id, row]));
const crosswalk = readTsv('mvdm-first-degree-rebaselined-definition-candidate-crosswalk-ledger.tsv');
const definitions = readTsv('mvdm-first-degree-rebaselined-mvdm-definition-form-candidate-ledger.tsv');
const definitionsByName = new Map();
for (const definition of definitions) definitionsByName.set(definition.symbol, [...(definitionsByName.get(definition.symbol) || []), definition]);
const callerRoots = new Map();
for (const definition of readTsv('mvdm-zero-degree-call-closure-ledger.tsv')) {
  const key = fileIdentity(definition);
  callerRoots.set(key, [...new Set([...(callerRoots.get(key) || []), definition.source_root])]);
}
const frontiers = new Map(readTsv('mvdm-first-degree-rebaselined-caller-include-frontier-ledger.tsv')
  .map((row) => [`${row.caller_source_path}\u0000${row.caller_source_sha256}`, new Set(row.allowed_package_roots.split(';').filter(Boolean))]));

const rows = crosswalk.map((entry) => {
  const call = boundaryById.get(entry.candidate_id);
  if (!call) throw new Error(`missing physical call ${entry.candidate_id}`);
  const rootCandidates = callerRoots.get(fileIdentity({
    source_path: call.caller_source_path,
    source_sha256: call.caller_source_sha256,
  })) || [];
  const callerRoot = rootCandidates.length === 1 ? rootCandidates[0] : '';
  const allowed = frontiers.get(`${call.caller_source_path}\u0000${call.caller_source_sha256}`) || new Set();
  const allCandidates = definitionsByName.get(call.callee_spelling) || [];
  const rootMatching = allCandidates.filter((definition) => definition.source_root === callerRoot);
  const linkageMatching = rootMatching.filter((definition) => definition.linkage === 'externally-linkable');
  const selectable = linkageMatching.filter((definition) => allowed.has(sourcePackage(definition)));
  const disposition = selectable.length === 1
    ? 'one original-MVDM body meets root/linkage/package gate; signature and conditional-form proof remains'
    : selectable.length > 1
      ? 'multiple original-MVDM bodies meet root/linkage/package gate; preserve variant ambiguity'
      : entry.original_mvdm_definition_candidate_count !== '0'
        ? 'original-MVDM body exists but fails root/linkage/package gate; preserve first-degree record'
        : 'no original-MVDM body candidate; continue approved-source-union resolution';
  return {
    candidate_id: entry.candidate_id,
    caller_symbol: call.caller_symbol,
    caller_source_path: call.caller_source_path,
    caller_source_sha256: call.caller_source_sha256,
    caller_source_line: call.caller_source_line,
    caller_source_root: callerRoot || 'unresolved-caller-root',
    callee_spelling: call.callee_spelling,
    original_mvdm_definition_candidate_count: String(allCandidates.length),
    root_matching_candidate_count: String(rootMatching.length),
    linkage_matching_candidate_count: String(linkageMatching.length),
    allowed_package_roots: [...allowed].sort().join(';'),
    selectable_definition_count: String(selectable.length),
    selectable_definition_identities: selectable.map((definition) => `${definition.source_root}|${definition.source_path}|${definition.source_sha256}|${definition.source_line}`).join(';'),
    next_disposition: disposition,
  };
});
const columns = Object.keys(rows[0]);
fs.writeFileSync(path.join(operations, 'mvdm-first-degree-rebaselined-mvdm-binding-gate-ledger.tsv'), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
const one = rows.filter((row) => row.selectable_definition_count === '1').length;
const many = rows.filter((row) => Number(row.selectable_definition_count) > 1).length;
console.log(`physical calls=${rows.length}; one selectable original-MVDM body=${one}; multiple selectable bodies=${many}; no selectable body=${rows.length - one - many}`);
