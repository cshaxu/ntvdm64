import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';

const repositoryRoot = process.argv[2] || process.cwd();
const sources = [
  { edition: 'OpenNT', root: 'O:\\repos.external\\OpenNT\\base\\mvdm\\tools16' },
  { edition: 'OpenNT-4.5', root: 'O:\\repos.external\\OpenNT-4.5\\nt\\private\\mvdm\\tools16' },
];
const targetRoot = path.join(repositoryRoot, 'tools', 'opennt', 'tools16');
const manifestPath = path.join(repositoryRoot, 'docs', 'etc', 'operations', 'm0-t296-s1-tools16-provenance-manifest.tsv');

function filesBelow(root) {
  const result = [];
  for (const entry of fs.readdirSync(root, { withFileTypes: true })) {
    const fullPath = path.join(root, entry.name);
    if (entry.isDirectory()) result.push(...filesBelow(fullPath));
    else if (entry.isFile()) result.push(fullPath);
  }
  return result;
}
function sha256(fileName) {
  return crypto.createHash('sha256').update(fs.readFileSync(fileName)).digest('hex');
}
function quote(value) {
  return `"${String(value).replaceAll('"', '""')}"`;
}
function parseTsv(fileName) {
  if (!fs.existsSync(fileName)) return [];
  const lines = fs.readFileSync(fileName, 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean);
  const header = lines.shift().split('\t');
  return lines.map((line) => {
    const values = []; let field = ''; let quoted = false;
    for (let index = 0; index < line.length; index += 1) {
      const ch = line[index];
      if (ch === '"') {
        if (quoted && line[index + 1] === '"') { field += '"'; index += 1; }
        else quoted = !quoted;
      } else if (ch === '\t' && !quoted) { values.push(field); field = ''; }
      else field += ch;
    }
    values.push(field);
    return Object.fromEntries(header.map((column, index) => [column, values[index] || '']));
  });
}
function writeTsv(fileName, rows, columns) {
  const content = [
    columns.join('\t'),
    ...rows.map((row) => columns.map((column) => quote(row[column] || '')).join('\t')),
  ].join('\r\n') + '\r\n';
  fs.writeFileSync(fileName, content, 'utf8');
}

const byRelativePath = new Map();
const priorActions = new Map(parseTsv(manifestPath).map((row) => [row.target_path, row.import_action]));
for (const source of sources) {
  if (!fs.existsSync(source.root)) throw new Error(`Missing approved source tree: ${source.root}`);
  for (const sourcePath of filesBelow(source.root)) {
    const relativePath = path.relative(source.root, sourcePath).replaceAll('\\', '/');
    if (!byRelativePath.has(relativePath)) byRelativePath.set(relativePath, []);
    byRelativePath.get(relativePath).push({ ...source, sourcePath, sha256: sha256(sourcePath) });
  }
}

const rows = [];
for (const relativePath of [...byRelativePath.keys()].sort()) {
  const candidates = byRelativePath.get(relativePath);
  const hashes = [...new Set(candidates.map((candidate) => candidate.sha256))];
  if (hashes.length !== 1) {
    throw new Error(`Unresolved OpenNT edition conflict: ${relativePath}`);
  }
  const selected = candidates.find((candidate) => candidate.edition === 'OpenNT') || candidates[0];
  const targetPath = path.join(targetRoot, relativePath);
  const alreadyPresent = fs.existsSync(targetPath);
  if (alreadyPresent) {
    if (sha256(targetPath) !== selected.sha256) {
      throw new Error(`Refusing to overwrite non-identical target: ${targetPath}`);
    }
  } else {
    fs.mkdirSync(path.dirname(targetPath), { recursive: true });
    fs.copyFileSync(selected.sourcePath, targetPath, fs.constants.COPYFILE_EXCL);
  }
  rows.push({
    target_path: `tools/opennt/tools16/${relativePath}`,
    role: 'historical-MVDM-16-bit-guest-build-tool-input',
    source_edition: candidates.map((candidate) => candidate.edition).join(';'),
    selected_source_path: selected.sourcePath,
    alternate_source_paths: candidates.filter((candidate) => candidate !== selected).map((candidate) => candidate.sourcePath).join(';'),
    source_sha256: selected.sha256,
    target_sha256: sha256(targetPath),
    selection: candidates.length === 2 ? 'dual-identical' : `${selected.edition}-only`,
    // Keep the one-time import fact stable across verification reruns; a
    // second run verifies bytes and must not erase the original provenance.
    import_action: (priorActions.get(`tools/opennt/tools16/${relativePath}`) !== 'already-identical'
      ? priorActions.get(`tools/opennt/tools16/${relativePath}`)
      : '')
      // T296's one-sided OpenNT-4.5 files are the audited missing carries;
      // retain that import fact after a verification rerun.
      || (candidates.length === 1 && selected.edition === 'OpenNT-4.5'
        ? 'copied-byte-exact'
        : (alreadyPresent ? 'already-identical' : 'copied-byte-exact')),
    product_boundary: 'not a src component; excluded from app, host, guest-image, adapter, firmware, test and default build/link/runtime inputs',
  });
}
writeTsv(manifestPath, rows, [
  'target_path', 'role', 'source_edition', 'selected_source_path', 'alternate_source_paths',
  'source_sha256', 'target_sha256', 'selection', 'import_action', 'product_boundary',
]);
if (rows.some((row) => row.source_sha256 !== row.target_sha256)) throw new Error('Target hash verification failed');
console.log(`tools16 paths: ${rows.length}; copied: ${rows.filter((row) => row.import_action === 'copied-byte-exact').length}; retained: ${rows.filter((row) => row.import_action === 'already-identical').length}`);
