/*
 * Source-first inventory for M0 T310 S8 P4.
 *
 * This intentionally does not infer a safety disposition from a compiler
 * warning. It emits every static ABI candidate so that review must read the
 * declaration, assignment and caller before selecting mirror, overlay,
 * adapter, or unavailable treatment.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const output = path.resolve(root, process.argv[3] ?? 'docs/etc/operations/m0-t310-s8-p4-whole-tree-static-abi-inventory.tsv');
const clusterOutput = path.resolve(root, process.argv[4] ?? 'docs/etc/operations/m0-t310-s8-p4-whole-tree-static-abi-cluster-ledger.tsv');
const extensions = new Set(['.c', '.h', '.inc', '.asm', '.s']);
const sourceRoots = ['src/mvdm-host', 'src/mvdm-support'];
const rows = [];

function collect(directory) {
  const result = [];
  for (const entry of fs.readdirSync(directory, { withFileTypes: true })) {
    const full = path.join(directory, entry.name);
    if (entry.isDirectory()) result.push(...collect(full));
    else if (extensions.has(path.extname(entry.name).toLowerCase())) result.push(full);
  }
  return result;
}

function clean(text) {
  return text.replaceAll('\t', ' ').replaceAll('\r', '').replaceAll('\n', ' ').replace(/\s+/g, ' ').trim();
}

function ownerFor(relative) {
  const pieces = relative.split('/');
  return pieces[1] === 'mvdm-host' ? (pieces[2] ?? 'mvdm-host-root') : 'mvdm-support';
}

function clusterFor(relative) {
  const pathName = relative.replace(/^src\/mvdm-host\//, '');
  const hostSourceClusters = new Map([
    ['nt_cprgs.c', 'SPC-CCPU-ACCESS-TABLE'], ['nt_cpu.c', 'SPC-CCPU-ACCESS-TABLE'], ['cpucstbs.c', 'SPC-CCPU-ACCESS-TABLE'],
    ['nt_mem.c', 'SPC-MEMORY-MAPPING-BINDINGS'], ['nt_sas.c', 'SPC-MEMORY-MAPPING-BINDINGS'], ['nt_emm.c', 'SPC-MEMORY-MAPPING-BINDINGS'], ['x86_emm.c', 'SPC-MEMORY-MAPPING-BINDINGS'], ['nt_umb.c', 'SPC-MEMORY-MAPPING-BINDINGS'],
    ['nt_eoi.c', 'SPC-HOST-SYSTEM-INTERRUPT-BINDINGS'], ['nt_timer.c', 'SPC-HOST-SYSTEM-INTERRUPT-BINDINGS'], ['nt_inthk.c', 'SPC-HOST-SYSTEM-INTERRUPT-BINDINGS'], ['nt_reset.c', 'SPC-HOST-SYSTEM-INTERRUPT-BINDINGS'],
    ['nt_ega.c', 'SPC-HOST-VIDEO-BINDINGS'], ['nt_vga.c', 'SPC-HOST-VIDEO-BINDINGS'], ['nt_cga.c', 'SPC-HOST-VIDEO-BINDINGS'], ['nt_graph.c', 'SPC-HOST-VIDEO-BINDINGS'], ['nt_aorc.c', 'SPC-HOST-VIDEO-BINDINGS'],
    ['nt_keycd.c', 'SPC-HOST-INPUT-BINDINGS'], ['nt_input.c', 'SPC-HOST-INPUT-BINDINGS'], ['nt_mouse.c', 'SPC-HOST-INPUT-BINDINGS'], ['nt_msscs.c', 'SPC-HOST-INPUT-BINDINGS'], ['nt_sound.c', 'SPC-HOST-INPUT-BINDINGS'],
    ['nt_fdisk.c', 'SPC-HOST-STORAGE-BINDINGS'], ['nt_rflop.c', 'SPC-HOST-STORAGE-BINDINGS'], ['nt_vflop.c', 'SPC-HOST-STORAGE-BINDINGS'],
    ['nt_com.c', 'SPC-HOST-COMMS-BINDINGS'], ['nt_lpt.c', 'SPC-HOST-COMMS-BINDINGS'], ['nt_wcom.c', 'SPC-HOST-COMMS-BINDINGS'],
    ['config.c', 'SPC-HOST-CONFIGURATION-BINDINGS'], ['stf_conf.c', 'SPC-HOST-CONFIGURATION-BINDINGS'], ['nt_pif.c', 'SPC-HOST-CONFIGURATION-BINDINGS'],
    ['nt_bop.c', 'SPC-HOST-BOP-BINDING'], ['nt_vdd.c', 'SPC-HOST-VDD-BINDING'], ['sim32.c', 'MVDM-SIM32-OWNER-PACKAGE']
  ]);
  const hostSourcePrefix = 'softpc.new/host/src/';
  if (pathName.startsWith(hostSourcePrefix)) {
    const byFile = hostSourceClusters.get(pathName.slice(hostSourcePrefix.length));
    if (byFile) return byFile;
  }
  const exact = [
    ['softpc.new/host/src/nt_cprgs.c', 'SPC-CCPU-ACCESS-TABLE'],
    ['softpc.new/host/inc/host_cpu.h', 'SPC-CCPU-ACCESS-TABLE'],
    ['softpc.new/base/inc/host_sas.h', 'SPC-SAS-MEMORY-CONTRACT'],
    ['softpc.new/base/inc/hunter.h', 'SPC-HUNTER-UNSELECTED'],
    ['softpc.new/base/cvidc/', 'SPC-CVIDC-GENERATED-DISPATCH'],
    ['softpc.new/base/ccpu386/', 'SPC-CCPU-EXECUTOR-DISPATCH'],
    ['softpc.new/base/video/', 'SPC-VIDEO-CONTROLLER-DISPATCH'],
    ['softpc.new/base/system/', 'SPC-SYSTEM-CONTROLLER-CALLBACKS'],
    ['softpc.new/base/keymouse/', 'SPC-INPUT-CONTROLLER-CALLBACKS'],
    ['softpc.new/base/disks/', 'SPC-STORAGE-CONTROLLER-CALLBACKS'],
    ['softpc.new/base/comms/', 'SPC-COMMS-CONTROLLER-CALLBACKS'],
    ['softpc.new/base/bios/', 'SPC-BIOS-FIRMWARE-BINDINGS'],
    ['softpc.new/base/support/', 'SPC-SUPPORT-CONTROLLER-CALLBACKS'],
    ['softpc.new/base/debug/', 'SPC-DEBUG-UNSELECTED'],
    ['softpc.new/host/', 'SPC-HOST-PLATFORM-BINDINGS'],
    ['softpc.new/', 'SPC-OTHER-MACHINE-SOURCE']
  ];
  for (const [prefix, cluster] of exact) if (pathName.startsWith(prefix)) return cluster;
  if (relative.startsWith('src/mvdm-support/')) return 'MVDM-SUPPORT-LIBRARY';
  const packageName = pathName.split('/')[0] ?? 'unknown';
  return `MVDM-${packageName.toUpperCase()}-OWNER-PACKAGE`;
}

function profileFor(relative, cluster) {
  if (cluster === 'SPC-HUNTER-UNSELECTED' || cluster === 'SPC-DEBUG-UNSELECTED') return 'unselected-debug-profile';
  if (relative.startsWith('src/mvdm-host/softpc.new/')) return 'selected-softpc-machine-source';
  if (relative.startsWith('src/mvdm-support/')) return 'support-library-source';
  return 'unselected-owner-package-source';
}

function isFunctionPointerDeclarationPrefix(prefix) {
  const value = prefix.trim().replace(/\s+/g, ' ');
  if (!value || /^#\s*define\b/i.test(value) || /\b(?:return|if|while|switch)\s*$/i.test(value)) return false;
  if (/[=,;{}]/.test(value)) return false;
  return /^(?:(?:typedef|extern|static|GLOBAL|LOCAL|IMPORT)\s+)*(?:(?:const|volatile|unsigned|signed|struct|union|enum)\s+)*[A-Za-z_][A-Za-z0-9_]*(?:\s+[A-Za-z_][A-Za-z0-9_]*)*\s*\**$/.test(value);
}

function add(relative, line, kind, symbol, excerpt) {
  rows.push({
    source_path: relative,
    owner_package: ownerFor(relative),
    review_cluster: clusterFor(relative),
    profile_class: profileFor(relative, clusterFor(relative)),
    source_line: line,
    candidate_kind: kind,
    symbol: symbol || '-',
    source_excerpt: clean(excerpt),
    review_requirement: 'read declaration, assignment/initializer, selected callers and original package context',
    x86_x64_disposition: 'unreviewed-source-context-required'
  });
}

for (const relativeRoot of sourceRoots) {
  const absoluteRoot = path.join(root, relativeRoot);
  for (const source of collect(absoluteRoot)) {
    const relative = path.relative(root, source).split(path.sep).join('/');
    const content = fs.readFileSync(source, 'utf8');
    const lines = content.split(/\r?\n/);
    for (let index = 0; index < lines.length; index += 1) {
      const line = lines[index];
      const lineNumber = index + 1;
      for (const match of line.matchAll(/\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]*\])?\s*\)\s*(?:\([^;{}]*\)|IFN\d+\()/g)) {
        const prefix = line.slice(0, match.index);
        if (isFunctionPointerDeclarationPrefix(prefix)) {
          add(relative, lineNumber, 'function-pointer-declaration-or-slot', match[1], line);
        }
      }
      for (const match of line.matchAll(/\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)\s*\(/g)) {
        add(relative, lineNumber, 'function-pointer-indirect-call', match[1], line);
      }
      if (/\(\s*(?:void|VOID|boolean|IBOOL|int|UINT|ULONG|DWORD|LONG|IU\d+|IS\d+|PVOID|LPVOID|HANDLE|[A-Za-z_][A-Za-z0-9_]*\s*\*)\s*\(\s*\*\s*\)\s*\)/.test(line)) {
        add(relative, lineNumber, 'function-pointer-cast', '-', line);
      }
      if (/\(\s*(?:ULONG|DWORD|UINT|LONG|IU32|IS32|USHORT|WORD)\s*\)\s*\(?\s*(?:\(\s*[A-Za-z_][A-Za-z0-9_]*\s*\*\s*\)|\*\s*[A-Za-z_][A-Za-z0-9_]*|(?:PVOID|LPVOID|HANDLE)\s*[A-Za-z_][A-Za-z0-9_]*)/.test(line)) {
        add(relative, lineNumber, 'pointer-to-fixed-width-conversion', '-', line);
      }
      if (/\(\s*(?:PVOID|LPVOID|HANDLE|[A-Za-z_][A-Za-z0-9_]*\s*\*)\s*\)\s*\(?\s*(?:\(\s*(?:ULONG|DWORD|UINT|LONG|IU32|IS32|USHORT|WORD)\s*\)|(?:ULONG|DWORD|UINT|LONG|IU32|IS32|USHORT|WORD)\s*\()/.test(line)) {
        add(relative, lineNumber, 'fixed-width-to-pointer-conversion', '-', line);
      }
      if (/^\s*#\s*(?:if|ifdef|ifndef|elif).*\b(?:i386|_M_IX86|_M_X64|_WIN64|WIN64|_X86_)\b/i.test(line)) {
        add(relative, lineNumber, 'architecture-conditional', '-', line);
      }
    }
    // Historical Insignia declarations often put the return type, `(*name)`
    // and IFN/IPT form on separate lines. The line scan above is deliberately
    // readable; this second pass covers those split forms without treating it
    // as a type-safety decision.
    for (const match of content.matchAll(/\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]*\])?\s*\)\s*(?:\([^;{}]*?\)|IFN\d+\()/gs)) {
      if (!match[0].includes('\n')) continue;
      const lineNumber = content.slice(0, match.index).split(/\r?\n/).length;
      const priorLine = lines[lineNumber - 1] ?? '';
      if (isFunctionPointerDeclarationPrefix(priorLine)) {
        add(relative, lineNumber, 'function-pointer-declaration-or-slot', match[1], match[0]);
      }
    }
    for (const match of content.matchAll(/\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)\s*\(/gs)) {
      if (!match[0].includes('\n')) continue;
      const lineNumber = content.slice(0, match.index).split(/\r?\n/).length;
      add(relative, lineNumber, 'function-pointer-indirect-call', match[1], match[0]);
    }
  }
}

rows.sort((left, right) => left.source_path.localeCompare(right.source_path) || left.source_line - right.source_line || left.candidate_kind.localeCompare(right.candidate_kind) || left.symbol.localeCompare(right.symbol));
const unique = [];
const seen = new Set();
for (const row of rows) {
  const key = [row.source_path, row.source_line, row.candidate_kind, row.symbol].join('|');
  if (!seen.has(key)) { seen.add(key); unique.push(row); }
}
const header = ['record_id', ...Object.keys(unique[0] ?? {
  source_path: '', owner_package: '', source_line: '', candidate_kind: '', symbol: '', source_excerpt: '', review_requirement: '', x86_x64_disposition: ''
})];
const escape = value => String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ');
const lines = [header.join('\t')];
unique.forEach((row, index) => lines.push([`T310-S8-P4-ABI-${String(index + 1).padStart(6, '0')}`, ...Object.values(row)].map(escape).join('\t')));
fs.mkdirSync(path.dirname(output), { recursive: true });
fs.writeFileSync(output, `${lines.join('\r\n')}\r\n`, 'utf8');
const summary = {};
for (const row of unique) summary[row.candidate_kind] = (summary[row.candidate_kind] ?? 0) + 1;
const clusters = new Map();
for (const row of unique) {
  const key = `${row.review_cluster}\t${row.profile_class}`;
  if (!clusters.has(key)) clusters.set(key, { review_cluster: row.review_cluster, profile_class: row.profile_class, candidates: 0, paths: new Set(), kinds: new Map() });
  const cluster = clusters.get(key);
  cluster.candidates += 1;
  cluster.paths.add(row.source_path);
  cluster.kinds.set(row.candidate_kind, (cluster.kinds.get(row.candidate_kind) ?? 0) + 1);
}
const clusterHeader = ['cluster_id', 'review_cluster', 'profile_class', 'candidate_count', 'source_file_count', 'candidate_kinds', 'review_state', 'required_source_review'];
const clusterLines = [clusterHeader.join('\t')];
const orderedClusters = [...clusters.values()].sort((left, right) => right.candidates - left.candidates || left.review_cluster.localeCompare(right.review_cluster));
orderedClusters.forEach((cluster, index) => {
  const kinds = [...cluster.kinds.entries()].sort((left, right) => left[0].localeCompare(right[0])).map(([kind, count]) => `${kind}:${count}`).join(';');
  clusterLines.push([
    `T310-S8-P4-CLUSTER-${String(index + 1).padStart(3, '0')}`,
    cluster.review_cluster,
    cluster.profile_class,
    cluster.candidates,
    cluster.paths.size,
    kinds,
    'pending-source-contract-review',
    'read original declarations, assignments/initializers, selected callers, conditional profile and x86/x64 compiler evidence as one contract'
  ].map(escape).join('\t'));
});
fs.mkdirSync(path.dirname(clusterOutput), { recursive: true });
fs.writeFileSync(clusterOutput, `${clusterLines.join('\r\n')}\r\n`, 'utf8');
console.log(JSON.stringify({ source_files: [...sourceRoots].flatMap(item => collect(path.join(root, item))).length, candidates: unique.length, clusters: orderedClusters.length, by_kind: summary }, null, 2));
