import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const operations = path.join(root, 'docs', 'etc', 'operations');

function read(name) {
  const [header, ...lines] = fs.readFileSync(path.join(operations, name), 'utf8')
    .trimEnd().split(/\r?\n/);
  const fields = header.split('\t');
  return lines.map((line) => {
    const cells = line.split('\t').map((cell) => cell.replace(/^"|"$/g, '').replace(/""/g, '"'));
    return Object.fromEntries(fields.map((field, index) => [field, cells[index] ?? '']));
  });
}

function quote(value) {
  return `"${String(value).replaceAll('"', '""')}"`;
}

function seedDisposition(seed) {
  if (seed.source_class === 'mvdm-internal-rebase') {
    return ['mvdm-zero-closure', 'selected MVDM mirror owner', 'return to selected MVDM closure'];
  }
  if (seed.source_class === 'mvdm-softpc-second-executor') {
    return ['bochs-machine-boundary', 'adapter-bochs + adapter-mvdm-host-out/softpc', 'retain source contract; never import a second executor'];
  }
  if (seed.source_class === 'baseclient-candidate') {
    return ['source-shaped-basevdm-boundary', 'adapter-mvdm-host-out/basesrv', 'use original BaseClient/BaseSrv as source contract; stop at CSR/CSRSS'];
  }
  if (seed.source_class === 'netapi-package-candidate') {
    if (seed.callee_spelling === 'NetpNtStatusToApiStatus') return ['opennt-host-netstatus-slice', 'opennt-host/netapi/netlib', 'future exact ntstatus.c mirror slice'];
    if (seed.callee_spelling === 'RxNetUserPasswordSet') return ['redirector-protocol-hard-boundary', 'adapter-mvdm-host-out/redir', 'preserve source contract; no substitute success'];
    return ['public-win32-redirector-binding', 'adapter-mvdm-host-out/win32 + redir', 'bind current public workstation API with bounded buffer ownership'];
  }
  if (seed.source_class === 'public-crt-leaf') {
    return ['public-crt-leaf', 'MSVC CRT', 'bind the supported CRT; do not import OpenNT CRT'];
  }
  if (seed.source_class === 'oleaut-public-abi-candidate' && seed.callee_spelling === 'ReadByte') {
    return ['mvdm-macro-rebase', 'selected MVDM debug owner', 'macro expands to local read_byte; no OLE dependency'];
  }
  if (seed.source_class === 'ntdll-csr-boundary') {
    return ['ntdll-csr-hard-boundary', 'adapter-mvdm-host-out/monitor', 'retain contract evidence; do not import CSR transport'];
  }
  return ['source-identity-hold', 'caller owner package', 'no import or adapter selection from this seed'];
}

const calls = read('one-ledger1.tsv').filter((row) => row.zero_binding === 'outside-zero-ledger1');
const resolutions = new Map(read('one-ledger1-definition-resolution.tsv')
  .map((row) => [row.one_ledger1_identity, row]));
const signatures = new Map(read('one-ledger1-signature-gate.tsv')
  .map((row) => [row.one_ledger1_identity, row]));
const seeds = new Map(read('m0-t306-s1-signature-seed-package-ledger.tsv')
  .map((row) => [row.one_ledger1_identity, row]));

const rows = calls.map((call) => {
  const resolution = resolutions.get(call.one_ledger1_identity);
  const signature = signatures.get(call.one_ledger1_identity);
  const seed = seeds.get(call.one_ledger1_identity);
  let disposition;
  let owner;
  let nextAction;
  if (seed) {
    [disposition, owner, nextAction] = seedDisposition(seed);
  } else if (signature) {
    disposition = 'owner-local-signature-or-condition-binding';
    owner = 'caller owner package';
    nextAction = 'not an import candidate; resolve only when the original caller package is admitted';
  } else if (resolution?.source_definition_result === 'physical-original-definition-candidate(s)-recorded') {
    disposition = 'owner-local-declaration-binding';
    owner = 'caller owner package';
    nextAction = 'not an import candidate; resolve declaration/linkage only in the caller owner package';
  } else {
    disposition = 'owner-local-source-absent-macro-or-import-boundary';
    owner = 'caller owner package';
    nextAction = 'not an import candidate; classify exact macro/assembly/public/native form when the caller owner package is admitted';
  }
  return {
    one_ledger1_identity: call.one_ledger1_identity,
    caller_physical_identity: call.caller_physical_identity,
    caller_source_path: call.caller_source_path,
    caller_symbol: call.caller_symbol,
    call_source_line: call.call_source_line,
    callee_spelling: call.callee_spelling,
    call_form: call.call_form,
    source_definition_result: resolution?.source_definition_result ?? 'missing-resolution-record',
    signature_disposition: signature?.signature_disposition ?? '',
    seed_class: seed?.source_class ?? '',
    final_adoption_disposition: disposition,
    owner: owner,
    next_action: nextAction
  };
});

const columns = Object.keys(rows[0] ?? {});
const text = [columns.join('\t'), ...rows.map((row) => columns.map((key) => quote(row[key])).join('\t'))].join('\n') + '\n';
const output = path.join(operations, 'm0-t306-s1-external-boundary-disposition-ledger.tsv');
fs.writeFileSync(output, text, 'utf8');
console.log(`Wrote ${rows.length} physical direct-boundary dispositions: ${path.relative(root, output)}`);
