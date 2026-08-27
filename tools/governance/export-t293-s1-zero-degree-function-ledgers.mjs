import crypto from 'node:crypto';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';

const repositoryRoot = process.argv[2] || process.cwd();
const operationsRoot = path.join(repositoryRoot, 'docs', 'etc', 'operations');
const keywords = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'typedef', 'defined', '__asm', 'asm', 'catch', '__try']);

function parseTsv(text) {
  const lines = text.replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean);
  return lines.map((line) => {
    const fields = []; let field = ''; let quoted = false;
    for (let index = 0; index < line.length; index += 1) {
      const ch = line[index];
      if (ch === '"') {
        if (quoted && line[index + 1] === '"') { field += '"'; index += 1; }
        else quoted = !quoted;
      } else if (ch === '\t' && !quoted) { fields.push(field); field = ''; }
      else field += ch;
    }
    fields.push(field);
    return fields;
  });
}
function rowsFromTsv(fileName) {
  const [header, ...data] = parseTsv(fs.readFileSync(fileName, 'utf8'));
  return data.map((values) => Object.fromEntries(header.map((key, index) => [key, values[index] || ''])));
}
function tsvValue(value) { return `"${String(value).replaceAll('"', '""')}"`; }
function writeTsv(fileName, rows, columns) {
  const text = [columns.join('\t'), ...rows.map((row) => columns.map((column) => tsvValue(row[column] || '')).join('\t'))].join('\r\n') + '\r\n';
  const temporary = `${fileName}.tmp`;
  fs.writeFileSync(temporary, text, 'utf8');
  fs.renameSync(temporary, fileName);
}
function sha256(fileName) { return crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex'); }

function maskCText(input) {
  const chars = [...input]; let state = 'code'; let escape = false;
  for (let index = 0; index < chars.length; index += 1) {
    const ch = chars[index]; const next = chars[index + 1] || '';
    if (state === 'code') {
      if (ch === '/' && next === '/') { chars[index] = ' '; chars[index + 1] = ' '; index += 1; state = 'line'; }
      else if (ch === '/' && next === '*') { chars[index] = ' '; chars[index + 1] = ' '; index += 1; state = 'block'; }
      else if (ch === '"') { chars[index] = ' '; state = 'string'; escape = false; }
      else if (ch === "'") { chars[index] = ' '; state = 'character'; escape = false; }
    } else if (state === 'line') {
      if (ch === '\n') state = 'code'; else if (ch !== '\r') chars[index] = ' ';
    } else if (state === 'block') {
      if (ch === '*' && next === '/') { chars[index] = ' '; chars[index + 1] = ' '; index += 1; state = 'code'; }
      else if (ch !== '\r' && ch !== '\n') chars[index] = ' ';
    } else {
      if (ch === '\n') state = 'code';
      else {
        if (ch !== '\r') chars[index] = ' ';
        if (escape) escape = false;
        else if (ch === '\\') escape = true;
        else if ((state === 'string' && ch === '"') || (state === 'character' && ch === "'")) state = 'code';
      }
    }
  }
  const lines = chars.join('').split(/(?<=\n)/); let continuation = false;
  return lines.map((line) => {
    const directive = continuation || /^\s*#/.test(line);
    continuation = directive && /\\\s*(?:\r?\n)?$/.test(line);
    return directive ? line.replace(/[^\r\n]/g, ' ') : line;
  }).join('');
}
function lineAt(text, offset) { return text.slice(0, offset).split('\n').length; }
function closeParen(text, open) {
  let depth = 0;
  for (let index = open; index < text.length; index += 1) {
    if (text[index] === '(') depth += 1;
    else if (text[index] === ')' && --depth === 0) return index;
  }
  return -1;
}
function closeBrace(text, open) {
  let depth = 0;
  for (let index = open; index < text.length; index += 1) {
    if (text[index] === '{') depth += 1;
    else if (text[index] === '}' && --depth === 0) return index;
  }
  return -1;
}
function closeBraceWithPreprocessor(masked, original, open) {
  // Historical MVDM bodies commonly contain mutually exclusive #if/#else
  // branches that each open a block.  Preserve all source alternatives in the
  // audit while resetting brace depth at each alternative boundary; otherwise
  // two source branches would be mistaken for one unbalanced function body.
  let depth = 0; const branches = [];
  for (let index = open; index < masked.length; index += 1) {
    if ((index === 0 || original[index - 1] === '\n') && /^\s*#/.test(original.slice(index))) {
      const end = original.indexOf('\n', index);
      const directive = original.slice(index, end < 0 ? original.length : end).replace(/^\s*#\s*/, '').trim();
      if (/^(if|ifdef|ifndef)\b/.test(directive)) branches.push({ depth });
      else if (/^(elif|else)\b/.test(directive) && branches.length) depth = branches[branches.length - 1].depth;
      else if (/^endif\b/.test(directive) && branches.length) { depth = branches.pop().depth; }
      if (end < 0) break;
      index = end;
      continue;
    }
    if (masked[index] === '{') depth += 1;
    else if (masked[index] === '}' && --depth === 0) return index;
  }
  return -1;
}
function nextCode(text, offset) {
  for (let index = offset; index < text.length; index += 1) if (!/\s/.test(text[index])) return text[index];
  return '';
}
function previousBoundary(text, offset) {
  return Math.max(text.lastIndexOf(';', offset), text.lastIndexOf('}', offset), text.lastIndexOf('{', offset)) + 1;
}
function linkageFor(masked, offset) {
  const prefix = masked.slice(previousBoundary(masked, offset), offset);
  if (/\bstatic\b/.test(prefix)) return 'file-static';
  if (/\bextern\b/.test(prefix)) return 'external-declared-definition';
  return 'external-or-unspecified';
}
function signatureFor(original, masked, offset, close) {
  // The immediately preceding C statement may be a large comment, include or
  // macro definition.  The definition line is stable source identity evidence
  // without accidentally serialising unrelated file prologue into the ledger.
  const start = original.lastIndexOf('\n', offset) + 1;
  return original.slice(start, close + 1).replace(/\s+/g, ' ').trim().slice(0, 512);
}
function recoveryFor(disposition) {
  if (disposition === 'direct') return ['yes', 'byte-exact-or-minimal-mirror'];
  if (disposition === 'binding-only') return ['yes', 'binding-only'];
  if (disposition === 'adapter-backed') return ['yes', 'original-body-through-same-shaped-adapter'];
  if (disposition === 'overlay-required') return ['yes', 'mirror-plus-registered-overlay'];
  return ['no', 'not-host-runtime'];
}

const fileLedger = rowsFromTsv(path.join(operationsRoot, 'mvdm-file-recovery-ledger.tsv'));
const semanticRows = rowsFromTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-semantic-symbol-ledger.tsv'));
const semanticBySymbol = new Map(semanticRows.map((row) => [row.symbol, row.stage1_final_disposition]));
const mirrorRoot = path.join(repositoryRoot, 'src', 'mvdm-host');
// Zero degree is the whole current mvdm-host component, including sources
// already classified tool-only or not-host-runtime.  The mirror path is used
// only to determine component membership; every parsed byte comes from the
// provenance-selected original path in the ledger.
const belongsToMvdmHost = (row) => fs.existsSync(path.join(mirrorRoot, row.target_path));
const sourceRows = fileLedger.filter((row) => row.file_kind === 'source' && /\.(c|cc|cpp|cxx)$/i.test(row.target_path) && belongsToMvdmHost(row));
const selectedHeaders = fileLedger.filter((row) => row.file_kind === 'declaration' && belongsToMvdmHost(row));
const macroNames = new Set();
const definitionMacroNames = new Set();
for (const row of [...sourceRows, ...selectedHeaders]) {
  if (!fs.existsSync(row.selected_source_path)) throw new Error(`Missing selected source ${row.selected_source_path}`);
  const source = fs.readFileSync(row.selected_source_path, 'utf8');
  for (const match of source.matchAll(/^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\b/gm)) {
    macroNames.add(match[1]);
    definitionMacroNames.add(match[1]);
  }
}
// MVDM frequently calls public macro forms such as NT_SUCCESS and
// RtlCopyMemory.  The selected-file ledger deliberately does not mirror every
// SDK/DDK declaration carrier, so collect macro spellings from both approved
// original trees.  This prevents macro calls from becoming fictitious degree-1
// function nodes.  The recorded result remains source-form evidence; actual
// function-body selection belongs to the next degree only.
for (const root of ['O:\\repos.external\\OpenNT', 'O:\\repos.external\\OpenNT-4.5']) {
  if (!fs.existsSync(root)) continue;
  try {
    const output = execFileSync('rg.exe', ['--no-messages', '--no-filename', '--only-matching', '-P', '^\\s*#\\s*define\\s+\\K[A-Za-z_][A-Za-z0-9_]*', '-g', '*.h', '-g', '*.inc', root], {
      encoding: 'utf8', maxBuffer: 128 * 1024 * 1024,
    });
    output.split(/\r?\n/).filter(Boolean).forEach((name) => macroNames.add(name));
  } catch (error) {
    if (error.status !== 1) throw error;
  }
}

const definitions = [];
for (const row of sourceRows) {
  const original = fs.readFileSync(row.selected_source_path, 'utf8');
  const masked = maskCText(original);
  for (const match of masked.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) {
    const symbol = match[1];
    // Only a macro declared in the selected source/declaration context can
    // suppress a definition.  A same spelling in an unrelated historical SDK
    // header must not erase a real selected definition.  This also filters the
    // SoftPC IFN/IPT declaration grammar before its brace is mistaken for a
    // body belonging to the macro itself.
    if (keywords.has(symbol.toLowerCase()) || definitionMacroNames.has(symbol) || /^(IFN|IPT|IPF)\d*$/.test(symbol)) continue;
    const open = masked.indexOf('(', match.index + match[0].length - 1);
    const close = closeParen(masked, open);
    if (close < 0 || nextCode(masked, close + 1) !== '{') continue;
    const bodyOpen = masked.indexOf('{', close + 1);
    const bodyClose = closeBraceWithPreprocessor(masked, original, bodyOpen);
    if (bodyClose < 0) throw new Error(`Unbalanced definition ${row.target_path}:${lineAt(masked, match.index)} ${symbol}`);
    const sourceLine = lineAt(masked, match.index);
    const linkage = linkageFor(masked, match.index);
    const sourceHash = row.selected_source_sha256 || sha256(row.selected_source_path);
    definitions.push({
      definition_id: `MVDM-ZERO-DEFINITION-${String(definitions.length + 1).padStart(6, '0')}`,
      file_id: row.file_id, source_path: row.target_path, source_sha256: sourceHash,
      package_root: row.package_root, source_line: String(sourceLine), symbol, linkage,
      signature_evidence: signatureFor(original, masked, match.index, close),
      file_disposition: row.expected_final_disposition, body_open: bodyOpen, body_close: bodyClose,
      masked, original,
    });
  }
}

const bySymbol = new Map(); const byFileAndSymbol = new Map();
for (const definition of definitions) {
  if (!bySymbol.has(definition.symbol)) bySymbol.set(definition.symbol, []);
  bySymbol.get(definition.symbol).push(definition);
  const key = `${definition.file_id}:${definition.symbol}`;
  if (!byFileAndSymbol.has(key)) byFileAndSymbol.set(key, []);
  byFileAndSymbol.get(key).push(definition);
}
function resolveInternal(caller, symbol) {
  const sameFile = byFileAndSymbol.get(`${caller.file_id}:${symbol}`) || [];
  if (sameFile.length === 1) return [sameFile[0], 'same-selected-source-definition', sameFile];
  const global = (bySymbol.get(symbol) || []).filter((definition) => definition.linkage !== 'file-static');
  if (global.length === 1) return [global[0], 'unique-nonstatic-selected-definition', global];
  // Original MVDM is a multi-product union.  Independent tools can legitimately
  // reuse a global spelling, but a caller's own original package/directory is
  // the first source-shaped linkage context before declaring an ambiguity.
  const samePackage = global.filter((definition) => definition.package_root === caller.package_root);
  if (samePackage.length === 1) return [samePackage[0], 'unique-same-package-selected-definition', samePackage];
  const callerDirectory = path.posix.dirname(caller.source_path.replaceAll('\\', '/'));
  const sameDirectory = global.filter((definition) => path.posix.dirname(definition.source_path.replaceAll('\\', '/')) === callerDirectory);
  if (sameDirectory.length === 1) return [sameDirectory[0], 'unique-same-directory-selected-definition', sameDirectory];
  // The selected MVDM corpus intentionally preserves historical product and
  // architecture alternatives.  When ordinary source-shaped linkage cannot
  // choose one body, retain every mutually-exclusive selected body as a
  // zero-degree conditional variant target.  It is not a first-degree
  // candidate: no source outside the zero-degree component needs discovery.
  if (sameFile.length > 1 || global.length > 1) return [[...sameFile, ...global], 'conditional-variant-zero-targets', [...sameFile, ...global]];
  return [null, 'no-selected-definition', []];
}

const candidates = [];
const conditionalVariantCalls = [];
for (const definition of definitions) {
  const body = definition.masked.slice(definition.body_open + 1, definition.body_close);
  const bodyOffset = definition.body_open + 1;
  const callDetails = []; let hasInternal = false; let hasExternal = false; let hasOpaque = false;
  for (const match of body.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) {
    const symbol = match[1];
    if (keywords.has(symbol.toLowerCase())) continue;
    const absolute = bodyOffset + match.index;
    const open = definition.masked.indexOf('(', absolute + match[0].length - 1);
    const close = closeParen(definition.masked, open);
    if (close < 0) { hasOpaque = true; callDetails.push(`${symbol}@${lineAt(definition.masked, absolute)}:unbalanced-expression`); continue; }
    const line = lineAt(definition.masked, absolute);
    if (macroNames.has(symbol)) { hasOpaque = true; callDetails.push(`${symbol}@${line}:macro-expression`); continue; }
    const [target, resolution, definitionOptions] = resolveInternal(definition, symbol);
    if (target && !Array.isArray(target)) {
      hasInternal = true;
      callDetails.push(`${symbol}@${line}:zero:${target.definition_id}`);
      continue;
    }
    if (resolution === 'conditional-variant-zero-targets') {
      hasInternal = true;
      const resolutionId = `MVDM-ZERO-CONDITIONAL-CALL-${String(conditionalVariantCalls.length + 1).padStart(6, '0')}`;
      conditionalVariantCalls.push({
        resolution_id: resolutionId, caller_definition_id: definition.definition_id,
        caller_source_path: definition.source_path, caller_source_sha256: definition.source_sha256,
        caller_source_line: String(line), caller_symbol: definition.symbol,
        callee_spelling: symbol, resolution_status: resolution,
        candidate_zero_definition_ids: [...new Set(definitionOptions.map((option) => option.definition_id))].join(';'),
        candidate_zero_source_identities: [...new Set(definitionOptions.map((option) => `${option.source_path}:${option.source_line}`))].join(';'),
        boundary: 'historical compile-time/product alternatives; all targets are zero-degree definitions and none is a first-degree function candidate',
      });
      callDetails.push(`${symbol}@${line}:zero-conditional:${resolutionId}`);
      continue;
    }
    hasExternal = true;
    const semantic = semanticBySymbol.get(symbol) || 'not-yet-classified-by-preceding-external-audit';
    const publicLeaf = semantic === 'public-win32-or-crt-binding';
    const candidateId = `MVDM-FIRST-CANDIDATE-${String(candidates.length + 1).padStart(6, '0')}`;
    candidates.push({
      candidate_id: candidateId, caller_definition_id: definition.definition_id,
      caller_source_path: definition.source_path, caller_source_sha256: definition.source_sha256,
      caller_source_line: String(line), caller_symbol: definition.symbol,
      callee_spelling: symbol, call_form: 'direct-named-call', internal_resolution: resolution,
      prior_semantic_disposition: semantic,
      initial_first_degree_status: publicLeaf ? 'public-modern-api-leaf-candidate' : 'requires-first-degree-source-audit',
      boundary: 'T293 records caller and call form only; T294 may inspect declaration/definition body',
    });
    callDetails.push(`${symbol}@${line}:first:${candidateId}`);
  }
  for (const pointer of body.matchAll(/\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)\s*\(/g)) {
    const line = lineAt(definition.masked, bodyOffset + pointer.index);
    hasOpaque = true;
    callDetails.push(`${pointer[1]}@${line}:function-pointer-expression`);
  }
  const [shouldRecover, recoveryForm] = recoveryFor(definition.file_disposition);
  let leafStatus; let leafBasis;
  if (!hasInternal && !hasExternal && !hasOpaque) { leafStatus = 'leaf'; leafBasis = 'no-direct-function-call'; }
  else if (!hasInternal && !hasOpaque && hasExternal) {
    const ownCandidates = candidates.filter((candidate) => candidate.caller_definition_id === definition.definition_id);
    if (ownCandidates.length && ownCandidates.every((candidate) => candidate.initial_first_degree_status === 'public-modern-api-leaf-candidate')) {
      leafStatus = 'leaf'; leafBasis = 'public-modern-win32-crt-calls-only';
    } else { leafStatus = 'non-leaf'; leafBasis = 'one-or-more-first-degree-candidate-callees'; }
  } else if (hasOpaque) { leafStatus = 'non-leaf'; leafBasis = 'macro-or-function-pointer-expression-present'; }
  else { leafStatus = 'non-leaf'; leafBasis = 'selected-zero-degree-callee-present'; }
  definition.should_recover = shouldRecover;
  definition.recovery_form = recoveryForm;
  definition.final_function_disposition = definition.file_disposition;
  definition.function_disposition_basis = 'inherited from audited enclosing MVDM file disposition; no body is modified or enabled by T293';
  definition.leaf_status = leafStatus;
  definition.leaf_basis = leafBasis;
  definition.direct_call_summary = callDetails.join(';');
}

const zeroColumns = ['definition_id', 'file_id', 'source_path', 'source_sha256', 'package_root', 'source_line', 'symbol', 'linkage', 'signature_evidence', 'file_disposition', 'final_function_disposition', 'function_disposition_basis', 'should_recover', 'recovery_form', 'leaf_status', 'leaf_basis', 'direct_call_summary'];
const candidateColumns = ['candidate_id', 'caller_definition_id', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'caller_symbol', 'callee_spelling', 'call_form', 'internal_resolution', 'prior_semantic_disposition', 'initial_first_degree_status', 'boundary'];
const ambiguousColumns = ['resolution_id', 'caller_definition_id', 'caller_source_path', 'caller_source_sha256', 'caller_source_line', 'caller_symbol', 'callee_spelling', 'resolution_status', 'candidate_zero_definition_ids', 'candidate_zero_source_identities', 'boundary'];
const definitionsByFile = new Map();
for (const definition of definitions) definitionsByFile.set(definition.file_id, (definitionsByFile.get(definition.file_id) || 0) + 1);
const coverageRows = sourceRows.map((row) => ({
  file_id: row.file_id, source_path: row.target_path, source_sha256: row.selected_source_sha256 || sha256(row.selected_source_path),
  package_root: row.package_root, final_file_disposition: row.expected_final_disposition,
  function_definition_count: String(definitionsByFile.get(row.file_id) || 0),
  source_coverage: definitionsByFile.has(row.file_id) ? 'one-or-more-definition-rows-recorded' : 'no-C-C++-function-definition-found',
  source_basis: 'component membership by src/mvdm-host path; parsed bytes from provenance-selected original source only',
}));
const coverageColumns = ['file_id', 'source_path', 'source_sha256', 'package_root', 'final_file_disposition', 'function_definition_count', 'source_coverage', 'source_basis'];
writeTsv(path.join(operationsRoot, 'mvdm-host-zero-degree-definition-ledger.tsv'), definitions, zeroColumns);
writeTsv(path.join(operationsRoot, 'mvdm-host-first-degree-candidate-ledger.tsv'), candidates, candidateColumns);
writeTsv(path.join(operationsRoot, 'mvdm-host-zero-degree-call-resolution-ledger.tsv'), conditionalVariantCalls, ambiguousColumns);
writeTsv(path.join(operationsRoot, 'mvdm-host-zero-degree-source-coverage-ledger.tsv'), coverageRows, coverageColumns);

const counts = (rows, key) => [...rows.reduce((map, row) => map.set(row[key], (map.get(row[key]) || 0) + 1), new Map()).entries()].sort(([a], [b]) => a.localeCompare(b));
console.log(`source files: ${sourceRows.length}; definitions: ${definitions.length}; first-degree call candidates: ${candidates.length}; zero-degree conditional-variant calls: ${conditionalVariantCalls.length}; no-definition sources: ${coverageRows.filter((row) => row.function_definition_count === '0').length}`);
console.log(`definition linkage: ${counts(definitions, 'linkage').map(([key, value]) => `${key}=${value}`).join('; ')}`);
console.log(`leaf status: ${counts(definitions, 'leaf_status').map(([key, value]) => `${key}=${value}`).join('; ')}`);
console.log(`first-degree status: ${counts(candidates, 'initial_first_degree_status').map(([key, value]) => `${key}=${value}`).join('; ')}`);
