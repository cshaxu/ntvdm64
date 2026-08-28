import crypto from 'node:crypto';
import fs from 'node:fs';
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
  const [header, ...body] = fs.readFileSync(path.join(operations, name), 'utf8')
    .split(/\r?\n/).filter(Boolean).map(split);
  return body.map((fields) => Object.fromEntries(header.map((key, index) => [key, fields[index] || ''])));
}
function readTsvIfPresent(name) {
  const file = path.join(operations, name);
  return fs.existsSync(file) ? readTsv(name) : [];
}
function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
function write(name, rows, columns) {
  const body = rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n');
  fs.writeFileSync(path.join(operations, name), `${columns.join('\t')}\n${body ? `${body}\n` : ''}`);
}
function identity(row) { return `${row.source_root}|${row.source_path}|${row.source_sha256}|${row.source_line}`; }
function sourceFile(row) { return path.join(row.source_root, ...row.source_path.split('/')); }
function isApprovedRoot(root) { return roots.some((candidate) => path.resolve(candidate).toLowerCase() === path.resolve(root).toLowerCase()); }

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
function paired(text, offset, open, close) {
  if (offset < 0) return -1;
  let depth = 0;
  for (let index = offset; index < text.length; index += 1) {
    if (text[index] === open) depth += 1;
    else if (text[index] === close && --depth === 0) return index;
  }
  return -1;
}
function lineOffset(raw, line) {
  const newlineWidth = raw.includes('\r\n') ? 2 : 1;
  return raw.split(/\r?\n/).slice(0, Number(line) - 1).reduce((total, text) => total + text.length + newlineWidth, 0);
}
function lineNumber(raw, offset) { return String(raw.slice(0, offset).split('\n').length); }
const sourceCache = new Map();
function sourceMaterial(file) {
  if (!sourceCache.has(file)) {
    const raw = fs.readFileSync(file, 'utf8');
    sourceCache.set(file, { raw, parsed: mask(raw), hash: crypto.createHash('sha256').update(raw).digest('hex') });
  }
  return sourceCache.get(file);
}
function bodyEvidence(row) {
  const file = sourceFile(row);
  if (!fs.existsSync(file)) return { result: 'source-file-missing', condition: 'unavailable', detail: file };
  const { raw, parsed, hash: actualHash } = sourceMaterial(file);
  if (actualHash !== row.source_sha256) return { result: 'source-hash-mismatch', condition: 'unavailable', detail: actualHash };
  const target = lineOffset(raw, row.source_line); const escaped = row.symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
  const matches = [...parsed.matchAll(new RegExp(`\\b${escaped}\\b`, 'g'))]
    .map((match) => match.index).filter((offset) => Math.abs(offset - target) < 8192).sort((left, right) => Math.abs(left - target) - Math.abs(right - target));
  for (const symbolOffset of matches) {
    // Historical SoftPC functions often use IFNn/IPTn calling-convention
    // macros.  The physical zero ledger already proves this is a definition;
    // find its first balanced body rather than imposing modern C syntax.
    const bodyOpen = parsed.indexOf('{', symbolOffset + row.symbol.length); const bodyClose = paired(parsed, bodyOpen, '{', '}');
    if (bodyOpen >= 0 && bodyClose >= 0 && bodyOpen - symbolOffset < 16384) {
      const body = raw.slice(bodyOpen, bodyClose + 1);
      const directives = [...body.matchAll(/^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b.*$/gm)].map((match) => match[0].trim());
      return {
        result: 'body-parsed',
        condition: directives.length ? 'conditional-directives-recorded' : 'unconditional',
        detail: directives.length ? directives.join(' | ').slice(0, 2048) : '',
        parsed_line: lineNumber(raw, symbolOffset),
      };
    }
  }
  return { result: 'body-unparsed', condition: 'conditional-or-historical-syntax', detail: 'No balanced original body was found near the recorded physical definition line.' };
}

const rootsRows = readTsv('mvdm-zero-degree-call-closure-ledger.tsv');
const admissionRows = readTsv('mvdm-zero-degree-rebase-admitted-definition-ledger.tsv');
const continuityRows = readTsv('mvdm-zero-degree-continuity-admission-ledger.tsv');
const topologyRows = readTsv('m0-t274-s5-mvdm-host-topology-manifest.tsv');
const signatureRows = readTsv('mvdm-first-degree-rebaselined-full-mvdm-signature-gate-ledger.tsv')
  .filter((row) => row.next_disposition.startsWith('source call'));
