import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const input = path.join(operations, 'mvdm-zero-degree-legality-ledger.tsv');
const output = path.join(operations, 'zero-ledger0.tsv');

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
function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
const [header, ...body] = fs.readFileSync(input, 'utf8').split(/\r?\n/).filter(Boolean).map(split);
const rows = body.map((fields) => Object.fromEntries(header.map((key, index) => [key, fields[index] || ''])));

// This is deliberately the component's physical subtree list, not an MVDM
// package-prefix heuristic. Guest, tool and support-only subtrees remain in
// the unchanged full ledger and its exclusion ledgers, never in ledger0.
const allowed = [
  'bde/', 'dbg/', 'dos/command/', 'dos/dem/', 'dpmi/', 'dpmi32/', 'fax/',
  'ieuvddex/', 'sim16/', 'sim32/', 'softpc.new/', 'v86/', 'vdd/', 'vdmdbg/',
  'vdmexts/', 'vdmredir/', 'wow32/', 'xms.486/',
];
function owner(pathname) { return allowed.find((prefix) => pathname.startsWith(prefix)) || ''; }
const accepted = rows.filter((row) => owner(row.source_path));
const columns = ['ledger0_identity', 'mvdm_host_subtree', ...header];
const contents = accepted.map((row, index) => columns.map((column) => quote(
  column === 'ledger0_identity' ? `MVDM-HOST-ZERO-${String(index + 1).padStart(6, '0')}`
    : column === 'mvdm_host_subtree' ? owner(row.source_path)
      : row[column],
)).join('\t')).join('\n');
fs.writeFileSync(output, `${columns.join('\t')}\n${contents}\n`);
console.log(`full historical rows=${rows.length}; true zero ledger0 rows=${accepted.length}; excluded from ledger0=${rows.length - accepted.length}`);
