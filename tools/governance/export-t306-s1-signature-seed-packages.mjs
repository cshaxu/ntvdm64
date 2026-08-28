import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] ?? '.');
const operations = path.join(root, 'docs', 'etc', 'operations');

function parseTsv(file) {
  const [header, ...lines] = fs.readFileSync(file, 'utf8').trimEnd().split(/\r?\n/);
  const names = header.split('\t');
  return lines.map((line) => {
    const cells = line.split('\t').map((cell) => cell.replace(/^"|"$/g, '').replace(/""/g, '"'));
    return Object.fromEntries(names.map((name, index) => [name, cells[index] ?? '']));
  });
}

function quote(value) {
  return `"${String(value).replaceAll('"', '""')}"`;
}

function disposition(definitionPath) {
  if (definitionPath.startsWith('base/mvdm/softpc.new/')) {
    return ['mvdm-softpc-second-executor', 'machine-owner disposition',
      'adapter-bochs + adapter-mvdm-host-out/softpc',
      'retain as a named machine-boundary seed; do not import the historical executor'];
  }
  if (definitionPath.startsWith('base/mvdm/')) {
    return ['mvdm-internal-rebase', 'zero-degree rebase candidate',
      'selected MVDM package',
      'prove package-local binding, then return this edge to the MVDM closure rather than an external adapter'];
  }
  if (definitionPath.startsWith('base/win32/client/')) {
    return ['baseclient-candidate', 'OpenNT BaseClient package candidate',
      'opennt-host / BaseClient',
      'inspect only the reached client source closure through public Win32 or named BaseSrv boundary'];
  }
  if (definitionPath.startsWith('base/crts/')) {
    return ['public-crt-leaf', 'public CRT leaf', 'MSVC CRT',
      'verify calling convention and source expectation; do not import historical CRT body by default'];
  }
  if (definitionPath.startsWith('base/ntdll/')) {
    return ['ntdll-csr-boundary', 'non-invasive hard boundary', 'adapter-mvdm-host-out/monitor',
      'record source contract only; do not import NTDLL CSR transport'];
  }
  if (definitionPath.startsWith('ds/netapi/')) {
    return ['netapi-package-candidate', 'OpenNT NetAPI package candidate', 'opennt-host / NetAPI',
      'inspect package closure only to public Win32/RPC or a hard boundary'];
  }
  if (definitionPath.startsWith('com/oleaut32/')) {
    return ['oleaut-public-abi-candidate', 'public COM/OLE ABI candidate', 'adapter-mvdm-host-out/win32',
      'verify the exact reached ABI against public COM/OLE before importing historical OLE source'];
  }
  return ['unclassified-source-root', 'requires bounded package selection', '',
    'inspect original package root before selecting an import or adapter'];
}

const input = path.join(operations, 'one-ledger1-signature-gate.tsv');
const rows = parseTsv(input)
  .filter((row) => row.signature_disposition === 'physical-call-declaration-definition-arity-confirmed')
  .map((row) => {
    const identity = row.selected_definition_identity.split('|');
    const definitionPath = identity[1] ?? '';
    const [sourceClass, planningDisposition, owner, nextAction] = disposition(definitionPath);
    return {
      signature_gate_id: row.signature_gate_id,
      one_ledger1_identity: row.one_ledger1_identity,
      caller_physical_identity: row.resolution_id,
      callee_spelling: row.callee_spelling,
      selected_definition_identity: row.selected_definition_identity,
      selected_definition_path: definitionPath,
      source_class: sourceClass,
      planning_disposition: planningDisposition,
      proposed_owner: owner,
      next_action: nextAction
    };
  });

const columns = Object.keys(rows[0] ?? {});
const output = [columns.join('\t'), ...rows.map((row) => columns.map((column) => quote(row[column])).join('\t'))].join('\n') + '\n';
const outputPath = path.join(operations, 'm0-t306-s1-signature-seed-package-ledger.tsv');
fs.writeFileSync(outputPath, output, 'utf8');
console.log(`Wrote ${rows.length} signature-confirmed physical seed rows: ${path.relative(root, outputPath)}`);