const edgeRows = readTsv('mvdm-zero-degree-rebase-expansion-edge-gate-ledger.tsv')
  .filter((row) => row.next_disposition.startsWith('confirmed'));
const incoming = new Map();
for (const row of signatureRows) {
  const list = incoming.get(row.selected_definition_identity) || [];
  list.push({ kind: 'confirmed-old-boundary', caller: row.caller_source_path, line: row.caller_source_line, id: row.candidate_id }); incoming.set(row.selected_definition_identity, list);
}
for (const row of edgeRows) {
  const target = row.selected_definition_identity || row.internal_candidate_identity;
  const list = incoming.get(target) || [];
  list.push({ kind: 'confirmed-MVDM-internal-edge', caller: row.caller_source_path, line: row.call_source_line, id: row.caller_identity }); incoming.set(target, list);
}
// The zero-degree root is not the whole historical MVDM directory.  It is
// strictly the physical original files selected into mvdm-host, plus any
// separately proven continuous-MVDM callee.  In particular wow16 is loaded
// guest material, not host source, even when an old spelling search finds a
// historical call into it.
function mvdmRelative(selectedPath) {
  const normal = selectedPath.replaceAll('/', '\\');
  const base = normal.toLowerCase().indexOf('\\base\\mvdm\\');
  if (base >= 0) return normal.slice(base + '\\base\\mvdm\\'.length).replaceAll('\\', '/');
  const privateMvdm = normal.toLowerCase().indexOf('\\nt\\private\\mvdm\\');
  if (privateMvdm >= 0) return normal.slice(privateMvdm + '\\nt\\private\\mvdm\\'.length).replaceAll('\\', '/');
  return '';
}
const hostTopology = new Set(topologyRows
  .filter((row) => row.selected_source_path && row.selected_source_sha256)
  .map((row) => `${mvdmRelative(row.selected_source_path)}|${row.selected_source_sha256}`));
