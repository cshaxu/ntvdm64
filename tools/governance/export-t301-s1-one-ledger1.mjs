import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

// Produce the *new* one-degree frontier directly from the physical identities
// retained by zero-ledger1.  Older mvdm-first-degree-* material is historical
// evidence and is deliberately neither read nor changed here.
const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const controls = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'typedef', 'defined', '__asm', 'asm', 'catch', '__try', '__except', 'except', '__finally', 'finally', 'try']);

function split(line) {
  const values = []; let value = ''; let quoted = false;
  for (let index = 0; index < line.length; index += 1) {
    const character = line[index];
    if (character === '"') { if (quoted && line[index + 1] === '"') { value += '"'; index += 1; } else quoted = !quoted; }
    else if (character === '\t' && !quoted) { values.push(value); value = ''; } else value += character;
  }
  values.push(value); return values;
}
function read(name) {
  const [header, ...body] = fs.readFileSync(path.join(operations, name), 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean).map(split);
  return body.map((fields) => Object.fromEntries(header.map((column, index) => [column, fields[index] || ''])));
}
function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
function write(name, columns, rows) {
  fs.writeFileSync(path.join(operations, name), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
}
function hash(fileName) { return crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex'); }
function sourceFile(row) { return path.join(row.source_root, ...row.source_path.split('/')); }
function identity(row) { return `${row.source_root}|${row.source_path}|${row.source_sha256}|${row.source_line}|${row.symbol}`; }
function sourceIdentity(row) { return `${row.source_root}|${row.source_path}|${row.source_sha256}`; }

function mask(input) {
  const characters = [...input]; let state = 'code'; let escaped = false;
  for (let index = 0; index < characters.length; index += 1) {
    const character = characters[index]; const next = characters[index + 1] || '';
    if (state === 'code') {
      if (character === '/' && next === '/') { characters[index] = characters[index + 1] = ' '; index += 1; state = 'line'; }
      else if (character === '/' && next === '*') { characters[index] = characters[index + 1] = ' '; index += 1; state = 'block'; }
      else if (character === '"') { characters[index] = ' '; escaped = false; state = 'string'; }
      else if (character === "'") { characters[index] = ' '; escaped = false; state = 'character'; }
    } else if (state === 'line') { if (character === '\n') state = 'code'; else if (character !== '\r') characters[index] = ' '; }
    else if (state === 'block') { if (character === '*' && next === '/') { characters[index] = characters[index + 1] = ' '; index += 1; state = 'code'; } else if (character !== '\r' && character !== '\n') characters[index] = ' '; }
    else if (character === '\n') state = 'code';
    else { if (character !== '\r') characters[index] = ' '; if (escaped) escaped = false; else if (character === '\\') escaped = true; else if ((state === 'string' && character === '"') || (state === 'character' && character === "'")) state = 'code'; }
  }
  const lines = characters.join('').split(/(?<=\n)/); let continuation = false;
  return lines.map((line) => { const directive = continuation || /^\s*#/.test(line); continuation = directive && /\\\s*(?:\r?\n)?$/.test(line); return directive ? line.replace(/[^\r\n]/g, ' ') : line; }).join('');
}
function paired(text, offset, open, close) { let depth = 0; for (let index = offset; index < text.length; index += 1) { if (text[index] === open) depth += 1; else if (text[index] === close && --depth === 0) return index; } return -1; }
function lineOffset(text, line) { let offset = 0; for (let current = 1; current < line && offset < text.length; current += 1) { const next = text.indexOf('\n', offset); offset = next < 0 ? text.length : next + 1; } return offset; }
function lineAt(text, offset) { return text.slice(0, offset).split('\n').length; }
function bodyFor(row, raw, parsed) {
  const start = lineOffset(parsed, Number(row.source_line));
  const before = parsed.lastIndexOf(row.symbol, start + 1024);
  const after = parsed.indexOf(row.symbol, start);
  const symbolOffset = before >= Math.max(0, start - 4096) ? before : after;
  if (symbolOffset < 0 || symbolOffset > start + 8192) return null;
  const open = parsed.indexOf('(', symbolOffset + row.symbol.length);
  const close = paired(parsed, open, '(', ')');
  const bodyOpen = close < 0 ? -1 : parsed.indexOf('{', close);
  const bodyClose = bodyOpen < 0 ? -1 : paired(parsed, bodyOpen, '{', '}');
  if (bodyClose < 0) return null;
  return { code: parsed.slice(bodyOpen + 1, bodyClose), offset: bodyOpen + 1 };
}

const zero = read('zero-ledger1.tsv');
const bySourceAndSymbol = new Map(); const bySymbol = new Map();
for (const row of zero) {
  const local = `${sourceIdentity(row)}|${row.symbol}`;
  bySourceAndSymbol.set(local, [...(bySourceAndSymbol.get(local) || []), row]);
  bySymbol.set(row.symbol, [...(bySymbol.get(row.symbol) || []), row]);
}
function internalBinding(caller, symbol) {
  const local = bySourceAndSymbol.get(`${sourceIdentity(caller)}|${symbol}`) || [];
  if (local.length === 1) return ['zero-ledger1-same-translation-unit', local];
  const global = (bySymbol.get(symbol) || []).filter((candidate) => candidate.linkage !== 'file-static');
  if (global.length === 1) return ['zero-ledger1-unique-nonstatic', global];
  if (local.length || global.length) return ['zero-ledger1-binding-ambiguous', [...local, ...global]];
  return ['outside-zero-ledger1', []];
}

const rows = []; const unparsed = [];
for (const caller of zero) {
  const fileName = sourceFile(caller);
  if (!fs.existsSync(fileName)) throw new Error(`Missing original zero-ledger1 source: ${fileName}`);
  if (hash(fileName) !== caller.source_sha256) throw new Error(`Source identity changed: ${fileName}`);
  const raw = fs.readFileSync(fileName, 'utf8'); const parsed = mask(raw); const body = bodyFor(caller, raw, parsed);
  if (!body) { unparsed.push({ ...caller, reason: 'definition-body-not-parsed-from-recorded-physical-identity' }); continue; }
  const ownMacros = new Set([...raw.matchAll(/^\s*#\s*define\s+([A-Za-z_]\w*)\b/gm)].map((match) => match[1]));
  for (const match of body.code.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) {
    const callee = match[1];
    // Public C APIs may use capitals, but all-capital spellings are the
    // historical macro convention.  Keep mixed-case names for the later
    // declaration audit; do not manufacture macro-only entries as functions.
    if (controls.has(callee.toLowerCase()) || ownMacros.has(callee) || /^[A-Z][A-Z0-9_]*$/.test(callee) || /^IFN\d+$/i.test(callee)) continue;
    const [binding, candidates] = internalBinding(caller, callee);
    if (binding === 'zero-ledger1-same-translation-unit' || binding === 'zero-ledger1-unique-nonstatic') continue;
    const line = lineAt(parsed, body.offset + match.index);
    rows.push({
      one_ledger1_identity: `MVDM-HOST-ONE1-${String(rows.length + 1).padStart(6, '0')}`,
      caller_zero_identity: caller.ledger0_identity,
      caller_physical_identity: identity(caller),
      caller_source_root: caller.source_root,
      caller_source_path: caller.source_path,
      caller_source_sha256: caller.source_sha256,
      caller_definition_line: caller.source_line,
      caller_symbol: caller.symbol,
      caller_linkage: caller.linkage,
      call_source_line: String(line),
      callee_spelling: callee,
      call_form: 'direct-named-call',
      zero_binding: binding,
      zero_definition_candidates: candidates.map(identity).join(';'),
      next_action: binding === 'outside-zero-ledger1' ? 'one-degree source/disposition audit; do not inspect callee body in this export' : 'resolve physical zero-ledger1 binding before treating as a first-degree external function',
      boundary: 'derived only from the caller body identified by zero-ledger1; no symbol-only merging, provider selection, second-degree traversal, or historical one-ledger input',
    });
  }
}
const columns = ['one_ledger1_identity', 'caller_zero_identity', 'caller_physical_identity', 'caller_source_root', 'caller_source_path', 'caller_source_sha256', 'caller_definition_line', 'caller_symbol', 'caller_linkage', 'call_source_line', 'callee_spelling', 'call_form', 'zero_binding', 'zero_definition_candidates', 'next_action', 'boundary'];
write('one-ledger1.tsv', columns, rows);
write('one-ledger1-unparsed-zero-body-ledger.tsv', ['ledger0_identity', 'source_root', 'source_path', 'source_sha256', 'source_line', 'symbol', 'reason'], unparsed);
const counts = new Map(); for (const row of rows) counts.set(row.zero_binding, (counts.get(row.zero_binding) || 0) + 1);
console.log(`zero-ledger1 callers=${zero.length}; one-ledger1 direct rows=${rows.length}; unparsed zero callers=${unparsed.length}; ${[...counts].map(([key, value]) => `${key}=${value}`).join('; ')}`);
