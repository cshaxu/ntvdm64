/*
 * Compiler-diagnostic cross-check for M0 T310 S8 P4.
 *
 * This is deliberately secondary to Export-T310WholeTreeAbiInventory.mjs.
 * It preserves every selected x86/x64 diagnostic with its source location so
 * source-contract review can prove whether it is relevant, rather than
 * treating the compiler's warning category as a repair prescription.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const x64Log = path.resolve(root, process.argv[3] ?? 'build/M0-T310/S8/p1-machine-source/x64/whole-tree-selected-candidate-rebuild.log');
const x86Log = path.resolve(root, process.argv[4] ?? 'build/M0-T310/S8/p1-machine-source/x86/whole-tree-selected-candidate-rebuild.log');
const output = path.resolve(root, process.argv[5] ?? 'docs/etc/operations/m0-t310-s8-p4-whole-tree-compiler-warning-ledger.tsv');
const clusterOutput = path.resolve(root, process.argv[6] ?? 'docs/etc/operations/m0-t310-s8-p4-whole-tree-compiler-warning-cluster-ledger.tsv');

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
    // `ccpusas4.c` owns physical-memory handler selection; it is not an
    // instruction-executor ABI contract merely because it is in ccpu386.
    ['softpc.new/base/ccpu386/ccpusas4.c', 'SPC-SAS-MEMORY-CONTRACT'],
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
  return `MVDM-${(pathName.split('/')[0] ?? 'unknown').toUpperCase()}-OWNER-PACKAGE`;
}

function parse(logPath, architecture) {
  const rows = [];
  const linePattern = /(?:^|\s)([A-Za-z]:\/[^:(]+|[^:(\s]+)\((\d+)\):\s+warning\s+(C\d+):\s*(.*)$/i;
  for (const line of fs.readFileSync(logPath, 'utf8').split(/\r?\n/)) {
    const match = line.match(linePattern);
    if (!match) continue;
    const absolute = match[1].replaceAll('\\', '/');
    const sourcePath = path.relative(root, absolute).split(path.sep).join('/');
    if (!sourcePath.startsWith('src/mvdm-host/') && !sourcePath.startsWith('src/mvdm-support/')) continue;
    rows.push({ architecture, source_path: sourcePath, source_line: Number(match[2]), warning: match[3].toUpperCase(), message: match[4].trim(), review_cluster: clusterFor(sourcePath) });
  }
  return rows;
}

const rawRows = [...parse(x64Log, 'x64'), ...parse(x86Log, 'x86')];
const deduplicated = new Map();
for (const row of rawRows) {
  const key = [row.architecture, row.source_path, row.source_line, row.warning, row.message, row.review_cluster].join('\u001f');
  const existing = deduplicated.get(key);
  if (existing) existing.occurrence_count += 1;
  else deduplicated.set(key, { ...row, occurrence_count: 1 });
}
const rows = [...deduplicated.values()]
  .sort((a, b) => a.review_cluster.localeCompare(b.review_cluster) || a.source_path.localeCompare(b.source_path) || a.source_line - b.source_line || a.architecture.localeCompare(b.architecture) || a.warning.localeCompare(b.warning));
const header = ['record_id', 'architecture', 'source_path', 'source_line', 'warning', 'message', 'occurrence_count', 'review_cluster', 'source_review_requirement', 'disposition'];
const escape = value => String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ');
const lines = [header.join('\t')];
rows.forEach((row, index) => lines.push([
  `T310-S8-P4-WARN-${String(index + 1).padStart(6, '0')}`,
  row.architecture, row.source_path, row.source_line, row.warning, row.message, row.occurrence_count, row.review_cluster,
  'read original declaration/caller/initializer and reconcile with static ABI inventory; warning alone is not a defect decision',
  'unreviewed-source-context-required'
].map(escape).join('\t')));
fs.mkdirSync(path.dirname(output), { recursive: true });
fs.writeFileSync(output, `${lines.join('\r\n')}\r\n`, 'utf8');
const clusters = new Map();
for (const row of rows) {
  if (!clusters.has(row.review_cluster)) clusters.set(row.review_cluster, { raw: 0, unique: 0, warnings: new Map(), paths: new Set() });
  const cluster = clusters.get(row.review_cluster);
  cluster.raw += row.occurrence_count;
  cluster.unique += 1;
  cluster.paths.add(row.source_path);
  cluster.warnings.set(row.warning, (cluster.warnings.get(row.warning) ?? 0) + row.occurrence_count);
}
const clusterLines = [['cluster_id', 'review_cluster', 'raw_occurrence_count', 'unique_source_diagnostic_count', 'source_file_count', 'warning_counts', 'review_state', 'required_source_review'].join('\t')];
[...clusters.entries()].sort((a, b) => b[1].raw - a[1].raw || a[0].localeCompare(b[0])).forEach(([clusterName, cluster], index) => {
  const warnings = [...cluster.warnings.entries()].sort((a, b) => b[1] - a[1] || a[0].localeCompare(b[0])).map(([warning, count]) => `${warning}:${count}`).join(';');
  clusterLines.push([
    `T310-S8-P4-WARN-CLUSTER-${String(index + 1).padStart(3, '0')}`,
    clusterName, cluster.raw, cluster.unique, cluster.paths.size, warnings,
    'pending-original-contract-review',
    'read original declarations, assignments/initializers and selected callers; classify every diagnostic family as minimal mirror correction, overlay, guest/physical mapping-manager crossing, or not selected'
  ].map(escape).join('\t'));
});
fs.mkdirSync(path.dirname(clusterOutput), { recursive: true });
fs.writeFileSync(clusterOutput, `${clusterLines.join('\r\n')}\r\n`, 'utf8');
const summary = {};
for (const row of rawRows) summary[`${row.architecture}:${row.warning}`] = (summary[`${row.architecture}:${row.warning}`] ?? 0) + 1;
console.log(JSON.stringify({ raw_diagnostic_occurrences: rawRows.length, unique_source_diagnostics: rows.length, by_architecture_and_warning: summary }, null, 2));
