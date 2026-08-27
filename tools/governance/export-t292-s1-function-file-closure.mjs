import crypto from 'node:crypto';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';

const repositoryRoot = process.argv[2] || process.cwd();
const operationsRoot = path.join(repositoryRoot, 'docs', 'etc', 'operations');
const sourceRoots = [
  'O:\\repos.external\\OpenNT',
  'O:\\repos.external\\OpenNT-4.5',
].filter((root) => fs.existsSync(root));

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

function candidateSourceFiles(root, symbols) {
  // Let ripgrep do the broad text prefilter in native code.  The JavaScript
  // parser below still masks and classifies every returned source file, so a
  // textual candidate can never become a definition merely by matching here.
  const patternFile = path.join(process.env.TEMP || process.env.TMP || '.', `t292-symbols-${process.pid}.regex`);
  fs.writeFileSync(patternFile, symbols.map((symbol) => `\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*\\(`).join('\n'), 'utf8');
  try {
    const output = execFileSync('rg.exe', ['-l', '-P', '--no-messages', '-f', patternFile, '-g', '*.c', '-g', '*.cc', '-g', '*.cpp', '-g', '*.cxx', root], {
      encoding: 'utf8', maxBuffer: 64 * 1024 * 1024,
    });
    return output.split(/\r?\n/).filter(Boolean);
  } catch (error) {
    // ripgrep uses status 1 for a valid no-match result.
    if (error.status === 1) return [];
    throw error;
  } finally {
    fs.unlinkSync(patternFile);
  }
}

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
function nextCode(text, offset) {
  for (let index = offset; index < text.length; index += 1) if (!/\s/.test(text[index])) return text[index];
  return '';
}
function externalPath(fileName) {
  const root = [...sourceRoots].sort((a, b) => b.length - a.length).find((candidate) => {
    const normalizedFile = fileName.toLowerCase();
    const normalizedRoot = candidate.toLowerCase();
    return normalizedFile === normalizedRoot || normalizedFile.startsWith(`${normalizedRoot}\\`);
  });
  return { root: root || '', relative: root ? path.relative(root, fileName).replaceAll('\\', '/') : fileName };
}
function sourcePackage(relative) {
  const parts = relative.split('/');
  if (parts[0] === 'base' && parts[1] === 'win32') return parts.slice(0, 3).join('/');
  if (parts[0] === 'base' && parts[1] === 'ntdll') return parts.slice(0, 2).join('/');
  if (parts[0] === 'private' && parts[1] === 'ntos') return parts.slice(0, 3).join('/');
  return parts.slice(0, Math.min(2, parts.length - 1)).join('/');
}
function sha256(fileName) { return crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex'); }

const symbols = rowsFromTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-semantic-symbol-ledger.tsv'));
const fileLedger = rowsFromTsv(path.join(operationsRoot, 'mvdm-file-recovery-ledger.tsv'));
const selectedDefinitionBySymbol = new Map();
for (const row of fileLedger.filter((row) => row.file_kind === 'source')) {
  if (!fs.existsSync(row.selected_source_path)) continue;
  const masked = maskCText(fs.readFileSync(row.selected_source_path, 'utf8'));
  for (const match of masked.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) {
    const close = closeParen(masked, masked.indexOf('(', match.index + match[0].length - 1));
    if (close < 0 || nextCode(masked, close + 1) !== '{') continue;
    const name = match[1];
    if (!selectedDefinitionBySymbol.has(name)) selectedDefinitionBySymbol.set(name, []);
    selectedDefinitionBySymbol.get(name).push({
      target_path: row.target_path, file_id: row.file_id, source_path: row.selected_source_path,
      source_line: lineAt(masked, match.index), package_root: row.package_root,
    });
  }
}

const targetSymbols = new Set(symbols.map((row) => row.symbol));
const alternation = [...targetSymbols].sort((a, b) => b.length - a.length).map((symbol) => symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')).join('|');
const tokenPattern = new RegExp(`\\b(?:${alternation})\\s*\\(`, 'g');
const externalDefinitions = new Map();
let scannedFiles = 0; let candidateFiles = 0;
for (const root of sourceRoots) {
  const candidates = candidateSourceFiles(root, [...targetSymbols]);
  scannedFiles += candidates.length;
  for (const fileName of candidates) {
    const raw = fs.readFileSync(fileName, 'utf8');
    tokenPattern.lastIndex = 0;
    if (!tokenPattern.test(raw)) continue;
    candidateFiles += 1;
    const masked = maskCText(raw);
    const { relative } = externalPath(fileName);
    for (const match of masked.matchAll(tokenPattern)) {
      const name = match[0].match(/[A-Za-z_][A-Za-z0-9_]*/)?.[0] || '';
      const open = masked.indexOf('(', match.index + match[0].length - 1);
      const close = closeParen(masked, open);
      if (close < 0 || nextCode(masked, close + 1) !== '{') continue;
      if (!externalDefinitions.has(name)) externalDefinitions.set(name, []);
      externalDefinitions.get(name).push({
        source_path: fileName, relative_path: relative, source_line: lineAt(masked, match.index),
        package_root: sourcePackage(relative), source_sha256: sha256(fileName),
      });
    }
  }
}

function dedupe(records, key) {
  const map = new Map(); for (const record of records) map.set(key(record), record); return [...map.values()];
}
function dispositionFor(row, selected, external) {
  const category = row.stage1_final_disposition;
  if (category === 'not-host-runtime-source-only') return ['source-excluded-by-selected-file-disposition', 'not applicable', 'all callers are tool/guest/firmware/not-host-runtime; no runtime first-hop file'];
  if (category === 'macro-or-function-pointer-contract') return ['non-function-expression-contract', 'not applicable', 'macro/function-pointer call site; concrete target belongs to its original context, not a new first-hop function'];
  if (category === 'public-win32-or-crt-binding') return ['modern-system-binding', 'external SDK/CRT', 'public contract terminates at a modern public Win32/CRT binding; OpenNT declaration evidence is retained but no OpenNT provider source is imported'];
  if (category === 'native-nt-or-loader-contract') return ['private-nt-contract-boundary', 'adapter-mvdm-host-out/win32', 'historical native/loader contract; external source candidates are evidence only and modern same-shaped facade or explicit unavailable result owns the binding'];
  if (category === 'csr-basesrv-private-host-boundary') return ['csr-product-boundary', 'adapter-mvdm-host-out/monitor', 'CSR/BaseSrv contract is a first-hop hard boundary; source candidates may be reused only under a separately admitted finite package slice'];
  if (category === 'kernel-vdm-monitor-contract') return ['kernel-vdm-contract-boundary', 'adapter-mvdm-host-out/monitor + adapter-bochs', 'kernel VDM contract ends at non-invasive monitor/Bochs mechanics; no kernel source is linked'];
  if (category === 'netapi-redirector-private-contract') return ['redirector-private-contract-boundary', 'adapter-mvdm-host-out/redir', 'NetAPI/Rx contract requires exact source package and public-modern/failure disposition during BFS'];
  if (category === 'private-console-contract') return ['console-private-contract-boundary', 'adapter-mvdm-host-out/win32', 'historical console contract requires package-specific modern route or source-shaped unavailable result'];
  if (category === 'wow-private-host-contract') return ['wow-private-contract-boundary', 'adapter-mvdm-host-out/wow', 'WOW product contract requires its own exact package/function review'];
  if (category === 'vdd-or-input-product-contract') return ['vdd-input-contract-boundary', 'adapter-mvdm-host-out/vdd', 'VDD/input product contract needs a separately admitted hardware/input boundary'];
  if (category === 'private-gdi-contract') return ['private-gdi-contract-boundary', 'adapter-mvdm-host-out/wow', 'private GDI contract stops before USER/GDI product internals'];
  if (category === 'selected-mvdm-internal-or-adapter-contract') return [selected.length ? 'selected-mvdm-definition' : 'selected-mvdm-adapter-contract', 'selected MVDM owner / adapter-mvdm-host-out', selected.length ? 'definition is inside the selected MVDM union' : 'same-shaped selected MVDM/adapter contract has no source definition in selected C/C++ rows'];
  if (category === 'non-mvdm-opennt-package-boundary') return [external.length ? 'named-non-mvdm-source-definition' : 'named-non-mvdm-declaration-boundary', 'candidate non-MVDM package', external.length ? 'exact external source definition candidate recorded; it is the Stage-2 BFS seed' : 'non-MVDM declaration is exact evidence; no C/C++ provider definition found in the two OpenNT trees'];
  return ['stage-one-resolution-error', 'unassigned', 'unexpected semantic disposition'];
}

const functionRows = [];
const fileRows = new Map();
for (const row of symbols) {
  const selected = dedupe(selectedDefinitionBySymbol.get(row.symbol) || [], (item) => `${item.file_id}:${item.source_line}`);
  const external = dedupe(externalDefinitions.get(row.symbol) || [], (item) => `${item.relative_path}:${item.source_line}:${item.source_sha256}`);
  const [functionDisposition, finalOwner, basis] = dispositionFor(row, selected, external);
  const selectedText = selected.map((item) => `${item.target_path}:${item.source_line}`).join(';');
  const externalText = external.map((item) => `${item.relative_path}:${item.source_line}@${item.source_sha256}`).join(';');
  functionRows.push({
    function_id: `MVDM-FIRST-HOP-FUNCTION-${String(functionRows.length + 1).padStart(4, '0')}`,
    symbol: row.symbol, stage1_semantic_disposition: row.stage1_final_disposition,
    function_disposition: functionDisposition, final_owner: finalOwner,
    runtime_direct_call_site_count: row.runtime_direct_call_site_count,
    caller_paths: row.caller_paths, selected_definition_candidates: selectedText,
    external_definition_candidates: externalText, external_package_roots: [...new Set(external.map((item) => item.package_root))].sort().join(';'),
    source_definition_status: selected.length || external.length ? 'source-definition-candidates-recorded' : 'no-C-C++-provider-definition-required-or-found',
    first_hop_basis: basis, next_stage: functionDisposition === 'named-non-mvdm-source-definition' ? 'expand this exact source file/package at BFS depth two' : 'terminal for one-degree audit; later work follows named owner package',
  });
  // Public Win32/CRT symbols terminate at the system ABI.  Historical trees
  // contain many unrelated same-named implementations, which must not become
  // fake one-degree package edges.  Keep their candidate evidence per function
  // but emit a file edge only for a non-public/private owner boundary.
  if (!['modern-system-binding', 'source-excluded-by-selected-file-disposition', 'non-function-expression-contract'].includes(functionDisposition)) for (const item of external) {
    const key = `${item.relative_path}@${item.source_sha256}`;
    if (!fileRows.has(key)) fileRows.set(key, { ...item, symbols: [], caller_paths: new Set(), function_dispositions: new Set(), owners: new Set() });
    const file = fileRows.get(key); file.symbols.push(row.symbol); row.caller_paths.split(';').filter(Boolean).forEach((value) => file.caller_paths.add(value)); file.function_dispositions.add(functionDisposition); file.owners.add(finalOwner);
  }
}
const fileOutput = [...fileRows.values()].sort((a, b) => a.relative_path.localeCompare(b.relative_path) || a.source_line - b.source_line).map((row, index) => ({
  file_id: `MVDM-FIRST-HOP-FILE-${String(index + 1).padStart(4, '0')}`,
  original_source_path: row.relative_path, original_source_sha256: row.source_sha256, original_package_root: row.package_root,
  reached_symbols: [...new Set(row.symbols)].sort().join(';'), source_definition_lines: String(row.source_line),
  caller_paths: [...row.caller_paths].sort().join(';'), function_dispositions: [...row.function_dispositions].sort().join(';'),
  final_owner: [...row.owners].sort().join(';'), final_file_disposition: 'BFS-depth-two-seed-or-hard-boundary-evidence',
  first_hop_rule: 'recorded exact provider candidate; not imported, linked or enabled by this audit',
}));

const functionColumns = ['function_id', 'symbol', 'stage1_semantic_disposition', 'function_disposition', 'final_owner', 'runtime_direct_call_site_count', 'caller_paths', 'selected_definition_candidates', 'external_definition_candidates', 'external_package_roots', 'source_definition_status', 'first_hop_basis', 'next_stage'];
const fileColumns = ['file_id', 'original_source_path', 'original_source_sha256', 'original_package_root', 'reached_symbols', 'source_definition_lines', 'caller_paths', 'function_dispositions', 'final_owner', 'final_file_disposition', 'first_hop_rule'];
writeTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-function-closure-ledger.tsv'), functionRows, functionColumns);
writeTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-file-closure-ledger.tsv'), fileOutput, fileColumns);

const counts = (rows, key) => [...rows.reduce((map, row) => map.set(row[key], (map.get(row[key]) || 0) + 1), new Map()).entries()].sort(([a], [b]) => a.localeCompare(b));
console.log(`scanned external C/C++ files: ${scannedFiles}; candidate files: ${candidateFiles}`);
console.log(`function rows: ${functionRows.length}; ${counts(functionRows, 'function_disposition').map(([key, value]) => `${key}=${value}`).join('; ')}`);
console.log(`external source-file rows: ${fileOutput.length}`);
