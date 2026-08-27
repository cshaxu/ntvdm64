import crypto from 'node:crypto';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

const root = process.argv[2] || process.cwd();
const ops = path.join(root, 'docs', 'etc', 'operations');
const sourceRoots = ['O:\\repos.external\\OpenNT', 'O:\\repos.external\\OpenNT-4.5'].filter(fs.existsSync).sort((left, right) => right.length - left.length);
const keywords = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'typedef', 'defined', '__asm', 'asm', 'catch', '__try']);

function parseTsv(fileName) {
  const lines = fs.readFileSync(fileName, 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean);
  const split = (line) => { const out = []; let field = ''; let quoted = false; for (let i = 0; i < line.length; i += 1) { const ch = line[i]; if (ch === '"') { if (quoted && line[i + 1] === '"') { field += '"'; i += 1; } else quoted = !quoted; } else if (ch === '\t' && !quoted) { out.push(field); field = ''; } else field += ch; } out.push(field); return out; };
  const [header, ...rows] = lines.map(split);
  return rows.map((row) => Object.fromEntries(header.map((key, index) => [key, row[index] || ''])));
}
function writeTsv(fileName, rows, columns) {
  const quote = (value) => `"${String(value || '').replaceAll('"', '""')}"`;
  fs.writeFileSync(path.join(ops, fileName), [columns.join('\t'), ...rows.map((row) => columns.map((column) => quote(row[column])).join('\t'))].join('\r\n') + '\r\n', 'utf8');
}
function sha256(fileName) { return crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex'); }
function maskC(input) {
  const chars = [...input]; let state = 'code'; let escaped = false;
  for (let i = 0; i < chars.length; i += 1) { const ch = chars[i]; const next = chars[i + 1] || ''; if (state === 'code') { if (ch === '/' && next === '/') { chars[i] = chars[i + 1] = ' '; i += 1; state = 'line'; } else if (ch === '/' && next === '*') { chars[i] = chars[i + 1] = ' '; i += 1; state = 'block'; } else if (ch === '"') { chars[i] = ' '; state = 'string'; escaped = false; } else if (ch === "'") { chars[i] = ' '; state = 'char'; escaped = false; } } else if (state === 'line') { if (ch === '\n') state = 'code'; else if (ch !== '\r') chars[i] = ' '; } else if (state === 'block') { if (ch === '*' && next === '/') { chars[i] = chars[i + 1] = ' '; i += 1; state = 'code'; } else if (ch !== '\r' && ch !== '\n') chars[i] = ' '; } else { if (ch === '\n') state = 'code'; else { if (ch !== '\r') chars[i] = ' '; if (escaped) escaped = false; else if (ch === '\\') escaped = true; else if ((state === 'string' && ch === '"') || (state === 'char' && ch === "'")) state = 'code'; } } }
  return chars.join('').split(/(?<=\n)/).map((line) => /^\s*#/.test(line) ? line.replace(/[^\r\n]/g, ' ') : line).join('');
}
function lineAt(text, offset) { return text.slice(0, offset).split('\n').length; }
function matching(text, offset, open, close) { let depth = 0; for (let i = offset; i < text.length; i += 1) { if (text[i] === open) depth += 1; else if (text[i] === close && --depth === 0) return i; } return -1; }
function originalLocation(fileName) { const sourceRoot = sourceRoots.find((candidate) => fileName.toLowerCase().startsWith(candidate.toLowerCase())); return { source_root: sourceRoot, source_path: path.relative(sourceRoot, fileName).replaceAll('\\', '/') }; }
function sourceFilesFor(symbols, sourceRoot) {
  const temporary = path.join(os.tmpdir(), `t299-${process.pid}.regex`);
  const escaped = symbols.map((symbol) => `\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*\\(`);
  fs.writeFileSync(temporary, escaped.join('\n'), 'utf8');
  try { return execFileSync('rg.exe', ['-l', '-P', '--no-messages', '-f', temporary, '-g', '*.c', '-g', '*.cc', '-g', '*.cpp', '-g', '*.cxx', sourceRoot], { encoding: 'utf8', maxBuffer: 256 * 1024 * 1024 }).split(/\r?\n/).filter(Boolean); }
  catch (error) { if (error.status === 1) return []; throw error; }
  finally { fs.unlinkSync(temporary); }
}
function isDefinition(masked, symbolOffset, closeParen) {
  const prefix = masked.slice(masked.lastIndexOf('\n', symbolOffset) + 1, symbolOffset).trim();
  if (prefix && /[()!<>=,.;+\-\/]/.test(prefix)) return false;
  const tail = masked.slice(closeParen + 1, closeParen + 2049); const brace = tail.indexOf('{'); const semi = tail.indexOf(';');
  if (brace < 0 || (semi >= 0 && semi < brace)) return false;
  return true;
}

const interfaces = parseTsv(path.join(ops, 'mvdm-host-first-degree-canonical-interface-ledger.tsv'));
const rawCalls = parseTsv(path.join(ops, 'mvdm-host-first-degree-candidate-ledger.tsv'));
const canonicalCallResolution = parseTsv(path.join(ops, 'mvdm-host-first-degree-canonical-resolution-ledger.tsv'));
const includeFrontier = parseTsv(path.join(ops, 'mvdm-host-first-degree-include-frontier-ledger.tsv'));
const zero = parseTsv(path.join(ops, 'mvdm-host-zero-degree-canonical-definition-ledger.tsv'));
const zeroNames = new Set(zero.map((row) => row.canonical_symbol));
const rawById = new Map(rawCalls.map((row) => [row.candidate_id, row]));
const canonicalByRawId = new Map(canonicalCallResolution.map((row) => [row.raw_candidate_id, row]));
const frontierByRawId = new Map(includeFrontier.map((row) => [row.candidate_id, row]));
const publicInterfaces = interfaces.filter((row) => row.initial_first_degree_statuses.split(';').includes('public-modern-api-leaf-candidate'));
const publicSymbols = new Set(publicInterfaces.map((row) => row.callee_spelling));
const scopesByInterfaceId = new Map();
for (const item of interfaces) {
  const packageRoots = new Set();
  for (const rawId of item.raw_candidate_ids.split(';').filter(Boolean)) {
    if (canonicalByRawId.get(rawId)?.canonical_resolution !== 'remains-first-degree-external-interface') continue;
    const frontier = frontierByRawId.get(rawId);
    if (!frontier || !['declaration-guided-package-search', 'caller-package-limited-search'].includes(frontier.frontier_status)) continue;
    for (const packageRoot of frontier.allowed_package_roots.split(';').filter(Boolean)) packageRoots.add(packageRoot);
  }
  scopesByInterfaceId.set(item.canonical_interface_id, [...packageRoots].sort());
}
const sourceInterfaces = interfaces.filter((row) => !publicInterfaces.includes(row) && scopesByInterfaceId.get(row.canonical_interface_id).length);
const definitions = [];
const fileSymbols = new Map();
for (const sourceRoot of sourceRoots) for (const [packageRoot, symbols] of [...new Map(sourceInterfaces.flatMap((item) => scopesByInterfaceId.get(item.canonical_interface_id).map((packageRoot) => [packageRoot, []]))).entries()]) {
  const packageSymbols = sourceInterfaces.filter((item) => scopesByInterfaceId.get(item.canonical_interface_id).includes(packageRoot)).map((item) => item.callee_spelling);
  const directory = path.join(sourceRoot, packageRoot); if (!fs.existsSync(directory)) continue;
  for (const fileName of sourceFilesFor(packageSymbols, directory)) fileSymbols.set(fileName, new Set([...(fileSymbols.get(fileName) || []), ...packageSymbols]));
}
for (const [fileName, symbols] of fileSymbols) {
  const raw = fs.readFileSync(fileName, 'utf8'); const masked = maskC(raw); const macroNames = new Set([...raw.matchAll(/^\s*#\s*define\s+([A-Za-z_]\w*)\b/gm)].map((match) => match[1]));
  for (const symbol of symbols) {
    const expression = new RegExp(`\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*\\(`, 'g');
    for (const match of masked.matchAll(expression)) {
      if (macroNames.has(symbol)) continue;
      const open = masked.indexOf('(', match.index + symbol.length); const close = matching(masked, open, '(', ')');
      if (close < 0 || !isDefinition(masked, match.index, close)) continue;
      const tail = masked.slice(close + 1); const braceDelta = tail.indexOf('{'); if (braceDelta < 0) continue;
      const bodyOpen = close + 1 + braceDelta; const bodyClose = matching(masked, bodyOpen, '{', '}'); if (bodyClose < 0) continue;
      const location = originalLocation(fileName); const sourceLine = lineAt(raw, match.index);
      definitions.push({ symbol, source_root: location.source_root, source_path: location.source_path, source_sha256: sha256(fileName), source_line: String(sourceLine), signature_evidence: raw.slice(raw.lastIndexOf('\n', match.index) + 1, bodyOpen).replace(/\s+/g, ' ').trim().slice(0, 512), body: masked.slice(bodyOpen + 1, bodyClose), body_offset: bodyOpen + 1, masked });
    }
  }
}

// Include closure chooses the normal source package.  When that constrained
// package has no C/C++ definition, search the approved original trees only
// for discovery.  Every hit is retained as an unselected candidate/variant;
// this fallback never chooses a runtime provider from a test or tool package.
const scopedDefinitionSymbols = new Set(definitions.map((definition) => definition.symbol));
const fallbackSymbols = sourceInterfaces.map((item) => item.callee_spelling).filter((symbol) => !scopedDefinitionSymbols.has(symbol));
for (const sourceRoot of sourceRoots) for (const fileName of sourceFilesFor(fallbackSymbols, sourceRoot)) {
  const raw = fs.readFileSync(fileName, 'utf8'); const masked = maskC(raw); const macroNames = new Set([...raw.matchAll(/^\s*#\s*define\s+([A-Za-z_]\w*)\b/gm)].map((match) => match[1]));
  for (const symbol of fallbackSymbols) {
    const expression = new RegExp(`\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*\\(`, 'g');
    for (const match of masked.matchAll(expression)) {
      if (macroNames.has(symbol)) continue;
      const open = masked.indexOf('(', match.index + symbol.length); const close = matching(masked, open, '(', ')');
      if (close < 0 || !isDefinition(masked, match.index, close)) continue;
      const tail = masked.slice(close + 1); const braceDelta = tail.indexOf('{'); if (braceDelta < 0) continue;
      const bodyOpen = close + 1 + braceDelta; const bodyClose = matching(masked, bodyOpen, '{', '}'); if (bodyClose < 0) continue;
      const location = originalLocation(fileName); const sourceLine = lineAt(raw, match.index);
      definitions.push({ symbol, source_root: location.source_root, source_path: location.source_path, source_sha256: sha256(fileName), source_line: String(sourceLine), signature_evidence: raw.slice(raw.lastIndexOf('\n', match.index) + 1, bodyOpen).replace(/\s+/g, ' ').trim().slice(0, 512), body: masked.slice(bodyOpen + 1, bodyClose), body_offset: bodyOpen + 1, masked });
    }
  }
}

// A source location is an implementation identity.  Even byte-identical
// cross-tree/file copies stay distinct: this audit must never merge functions
// merely because their spelling, bytes or signature coincide.
for (const definition of definitions) {
  definition.source_aliases = `${definition.source_root}:${definition.source_path}`;
  definition.provenance_roots = definition.source_root;
}
definitions.sort((left, right) => left.symbol.localeCompare(right.symbol) || left.source_path.localeCompare(right.source_path) || Number(left.source_line) - Number(right.source_line));
definitions.forEach((definition, index) => { definition.definition_id = `MVDM-FIRST-DEFINITION-${String(index + 1).padStart(6, '0')}`; });
const bySymbol = new Map(); for (const definition of definitions) bySymbol.set(definition.symbol, [...(bySymbol.get(definition.symbol) || []), definition]);
const callImplementationRows = [];
for (const rawCall of rawCalls) {
  if (canonicalByRawId.get(rawCall.candidate_id)?.canonical_resolution !== 'remains-first-degree-external-interface') continue;
  const frontier = frontierByRawId.get(rawCall.candidate_id);
  const allowed = new Set((frontier?.allowed_package_roots || '').split(';').filter(Boolean));
  const allCandidates = bySymbol.get(rawCall.callee_spelling) || [];
  const scopedCandidates = allCandidates.filter((definition) => !allowed.size || [...allowed].some((packageRoot) => definition.source_path.startsWith(`${packageRoot}/`)));
  const candidates = scopedCandidates.length ? scopedCandidates : allCandidates;
  if (!candidates.length) {
    const publicProvider = publicSymbols.has(rawCall.callee_spelling);
    callImplementationRows.push({ raw_candidate_id: rawCall.candidate_id, caller_definition_id: rawCall.caller_definition_id, caller_source_path: rawCall.caller_source_path, caller_source_sha256: rawCall.caller_source_sha256, caller_source_line: rawCall.caller_source_line, callee_spelling: rawCall.callee_spelling, implementation_definition_id: '', implementation_source_identity: '', relation: publicProvider ? 'public-system-provider-no-opennt-body' : 'missing-original-definition', missing_basis: publicProvider ? 'Public Win32/CRT API: implementation is supplied by the host system, not an OpenNT source body.' : allowed.size ? 'No implementation body in the original include-constrained scope or approved-tree discovery fallback.' : 'No original C/C++ implementation is selected at this private/kernel historical boundary; retain this raw call as missing-definition evidence.' });
    continue;
  }
  for (const definition of candidates) callImplementationRows.push({ raw_candidate_id: rawCall.candidate_id, caller_definition_id: rawCall.caller_definition_id, caller_source_path: rawCall.caller_source_path, caller_source_sha256: rawCall.caller_source_sha256, caller_source_line: rawCall.caller_source_line, callee_spelling: rawCall.callee_spelling, implementation_definition_id: definition.definition_id, implementation_source_identity: `${definition.source_root}:${definition.source_path}:${definition.source_line}@${definition.source_sha256}`, relation: scopedCandidates.length ? 'original-implementation-in-include-constrained-scope' : 'original-implementation-discovery-candidate-outside-include-scope', missing_basis: '' });
}
const second = [];
for (const definition of definitions) {
  const calls = []; let macroExpression = false;
  for (const match of definition.body.matchAll(/\b([A-Za-z_]\w*)\s*\(/g)) {
    const callee = match[1]; if (keywords.has(callee.toLowerCase())) continue;
    // Function-like OpenNT/Win32 macros are conventionally all-uppercase.
    // They are recorded only in the body summary, never emitted as a false
    // second-degree function candidate. Mixed-case names remain candidates
    // until their next BFS audit proves function, macro or other form.
    if (/^[A-Z][A-Z0-9_]*$/.test(callee)) { macroExpression = true; calls.push(`${callee}:macro-expression`); continue; }
    if (zeroNames.has(callee)) { calls.push(`${callee}:zero`); continue; }
    if (bySymbol.has(callee)) { calls.push(`${callee}:first`); continue; }
    const sourceLine = lineAt(definition.masked, definition.body_offset + match.index);
    const candidateId = `MVDM-SECOND-CANDIDATE-${String(second.length + 1).padStart(6, '0')}`;
    second.push({ candidate_id: candidateId, caller_definition_id: definition.definition_id, caller_source_root: definition.source_root, caller_source_path: definition.source_path, caller_source_sha256: definition.source_sha256, caller_source_line: String(sourceLine), caller_symbol: definition.symbol, callee_spelling: callee, call_form: 'direct-named-call', boundary: 'Initial second-degree candidate only; T299 does not inspect this callee body.' });
    calls.push(`${callee}:${candidateId}`);
  }
  definition.leaf_status = calls.length === 0 ? 'leaf' : 'non-leaf';
  definition.leaf_basis = calls.length === 0 ? 'no-direct-call-expression-in-original-body' : macroExpression ? 'direct-call-expression-with-macro-expression' : 'direct-call-expression-present';
  definition.direct_call_summary = calls.join(';');
  delete definition.body; delete definition.body_offset; delete definition.masked;
}

const resolution = interfaces.map((item) => {
  const rawIds = item.raw_candidate_ids.split(';').filter(Boolean); const callers = rawIds.map((id) => rawById.get(id)).filter(Boolean);
  if (item.initial_first_degree_statuses.split(';').includes('public-modern-api-leaf-candidate')) return { canonical_interface_id: item.canonical_interface_id, callee_spelling: item.callee_spelling, raw_candidate_ids: item.raw_candidate_ids, raw_call_site_count: item.raw_call_site_count, resolution: 'public-win32-or-crt-leaf', original_definition_ids: '', original_definition_identities: '', basis: 'T298 public Win32/CRT leaf classification; no original body search is required.' };
  const matches = bySymbol.get(item.callee_spelling) || [];
  const scopes = scopesByInterfaceId.get(item.canonical_interface_id) || [];
  const terminal = !scopes.length;
  return { canonical_interface_id: item.canonical_interface_id, callee_spelling: item.callee_spelling, raw_candidate_ids: item.raw_candidate_ids, raw_call_site_count: item.raw_call_site_count, resolution: matches.length ? (matches.length === 1 ? 'original-definition-found' : 'original-definition-variant-family') : terminal ? 'original-boundary-no-body-search' : 'no-original-c-cpp-definition-found-in-approved-trees', original_definition_ids: matches.map((match) => match.definition_id).join(';'), original_definition_identities: matches.map((match) => `${match.source_aliases}:${match.source_line}@${match.source_sha256}`).join(';'), basis: matches.length ? (fallbackSymbols.includes(item.callee_spelling) ? `Include-constrained package roots (${scopes.join(';')}) had no body; every approved-tree candidate is retained without selecting a provider.` : `Parsed only from original include-constrained package roots: ${scopes.join(';')}. Each source-root/path/hash/definition-line implementation remains distinct, even when byte-identical.`) : terminal ? 'All original raw call sites terminate at a public/private/kernel/not-host-runtime boundary; T299 does not search beyond that boundary.' : `No C/C++ body definition was found in either approved tree after include-constrained roots (${scopes.join(';')}) and the discovery fallback.` };
});
if (resolution.length !== interfaces.length) throw new Error('Lost canonical first-degree interface during resolution');
writeTsv('mvdm-host-first-degree-original-definition-ledger.tsv', definitions, ['definition_id', 'symbol', 'source_root', 'source_path', 'source_aliases', 'provenance_roots', 'source_sha256', 'source_line', 'signature_evidence', 'leaf_status', 'leaf_basis', 'direct_call_summary']);
writeTsv('mvdm-host-first-degree-original-resolution-ledger.tsv', resolution, ['canonical_interface_id', 'callee_spelling', 'raw_candidate_ids', 'raw_call_site_count', 'resolution', 'original_definition_ids', 'original_definition_identities', 'basis']);
writeTsv('mvdm-host-first-degree-call-implementation-ledger.tsv', callImplementationRows, ['raw_candidate_id', 'caller_definition_id', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'callee_spelling', 'implementation_definition_id', 'implementation_source_identity', 'relation', 'missing_basis']);
writeTsv('mvdm-host-second-degree-initial-candidate-ledger.tsv', second, ['candidate_id', 'caller_definition_id', 'caller_source_root', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'caller_symbol', 'callee_spelling', 'call_form', 'boundary']);
console.log(`canonical interfaces=${interfaces.length}; public leaves=${publicInterfaces.length}; include-constrained source interfaces=${sourceInterfaces.length}; original definitions=${definitions.length}; original source files=${fileSymbols.size}; second-degree initial candidates=${second.length}`);