function selectedHostSource(row) {
  return hostTopology.has(`${row.source_path}|${row.source_sha256}`);
}
const continuityAdmissions = new Set(continuityRows.map((row) => row.zero_identity));
function isExcludedSubtree(row) {
  // These historical subtrees have a settled non-host owner.  Physical MVDM
  // colocation is not permission to promote them into the host call graph.
  return ['bin86/', 'dos/v86/', 'wow16/', 'tools16/', 'vdmutils/']
    .some((prefix) => row.source_path.startsWith(prefix));
}
const raw = new Map();
for (const row of rootsRows) raw.set(identity(row), { ...row, membership_origin: 'selected-mvdm-host-root' });
for (const row of admissionRows) {
  const key = identity(row);
  // A prior run has already materialized this source-shaped admission in the
  // filtered closure.  Preserve its closure row and retain the separate
  // admission ledger as its provenance, rather than treating idempotence as a
  // graph conflict.
  if (!raw.has(key)) raw.set(key, { ...row, membership_origin: row.rebase_admission });
}
const excluded = [];
const all = new Map();
for (const [key, row] of raw) {
  if (!isExcludedSubtree(row) && (selectedHostSource(row) || continuityAdmissions.has(key))) {
    all.set(key, row);
  } else {
    excluded.push({
      zero_identity: key,
      membership_origin: row.membership_origin,
      source_root: row.source_root,
      source_path: row.source_path,
      source_sha256: row.source_sha256,
      source_line: row.source_line,
      symbol: row.symbol,
      exclusion: isExcludedSubtree(row)
        ? 'settled guest/tool MVDM subtree is not a zero-degree host root or admitted host closure member'
        : 'source is neither a selected mvdm-host physical file nor a separately admitted continuous-MVDM closure member',
    });
  }
}
// Preserve the complete first correction set when the generator is rerun
// against its already-filtered closure input.  The retained rows are evidence
// of why those definitions must not silently re-enter zero degree.
const priorExcluded = readTsvIfPresent('mvdm-zero-degree-root-exclusion-ledger.tsv');
const exclusionsByIdentity = new Map();
for (const row of [...priorExcluded, ...excluded]) exclusionsByIdentity.set(row.zero_identity, row);
const closureColumns = ['zero_definition_id', 'source_root', 'source_path', 'source_sha256', 'source_line', 'symbol', 'signature_evidence', 'linkage', 'project_components', 'zero_origin', 'final_runtime_note'];
// The historical full ledgers are evidence and deliberately immutable.  This
// validator must never rewrite them; zero-ledger0.tsv is generated separately
// from that full record.
const rows = [];
for (const row of [...all.values()].sort((left, right) => left.source_root.localeCompare(right.source_root) || left.source_path.localeCompare(right.source_path) || Number(left.source_line) - Number(right.source_line))) {
  const key = identity(row); const evidence = bodyEvidence(row); const inbound = incoming.get(key) || [];
  const oldOrigin = row.zero_origin || '';
  const oldOriginParts = oldOrigin.split('\0');
  const isRoot = selectedHostSource(row);
  const historicEdge = row.membership_origin === 'selected-mvdm-host-root' && oldOriginParts.length >= 5 ? [{ kind: 'historic-MVDM-continuity-edge', caller: oldOriginParts[1], line: oldOriginParts[3], id: `${oldOriginParts[4]}${oldOrigin.includes(';variant-family') ? ';variant-family' : ''}` }] : [];
  const allInbound = [...historicEdge, ...inbound];
  // A historic selected-mvdm-host root is lawful by membership, even when a
  // later spelling search happens to produce a cross-file candidate with the
  // same name.  The same-translation-unit rule applies to rebase admission,
  // where an inbound edge is the sole authority for adding a new static body.
  const staticInvalid = !isRoot && row.linkage === 'translation-unit-local' && allInbound.some((edge) => edge.caller !== row.source_path);
  const admissionLegal = isRoot || continuityAdmissions.has(key);
  const sourceLegal = isApprovedRoot(row.source_root) && evidence.result !== 'source-file-missing' && evidence.result !== 'source-hash-mismatch';
  const structural = sourceLegal && admissionLegal && !staticInvalid && evidence.result === 'body-parsed';
  const legality = structural ? (evidence.condition === 'unconditional' ? 'structurally-valid' : 'structurally-valid-conditional-context-recorded') : 'requires-correction-or-variant-proof';
  const reasons = [
    !sourceLegal ? evidence.result : '',
    !admissionLegal ? 'no root qualification or confirmed physical inbound edge' : '',
    staticInvalid ? 'translation-unit-local target has a cross-file inbound edge' : '',
    evidence.result === 'body-unparsed' ? evidence.detail : '',
  ].filter(Boolean).join('; ');
  rows.push({
    zero_identity: key,
    membership_origin: row.membership_origin,
    source_root: row.source_root,
    source_path: row.source_path,
    source_sha256: row.source_sha256,
    source_line: row.source_line,
    symbol: row.symbol,
    linkage: row.linkage,
    inbound_evidence: isRoot ? `root:${row.zero_origin}` : allInbound.map((edge) => `${edge.kind}:${edge.caller}:${edge.line}:${edge.id}`).join(' | '),
    body_parse_result: evidence.result,
    condition_context: evidence.condition,
    condition_evidence: evidence.detail,
    legality,
    reason: reasons,
  });
}
const columns = ['zero_identity', 'membership_origin', 'source_root', 'source_path', 'source_sha256', 'source_line', 'symbol', 'linkage', 'inbound_evidence', 'body_parse_result', 'condition_context', 'condition_evidence', 'legality', 'reason'];
const count = (predicate) => rows.filter(predicate).length;
console.log(`zero identities=${rows.length}; excluded=${exclusionsByIdentity.size}; structural unconditional=${count((row) => row.legality === 'structurally-valid')}; structural conditional=${count((row) => row.legality === 'structurally-valid-conditional-context-recorded')}; exceptions=${count((row) => row.legality === 'requires-correction-or-variant-proof')}; roots=${count((row) => selectedHostSource(row))}; rebased=${count((row) => row.membership_origin !== 'selected-mvdm-host-root')}`);
