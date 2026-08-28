import childProcess from 'node:child_process';
import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const indexDirectory = path.join(repository, 'build', 't301', 'source-index');
const indexPath = path.join(indexDirectory, 'opennt-union-functions.ctags.jsonl');
const reusableIndexPath = path.join(repository, 'artifacts', 'research', 't301-opennt-source-definitions.ctags.jsonl');
const roots = [
  ['OpenNT', 'O:\\repos.external\\OpenNT'],
  ['OpenNT-4.5', 'O:\\repos.external\\OpenNT-4.5'],
].filter(([, root]) => fs.existsSync(root));

function split(line) {
  const fields = []; let field = ''; let quoted = false;
  for (let index = 0; index < line.length; index += 1) { const character = line[index]; if (character === '"') { if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted; } else if (character === '\t' && !quoted) { fields.push(field); field = ''; } else field += character; }
  fields.push(field); return fields;
}
function read(name) { const [header, ...body] = fs.readFileSync(path.join(operations, name), 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean).map(split); return body.map((fields) => Object.fromEntries(header.map((column, index) => [column, fields[index] || '']))); }
function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
function write(name, columns, rows) { fs.writeFileSync(path.join(operations, name), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`); }
function normal(value) { return path.resolve(value).replaceAll('\\', '/').toLowerCase(); }
function rootFor(fileName) { return roots.find(([, root]) => normal(fileName).startsWith(`${normal(root)}/`)); }
function hash(fileName) { return crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex'); }
function packagePath(fileName, root) { return path.relative(root, fileName).replaceAll('\\', '/'); }
function isMvdm(relative) { return /^base\/mvdm\//i.test(relative) || /^nt\/private\/mvdm\//i.test(relative); }

function ensureIndex() {
  // This cache is derived from the approved source union, never a source or
  // runtime input.  Each matched candidate file is re-hashed below, so a stale
  // cache cannot silently establish a definition identity.
  if (fs.existsSync(reusableIndexPath) && fs.statSync(reusableIndexPath).size > 0) return reusableIndexPath;
  if (fs.existsSync(indexPath) && fs.statSync(indexPath).size > 0) return indexPath;
  fs.mkdirSync(indexDirectory, { recursive: true });
  const result = childProcess.spawnSync('ctags.exe', ['-R', '--languages=C,C++', '--kinds-C=f', '--kinds-C++=f', '--fields=+nKSt', '--output-format=json', '-f', indexPath, ...roots.map(([, root]) => root)], { encoding: 'utf8', maxBuffer: 16 * 1024 * 1024 });
  if (result.status !== 0) throw new Error(result.stderr || 'OpenNT union function-index generation failed');
  return indexPath;
}

const calls = read('one-ledger1.tsv').filter((row) => row.zero_binding === 'outside-zero-ledger1');
const selectedIndex = ensureIndex();
const names = new Set(calls.map((row) => row.callee_spelling));
const byName = new Map(); const hashes = new Map();
for (const line of fs.readFileSync(selectedIndex, 'utf8').split(/\r?\n/)) {
  if (!line) continue;
  const tag = JSON.parse(line);
  if (tag._type !== 'tag' || tag.kind !== 'function' || !names.has(tag.name)) continue;
  const root = rootFor(tag.path); if (!root || !fs.existsSync(tag.path)) continue;
  const relative = packagePath(tag.path, root[1]);
  const cacheKey = normal(tag.path); if (!hashes.has(cacheKey)) hashes.set(cacheKey, hash(tag.path));
  const candidate = { definition_source_root: root[1], definition_source_path: relative, definition_source_sha256: hashes.get(cacheKey), definition_source_line: String(tag.line), definition_symbol: tag.name, definition_linkage: /\bstatic\b/.test(fs.readFileSync(tag.path, 'utf8').split(/\r?\n/)[Number(tag.line) - 1] || '') ? 'translation-unit-local' : 'externally-linkable', source_location: isMvdm(relative) ? 'selected-MVDM-or-extracted-SoftPC-source' : 'non-MVDM-OpenNT-source' };
  const identity = `${candidate.definition_source_root}|${candidate.definition_source_path}|${candidate.definition_source_sha256}|${candidate.definition_source_line}|${candidate.definition_symbol}`;
  const known = byName.get(tag.name) || new Map(); known.set(identity, candidate); byName.set(tag.name, known);
}

const resolution = []; const candidates = [];
for (const call of calls) {
  const matches = [...(byName.get(call.callee_spelling) || new Map()).values()].sort((left, right) => `${left.definition_source_root}|${left.definition_source_path}|${left.definition_source_line}`.localeCompare(`${right.definition_source_root}|${right.definition_source_path}|${right.definition_source_line}`));
  const id = `MVDM-HOST-ONE1-RESOLUTION-${String(resolution.length + 1).padStart(6, '0')}`;
  const locations = [...new Set(matches.map((item) => item.source_location))].sort();
  resolution.push({ resolution_id: id, one_ledger1_identity: call.one_ledger1_identity, caller_physical_identity: call.caller_physical_identity, caller_source_path: call.caller_source_path, caller_definition_line: call.caller_definition_line, caller_symbol: call.caller_symbol, call_source_line: call.call_source_line, callee_spelling: call.callee_spelling, original_definition_candidate_count: String(matches.length), candidate_location_classes: locations.join(';'), source_definition_result: matches.length ? 'physical-original-definition-candidate(s)-recorded' : 'no-original-C-or-C++-definition-candidate', next_action: matches.length ? 'declaration/linkage and package-boundary audit; do not inspect candidate body in P18' : 'classify as public-platform, macro/indirect form, assembly/import-only, or genuinely missing interface in the next ABI pass', boundary: 'P18 records only physical original definition candidates; no spelling merge selects an implementation and no candidate body was traversed' });
  for (const match of matches) candidates.push({ resolution_id: id, one_ledger1_identity: call.one_ledger1_identity, callee_spelling: call.callee_spelling, ...match, candidate_identity: `${match.definition_source_root}|${match.definition_source_path}|${match.definition_source_sha256}|${match.definition_source_line}|${match.definition_symbol}`, disposition: 'physical original source definition candidate; linkage/declaration/package selection remains pending' });
}
write('one-ledger1-definition-resolution.tsv', ['resolution_id', 'one_ledger1_identity', 'caller_physical_identity', 'caller_source_path', 'caller_definition_line', 'caller_symbol', 'call_source_line', 'callee_spelling', 'original_definition_candidate_count', 'candidate_location_classes', 'source_definition_result', 'next_action', 'boundary'], resolution);
write('one-ledger1-definition-candidate-ledger.tsv', ['resolution_id', 'one_ledger1_identity', 'callee_spelling', 'candidate_identity', 'definition_source_root', 'definition_source_path', 'definition_source_sha256', 'definition_source_line', 'definition_symbol', 'definition_linkage', 'source_location', 'disposition'], candidates);
const count = new Map(); for (const row of resolution) count.set(row.source_definition_result, (count.get(row.source_definition_result) || 0) + 1);
console.log(`calls=${calls.length}; candidate rows=${candidates.length}; ${[...count].map(([key, value]) => `${key}=${value}`).join('; ')}; index=${selectedIndex}`);
