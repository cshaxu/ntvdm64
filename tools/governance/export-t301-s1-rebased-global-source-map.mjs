import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const indexPath = path.join(repository, 'artifacts', 'research', 't301-opennt-source-definitions.ctags.jsonl');

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
function sourcePackage(sourcePath) {
  const normalized = sourcePath.replaceAll('\\', '/');
  const marker = normalized.includes('/OpenNT-4.5/') ? '/OpenNT-4.5/' : '/OpenNT/';
  const suffix = normalized.slice(normalized.indexOf(marker) + marker.length);
  return suffix.split('/').slice(0, 3).join('/');
}
function classifyPackage(packages) {
  if (!packages.length) return 'no-OpenNT-source-definition-candidate';
  if (packages.some((item) => /(?:^|\/)crts(?:\/|$)/i.test(item))) return 'CRT-source-candidate-present';
  if (packages.some((item) => /(?:^|\/)(base\/win32\/client|base\/subsys\/csr|ntos\/vdm)(?:\/|$)/i.test(item))) return 'host-boundary-source-candidate-present';
  if (packages.some((item) => /(?:^|\/)(base\/mvdm|nt\/private\/mvdm)(?:\/|$)/i.test(item))) return 'MVDM-variant-or-parser-gap-candidate-present';
  return 'non-MVDM-OpenNT-source-candidate-present';
}

if (!fs.existsSync(indexPath)) throw new Error(`missing reusable source index ${indexPath}`);
const frontier = readTsv('mvdm-first-degree-rebased-boundary-ledger.tsv');
const names = new Set(frontier.map((row) => row.callee_spelling));
const byName = new Map();
for (const line of fs.readFileSync(indexPath, 'utf8').split(/\r?\n/)) {
  if (!line) continue; const record = JSON.parse(line);
  if (record._type !== 'tag' || record.kind !== 'function' || !names.has(record.name)) continue;
  byName.set(record.name, [...(byName.get(record.name) || []), record]);
}
const rows = frontier.map((call) => {
  const candidates = byName.get(call.callee_spelling) || [];
  const packages = [...new Set(candidates.map((candidate) => sourcePackage(candidate.path)))].sort();
  return {
    candidate_id: call.candidate_id,
    caller_source_path: call.caller_source_path,
    caller_source_sha256: call.caller_source_sha256,
    caller_source_line: call.caller_source_line,
    callee_spelling: call.callee_spelling,
    global_original_definition_candidate_count: String(candidates.length),
    global_original_definition_packages: packages.join(';'),
    source_map_disposition: classifyPackage(packages),
  };
});
const columns = Object.keys(rows[0]);
fs.writeFileSync(path.join(operations, 'mvdm-first-degree-rebased-global-source-map-ledger.tsv'), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
const counts = new Map(); for (const row of rows) counts.set(row.source_map_disposition, (counts.get(row.source_map_disposition) || 0) + 1);
console.log([...counts].sort(([left], [right]) => left.localeCompare(right)).map(([name, count]) => `${name}=${count}`).join('; '));
