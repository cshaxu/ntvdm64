import { createHash } from 'node:crypto';
import { mkdir, readdir, readFile, stat, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const repo = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..', '..');
const outputIndex = process.argv.indexOf('--output');
if (outputIndex < 0 || !process.argv[outputIndex + 1]) throw new Error('usage: node Export-MvdmWholeClosureInventory.mjs --output build/t473-s1/name');
const output = path.resolve(repo, process.argv[outputIndex + 1]);
const build = `${path.resolve(repo, 'build')}${path.sep}`;
if (!output.startsWith(build)) throw new Error('output must be below build');
const roots = { opennt: 'O:/repos.external/OpenNT/base/mvdm', opennt45: 'O:/repos.external/OpenNT-4.5/nt/private/mvdm', current: path.join(repo, 'src/mvdm') };
const sha256 = async (file) => createHash('sha256').update(await readFile(file)).digest('hex').toUpperCase();
async function index(root) {
  const candidates = [];
  async function visit(directory) {
    for (const entry of await readdir(directory, { withFileTypes: true })) {
      const full = path.join(directory, entry.name);
      if (entry.isSymbolicLink()) throw new Error(`symbolic link is not an inventory input: ${full}`);
      if (entry.isDirectory()) await visit(full);
      else if (entry.isFile()) {
        const relative = path.relative(root, full).split(path.sep).join('/');
        candidates.push({ full, relative });
      }
    }
  }
  if (!(await stat(root)).isDirectory()) throw new Error(`missing inventory root: ${root}`);
  await visit(root);
  const hashes = new Array(candidates.length);
  let next = 0;
  const worker = async () => {
    while (next < candidates.length) {
      const index = next++;
      hashes[index] = await sha256(candidates[index].full);
    }
  };
  await Promise.all(Array.from({ length: Math.min(32, candidates.length) }, worker));
  const result = new Map();
  for (let index = 0; index < candidates.length; index += 1) {
    if (result.has(candidates[index].relative)) throw new Error(`duplicate relative path: ${candidates[index].relative}`);
    result.set(candidates[index].relative, hashes[index]);
  }
  return result;
}
const csv = (value) => { const text = String(value ?? ''); return /[",\r\n]/.test(text) ? `"${text.replaceAll('"', '""')}"` : text; };
const [opennt, opennt45, current] = await Promise.all(Object.values(roots).map(index));
const paths = [...new Set([...opennt.keys(), ...opennt45.keys(), ...current.keys()])].sort();
const rows = paths.map((relativePath) => {
  const a = opennt.get(relativePath), b = opennt45.get(relativePath), c = current.get(relativePath);
  const upstreamOrigin = a && b ? (a === b ? 'both-identical' : 'both-conflict') : a ? 'opennt-only' : b ? 'opennt45-only' : 'current-only';
  const currentIdentity = !c ? 'not-currently-mirrored' : a && c === a && (!b || a === b) ? 'exact-opennt-selected-or-identical' : b && c === b && (!a || a === b) ? 'exact-opennt45-selected-or-identical' : a && b && c === a ? 'exact-opennt-on-conflict' : a && b && c === b ? 'exact-opennt45-on-conflict' : upstreamOrigin === 'current-only' ? 'current-only-needs-provenance' : 'mirror-different-needs-divergence-review';
  return { relative_path: relativePath, upstream_origin: upstreamOrigin, opennt_sha256: a ?? '', opennt45_sha256: b ?? '', current_sha256: c ?? '', current_identity: currentIdentity };
});
const counts = (field) => Object.entries(Object.groupBy(rows, (row) => row[field])).map(([classification, values]) => ({ classification, count: values.length })).sort((a, b) => a.classification.localeCompare(b.classification));
const summary = { rows: rows.length, upstream_origin: counts('upstream_origin'), current_identity: counts('current_identity'), limitation: 'Identity inventory only. It does not select a conflict, prove build/link selection, or claim runtime reachability.' };
await mkdir(output, { recursive: false });
const columns = Object.keys(rows[0]);
await writeFile(path.join(output, 'mvdm-union.csv'), `${columns.join(',')}\n${rows.map((row) => columns.map((column) => csv(row[column])).join(',')).join('\n')}\n`);
await writeFile(path.join(output, 'summary.json'), `${JSON.stringify(summary, null, 2)}\n`);
process.stdout.write(`${JSON.stringify(summary)}\n`);
