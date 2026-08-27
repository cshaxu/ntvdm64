import fs from 'node:fs';
import path from 'node:path';

const repositoryRoot = process.argv[2] || process.cwd();
const operationsRoot = path.join(repositoryRoot, 'docs', 'etc', 'operations');

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
    fields.push(field); return fields;
  });
}
function rowsFromTsv(fileName) {
  const [header, ...data] = parseTsv(fs.readFileSync(fileName, 'utf8'));
  return data.map((values) => Object.fromEntries(header.map((key, index) => [key, values[index] || ''])));
}
function quote(value) { return `"${String(value).replaceAll('"', '""')}"`; }
function writeTsv(fileName, rows, columns) {
  fs.writeFileSync(fileName, [columns.join('\t'), ...rows.map((row) => columns.map((column) => quote(row[column] || '')).join('\t'))].join('\r\n') + '\r\n', 'utf8');
}
function unique(values) { return [...new Set(values.filter(Boolean))]; }
function lineAt(fileName, lineNumber) {
  const lines = fs.readFileSync(fileName, 'utf8').split(/\r?\n/);
  return lines[Number(lineNumber) - 1] || '';
}

const rawDefinitions = rowsFromTsv(path.join(operationsRoot, 'mvdm-host-zero-degree-definition-ledger.tsv'));
const rawCalls = rowsFromTsv(path.join(operationsRoot, 'mvdm-host-first-degree-candidate-ledger.tsv'));
const fileLedger = rowsFromTsv(path.join(operationsRoot, 'mvdm-file-recovery-ledger.tsv'));
const nativePathByTarget = new Map(fileLedger.map((row) => [row.target_path, row.selected_source_path]));
const mirrorRoot = path.join(repositoryRoot, 'src', 'mvdm-host');
const macroDefinitionSources = fileLedger.filter((row) =>
  fs.existsSync(path.join(mirrorRoot, row.target_path)) && fs.existsSync(row.selected_source_path));
