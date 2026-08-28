import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const sourceRoots = ['O:\\repos.external\\OpenNT', 'O:\\repos.external\\OpenNT-4.5']
  .map((root) => path.join(root, 'base', 'mvdm')).filter(fs.existsSync);
const mirrorRoots = [
  ['mvdm-host', path.join(repository, 'src', 'mvdm-host')],
  ['mvdm-support', path.join(repository, 'src', 'mvdm-support')],
  ['mvdm-tools', path.join(repository, 'src', 'mvdm-tools')],
  ['mvdm-softpc-firmware', path.join(repository, 'src', 'mvdm-softpc-firmware')],
  ['mvdm-guest', path.join(repository, 'src', 'mvdm-guest')],
].filter(([, root]) => fs.existsSync(root));
const sourceExtensions = new Set(['.c', '.cc', '.cpp', '.cxx']);
const controls = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'typedef', 'defined', '__asm', 'asm', 'catch', '__try', '__except', 'except', '__finally', 'finally', 'try']);

function walk(root) {
  const output = [];
  for (const entry of fs.readdirSync(root, { withFileTypes: true })) {
    const full = path.join(root, entry.name);
    if (entry.isDirectory()) output.push(...walk(full));
    else if (sourceExtensions.has(path.extname(entry.name).toLowerCase())) output.push(full);
  }
  return output;
}
function hash(file) { return crypto.createHash('sha256').update(fs.readFileSync(file)).digest('hex'); }
function writeTsv(name, rows, columns) {
  const quote = (value) => `"${String(value ?? '').replaceAll('"', '""')}"`;
  fs.writeFileSync(path.join(operations, name), [columns.join('\t'), ...rows.map((row) => columns.map((column) => quote(row[column])).join('\t'))].join('\n') + '\n');
}
function readTsv(name) {
  const lines = fs.readFileSync(path.join(operations, name), 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean);
  const split = (line) => { const row = []; let field = ''; let quoted = false; for (let index = 0; index < line.length; index += 1) { const ch = line[index]; if (ch === '"') { if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted; } else if (ch === '\t' && !quoted) { row.push(field); field = ''; } else field += ch; } row.push(field); return row; };
  const [header, ...body] = lines.map(split); return body.map((row) => Object.fromEntries(header.map((key, index) => [key, row[index] || ''])));
}
function lineAt(text, index) { return text.slice(0, index).split('\n').length; }
function mask(input) {
  const chars = [...input]; let state = 'code'; let escaped = false;
  for (let i = 0; i < chars.length; i += 1) {
    const ch = chars[i]; const next = chars[i + 1] || '';
    if (state === 'code') {
      if (ch === '/' && next === '/') { chars[i] = chars[i + 1] = ' '; i += 1; state = 'line'; }
      else if (ch === '/' && next === '*') { chars[i] = chars[i + 1] = ' '; i += 1; state = 'block'; }
      else if (ch === '"') { chars[i] = ' '; state = 'string'; escaped = false; }
      else if (ch === "'") { chars[i] = ' '; state = 'char'; escaped = false; }
    } else if (state === 'line') { if (ch === '\n') state = 'code'; else if (ch !== '\r') chars[i] = ' '; }
    else if (state === 'block') { if (ch === '*' && next === '/') { chars[i] = chars[i + 1] = ' '; i += 1; state = 'code'; } else if (ch !== '\r' && ch !== '\n') chars[i] = ' '; }
    else if (ch === '\n') state = 'code';
    else { if (ch !== '\r') chars[i] = ' '; if (escaped) escaped = false; else if (ch === '\\') escaped = true; else if ((state === 'string' && ch === '"') || (state === 'char' && ch === "'")) state = 'code'; }
  }
  return chars.join('').split(/(?<=\n)/).map((line) => /^\s*#/.test(line) ? line.replace(/[^\r\n]/g, ' ') : line).join('');
}
function paired(text, offset, open, close) { let depth = 0; for (let i = offset; i < text.length; i += 1) { if (text[i] === open) depth += 1; else if (text[i] === close && --depth === 0) return i; } return -1; }
function argumentCount(text, open, close) {
  const inner = text.slice(open + 1, close).trim(); if (!inner || inner === 'void') return 0;
  let depth = 0; let count = 1;
  for (let index = 0; index < inner.length; index += 1) { const ch = inner[index]; if (ch === '(' || ch === '[' || ch === '{') depth += 1; else if (ch === ')' || ch === ']' || ch === '}') depth -= 1; else if (ch === ',' && depth === 0) count += 1; }
  return count;
}
function isDefinition(text, symbolOffset, closeParen) {
  const prefix = text.slice(text.lastIndexOf('\n', symbolOffset) + 1, symbolOffset).trim();
  if (prefix && /[()!<>=,.;+\-\\/]/.test(prefix)) return false;
  const tail = text.slice(closeParen + 1, closeParen + 2049); const next = tail.search(/\S/);
  return next >= 0 && tail[next] === '{';
}
function parseDefinitions(sourceFile, identity) {
  const raw = fs.readFileSync(sourceFile, 'utf8'); const masked = mask(raw); const macros = new Set([...raw.matchAll(/^\s*#\s*define\s+([A-Za-z_]\w*)\b/gm)].map((match) => match[1]));
  const output = [];
  for (const match of masked.matchAll(/\b([A-Za-z_]\w*)\s*\(/g)) {
    const symbol = match[1]; if (macros.has(symbol) || controls.has(symbol.toLowerCase())) continue;
    const open = masked.indexOf('(', match.index + symbol.length); const close = paired(masked, open, '(', ')');
    if (close < 0 || !isDefinition(masked, match.index, close)) continue;
    const bodyOpen = masked.indexOf('{', close); const bodyClose = paired(masked, bodyOpen, '{', '}');
    if (bodyOpen < 0 || bodyClose < 0) continue;
    const signature = raw.slice(raw.lastIndexOf('\n', match.index) + 1, bodyOpen).replace(/\s+/g, ' ').trim().slice(0, 512);
    output.push({ ...identity, symbol, source_line: String(lineAt(raw, match.index)), signature_evidence: signature, parameter_count: argumentCount(masked, open, close), linkage: /\bstatic\b/.test(signature) ? 'translation-unit-local' : 'externally-linkable', body: masked.slice(bodyOpen + 1, bodyClose), body_offset: bodyOpen + 1, masked });
  }
  return output;
}
function sourceIdentity(sourceFile, root) { return `${root}\u0000${path.relative(root, sourceFile).replaceAll('\\', '/')}`; }
function originalFileName(root, relative) { return path.join(root, ...relative.split('/')); }
const headerIncludeCache = new Map();
function includeClosure(sourceRoot, relative) {
  const cacheKey = `${sourceRoot}\u0000${relative}`;
  if (headerIncludeCache.has(cacheKey)) return headerIncludeCache.get(cacheKey);
  const visited = new Set(); const pending = [originalFileName(sourceRoot, relative)]; const headers = new Set();
  while (pending.length) {
    const current = pending.pop(); if (visited.has(current) || !fs.existsSync(current)) continue; visited.add(current);
    const raw = fs.readFileSync(current, 'utf8');
    for (const match of raw.matchAll(/^\s*#\s*include\s*["<]([^">]+)[">]/gm)) {
      const name = match[1]; const candidates = [path.resolve(path.dirname(current), name), path.resolve(sourceRoot, name)];
      for (const candidate of candidates) if (fs.existsSync(candidate)) {
        const normalized = path.normalize(candidate); if (path.extname(normalized).toLowerCase() === '.h') headers.add(normalized);
        if (!visited.has(normalized)) pending.push(normalized); break;
      }
    }
  }
  headerIncludeCache.set(cacheKey, headers); return headers;
}
function headerDeclaresSymbol(header, symbol) {
  const raw = fs.readFileSync(header, 'utf8');
  return new RegExp(`\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*\\(`).test(mask(raw));
}

// Establish the selected original identity first.  The project file can prove
// a mirror only when a unique original MVDM source file has the same bytes and
// either the matching re-rooted suffix or a unique content identity.
const originals = [];
for (const root of sourceRoots) for (const file of walk(root)) originals.push({ root, file, relative: path.relative(root, file).replaceAll('\\', '/'), sha256: hash(file) });
const originalByHash = new Map();
for (const item of originals) originalByHash.set(item.sha256, [...(originalByHash.get(item.sha256) || []), item]);
const originalByRelative = new Map();
for (const item of originals) originalByRelative.set(item.relative, [...(originalByRelative.get(item.relative) || []), item]);
const localFiles = [];
for (const [component, root] of mirrorRoots) for (const file of walk(root)) {
  const relative = path.relative(root, file).replaceAll('\\', '/'); const sha256 = hash(file);
  const candidates = originalByHash.get(sha256) || [];
  const suffix = candidates.filter((item) => item.relative === relative || item.relative.endsWith(`/${relative}`));
  const samePath = originalByRelative.get(relative) || [];
  const selected = suffix.length === 1 ? suffix[0] : candidates.length === 1 ? candidates[0] : samePath.length === 1 ? samePath[0] : null;
  const identity_status = suffix.length === 1 ? 'byte-exact-relative-path' : candidates.length === 1 ? 'byte-exact-unique-content' : samePath.length === 1 ? 'same-path-divergent-mirror; original body selected for source audit' : 'unresolved-project-mirror-identity';
  localFiles.push({ component, file, relative, sha256, selected, candidate_count: candidates.length, suffix_count: suffix.length, same_path_count: samePath.length, identity_status });
}
const selectedFiles = localFiles.filter((item) => item.selected);
const definitions = [];
for (const item of selectedFiles) {
  const selected = item.selected; const id = { source_root: selected.root, source_path: selected.relative, source_sha256: selected.sha256 };
  for (const definition of parseDefinitions(selected.file, id)) definitions.push({ ...definition, components: item.component });
}
const definitionKey = (item) => `${item.source_root}\u0000${item.source_path}\u0000${item.source_sha256}\u0000${item.source_line}\u0000${item.symbol}`;
const filesByIdentity = new Map();
for (const item of selectedFiles) {
  const key = `${item.selected.root}\u0000${item.selected.relative}\u0000${item.selected.sha256}`;
  filesByIdentity.set(key, [...(filesByIdentity.get(key) || []), item.component]);
}
const canonical = new Map();
for (const definition of definitions) {
  const key = definitionKey(definition); const prior = canonical.get(key);
  if (prior) prior.components = [...new Set([...prior.components.split(';'), definition.components])].sort().join(';');
  else canonical.set(key, { ...definition, components: definition.components });
}
const all = [...canonical.values()];
const byFileSymbol = new Map(); const bySymbol = new Map();
for (const definition of all) {
  const fileKey = `${definition.source_root}\u0000${definition.source_path}\u0000${definition.source_sha256}\u0000${definition.symbol}`;
  byFileSymbol.set(fileKey, [...(byFileSymbol.get(fileKey) || []), definition]);
  if (definition.linkage === 'externally-linkable') bySymbol.set(definition.symbol, [...(bySymbol.get(definition.symbol) || []), definition]);
}
const roots = all.filter((definition) => definition.components.split(';').includes('mvdm-host'));
const closure = new Map(); const predecessor = new Map(); const pending = [...roots];
for (const root of roots) { closure.set(definitionKey(root), root); predecessor.set(definitionKey(root), 'mvdm-host-root'); }
const boundary = []; const ambiguous = []; const variantFamilies = new Map();
while (pending.length) {
  const caller = pending.shift();
  for (const match of caller.body.matchAll(/\b([A-Za-z_]\w*)\s*\(/g)) {
    const callee = match[1]; if (controls.has(callee.toLowerCase()) || /^[A-Z][A-Z0-9_]*$/.test(callee)) continue;
    const fileKey = `${caller.source_root}\u0000${caller.source_path}\u0000${caller.source_sha256}\u0000${callee}`;
    const local = byFileSymbol.get(fileKey) || [];
    const external = local.length ? local : (bySymbol.get(callee) || []);
    // A same-package definition is a source-build constraint, not a spelling
    // heuristic: selected original MVDM packages are built as separate object
    // groups.  Use it only when it leaves one physical external definition.
    const callerPackage = caller.source_path.split('/')[0];
    const packageLocal = local.length ? local : external.filter((item) => item.source_path.split('/')[0] === callerPackage);
    const candidates = packageLocal.length === 1 ? packageLocal : external;
    let resolvedCandidates = candidates;
    const callOpen = caller.masked.indexOf('(', caller.body_offset + match.index + callee.length);
    const callClose = paired(caller.masked, callOpen, '(', ')');
    const callArity = callClose < 0 ? -1 : argumentCount(caller.masked, callOpen, callClose);
    // For a remaining cross-package collision, follow the original include
    // graph. A header that declares the called spelling can constrain only a
    // same-package candidate; it never selects an arbitrary same-named body.
    if (resolvedCandidates.length > 1) {
      const declaredPackages = new Set();
      for (const header of includeClosure(caller.source_root, caller.source_path)) {
        if (!headerDeclaresSymbol(header, callee)) continue;
        const relativeHeader = path.relative(caller.source_root, header).replaceAll('\\', '/');
        if (!relativeHeader.startsWith('..')) declaredPackages.add(relativeHeader.split('/')[0]);
      }
      const declarationScoped = resolvedCandidates.filter((item) => declaredPackages.has(item.source_path.split('/')[0]));
      if (declarationScoped.length === 1) resolvedCandidates = declarationScoped;
    }
    if (resolvedCandidates.length > 1 && callArity >= 0) {
      const arityScoped = resolvedCandidates.filter((item) => item.parameter_count === callArity);
      if (arityScoped.length === 1) resolvedCandidates = arityScoped;
    }
    const unique = resolvedCandidates.length === 1 ? resolvedCandidates[0] : null;
    const sourceLine = lineAt(caller.masked, caller.body_offset + match.index);
    if (unique) {
      const targetKey = definitionKey(unique);
      if (!closure.has(targetKey)) { closure.set(targetKey, unique); predecessor.set(targetKey, `${definitionKey(caller)}@${sourceLine}`); pending.push(unique); }
    } else if (resolvedCandidates.length > 1) {
      // All candidates came from the selected project MVDM mirror set.  The
      // caller therefore remains zero-degree under the source-closure rule,
      // but the identities must remain separate historical build variants.
      const familyKey = `${definitionKey(caller)}@${sourceLine}\u0000${callee}`;
      variantFamilies.set(familyKey, { caller_symbol: caller.symbol, caller_source_path: caller.source_path, caller_source_sha256: caller.source_sha256, caller_source_line: String(sourceLine), callee_spelling: callee, candidate_count: String(resolvedCandidates.length), candidate_identities: resolvedCandidates.map((item) => definitionKey(item)).join(';'), disposition: 'zero-degree original-MVDM variant family; no name-only implementation selection' });
      for (const variant of resolvedCandidates) {
        const variantKey = definitionKey(variant);
        if (!closure.has(variantKey)) { closure.set(variantKey, variant); predecessor.set(variantKey, `${definitionKey(caller)}@${sourceLine};variant-family`); pending.push(variant); }
      }
    } else {
      boundary.push({ caller_key: definitionKey(caller), caller_symbol: caller.symbol, caller_source_path: caller.source_path, caller_source_sha256: caller.source_sha256, caller_source_line: String(sourceLine), callee_spelling: callee, call_form: 'direct-named-call', boundary: 'Resolved call leaves selected original MVDM definition closure; first-degree candidate.' });
    }
  }
}
const closureRows = [...closure.values()].sort((a, b) => a.source_path.localeCompare(b.source_path) || Number(a.source_line) - Number(b.source_line)).map((definition, index) => ({ zero_definition_id: `MVDM-ZERO-CLOSURE-${String(index + 1).padStart(6, '0')}`, source_root: definition.source_root, source_path: definition.source_path, source_sha256: definition.source_sha256, source_line: definition.source_line, symbol: definition.symbol, signature_evidence: definition.signature_evidence, linkage: definition.linkage, project_components: definition.components, zero_origin: predecessor.get(definitionKey(definition)), final_runtime_note: definition.components.split(';').includes('mvdm-host') ? 'mvdm-host source root; runtime disposition remains separately governed' : 'reachable original-MVDM source only; runtime disposition remains separately governed' }));
const identityRows = all.sort((a, b) => a.source_path.localeCompare(b.source_path) || Number(a.source_line) - Number(b.source_line)).map((definition) => ({ source_root: definition.source_root, source_path: definition.source_path, source_sha256: definition.source_sha256, source_line: definition.source_line, symbol: definition.symbol, signature_evidence: definition.signature_evidence, project_components: definition.components, selected_identity_status: 'project-mirror-to-original-identity-resolved' }));
const dedupeBoundary = new Map();
for (const row of boundary) dedupeBoundary.set(`${row.caller_key}\u0000${row.caller_source_line}\u0000${row.callee_spelling}`, row);
const boundaryRows = [...dedupeBoundary.values()].sort((a, b) => a.caller_source_path.localeCompare(b.caller_source_path) || Number(a.caller_source_line) - Number(b.caller_source_line) || a.callee_spelling.localeCompare(b.callee_spelling));
boundaryRows.forEach((row, index) => { row.candidate_id = `MVDM-FIRST-REBASELINED-${String(index + 1).padStart(6, '0')}`; delete row.caller_key; });
const priorInterfaces = readTsv('mvdm-host-first-degree-canonical-interface-ledger.tsv');
const priorBySpelling = new Map();
for (const row of priorInterfaces) priorBySpelling.set(row.callee_spelling, row);
const firstBySpelling = new Map();
for (const row of boundaryRows) firstBySpelling.set(row.callee_spelling, [...(firstBySpelling.get(row.callee_spelling) || []), row]);
const firstFunctionRows = [...firstBySpelling.entries()].sort(([left], [right]) => left.localeCompare(right)).map(([callee, calls], index) => {
  const prior = priorBySpelling.get(callee);
  const callers = [...new Set(calls.map((row) => `${row.caller_source_path}:${row.caller_source_line}:${row.caller_symbol}`))].sort();
  return {
    first_function_id: `MVDM-FIRST-FUNCTION-${String(index + 1).padStart(6, '0')}`,
    callee_spelling: callee,
    call_site_count: String(calls.length),
    raw_candidate_ids: calls.map((row) => row.candidate_id).join(';'),
    caller_definition_count: String(new Set(calls.map((row) => `${row.caller_source_path}:${row.caller_symbol}`)).size),
    caller_source_identities: callers.join(';'),
    prior_t299_interface_id: prior?.canonical_interface_id || '',
    prior_t299_status: prior?.initial_first_degree_statuses || '',
    first_degree_disposition: prior ? 'carried-forward interface audit evidence; physical implementation identity must be revalidated' : 'new external interface candidate; physical implementation identity pending first-degree audit',
    boundary: 'Every call site is emitted only after the resolved caller is in the MVDM-local zero closure and no unique selected original-MVDM target was found.'
  };
});
writeTsv('mvdm-project-definition-identity-ledger.tsv', identityRows, ['source_root', 'source_path', 'source_sha256', 'source_line', 'symbol', 'signature_evidence', 'project_components', 'selected_identity_status']);
writeTsv('mvdm-zero-degree-call-closure-ledger.tsv', closureRows, ['zero_definition_id', 'source_root', 'source_path', 'source_sha256', 'source_line', 'symbol', 'signature_evidence', 'linkage', 'project_components', 'zero_origin', 'final_runtime_note']);
writeTsv('mvdm-first-degree-rebaselined-boundary-ledger.tsv', boundaryRows, ['candidate_id', 'caller_symbol', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'callee_spelling', 'call_form', 'boundary']);
writeTsv('mvdm-first-degree-function-candidate-ledger.tsv', firstFunctionRows, ['first_function_id', 'callee_spelling', 'call_site_count', 'raw_candidate_ids', 'caller_definition_count', 'caller_source_identities', 'prior_t299_interface_id', 'prior_t299_status', 'first_degree_disposition', 'boundary']);
writeTsv('mvdm-zero-degree-ambiguous-internal-call-ledger.tsv', ambiguous, ['caller_symbol', 'caller_source_path', 'caller_source_line', 'callee_spelling', 'candidate_count', 'candidate_identities']);
writeTsv('mvdm-zero-degree-variant-family-ledger.tsv', [...variantFamilies.values()].sort((left, right) => left.caller_source_path.localeCompare(right.caller_source_path) || Number(left.caller_source_line) - Number(right.caller_source_line)), ['caller_symbol', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'callee_spelling', 'candidate_count', 'candidate_identities', 'disposition']);
const unresolvedMirrors = localFiles.filter((item) => !item.selected).map((item) => ({ component: item.component, project_path: path.relative(repository, item.file).replaceAll('\\', '/'), sha256: item.sha256, same_hash_original_candidates: item.candidate_count, suffix_candidates: item.suffix_count, disposition: 'not-used-for-original-identity-closure' }));
writeTsv('mvdm-project-mirror-identity-exception-ledger.tsv', unresolvedMirrors, ['component', 'project_path', 'sha256', 'same_hash_original_candidates', 'suffix_candidates', 'disposition']);
const divergenceMirrors = localFiles.filter((item) => item.selected && item.identity_status.startsWith('same-path-divergent')).map((item) => ({ component: item.component, project_path: path.relative(repository, item.file).replaceAll('\\', '/'), project_sha256: item.sha256, original_source_path: item.selected.relative, original_source_sha256: item.selected.sha256, disposition: item.identity_status }));
writeTsv('mvdm-project-mirror-source-divergence-ledger.tsv', divergenceMirrors, ['component', 'project_path', 'project_sha256', 'original_source_path', 'original_source_sha256', 'disposition']);
const closureIdentity = new Set(closureRows.map((row) => `${row.source_root}\u0000${row.source_path}\u0000${row.source_sha256}\u0000${row.source_line}\u0000${row.symbol}`));
const priorFirst = readTsv('mvdm-host-first-degree-original-definition-ledger.tsv');
const promotedPriorFirst = priorFirst.filter((row) => closureIdentity.has(`${row.source_root}\u0000${row.source_path}\u0000${row.source_sha256}\u0000${row.source_line}\u0000${row.symbol}`)).map((row) => ({ prior_first_definition_id: row.definition_id, source_root: row.source_root, source_path: row.source_path, source_sha256: row.source_sha256, source_line: row.source_line, symbol: row.symbol, reclassification: 'zero-degree: reachable selected original MVDM definition' }));
writeTsv('mvdm-zero-degree-prior-first-reclassification-ledger.tsv', promotedPriorFirst, ['prior_first_definition_id', 'source_root', 'source_path', 'source_sha256', 'source_line', 'symbol', 'reclassification']);
const priorSecond = readTsv('mvdm-host-second-degree-initial-candidate-ledger.tsv');
const priorFirstById = new Map(priorFirst.map((row) => [row.definition_id, row]));
const promotedPriorSecond = priorSecond.filter((row) => { const caller = priorFirstById.get(row.caller_definition_id); return caller && closureIdentity.has(`${caller.source_root}\u0000${caller.source_path}\u0000${caller.source_sha256}\u0000${caller.source_line}\u0000${caller.symbol}`); }).map((row) => ({ prior_second_candidate_id: row.candidate_id, prior_caller_definition_id: row.caller_definition_id, caller_symbol: row.caller_symbol, caller_source_path: row.caller_source_path, caller_source_line: row.caller_source_line, callee_spelling: row.callee_spelling, reclassification: 'first-degree candidate: prior caller is now zero-degree MVDM-local' }));
writeTsv('mvdm-first-degree-prior-second-reclassification-ledger.tsv', promotedPriorSecond, ['prior_second_candidate_id', 'prior_caller_definition_id', 'caller_symbol', 'caller_source_path', 'caller_source_line', 'callee_spelling', 'reclassification']);
console.log(`project original definitions=${all.length}; mvdm-host root definitions=${roots.length}; reachable zero closure=${closureRows.length}; first-degree external boundary calls=${boundaryRows.length}; unique first-degree function candidates=${firstFunctionRows.length}; zero-degree variant-family edges=${variantFamilies.size}; unresolved internal call identities=${ambiguous.length}; unresolved local source files=${unresolvedMirrors.length}; divergent same-path mirrors=${divergenceMirrors.length}; prior-first-to-zero=${promotedPriorFirst.length}; prior-second-to-first=${promotedPriorSecond.length}`);
