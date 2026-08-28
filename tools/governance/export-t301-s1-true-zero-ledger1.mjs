import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');

function split(line) {
  const fields = []; let field = ''; let quoted = false;
  for (let index = 0; index < line.length; index += 1) {
    const character = line[index];
    if (character === '"') {
      if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted;
    } else if (character === '\t' && !quoted) { fields.push(field); field = ''; } else field += character;
  }
  fields.push(field); return fields;
}
function read(name) {
  const [header, ...body] = fs.readFileSync(path.join(operations, name), 'utf8')
    .split(/\r?\n/).filter(Boolean).map(split);
  return { header, rows: body.map((fields) => Object.fromEntries(header.map((key, index) => [key, fields[index] || '']))) };
}
function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
function write(name, columns, rows) {
  fs.writeFileSync(path.join(operations, name), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
}

const ledger0 = read('zero-ledger0.tsv');
const recovery = read('mvdm-file-recovery-ledger.tsv');
const recoveryByIdentity = new Map(recovery.rows.map((row) => [`${row.target_path}|${row.selected_source_sha256}`, row]));

function softpcClass(pathname, disposition, owner) {
  if (disposition === 'adapter-backed') return ['retain-source-shaped-host-provider', owner];
  if (disposition === 'overlay-required') return ['retain-source-shaped-overlay-provider', owner];
  if (disposition === 'firmware-only') return ['exclude-firmware-input', 'mvdm-softpc-firmware -> adapter-bochs immutable-input manifest'];
  if (disposition === 'tool-only') return ['exclude-tool-only', 'mvdm-tools; no host-runtime link edge'];
  if (disposition === 'binding-only') return ['exclude-declaration-only', owner];
  if (disposition !== 'not-host-runtime') throw new Error(`unclassified SoftPC disposition: ${disposition} for ${pathname}`);
  if (/^softpc\.new\/base\/(ccpu386|cvidc|cpu)\//.test(pathname) || pathname === 'softpc.new/base/c_regs.c') {
    return ['exclude-bochs-cpu-fpu-executor', 'bochs-core via adapter-bochs; no second CPU/FPU/decode/register executor'];
  }
  if (/^softpc\.new\/base\/(system|video|disks|comms|keymouse|dos)\//.test(pathname)) {
    return ['exclude-bochs-pc-device-executor', 'bochs-core via adapter-bochs; no second PIC/PIT/CMOS/DMA/video/disk/serial/keyboard/mouse/EMS device executor'];
  }
  if (pathname.startsWith('softpc.new/host/src/')) {
    return ['exclude-alternate-softpc-product-control', 'adapter-bochs/bochs-core/app own final machine lifecycle and product composition'];
  }
  if (pathname.startsWith('softpc.new/base/support/')) {
    return ['exclude-alternate-softpc-support-shell', 'bochs-core and app own corresponding machine lifecycle/product composition'];
  }
  return ['exclude-not-host-runtime', owner];
}

const allSoftpc = [];
const retained = [];
const excluded = [];
for (const row of ledger0.rows) {
  if (!row.source_path.startsWith('softpc.new/')) {
    retained.push({ ...row, ledger1_disposition: 'retain-non-softpc', replacement_owner: '' });
    continue;
  }
  const file = recoveryByIdentity.get(`${row.source_path}|${row.source_sha256}`);
  if (!file) throw new Error(`No final file disposition for ${row.source_path}|${row.source_sha256}`);
  const [ledger1Disposition, replacementOwner] = softpcClass(row.source_path, file.expected_final_disposition, file.final_owner_or_link_boundary);
  const record = {
    ...row,
    softpc_file_disposition: file.expected_final_disposition,
    ledger1_disposition: ledger1Disposition,
    replacement_owner: replacementOwner,
    final_disposition_evidence: file.final_disposition_evidence,
  };
  allSoftpc.push(record);
  if (ledger1Disposition.startsWith('retain-')) retained.push(record); else excluded.push(record);
}

const retainedColumns = [...ledger0.header, 'ledger1_disposition', 'replacement_owner'];
const excludedColumns = [...ledger0.header, 'softpc_file_disposition', 'ledger1_disposition', 'replacement_owner', 'final_disposition_evidence'];
write('zero-ledger1.tsv', retainedColumns, retained.map((row, index) => ({
  ...row,
  ledger0_identity: `MVDM-HOST-ZERO1-${String(index + 1).padStart(6, '0')}`,
})));
write('zero-ledger1-softpc-exclusion-ledger.tsv', excludedColumns, excluded);
write('zero-ledger1-softpc-disposition-ledger.tsv', excludedColumns, allSoftpc);
console.log(`ledger0=${ledger0.rows.length}; retained ledger1=${retained.length}; softpc reviewed=${allSoftpc.length}; softpc retained=${allSoftpc.length - excluded.length}; softpc excluded=${excluded.length}`);
