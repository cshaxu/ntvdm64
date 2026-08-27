import fs from 'node:fs';
import path from 'node:path';

const repositoryRoot = process.argv[2] || process.cwd();
const operationsRoot = path.join(repositoryRoot, 'docs', 'etc', 'operations');

function parseTsv(text) {
  const lines = text.replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean);
  return lines.map((line) => {
    const fields = [];
    let field = '';
    let quoted = false;
    for (let index = 0; index < line.length; index += 1) {
      const ch = line[index];
      if (ch === '"') {
        if (quoted && line[index + 1] === '"') { field += '"'; index += 1; }
        else { quoted = !quoted; }
      } else if (ch === '\t' && !quoted) { fields.push(field); field = ''; }
      else { field += ch; }
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

function maskCText(input) {
  const chars = [...input];
  let state = 'code';
  let escape = false;
  for (let index = 0; index < chars.length; index += 1) {
    const ch = chars[index];
    const next = chars[index + 1] || '';
    if (state === 'code') {
      if (ch === '/' && next === '/') { chars[index] = ' '; chars[index + 1] = ' '; index += 1; state = 'line'; }
      else if (ch === '/' && next === '*') { chars[index] = ' '; chars[index + 1] = ' '; index += 1; state = 'block'; }
      else if (ch === '"') { chars[index] = ' '; state = 'string'; escape = false; }
      else if (ch === "'") { chars[index] = ' '; state = 'character'; escape = false; }
    } else if (state === 'line') {
      if (ch === '\n') { state = 'code'; } else if (ch !== '\r') { chars[index] = ' '; }
    } else if (state === 'block') {
      if (ch === '*' && next === '/') { chars[index] = ' '; chars[index + 1] = ' '; index += 1; state = 'code'; }
      else if (ch !== '\r' && ch !== '\n') { chars[index] = ' '; }
    } else {
      if (ch === '\n') { state = 'code'; }
      else {
        if (ch !== '\r') { chars[index] = ' '; }
        if (escape) { escape = false; }
        else if (ch === '\\') { escape = true; }
        else if ((state === 'string' && ch === '"') || (state === 'character' && ch === "'")) { state = 'code'; }
      }
    }
  }
  const lines = chars.join('').split(/(?<=\n)/);
  let continuation = false;
  return lines.map((line) => {
    const directive = continuation || /^\s*#/.test(line);
    continuation = directive && /\\\s*(?:\r?\n)?$/.test(line);
    return directive ? line.replace(/[^\r\n]/g, ' ') : line;
  }).join('');
}

function lineStarts(text) {
  const starts = [0];
  for (let index = 0; index < text.length; index += 1) if (text[index] === '\n' && index + 1 < text.length) starts.push(index + 1);
  return starts;
}

function lineAt(starts, offset) {
  let low = 0; let high = starts.length - 1;
  while (low <= high) {
    const middle = (low + high) >> 1;
    if (starts[middle] <= offset) low = middle + 1; else high = middle - 1;
  }
  return high + 1;
}

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

function braceDepthAt(text, offset) {
  let depth = 0;
  for (let index = 0; index < offset; index += 1) {
    if (text[index] === '{') depth += 1;
    else if (text[index] === '}') depth = Math.max(0, depth - 1);
  }
  return depth;
}

const fileLedger = rowsFromTsv(path.join(operationsRoot, 'mvdm-file-recovery-ledger.tsv'));
const headerRows = rowsFromTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-invocation-symbol-ledger.tsv'));
const sourceRows = fileLedger.filter((row) => row.file_kind === 'source' && /\.(c|cc|cpp)$/i.test(row.target_path));
const selectedHeaders = fileLedger.filter((row) => row.file_kind === 'declaration');
const sourceDisposition = new Map(sourceRows.map((row) => [row.file_id, row.expected_final_disposition]));
const macroNames = new Set();

for (const row of [...sourceRows, ...selectedHeaders]) {
  const sourcePath = row.selected_source_path;
  if (!fs.existsSync(sourcePath)) throw new Error(`Missing provenance-selected source: ${sourcePath}`);
  const source = fs.readFileSync(sourcePath, 'utf8');
  for (const match of source.matchAll(/^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\b/gm)) macroNames.add(match[1]);
}
const abiRoot = path.join(repositoryRoot, 'src', 'mvdm-platform-abi');
if (fs.existsSync(abiRoot)) {
  const stack = [abiRoot];
  while (stack.length) {
    const entry = stack.pop();
    const stat = fs.statSync(entry);
    if (stat.isDirectory()) for (const child of fs.readdirSync(entry)) stack.push(path.join(entry, child));
    else if (/\.(h|inc)$/i.test(entry)) {
      const source = fs.readFileSync(entry, 'utf8');
      for (const match of source.matchAll(/^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\b/gm)) macroNames.add(match[1]);
    }
  }
}

const keyword = new Set(['if', 'for', 'while', 'switch', 'return', 'sizeof', 'case', 'do', 'typedef', 'defined', '__asm', 'asm', 'catch', '__try']);
const sourceCache = new Map();
const localDefinitions = new Set();
const localDeclarations = new Set();
for (const row of sourceRows) {
  const original = fs.readFileSync(row.selected_source_path, 'utf8');
  const masked = maskCText(original);
  sourceCache.set(row.target_path, masked);
  // SoftPC generated C carries declarations in its historical IFN/IPT macro
  // grammar rather than normal C prototypes.  They are selected MVDM internal
  // declarations, not unresolved external calls.
  for (const declaration of masked.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s+(?:IFN\d*|IPT\d*|IPF\d*)\s*\(/g)) {
    localDeclarations.add(declaration[1]);
  }
  for (const match of masked.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) {
    const name = match[1];
    if (keyword.has(name.toLowerCase())) continue;
    const open = masked.indexOf('(', match.index + match[0].length - 1);
    const close = closeParen(masked, open);
    if (close < 0 || macroNames.has(name)) continue;
    const next = nextCode(masked, close + 1);
    const depth = braceDepthAt(masked, match.index);
    if (next === '{') localDefinitions.add(name);
    else if (depth === 0) localDeclarations.add(name);
  }
}

const sites = [];
for (const row of sourceRows) {
  const masked = sourceCache.get(row.target_path);
  const lines = lineStarts(masked);
  for (const match of masked.matchAll(/\b([A-Za-z_][A-Za-z0-9_]*)\s*\(/g)) {
    const symbol = match[1];
    if (keyword.has(symbol.toLowerCase())) continue;
    const open = masked.indexOf('(', match.index + match[0].length - 1);
    const close = closeParen(masked, open);
    const next = close >= 0 ? nextCode(masked, close + 1) : '';
    const depth = braceDepthAt(masked, match.index);
    let syntaxClass;
    if (close < 0) syntaxClass = 'unbalanced-source-expression';
    else if (macroNames.has(symbol)) syntaxClass = 'function-like-macro-use';
    else if (next === '{') syntaxClass = 'function-definition';
    else if (depth === 0) syntaxClass = 'top-level-declaration-or-initializer';
    else if (localDefinitions.has(symbol) || localDeclarations.has(symbol)) syntaxClass = 'selected-mvdm-direct-call';
    else syntaxClass = 'external-or-unresolved-direct-call';
    const sourceLine = lineAt(lines, match.index);
    sites.push({
      site_id: `MVDM-EGRESS-SITE-${String(sites.length + 1).padStart(6, '0')}`,
      caller_file_id: row.file_id,
      caller_path: row.target_path,
      caller_package: row.package_root,
      caller_expected_disposition: sourceDisposition.get(row.file_id) || 'unclassified',
      source_line: String(sourceLine),
      symbol,
      syntax_class: syntaxClass,
      source_reachability: 'source-present; conditional-build-resolution-pending',
      evidence: `${row.target_path}:${sourceLine}`,
    });
  }
}

const upstream = new Map(headerRows.map((row) => [row.symbol, row]));
const publicOrCrtSymbols = new Set([
  'BitBlt', 'CloseHandle', 'CreateDirectoryW', 'CreateFileW', 'CreateProcessW',
  'DeleteFileW', 'EndDoc', 'EnterCriticalSection', 'EnumObjects',
  'FindFirstFileW', 'FindNextFileW', 'GetComputerNameW', 'GetDiskFreeSpaceW',
  'GetDriveTypeW', 'GetEnvironmentVariableA', 'GetFileAttributesW',
  'GetHandleInformation', 'GetSystemDirectoryW', 'GetTempFileNameW',
  'GetTempPathW', 'GetVolumeInformationW', 'GetWindowsDirectoryW', 'GetWindow',
  'HeapAlloc', 'LeaveCriticalSection', 'LocalAlloc', 'MoveFileExW',
  'RemoveDirectoryW', 'RemoveFontResourceW', 'SearchPathW',
  'SetEnvironmentVariableA', 'SetFileAttributesW', 'TlsSetValue', 'ToAscii',
  'abs', 'exit', 'free', 'memcpy', 'sprintf', 'strlen', 'swprintf', 'wcslen',
  'wcsncpy', 'SendDlgItemMessage',
]);
const macroOnlySymbols = new Set([
  'ARGUMENT_PRESENT', 'CONTAINING_RECORD', 'CSR_MAKE_API_NUMBER', 'DBGPRINT',
  'DIAGNOSTIC_ENTRY', 'DUMPCCB', 'FIELD_OFFSET', 'IS_POINTER', 'RANGE_F',
  'else', 'offsetof', 'pfnComPropSheet', 'lpAcsLan', 'lpDlcCallDriver',
  'lpNtAcsLan', 'DumpParms',
]);
const selectedMvdmSymbols = new Set([
  'AsyncDispositionRoutine', 'FastWOWCallbackCall', 'GdpOffsetFromName',
  'GetFaxDataMapName', 'GetFastBopEntryAddress', 'GlobalAllocLock16',
  'GlobalUnlockFree16', 'IsW32WorkerException', 'MoveMemRoutine',
  'VdmSetPhysRecStructs', 'W32SetExceptionContext', 'WK32ICallProc32MakeCall',
  'host_config_init',
]);

function finalFirstHop(symbol, provisional, runtimeSites) {
  if (runtimeSites.length === 0) return ['not-host-runtime-source-only', 'none', 'caller files are selected as tool-only, guest-only, firmware-only or not-host-runtime'];
  if (macroOnlySymbols.has(symbol)) return ['macro-or-function-pointer-contract', 'selected MVDM declaration/context', 'source expression is a macro or function-pointer invocation, not a new package import'];
  if (publicOrCrtSymbols.has(symbol) || provisional === 'public-api-or-crt-binding-candidate') return ['public-win32-or-crt-binding', 'adapter-mvdm-host-out/win32', 'public API/CRT contract; same-shaped binding is sufficient unless its ABI audit records otherwise'];
  if (symbol === 'DbgPrint') return ['kernel-debug-output-contract', 'adapter-mvdm-host-out/win32', 'kernel debug-print source contract; bind to session-scoped modern diagnostic output, never kernel debug transport'];
  if (selectedMvdmSymbols.has(symbol) || /^(host_|com_|fdisk_|io_|ica_|dma_|get_page_seg$|get_no_phys_pages$|get_base_address$|read_descriptor$)/.test(symbol)) return ['selected-mvdm-internal-or-adapter-contract', 'selected MVDM owner package / adapter-mvdm-host-out', 'same selected MVDM package or its admitted same-shaped adapter contract'];
  if (/^(Csr|BaseSrv|Basep)/.test(symbol)) return ['csr-basesrv-private-host-boundary', 'adapter-mvdm-host-out/monitor', 'BaseClient/BaseSrv/CSR product contract; source may be cited, but no CSR transport/server is linked'];
  if (/^(Vdm|NtVdm)/.test(symbol)) return ['kernel-vdm-monitor-contract', 'adapter-mvdm-host-out/monitor + adapter-bochs', 'kernel VDM contract; translate to non-invasive monitor/Bochs mechanics or retain source-shaped unavailable result'];
  if (/^(Nt|Rtl|Ldr)/.test(symbol)) return ['native-nt-or-loader-contract', 'adapter-mvdm-host-out/win32', 'same-shaped native/RTL/loader facade; public Win32 is preferred where it preserves the reached contract'];
  if (/^(Netp|Rx|XsNet)/.test(symbol)) return ['netapi-redirector-private-contract', 'adapter-mvdm-host-out/redir', 'NetAPI/Rx private contract; stage-two package expansion owns exact source and modern route'];
  if (/^(VDMConsole|ShowConsole|SetConsole|GetConsole|WriteConsole|ReadConsole|ConsoleMenu|SetLastConsole)/.test(symbol)) return ['private-console-contract', 'adapter-mvdm-host-out/win32', 'historical console VDM private operation; stage-two Console package expansion determines facade or explicit unavailable outcome'];
  if (/^(RegisterWow|UserRegisterWow|GetETM|GetGlyphOutlineWow|SetCursorContents|MBToWCS|WOWShell|DragQuery|DialogBoxIndirect)/.test(symbol)) return ['wow-private-host-contract', 'adapter-mvdm-host-out/wow', 'WOW private host operation; stage-two WOW package expansion determines exact retained source body and binding'];
  if (symbol === 'EnableHardwareInput') return ['vdd-or-input-product-contract', 'adapter-mvdm-host-out/vdd', 'historical VDD/input product contract'];
  if (symbol === 'GetRandomRgn') return ['private-gdi-contract', 'adapter-mvdm-host-out/wow', 'WOW private GDI contract'];
  if (provisional === 'selected-mvdm-definition-resolution-required') return ['selected-mvdm-definition-resolution-required', 'selected MVDM owner package', 'selected declaration exists; source/package pass must identify exact definition'];
  if (provisional === 'non-mvdm-owner-resolution-required') return ['non-mvdm-opennt-package-boundary', 'stage-two candidate package owner', 'non-public declaration carrier exists; package BFS must establish exact provider'];
  return ['unresolved-stage-one-symbol', 'unassigned', 'requires manual first-hop disposition before stage-one closure'];
}
const grouped = new Map();
for (const site of sites) {
  if (site.syntax_class !== 'external-or-unresolved-direct-call') continue;
  if (!grouped.has(site.symbol)) grouped.set(site.symbol, []);
  grouped.get(site.symbol).push(site);
}
const symbols = [];
for (const [symbol, directSites] of [...grouped.entries()].sort(([a], [b]) => a.localeCompare(b))) {
  const header = upstream.get(symbol) || {};
  const roots = header.upstream_package_roots || '';
  const runtimeSites = directSites.filter((site) => ['direct', 'binding-only', 'adapter-backed', 'overlay-required'].includes(site.caller_expected_disposition));
  let disposition;
  if (runtimeSites.length === 0) disposition = 'not-host-runtime-source-only';
  else if (!roots) disposition = 'unresolved-direct-call';
  // A declaration can be co-located in historical private trees even when its
  // authoritative reachable ABI is public SDK/CRT.  The presence of such a
  // public declaration wins at first hop; do not create a fake dependency on
  // every historical header that repeats a public function prototype.
  else if (/(^|;)(public\/sdk\/inc|base\/crts\/(crtw32|libw32))(;|$)/.test(roots)) disposition = 'public-api-or-crt-binding-candidate';
  else if (header.first_hop_state?.startsWith('selected-MVDM')) disposition = 'selected-mvdm-definition-resolution-required';
  else disposition = 'non-mvdm-owner-resolution-required';
  const [finalDisposition, finalOwner, finalBasis] = finalFirstHop(symbol, disposition, runtimeSites);
  symbols.push({
    symbol,
    direct_call_site_count: String(directSites.length),
    runtime_direct_call_site_count: String(runtimeSites.length),
    caller_paths: [...new Set(directSites.map((site) => site.caller_path))].sort().join(';'),
    caller_packages: [...new Set(directSites.map((site) => site.caller_package))].sort().join(';'),
    caller_dispositions: [...new Set(directSites.map((site) => site.caller_expected_disposition))].sort().join(';'),
    provisional_first_hop: disposition,
    stage1_final_disposition: finalDisposition,
    stage1_owner: finalOwner,
    stage1_basis: finalBasis,
    upstream_header_state: header.first_hop_state || 'no-upstream-header-candidate',
    upstream_package_roots: roots,
    upstream_declaration_candidates: header.upstream_declaration_candidates || '',
    next_stage: 'resolve exact declaration/import definition and final owner; do not expand every header co-location',
  });
}

const siteColumns = ['site_id', 'caller_file_id', 'caller_path', 'caller_package', 'caller_expected_disposition', 'source_line', 'symbol', 'syntax_class', 'source_reachability', 'evidence'];
const symbolColumns = ['symbol', 'direct_call_site_count', 'runtime_direct_call_site_count', 'caller_paths', 'caller_packages', 'caller_dispositions', 'provisional_first_hop', 'stage1_final_disposition', 'stage1_owner', 'stage1_basis', 'upstream_header_state', 'upstream_package_roots', 'upstream_declaration_candidates', 'next_stage'];
writeTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-semantic-site-ledger.tsv'), sites, siteColumns);
writeTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-semantic-symbol-ledger.tsv'), symbols, symbolColumns);
writeTsv(path.join(operationsRoot, 'opennt-non-mvdm-stage1-true-egress-ledger.tsv'), symbols.filter((row) => !['not-host-runtime-source-only', 'selected-mvdm-internal-or-adapter-contract', 'macro-or-function-pointer-contract', 'selected-mvdm-definition-resolution-required'].includes(row.stage1_final_disposition)), symbolColumns);

const countBy = (rows, key) => [...rows.reduce((counts, row) => counts.set(row[key], (counts.get(row[key]) || 0) + 1), new Map()).entries()].sort(([a], [b]) => a.localeCompare(b));
console.log(`semantic sites: ${sites.length}; ${countBy(sites, 'syntax_class').map(([key, value]) => `${key}=${value}`).join('; ')}`);
console.log(`direct-call symbols: ${symbols.length}; ${countBy(symbols, 'provisional_first_hop').map(([key, value]) => `${key}=${value}`).join('; ')}`);
console.log(`final first-hop dispositions: ${countBy(symbols, 'stage1_final_disposition').map(([key, value]) => `${key}=${value}`).join('; ')}`);