const macroDefinitions = new Map();
for (const row of macroDefinitionSources) {
  const text = fs.readFileSync(row.selected_source_path, 'utf8');
  for (const match of text.matchAll(/^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(/gm)) {
    const name = match[1];
    macroDefinitions.set(name, [...(macroDefinitions.get(name) || []), `${row.target_path}:${text.slice(0, match.index).split('\n').length}`]);
  }
}

const canonicalRows = rawDefinitions.map((definition) => {
  const sourceFile = nativePathByTarget.get(definition.source_path);
  if (!sourceFile || !fs.existsSync(sourceFile)) throw new Error(`Missing selected original source for ${definition.definition_id}: ${definition.source_path}`);
  const sourceLine = lineAt(sourceFile, definition.source_line);
  const escaped = definition.symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
  const macroMatch = sourceLine.match(new RegExp(`\\b${escaped}\\s*\\(\\s*([A-Za-z_][A-Za-z0-9_]*)`));
  const macroDefinition = macroDefinitions.has(definition.symbol);
  // A normal function can share a spelling with a conditional empty macro
  // (for example demPrintMsg).  Only the Ctags macro-name form itself is
  // normalized here: historical function-definition macros use all-caps
  // spellings, while a regular mixed/lower-case function retains its name.
  const macroGenerated = Boolean(/^[A-Z][A-Z0-9_]*$/.test(definition.symbol) && macroMatch && macroDefinition);
  const canonicalSymbol = macroGenerated ? macroMatch[1] : definition.symbol;
  const nameResolution = macroGenerated ? 'source-local-function-definition-macro-first-argument' : 'raw-ctags-symbol';
  const sourceScope = `${definition.source_path}@${definition.source_sha256}`;
  const canonicalGroupKey = definition.linkage === 'file-static'
    ? `file-static:${sourceScope}:${canonicalSymbol}`
    : `linkable:${canonicalSymbol}`;
  return {
    canonical_definition_id: `MVDM-ZERO-CANONICAL-DEFINITION-${definition.definition_id.slice(-6)}`,
    raw_definition_id: definition.definition_id,
    source_path: definition.source_path,
    source_sha256: definition.source_sha256,
    source_line: definition.source_line,
    linkage: definition.linkage,
    raw_symbol: definition.symbol,
    canonical_symbol: canonicalSymbol,
    name_resolution: nameResolution,
    macro_definition_spelling: macroGenerated ? definition.symbol : '',
    source_line_evidence: macroGenerated
      ? `${sourceLine.trim()} [macro defined at ${macroDefinitions.get(definition.symbol).join(';')}]`
      : definition.signature_evidence,
    canonical_group_key: canonicalGroupKey,
    final_function_disposition: definition.final_function_disposition,
    leaf_status: definition.leaf_status,
  };
});

if (canonicalRows.length !== rawDefinitions.length || new Set(canonicalRows.map((row) => row.raw_definition_id)).size !== rawDefinitions.length) {
  throw new Error('Canonical projection does not cover every raw zero-degree identity exactly once');
}
for (const row of canonicalRows.filter((candidate) => candidate.name_resolution.startsWith('source-local'))) {
  if (row.canonical_symbol === row.raw_symbol || !row.source_line_evidence.includes(`${row.raw_symbol}(`)) {
    throw new Error(`Invalid macro-generated name recovery: ${row.raw_definition_id}`);
  }
}

const canonicalGroups = new Map();
for (const row of canonicalRows) canonicalGroups.set(row.canonical_group_key, [...(canonicalGroups.get(row.canonical_group_key) || []), row]);
const summaryRows = [...canonicalGroups.entries()].sort(([left], [right]) => left.localeCompare(right)).map(([key, rows], index) => {
  const kinds = unique(rows.map((row) => row.name_resolution));
  const isStatic = rows[0].linkage === 'file-static';
  const kind = rows.length === 1
    ? (kinds.some((item) => item.startsWith('source-local')) ? 'macro-generated-singleton' : 'singleton')
    : (isStatic ? 'same-source-static-redeclaration-or-variant' : 'product-or-conditional-variant-family');
  return {
    canonical_group_id: `MVDM-ZERO-CANONICAL-GROUP-${String(index + 1).padStart(6, '0')}`,
    canonical_group_key: key,
    canonical_symbol: rows[0].canonical_symbol,
    grouping_scope: isStatic ? 'selected-source-path-and-hash' : 'linkable-original-symbol-family',
    group_kind: kind,
    raw_identity_count: String(rows.length),
    macro_generated_identity_count: String(rows.filter((row) => row.name_resolution.startsWith('source-local')).length),
    raw_definition_ids: rows.map((row) => row.raw_definition_id).join(';'),
    source_identities: unique(rows.map((row) => `${row.source_path}:${row.source_line}`)).join(';'),
    dispositions: unique(rows.map((row) => row.final_function_disposition)).join(';'),
  };
});

const zeroCanonicalNames = new Set(canonicalRows.map((row) => row.canonical_symbol));
const interfaces = new Map();
const canonicalBySymbol = new Map();
for (const row of canonicalRows) canonicalBySymbol.set(row.canonical_symbol, [...(canonicalBySymbol.get(row.canonical_symbol) || []), row]);
const callResolutionRows = [];
for (const call of rawCalls) {
  const canonicalTargets = canonicalBySymbol.get(call.callee_spelling) || [];
  if (canonicalTargets.length) {
    callResolutionRows.push({
      raw_candidate_id: call.candidate_id,
      callee_spelling: call.callee_spelling,
      canonical_resolution: 'canonical-zero-definition-after-macro-normalization',
      canonical_group_keys: unique(canonicalTargets.map((target) => target.canonical_group_key)).join(';'),
      canonical_raw_definition_ids: canonicalTargets.map((target) => target.raw_definition_id).join(';'),
      boundary: 'Raw T295 first-degree record is preserved; source-defined macro-name normalization proves this callee is zero degree.',
    });
    continue;
  }
  callResolutionRows.push({
    raw_candidate_id: call.candidate_id,
    callee_spelling: call.callee_spelling,
    canonical_resolution: 'remains-first-degree-external-interface',
    canonical_group_keys: '',
    canonical_raw_definition_ids: '',
    boundary: 'No canonical zero-degree function name matches this direct callee spelling.',
  });
  interfaces.set(call.callee_spelling, [...(interfaces.get(call.callee_spelling) || []), call]);
}
const interfaceRows = [...interfaces.entries()].sort(([left], [right]) => left.localeCompare(right)).map(([symbol, calls], index) => ({
  canonical_interface_id: `MVDM-FIRST-INTERFACE-${String(index + 1).padStart(6, '0')}`,
  callee_spelling: symbol,
  raw_call_site_count: String(calls.length),
  raw_candidate_ids: calls.map((call) => call.candidate_id).join(';'),
  caller_definition_count: String(unique(calls.map((call) => call.caller_definition_id)).length),
  caller_source_identities: unique(calls.map((call) => `${call.caller_source_path}:${call.caller_source_line}`)).join(';'),
  initial_first_degree_statuses: unique(calls.map((call) => call.initial_first_degree_status)).join(';'),
  prior_semantic_dispositions: unique(calls.map((call) => call.prior_semantic_disposition)).join(';'),
  boundary: 'Direct external interface spelling only; T298 does not inspect its definition or select an adapter.',
}));
if (callResolutionRows.length !== rawCalls.length || new Set(callResolutionRows.map((row) => row.raw_candidate_id)).size !== rawCalls.length) {
  throw new Error('Canonical first-degree resolution projection lost a raw call site');
}

writeTsv(path.join(operationsRoot, 'mvdm-host-zero-degree-canonical-definition-ledger.tsv'), canonicalRows, [
  'canonical_definition_id', 'raw_definition_id', 'source_path', 'source_sha256', 'source_line', 'linkage', 'raw_symbol', 'canonical_symbol', 'name_resolution', 'macro_definition_spelling', 'source_line_evidence', 'canonical_group_key', 'final_function_disposition', 'leaf_status',
]);
writeTsv(path.join(operationsRoot, 'mvdm-host-zero-degree-canonical-group-ledger.tsv'), summaryRows, [
  'canonical_group_id', 'canonical_group_key', 'canonical_symbol', 'grouping_scope', 'group_kind', 'raw_identity_count', 'macro_generated_identity_count', 'raw_definition_ids', 'source_identities', 'dispositions',
]);
writeTsv(path.join(operationsRoot, 'mvdm-host-first-degree-canonical-interface-ledger.tsv'), interfaceRows, [
  'canonical_interface_id', 'callee_spelling', 'raw_call_site_count', 'raw_candidate_ids', 'caller_definition_count', 'caller_source_identities', 'initial_first_degree_statuses', 'prior_semantic_dispositions', 'boundary',
]);
writeTsv(path.join(operationsRoot, 'mvdm-host-first-degree-canonical-resolution-ledger.tsv'), callResolutionRows, [
  'raw_candidate_id', 'callee_spelling', 'canonical_resolution', 'canonical_group_keys', 'canonical_raw_definition_ids', 'boundary',
]);

console.log(`raw zero identities=${rawDefinitions.length}; canonical identity rows=${canonicalRows.length}; canonical groups=${summaryRows.length}; macro-generated=${canonicalRows.filter((row) => row.name_resolution.startsWith('source-local')).length}; raw first call sites=${rawCalls.length}; canonical first interfaces=${interfaceRows.length}; reclassified-zero-calls=${callResolutionRows.filter((row) => row.canonical_resolution.startsWith('canonical-zero')).length}`);
