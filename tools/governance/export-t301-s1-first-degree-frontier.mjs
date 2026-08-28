import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const sources = [
  ['OpenNT', 'O:\\repos.external\\OpenNT'],
  ['OpenNT-4.5', 'O:\\repos.external\\OpenNT-4.5'],
].filter(([, root]) => fs.existsSync(root)).sort((left, right) => right[1].length - left[1].length);

function parseTsv(fileName) {
  const lines = fs.readFileSync(path.join(operations, fileName), 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean);
  const split = (line) => { const row = []; let field = ''; let quoted = false; for (let index = 0; index < line.length; index += 1) { const ch = line[index]; if (ch === '"') { if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted; } else if (ch === '\t' && !quoted) { row.push(field); field = ''; } else field += ch; } row.push(field); return row; };
  const [header, ...body] = lines.map(split);
  return body.map((row) => Object.fromEntries(header.map((key, index) => [key, row[index] || ''])));
}
function writeTsv(fileName, rows, columns) {
  const quote = (value) => `"${String(value ?? '').replaceAll('"', '""')}"`;
  fs.writeFileSync(path.join(operations, fileName), [columns.join('\t'), ...rows.map((row) => columns.map((column) => quote(row[column])).join('\t'))].join('\n') + '\n');
}
const hashes = new Map();
function sha256(fileName) { if (!hashes.has(fileName)) hashes.set(fileName, crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex')); return hashes.get(fileName); }
function sourceRootFor(pathName) { return sources.find(([, root]) => pathName.toLowerCase().startsWith(root.toLowerCase())); }
function packageRoot(relative) {
  const parts = relative.replaceAll('\\', '/').split('/');
  if (parts[0] === 'base' && parts.length >= 3) return parts.slice(0, 3).join('/');
  if (parts[0] === 'nt' && parts.length >= 4) return parts.slice(0, 4).join('/');
  if (parts[0] === 'windows' && parts.length >= 3) return parts.slice(0, 3).join('/');
  if (parts[0] === 'private' && parts.length >= 3) return parts.slice(0, 3).join('/');
  return parts.slice(0, Math.min(2, parts.length)).join('/');
}

const headers = [];
for (const [edition, root] of sources) {
  const walk = (directory) => {
    for (const entry of fs.readdirSync(directory, { withFileTypes: true })) {
      const fileName = path.join(directory, entry.name);
      if (entry.isDirectory()) walk(fileName);
      else if (/\.(h|inc)$/i.test(entry.name)) headers.push({ edition, root, fileName, relative: path.relative(root, fileName).replaceAll('\\', '/') });
    }
  };
  walk(root);
}
const headersByTail = new Map();
for (const header of headers) {
  const tail = header.relative.toLowerCase();
  headersByTail.set(tail, [...(headersByTail.get(tail) || []), header]);
  const base = path.posix.basename(tail);
  headersByTail.set(base, [...(headersByTail.get(base) || []), header]);
}
function uniqueByBytes(candidates) {
  const groups = new Map();
  for (const candidate of candidates) {
    const digest = sha256(candidate.fileName);
    groups.set(digest, [...(groups.get(digest) || []), candidate]);
  }
  return groups.size === 1 ? [...groups.values()][0] : [];
}
const includeResolutionCache = new Map();
function resolveInclude(caller, spelling) {
  const cacheKey = `${caller}\u0000${spelling}`;
  if (includeResolutionCache.has(cacheKey)) return includeResolutionCache.get(cacheKey);
  const local = path.resolve(path.dirname(caller), spelling);
  if (fs.existsSync(local)) {
    const entry = sourceRootFor(local);
    if (entry) { const result = [{ edition: entry[0], root: entry[1], fileName: local, relative: path.relative(entry[1], local).replaceAll('\\', '/') }]; includeResolutionCache.set(cacheKey, result); return result; }
  }
  const normalized = spelling.replaceAll('\\', '/').toLowerCase();
  const tail = headersByTail.get(normalized) || [];
  const result = tail.length ? uniqueByBytes(tail) : uniqueByBytes(headersByTail.get(path.posix.basename(normalized)) || []);
  includeResolutionCache.set(cacheKey, result);
  return result;
}
const includesCache = new Map();
function directIncludes(caller) {
  if (includesCache.has(caller)) return includesCache.get(caller);
  const raw = fs.readFileSync(caller, 'utf8');
  const result = [...raw.matchAll(/^\s*#\s*include\s*["<]([^">]+)[">]/gm)].map((match) => match[1]);
  includesCache.set(caller, result);
  return result;
}
function headerIdentity(header) { return `${header.edition}:${header.relative}@${sha256(header.fileName)}`; }
function includeClosure(caller, includes) {
  const resolved = new Map();
  const queue = [];
  for (const spelling of includes) for (const header of resolveInclude(caller, spelling)) {
    const id = headerIdentity(header); if (!resolved.has(id)) { resolved.set(id, header); queue.push(header); }
  }
  while (queue.length) {
    const header = queue.shift();
    for (const spelling of directIncludes(header.fileName)) for (const nested of resolveInclude(header.fileName, spelling)) {
      const id = headerIdentity(nested); if (!resolved.has(id)) { resolved.set(id, nested); queue.push(nested); }
    }
  }
  return [...resolved.values()];
}

// P15 made the rebased frontier authoritative.  The older 12,426-row T300
// boundary and its spelling-level resolution drafts are historical evidence,
// never a T301 worklist.
const boundaries = parseTsv('mvdm-first-degree-rebased-boundary-ledger.tsv');
const zero = [
  ...parseTsv('mvdm-zero-degree-call-closure-ledger.tsv'),
  ...parseTsv('mvdm-zero-degree-rebase-admitted-definition-ledger.tsv'),
];
const zeroRoots = new Map();
for (const row of zero) zeroRoots.set(`${row.source_path}\u0000${row.source_sha256}`, row.source_root);
const targetSpellings = new Set(boundaries.map((row) => row.callee_spelling));
const callers = new Map();
for (const row of boundaries) {
  const key = `${row.caller_source_path}\u0000${row.caller_source_sha256}`;
  if (!callers.has(key)) callers.set(key, row);
}
const callerHeaders = new Map();
for (const [key, row] of callers) {
  const root = zeroRoots.get(key);
  if (!root) throw new Error(`T300 caller identity is absent from zero closure: ${row.caller_source_path}@${row.caller_source_sha256}`);
  const fileName = path.join(root, row.caller_source_path);
  if (!fs.existsSync(fileName) || sha256(fileName) !== row.caller_source_sha256) throw new Error(`T300 caller source identity no longer matches: ${fileName}`);
  const includes = directIncludes(fileName);
  const resolved = includeClosure(fileName, includes);
  callerHeaders.set(key, { includes, resolved });
}
const declarationTokens = new Map();
for (const info of callerHeaders.values()) for (const header of info.resolved) {
  const id = headerIdentity(header); if (declarationTokens.has(id)) continue;
  const raw = fs.readFileSync(header.fileName, 'utf8');
  const lines = new Map();
  for (const match of raw.matchAll(/\b([A-Za-z_]\w*)\s*\(/g)) {
    const symbol = match[1]; if (!targetSpellings.has(symbol)) continue;
    const line = raw.slice(0, match.index).split('\n').length;
    lines.set(symbol, [...(lines.get(symbol) || []), line]);
  }
  declarationTokens.set(id, lines);
}
const frontierRows = [];
const workRows = [];
const declarationRows = [];
const priorDefinitionRows = [];
const callerFrontierRows = [...callerHeaders.entries()].map(([key, info], index) => {
  const [caller_source_path, caller_source_sha256] = key.split('\u0000');
  return {
    caller_frontier_id: `MVDM-FIRST-CALLER-FRONTIER-${String(index + 1).padStart(6, '0')}`,
    caller_source_path,
    caller_source_sha256,
    direct_include_spellings: info.includes.join(';'),
    direct_original_headers: info.resolved.map(headerIdentity).join(';'),
    allowed_package_roots: [...new Set(info.resolved.map((header) => packageRoot(header.relative)).filter(Boolean))].sort().join(';'),
    frontier_basis: info.resolved.length ? 'recursive original include closure from the physical zero-degree caller; declaration match and body selection remain the next resolution step' : 'no unique original direct-header identity; retain source-unavailable or declaration-search outcome explicitly',
  };
});
const frontierIdByCaller = new Map(callerFrontierRows.map((row) => [`${row.caller_source_path}\u0000${row.caller_source_sha256}`, row.caller_frontier_id]));
for (const row of boundaries) {
  const key = `${row.caller_source_path}\u0000${row.caller_source_sha256}`;
  const info = callerHeaders.get(key);
  const roots = [...new Set(info.resolved.map((header) => packageRoot(header.relative)).filter(Boolean))].sort();
  const preliminary = 'requires-original-declaration-and-body-search';
  frontierRows.push({
    candidate_id: row.candidate_id,
    caller_symbol: row.caller_symbol,
    caller_source_path: row.caller_source_path,
    caller_source_sha256: row.caller_source_sha256,
    caller_source_line: row.caller_source_line,
    callee_spelling: row.callee_spelling,
    caller_frontier_id: frontierIdByCaller.get(key),
  });
  workRows.push({
    candidate_id: row.candidate_id,
    callee_spelling: row.callee_spelling,
    preliminary_disposition: preliminary,
    required_next_action: 'resolve declaration-guided original body variants or record public leaf, hard-boundary, finite-shallow or source-unavailable outcome',
    constraint: 'T301 must not merge by spelling, inspect a second-degree body, import source, or select a runtime provider.',
  });
  for (const header of info.resolved) for (const line of declarationTokens.get(headerIdentity(header)).get(row.callee_spelling) || []) declarationRows.push({
    candidate_id: row.candidate_id,
    callee_spelling: row.callee_spelling,
    caller_frontier_id: frontierIdByCaller.get(key),
    declaration_header_identity: headerIdentity(header),
    declaration_header_path: header.relative,
    declaration_line: String(line),
    declaration_package_root: packageRoot(header.relative),
    disposition: 'original-header token in the physical caller include closure; declaration form and compatible definition remain to be resolved',
  });
}
writeTsv('mvdm-first-degree-rebaselined-caller-include-frontier-ledger.tsv', callerFrontierRows, ['caller_frontier_id', 'caller_source_path', 'caller_source_sha256', 'direct_include_spellings', 'direct_original_headers', 'allowed_package_roots', 'frontier_basis']);
writeTsv('mvdm-first-degree-rebaselined-include-frontier-ledger.tsv', frontierRows, ['candidate_id', 'caller_symbol', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'callee_spelling', 'caller_frontier_id']);
writeTsv('mvdm-first-degree-rebaselined-worklist-ledger.tsv', workRows, ['candidate_id', 'callee_spelling', 'preliminary_disposition', 'required_next_action', 'constraint']);
writeTsv('mvdm-first-degree-rebaselined-declaration-frontier-ledger.tsv', declarationRows, ['candidate_id', 'callee_spelling', 'caller_frontier_id', 'declaration_header_identity', 'declaration_header_path', 'declaration_line', 'declaration_package_root', 'disposition']);
writeTsv('mvdm-first-degree-rebaselined-prior-definition-identity-ledger.tsv', priorDefinitionRows, ['candidate_id', 'callee_spelling', 'prior_definition_id', 'source_root', 'source_path', 'source_sha256', 'source_line', 'linkage', 'disposition']);
const counts = new Map(); for (const row of workRows) counts.set(row.preliminary_disposition, (counts.get(row.preliminary_disposition) || 0) + 1);
console.log(`boundary calls=${boundaries.length}; physical caller files=${callers.size}; original header identities=${new Set(callerFrontierRows.flatMap((row) => row.direct_original_headers.split(';').filter(Boolean))).size}; preliminary outcomes=${[...counts].map(([key, value]) => `${key}=${value}`).join('; ')}`);
