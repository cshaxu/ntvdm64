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
const controls = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'typedef', 'defined', '__asm', 'asm', 'catch', '__try', '__except', 'except', '__finally', 'finally', 'try']);

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
function key(item) { return `${item.source_root}|${item.source_path}|${item.source_sha256}|${item.source_line}`; }
function packageRoot(item) {
  const family = item.source_path.split('/')[0];
  return item.source_root.includes('OpenNT-4.5') ? `nt/private/mvdm/${family}` : `base/mvdm/${family}`;
}
function mask(input) {
  const chars = [...input]; let state = 'code'; let escaped = false;
  for (let index = 0; index < chars.length; index += 1) {
    const character = chars[index]; const next = chars[index + 1] || '';
    if (state === 'code') {
      if (character === '/' && next === '/') { chars[index] = chars[index + 1] = ' '; index += 1; state = 'line'; }
      else if (character === '/' && next === '*') { chars[index] = chars[index + 1] = ' '; index += 1; state = 'block'; }
      else if (character === '"') { chars[index] = ' '; state = 'string'; escaped = false; }
      else if (character === "'") { chars[index] = ' '; state = 'char'; escaped = false; }
    } else if (state === 'line') { if (character === '\n') state = 'code'; else if (character !== '\r') chars[index] = ' '; }
    else if (state === 'block') { if (character === '*' && next === '/') { chars[index] = chars[index + 1] = ' '; index += 1; state = 'code'; } else if (character !== '\r' && character !== '\n') chars[index] = ' '; }
    else if (character === '\n') state = 'code';
    else { if (character !== '\r') chars[index] = ' '; if (escaped) escaped = false; else if (character === '\\') escaped = true; else if ((state === 'string' && character === '"') || (state === 'char' && character === "'")) state = 'code'; }
  }
  return chars.join('');
}
function paired(text, offset, open, close) { let depth = 0; for (let index = offset; index < text.length; index += 1) { if (text[index] === open) depth += 1; else if (text[index] === close && --depth === 0) return index; } return -1; }
function sourceFile(item) { return path.join(item.source_root, ...item.source_path.split('/')); }
function indexDefinitions() {
  const indexPath = path.join(os.tmpdir(), 'ntvdm64-t301-rebase-definitions.jsonl'); fs.rmSync(indexPath, { force: true });
  const result = childProcess.spawnSync('ctags', ['-R', '--languages=C,C++', '--kinds-C=f', '--kinds-C++=f', '--fields=+nKSt', '--output-format=json', '-f', indexPath, ...roots], { encoding: 'utf8' });
  if (result.status !== 0) throw new Error(result.stderr || 'ctags original-definition index failed');
  const records = fs.readFileSync(indexPath, 'utf8').split(/\r?\n/).filter(Boolean).map((line) => JSON.parse(line)); fs.rmSync(indexPath, { force: true });
  const digest = new Map(); const linkage = new Map();
  return records.filter((record) => record._type === 'tag' && record.kind === 'function' && owningRoot(record.path)).map((record) => {
    const root = owningRoot(record.path); if (!digest.has(record.path)) digest.set(record.path, crypto.createHash('sha256').update(fs.readFileSync(record.path)).digest('hex'));
    const line = fs.readFileSync(record.path, 'utf8').split(/\r?\n/)[record.line - 1] || '';
    const result = { symbol: record.name, source_root: root, source_path: path.relative(root, record.path).replaceAll('\\', '/'), source_sha256: digest.get(record.path), source_line: String(record.line), linkage: /\bstatic\b/.test(line) ? 'translation-unit-local' : 'externally-linkable' };
    linkage.set(key(result), result); return result;
  });
}
function bodyOf(definition) {
  const raw = fs.readFileSync(sourceFile(definition), 'utf8'); const parsed = mask(raw); const lines = raw.split(/\r?\n/);
  const newlineWidth = raw.includes('\r\n') ? 2 : 1;
  const start = lines.slice(0, Number(definition.source_line) - 1).reduce((total, line) => total + line.length + newlineWidth, 0);
  const atMarkedLine = parsed.indexOf(definition.symbol, start);
  const prior = parsed.lastIndexOf(definition.symbol, start);
  const symbolOffset = atMarkedLine >= start && atMarkedLine < start + 8192 ? atMarkedLine : prior;
  const open = parsed.indexOf('(', symbolOffset + definition.symbol.length); const close = paired(parsed, open, '(', ')'); const bodyOpen = parsed.indexOf('{', close); const bodyClose = paired(parsed, bodyOpen, '{', '}');
  if (symbolOffset < Math.max(0, start - 2048) || close < 0 || bodyOpen < 0 || bodyClose < 0) {
    return { unexpanded_reason: 'conditional-or-unbalanced original source body requires build-variant parsing' };
  }
  const ownMacros = new Set([...raw.matchAll(/^\s*#\s*define\s+([A-Za-z_]\w*)\b/gm)].map((match) => match[1]));
  return { code: parsed.slice(bodyOpen + 1, bodyClose), ownMacros, body_offset: bodyOpen + 1, source: raw };
}

const all = indexDefinitions(); const byName = new Map(); const byKey = new Map();
for (const definition of all) { byName.set(definition.symbol, [...(byName.get(definition.symbol) || []), definition]); byKey.set(key(definition), definition); }
const old = new Set(readTsv('mvdm-zero-degree-call-closure-ledger.tsv').map(key));
const seedRows = readTsv('mvdm-first-degree-rebaselined-full-mvdm-signature-gate-ledger.tsv').filter((row) => row.next_disposition.startsWith('source call'));
const seeds = [...new Set(seedRows.map((row) => row.selected_definition_identity))].map((identity) => {
  const [source_root, source_path, source_sha256, source_line] = identity.split('|'); return byKey.get(key({ source_root, source_path, source_sha256, source_line }));
}).filter(Boolean);
const reached = new Map(); const queue = [...seeds]; const edges = []; const unexpanded = [];
while (queue.length) {
  const caller = queue.shift(); const callerKey = key(caller); if (reached.has(callerKey) || old.has(callerKey)) continue; reached.set(callerKey, caller);
  const extracted = bodyOf(caller);
  if (extracted.unexpanded_reason) { unexpanded.push({ ...caller, reason: extracted.unexpanded_reason }); continue; }
  const { code, ownMacros, body_offset, source } = extracted;
  for (const match of code.matchAll(/\b([A-Za-z_]\w*)\s*\(/g)) {
    const callee = match[1]; if (controls.has(callee.toLowerCase()) || ownMacros.has(callee) || /^IFN\d+$/i.test(callee) || /^[A-Z][A-Z0-9_]*$/.test(callee)) continue;
    const candidates = (byName.get(callee) || []).filter((candidate) => candidate.source_root === caller.source_root && (candidate.linkage === 'externally-linkable' || candidate.source_path === caller.source_path));
    const internal = candidates.length === 1 ? candidates[0] : null;
    const callLine = String(source.slice(0, body_offset + match.index).split('\n').length);
    edges.push({ caller_identity: callerKey, caller_symbol: caller.symbol, caller_source_root: caller.source_root, caller_source_path: caller.source_path, caller_source_sha256: caller.source_sha256, caller_source_line: caller.source_line, call_source_line: callLine, callee_spelling: callee, internal_candidate_identity: internal ? key(internal) : '', disposition: internal ? 'original-MVDM direct internal candidate; requires next shape/frontier gate if newly reached' : 'direct exit from newly discovered original-MVDM body; classify in T301 first-degree audit' });
    if (internal && !old.has(key(internal)) && !reached.has(key(internal))) queue.push(internal);
  }
}
const definitionRows = [...reached.values()].map((definition) => ({ ...definition, source: seeds.some((seed) => key(seed) === key(definition)) ? 'P10 seed body' : 'directly reached during P12 expansion' }));
const definitionColumns = ['symbol', 'source_root', 'source_path', 'source_sha256', 'source_line', 'linkage', 'source'];
const edgeColumns = ['caller_identity', 'caller_symbol', 'caller_source_root', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'call_source_line', 'callee_spelling', 'internal_candidate_identity', 'disposition'];
const unexpandedColumns = ['symbol', 'source_root', 'source_path', 'source_sha256', 'source_line', 'linkage', 'reason'];
fs.writeFileSync(path.join(operations, 'mvdm-zero-degree-rebase-expansion-definition-ledger.tsv'), `${definitionColumns.join('\t')}\n${definitionRows.map((row) => definitionColumns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
fs.writeFileSync(path.join(operations, 'mvdm-zero-degree-rebase-expansion-edge-ledger.tsv'), `${edgeColumns.join('\t')}\n${edges.map((row) => edgeColumns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
fs.writeFileSync(path.join(operations, 'mvdm-zero-degree-rebase-expansion-unparsed-body-ledger.tsv'), `${unexpandedColumns.join('\t')}\n${unexpanded.map((row) => unexpandedColumns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
console.log(`P10 seed bodies=${seeds.length}; additional reached original-MVDM bodies=${definitionRows.length - seeds.length}; direct edges=${edges.length}; unexpanded conditional bodies=${unexpanded.length}; direct exits requiring T301 classification=${edges.filter((edge) => !edge.internal_candidate_identity).length}`);
