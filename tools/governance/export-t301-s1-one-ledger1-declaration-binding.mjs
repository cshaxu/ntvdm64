import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const roots = [
  ['OpenNT', 'O:\\repos.external\\OpenNT'],
  ['OpenNT-4.5', 'O:\\repos.external\\OpenNT-4.5'],
].filter(([, root]) => fs.existsSync(root)).sort((left, right) => right[1].length - left[1].length);

function split(line) { const fields = []; let field = ''; let quoted = false; for (let index = 0; index < line.length; index += 1) { const character = line[index]; if (character === '"') { if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted; } else if (character === '\t' && !quoted) { fields.push(field); field = ''; } else field += character; } fields.push(field); return fields; }
function read(name) { const [header, ...body] = fs.readFileSync(path.join(operations, name), 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean).map(split); return body.map((fields) => Object.fromEntries(header.map((column, index) => [column, fields[index] || '']))); }
function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
function write(name, columns, rows) { fs.writeFileSync(path.join(operations, name), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`); }
const hashCache = new Map(); function hash(fileName) { if (!hashCache.has(fileName)) hashCache.set(fileName, crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex')); return hashCache.get(fileName); }
function rootFor(fileName) { return roots.find(([, root]) => path.resolve(fileName).toLowerCase().startsWith(path.resolve(root).toLowerCase())); }
function headerIdentity(header) { return `${header.edition}|${header.relative}|${hash(header.fileName)}`; }
function sourceFile(call) { return path.join(call.caller_source_root, ...call.caller_source_path.split('/')); }

const headers = [];
for (const [edition, root] of roots) {
  const walk = (directory) => { for (const entry of fs.readdirSync(directory, { withFileTypes: true })) { const fileName = path.join(directory, entry.name); if (entry.isDirectory()) walk(fileName); else if (/\.(h|inc)$/i.test(entry.name)) headers.push({ edition, root, fileName, relative: path.relative(root, fileName).replaceAll('\\', '/') }); } };
  walk(root);
}
const headersByTail = new Map();
for (const header of headers) for (const key of [header.relative.toLowerCase(), path.posix.basename(header.relative).toLowerCase()]) headersByTail.set(key, [...(headersByTail.get(key) || []), header]);
function uniqueByBytes(items) { const groups = new Map(); for (const item of items) { const digest = hash(item.fileName); groups.set(digest, [...(groups.get(digest) || []), item]); } return groups.size === 1 ? [...groups.values()][0] : []; }
const includeCache = new Map();
function resolveInclude(caller, spelling) {
  const key = `${caller}\u0000${spelling}`; if (includeCache.has(key)) return includeCache.get(key);
  const local = path.resolve(path.dirname(caller), spelling);
  if (fs.existsSync(local)) { const root = rootFor(local); if (root) { const result = [{ edition: root[0], root: root[1], fileName: local, relative: path.relative(root[1], local).replaceAll('\\', '/') }]; includeCache.set(key, result); return result; } }
  const normalized = spelling.replaceAll('\\', '/').toLowerCase();
  const exact = uniqueByBytes(headersByTail.get(normalized) || []);
  const result = exact.length ? exact : uniqueByBytes(headersByTail.get(path.posix.basename(normalized)) || []);
  includeCache.set(key, result); return result;
}
const directCache = new Map();
function directIncludes(fileName) { if (directCache.has(fileName)) return directCache.get(fileName); const spellings = [...fs.readFileSync(fileName, 'utf8').matchAll(/^\s*#\s*include\s*["<]([^">]+)[">]/gm)].map((match) => match[1]); directCache.set(fileName, spellings); return spellings; }
function includeClosure(fileName) {
  const found = new Map(); const queue = [];
  for (const spelling of directIncludes(fileName)) for (const header of resolveInclude(fileName, spelling)) { const id = headerIdentity(header); if (!found.has(id)) { found.set(id, header); queue.push(header); } }
  while (queue.length) { const header = queue.shift(); for (const spelling of directIncludes(header.fileName)) for (const nested of resolveInclude(header.fileName, spelling)) { const id = headerIdentity(nested); if (!found.has(id)) { found.set(id, nested); queue.push(nested); } } }
  return [...found.values()];
}

const oneById = new Map(read('one-ledger1.tsv').map((row) => [row.one_ledger1_identity, row]));
const calls = read('one-ledger1-definition-resolution.tsv').filter((row) => row.source_definition_result === 'physical-original-definition-candidate(s)-recorded').map((row) => {
  const original = oneById.get(row.one_ledger1_identity); if (!original) throw new Error(`P18 resolution has no one-ledger1 caller: ${row.one_ledger1_identity}`);
  return { ...row, caller_source_root: original.caller_source_root, caller_source_sha256: original.caller_source_sha256 };
});
const candidates = read('one-ledger1-definition-candidate-ledger.tsv');
const candidateByResolution = new Map(); for (const candidate of candidates) candidateByResolution.set(candidate.resolution_id, [...(candidateByResolution.get(candidate.resolution_id) || []), candidate]);
const callerMap = new Map();
for (const call of calls) { const key = call.caller_physical_identity; if (callerMap.has(key)) continue; const source = sourceFile(call); if (!fs.existsSync(source)) throw new Error(`missing original caller source ${source}`); const closure = includeClosure(source); callerMap.set(key, { call, source, direct: directIncludes(source), closure }); }
const tokens = new Map();
for (const info of callerMap.values()) for (const header of info.closure) {
  const id = headerIdentity(header); if (tokens.has(id)) continue;
  const raw = fs.readFileSync(header.fileName, 'utf8'); const matches = new Map();
  for (const match of raw.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) { const symbol = match[1]; const line = raw.slice(0, match.index).split('\n').length; matches.set(symbol, [...(matches.get(symbol) || []), line]); }
  tokens.set(id, matches);
}
const callerRows = []; const bindingRows = [];
for (const [identity, info] of callerMap) callerRows.push({ caller_include_frontier_id: `MVDM-HOST-ONE1-CALLER-${String(callerRows.length + 1).padStart(6, '0')}`, caller_physical_identity: identity, caller_source_root: info.call.caller_source_root, caller_source_path: info.call.caller_source_path, caller_source_sha256: info.call.caller_source_sha256, direct_include_spellings: info.direct.join(';'), original_header_identities: info.closure.map(headerIdentity).join(';'), frontier_note: info.closure.length ? 'recursive original include closure; declaration tokens are evidence only' : 'no unique original include identity resolved' });
const frontierByCaller = new Map(callerRows.map((row) => [row.caller_physical_identity, row]));
for (const call of calls) {
  const info = callerMap.get(call.caller_physical_identity); const headersForSymbol = [];
  for (const header of info.closure) for (const line of tokens.get(headerIdentity(header)).get(call.callee_spelling) || []) headersForSymbol.push(`${headerIdentity(header)}@${line}`);
  const all = candidateByResolution.get(call.resolution_id) || [];
  for (const candidate of all) {
    const sameTuStatic = candidate.definition_linkage === 'translation-unit-local' && candidate.definition_source_path === call.caller_source_path;
    const disposition = candidate.definition_linkage === 'translation-unit-local' && !sameTuStatic
      ? 'static-candidate-not-bindable-across-translation-unit'
      : headersForSymbol.length === 0
        ? 'original-definition-candidate-without-reachable-declaration-token'
        : all.length === 1
          ? 'unique-source-candidate-with-reachable-declaration-token'
          : 'multiple-source-candidates-with-reachable-declaration-token';
    bindingRows.push({ binding_id: `MVDM-HOST-ONE1-BINDING-${String(bindingRows.length + 1).padStart(6, '0')}`, resolution_id: call.resolution_id, one_ledger1_identity: call.one_ledger1_identity, caller_include_frontier_id: frontierByCaller.get(call.caller_physical_identity).caller_include_frontier_id, callee_spelling: call.callee_spelling, candidate_identity: candidate.candidate_identity, definition_source_root: candidate.definition_source_root, definition_source_path: candidate.definition_source_path, definition_source_sha256: candidate.definition_source_sha256, definition_source_line: candidate.definition_source_line, definition_linkage: candidate.definition_linkage, reachable_declaration_tokens: headersForSymbol.join(';'), binding_disposition: disposition, next_action: disposition.startsWith('unique-') ? 'P20 may inspect only the selected candidate body after a physical signature/condition gate' : 'retain each candidate; declaration/signature/condition analysis must resolve or explicitly defer before body expansion', boundary: 'P19 reads caller include/declaration tokens and candidate declaration-line linkage only; it does not inspect a candidate body or select a runtime provider' });
  }
}
write('one-ledger1-caller-include-frontier.tsv', ['caller_include_frontier_id', 'caller_physical_identity', 'caller_source_root', 'caller_source_path', 'caller_source_sha256', 'direct_include_spellings', 'original_header_identities', 'frontier_note'], callerRows);
write('one-ledger1-declaration-binding-ledger.tsv', ['binding_id', 'resolution_id', 'one_ledger1_identity', 'caller_include_frontier_id', 'callee_spelling', 'candidate_identity', 'definition_source_root', 'definition_source_path', 'definition_source_sha256', 'definition_source_line', 'definition_linkage', 'reachable_declaration_tokens', 'binding_disposition', 'next_action', 'boundary'], bindingRows);
const count = new Map(); for (const row of bindingRows) count.set(row.binding_disposition, (count.get(row.binding_disposition) || 0) + 1);
console.log(`caller physical identities=${callerRows.length}; candidate bindings=${bindingRows.length}; ${[...count].map(([key, value]) => `${key}=${value}`).join('; ')}`);
