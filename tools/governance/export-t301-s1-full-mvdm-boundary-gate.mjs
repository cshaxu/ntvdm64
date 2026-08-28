import childProcess from 'node:child_process';
import crypto from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const roots = [
  'O:\\repos.external\\OpenNT\\base\\mvdm',
  'O:\\repos.external\\OpenNT-4.5\\nt\\private\\mvdm',
];

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
function normal(value) { return path.resolve(value).replaceAll('\\', '/').toLowerCase(); }
function owningRoot(sourcePath) { return roots.find((root) => normal(sourcePath).startsWith(`${normal(root)}/`)); }
function fileIdentity(row) { return `${row.source_path}\u0000${row.source_sha256}`; }
function sourcePackage(definition) {
  const family = definition.source_path.split('/')[0];
  return definition.source_root.includes('OpenNT-4.5') ? `nt/private/mvdm/${family}` : `base/mvdm/${family}`;
}
function indexOriginalDefinitions() {
  const indexPath = path.join(os.tmpdir(), 'ntvdm64-t301-full-mvdm-definitions.jsonl');
  fs.rmSync(indexPath, { force: true });
  const result = childProcess.spawnSync('ctags', [
    '-R', '--languages=C,C++', '--kinds-C=f', '--kinds-C++=f', '--fields=+nKSt',
    '--output-format=json', '-f', indexPath, ...roots,
  ], { encoding: 'utf8' });
  if (result.status !== 0) throw new Error(result.stderr || 'ctags original-definition index failed');
  const records = fs.readFileSync(indexPath, 'utf8').split(/\r?\n/).filter(Boolean).map((line) => JSON.parse(line));
  fs.rmSync(indexPath, { force: true });
  return records.filter((record) => record._type === 'tag' && record.kind === 'function' && owningRoot(record.path));
}
function linkage(record, cache) {
  const key = `${record.path}\u0000${record.line}`;
  if (cache.has(key)) return cache.get(key);
  const source = fs.readFileSync(record.path, 'utf8');
  const line = source.split(/\r?\n/)[record.line - 1] || '';
  const value = /\bstatic\b/.test(line) ? 'translation-unit-local' : 'externally-linkable';
  cache.set(key, value); return value;
}

const boundary = readTsv('mvdm-first-degree-rebaselined-boundary-ledger.tsv');
const names = new Set(boundary.map((row) => row.callee_spelling));
const callerRoots = new Map();
for (const definition of readTsv('mvdm-zero-degree-call-closure-ledger.tsv')) {
  const key = fileIdentity(definition);
  callerRoots.set(key, [...new Set([...(callerRoots.get(key) || []), definition.source_root])]);
}
const frontiers = new Map(readTsv('mvdm-first-degree-rebaselined-caller-include-frontier-ledger.tsv')
  .map((row) => [`${row.caller_source_path}\u0000${row.caller_source_sha256}`, new Set(row.allowed_package_roots.split(';').filter(Boolean))]));
const digestCache = new Map(); const linkageCache = new Map(); const candidatesByName = new Map();
for (const record of indexOriginalDefinitions()) {
  if (!names.has(record.name)) continue;
  const root = owningRoot(record.path);
  if (!digestCache.has(record.path)) digestCache.set(record.path, crypto.createHash('sha256').update(fs.readFileSync(record.path)).digest('hex'));
  const candidate = {
    symbol: record.name,
    source_root: root,
    source_path: path.relative(root, record.path).replaceAll('\\', '/'),
    source_sha256: digestCache.get(record.path),
    source_line: String(record.line),
    linkage: linkage(record, linkageCache),
  };
  candidatesByName.set(candidate.symbol, [...(candidatesByName.get(candidate.symbol) || []), candidate]);
}

const rows = boundary.map((call) => {
  const rootsForCaller = callerRoots.get(fileIdentity({ source_path: call.caller_source_path, source_sha256: call.caller_source_sha256 })) || [];
  const callerRoot = rootsForCaller.length === 1 ? rootsForCaller[0] : '';
  const allowed = frontiers.get(`${call.caller_source_path}\u0000${call.caller_source_sha256}`) || new Set();
  const rawCandidates = candidatesByName.get(call.callee_spelling) || [];
  const selectable = rawCandidates.filter((candidate) => candidate.source_root === callerRoot
    && candidate.linkage === 'externally-linkable'
    && allowed.has(sourcePackage(candidate)));
  return {
    candidate_id: call.candidate_id,
    caller_symbol: call.caller_symbol,
    caller_source_path: call.caller_source_path,
    caller_source_sha256: call.caller_source_sha256,
    caller_source_line: call.caller_source_line,
    callee_spelling: call.callee_spelling,
    caller_source_root: callerRoot || 'unresolved-caller-root',
    all_original_mvdm_definition_candidate_count: String(rawCandidates.length),
    selectable_original_mvdm_definition_count: String(selectable.length),
    selectable_original_mvdm_definition_identities: selectable.map((candidate) => `${candidate.source_root}|${candidate.source_path}|${candidate.source_sha256}|${candidate.source_line}`).join(';'),
    next_disposition: selectable.length === 1
      ? 'unique original-MVDM candidate passes source-root/linkage/include-package gate; shape review or closure rebase required'
      : selectable.length > 1
        ? 'multiple original-MVDM candidates pass gate; retain variant ambiguity'
        : rawCandidates.length
          ? 'original-MVDM candidate exists but fails gate; retain first-degree record'
          : 'no original-MVDM candidate; continue first-degree source-union classification',
  };
});
const columns = Object.keys(rows[0]);
fs.writeFileSync(path.join(operations, 'mvdm-first-degree-rebaselined-full-mvdm-boundary-gate-ledger.tsv'), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
const one = rows.filter((row) => row.selectable_original_mvdm_definition_count === '1').length;
const many = rows.filter((row) => Number(row.selectable_original_mvdm_definition_count) > 1).length;
console.log(`physical boundary calls=${rows.length}; unique original-MVDM candidates=${one}; ambiguous candidates=${many}; still external=${rows.length - one - many}`);
