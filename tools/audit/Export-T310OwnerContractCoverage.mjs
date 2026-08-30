/*
 * Coverage ledger for M0 T310 S8 P4 owner-contract review.
 *
 * A clean warning is not proof that a prior mirror repair is compatible with
 * every declaration, definition, initializer and selected caller.  This
 * combines the full classification, current worklist and all P4 evidence
 * names so each owner cluster re-reads its prior closures as part of the
 * same source-shaped review.
 */
import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const operations = path.join(root, 'docs/etc/operations');
const evidenceRoot = path.join(root, 'docs/etc/evidence');
const classificationPath = path.join(operations, 'm0-t310-s8-p4-whole-tree-warning-x86-x64-classification.tsv');
const worklistPath = path.join(operations, 'm0-t310-s8-p4-whole-tree-warning-x86-x64-classification-worklist.tsv');
const staticClusterPath = path.join(operations, 'm0-t310-s8-p4-whole-tree-static-abi-cluster-ledger.tsv');
const outputPath = path.join(operations, 'm0-t310-s8-p4-owner-contract-coverage-ledger.tsv');

function tsv(file) {
  const [header, ...lines] = fs.readFileSync(file, 'utf8').replace(/^\uFEFF/, '').trimEnd().split(/\r?\n/);
  const fields = header.split('\t');
  return lines.filter(Boolean).map(line => {
    const values = line.split('\t');
    return Object.fromEntries(fields.map((field, index) => [field, values[index] ?? '']));
  });
}

const evidenceMatchers = [
  ['SPC-CCPU-EXECUTOR-DISPATCH', /(?:^|-)ccpu|zfrsrvd/i],
  ['SPC-SAS-MEMORY-CONTRACT', /ccpusas|sas-memory/i],
  ['SPC-CVIDC-GENERATED-DISPATCH', /cvidc|gdp|copy-(?:fill|fnc)|native-c-varargs/i],
  ['SPC-VIDEO-CONTROLLER-DISPATCH', /(?:^|-)video-|ega-|cga-|aorc|x86gfx/i],
  ['SPC-SYSTEM-CONTROLLER-CALLBACKS', /system-controller|cmos-reset/i],
  ['SPC-HOST-SYSTEM-INTERRUPT-BINDINGS', /system-interrupt|nt-event/i],
  ['SPC-STORAGE-CONTROLLER-CALLBACKS', /floppy|storage-controller/i],
  ['SPC-HOST-STORAGE-BINDINGS', /storage-(?:private|native)|fprt/i],
  ['SPC-MEMORY-MAPPING-BINDINGS', /sas-|emm-|ems-|umb-|nt-mem|rtl-fill/i],
  ['SPC-INPUT-CONTROLLER-CALLBACKS', /keyboard|mouse|sound/i],
  ['SPC-HOST-INPUT-BINDINGS', /keyboard|mouse/i],
  ['SPC-COMMS-CONTROLLER-CALLBACKS', /comms/i],
  ['SPC-HOST-COMMS-BINDINGS', /comms/i],
  ['SPC-BIOS-FIRMWARE-BINDINGS', /rom-|firmware|softpc-startup/i],
  ['SPC-HOST-PLATFORM-BINDINGS', /host-native|host-simulate|host-stub|nt-unix|hard-error|nt-sec|event-thread|multiple-object|console|error-dialog|stubs-x86/i],
  ['MVDM-DOS-OWNER-PACKAGE', /command-|dem-/i],
  ['MVDM-SIM32-OWNER-PACKAGE', /sim32|vdd-/i],
  ['MVDM-XMS.486-OWNER-PACKAGE', /xms-/i],
  ['SPC-HOST-BOP-BINDING', /(?:^|-)bop-/i],
  ['SPC-HOST-CONFIGURATION-BINDINGS', /config-|pif-lim|configuration-guest/i],
  ['SPC-SUPPORT-CONTROLLER-CALLBACKS', /support-x86/i],
  ['P4-GLOBAL-BASELINE', /selected-softpc-contract-reading|whole-tree/i],
  ['SPC-CCPU-EXECUTOR-DISPATCH', /vglob/i]
];

const clusters = new Map();
function cluster(name) {
  if (!clusters.has(name)) clusters.set(name, {
    staticCandidates: 0,
    classificationRecords: 0,
    worklistRecords: 0,
    dispositions: new Map(),
    evidence: []
  });
  return clusters.get(name);
}

for (const row of tsv(staticClusterPath)) {
  const item = cluster(row.review_cluster);
  item.staticCandidates = Number(row.candidate_count || 0);
}
for (const row of tsv(classificationPath)) {
  const item = cluster(row.review_cluster);
  item.classificationRecords += 1;
  item.dispositions.set(row.x86_x64_disposition, (item.dispositions.get(row.x86_x64_disposition) ?? 0) + 1);
}
for (const row of tsv(worklistPath)) cluster(row.review_cluster).worklistRecords += 1;

const unassigned = [];
for (const filename of fs.readdirSync(evidenceRoot).filter(name => /^m0-t310-s8-p4-.*\.md$/i.test(name)).sort()) {
  const matched = evidenceMatchers.filter(([, pattern]) => pattern.test(filename)).map(([name]) => name);
  if (matched.length === 0) unassigned.push(filename);
  else for (const name of matched) cluster(name).evidence.push(`docs/etc/evidence/${filename}`);
}

const header = [
  'coverage_id', 'review_cluster', 'static_candidate_count',
  'classification_record_count', 'current_worklist_count',
  'current_dispositions', 'prior_evidence_to_reread', 'coverage_state',
  'mandatory_cluster_action'
];
const sanitize = value => String(value).replaceAll('\t', ' ').replaceAll('\r', ' ').replaceAll('\n', ' ');
const rows = [...clusters.entries()].sort(([left], [right]) => left.localeCompare(right)).map(([name, item], index) => [
  `T310-S8-P4-COVERAGE-${String(index + 1).padStart(3, '0')}`,
  name,
  item.staticCandidates,
  item.classificationRecords,
  item.worklistRecords,
  [...item.dispositions.entries()].sort(([left], [right]) => left.localeCompare(right)).map(([kind, count]) => `${kind}:${count}`).join(';'),
  item.evidence.join(';') || '-',
  'requires-full-owner-contract-reread',
  're-read original declaration, definition, initializer/assignment, selected callers and every listed prior closure; retain, replace or remove each prior repair before closing the cluster'
]);
for (const filename of unassigned) rows.push([
  'T310-S8-P4-COVERAGE-EVIDENCE-UNASSIGNED',
  'EVIDENCE-UNASSIGNED', '0', '0', '0', '-',
  `docs/etc/evidence/${filename}`,
  'requires-manual-owner-assignment',
  'assign this prior P4 evidence to one owner-contract cluster before that cluster can close'
]);
fs.writeFileSync(outputPath, `${[header, ...rows].map(row => row.map(sanitize).join('\t')).join('\r\n')}\r\n`, 'utf8');
console.log(JSON.stringify({ clusters: clusters.size, evidence_files: fs.readdirSync(evidenceRoot).filter(name => /^m0-t310-s8-p4-.*\.md$/i.test(name)).length, unassigned_evidence: unassigned.length, output: path.relative(root, outputPath).split(path.sep).join('/') }, null, 2));
