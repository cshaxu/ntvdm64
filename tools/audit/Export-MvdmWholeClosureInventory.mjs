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
const hash = (bytes) => createHash('sha256').update(bytes).digest('hex').toUpperCase();
async function fingerprint(file) {
  const bytes = await readFile(file);
  return {
    sha256: hash(bytes),
    normalizedSha256: bytes.includes(0) ? '' : hash(Buffer.from(bytes.toString().replaceAll('\r\n', '\n').replace(/[ \t]+(?=\n|$)/g, ''))),
  };
}
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
      hashes[index] = await fingerprint(candidates[index].full);
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
async function selectedBy(graph, label) {
  const text = await readFile(graph, 'utf8');
  const selected = new Set();
  const pattern = /(?:O\$?:\/repos\.hobby\/ntvdm64\/|O:\\repos\.hobby\\ntvdm64\\)src(?:\/|\\)mvdm(?:\/|\\)([^\s]+)/gi;
  for (const match of text.matchAll(pattern)) selected.add(match[1].replaceAll('\\', '/').toLowerCase());
  return [label, selected];
}
const csv = (value) => { const text = String(value ?? ''); return /[",\r\n]/.test(text) ? `"${text.replaceAll('"', '""')}"` : text; };
const [opennt, opennt45, current, workerSelection, wow32Selection] = await Promise.all([
  ...Object.values(roots).map(index),
  selectedBy(path.join(repo, 'build/.ninja/msvc-x86-release/worker/build.ninja'), 'worker-graph'),
  selectedBy(path.join(repo, 'build/.ninja/msvc-x86-release/wow32/build.ninja'), 'wow32-graph'),
]);
const selections = new Map([workerSelection, wow32Selection]);
function disposition(relativePath, currentFile, graphCompileSelection) {
  if (graphCompileSelection !== 'not-directly-compiled') return 'selected-direct-compile';
  if (!currentFile) return relativePath.startsWith('tools16/') ? 'unmirrored-tool-build-input' : relativePath.startsWith('v86/') ? 'unmirrored-retired-cpu30-executor' : 'unmirrored-needs-explicit-disposition';
  if (relativePath.startsWith('dos/v86/') || relativePath.startsWith('wow16/') || relativePath.startsWith('dpmi/')) return 'immutable-guest-or-guest-build-input';
  if (relativePath.startsWith('v86/')) return 'retired-cpu30-executor';
  if (/^(bin86|build\.wrn|build\.log|build\.err|makefil0|inc)\b/.test(relativePath)) return 'tooling-or-build-carrier';
  if (relativePath.endsWith('.obj')) return 'retained-build-artifact';
  return 'unselected-host-package-requires-s2-disposition';
}
const paths = [...new Set([...opennt.keys(), ...opennt45.keys(), ...current.keys()])].sort();
const rows = paths.map((relativePath) => {
  const a = opennt.get(relativePath), b = opennt45.get(relativePath), c = current.get(relativePath);
  const upstreamOrigin = a && b ? (a.sha256 === b.sha256 ? 'both-identical' : 'both-conflict') : a ? 'opennt-only' : b ? 'opennt45-only' : 'current-only';
  const rawOpenNt = a && c && c.sha256 === a.sha256;
  const rawOpenNt45 = b && c && c.sha256 === b.sha256;
  const normalizedOpenNt = a && c && a.normalizedSha256 && a.normalizedSha256 === c.normalizedSha256;
  const normalizedOpenNt45 = b && c && b.normalizedSha256 && b.normalizedSha256 === c.normalizedSha256;
  const currentIdentity = !c ? 'not-currently-mirrored' : rawOpenNt && (!b || a.sha256 === b.sha256) ? 'exact-opennt-selected-or-identical' : rawOpenNt45 && (!a || a.sha256 === b.sha256) ? 'exact-opennt45-selected-or-identical' : rawOpenNt ? 'exact-opennt-on-conflict' : rawOpenNt45 ? 'exact-opennt45-on-conflict' : normalizedOpenNt && (!b || a.normalizedSha256 === b.normalizedSha256) ? 'formatting-only-opennt-selected-or-identical' : normalizedOpenNt45 && (!a || a.normalizedSha256 === b.normalizedSha256) ? 'formatting-only-opennt45-selected-or-identical' : normalizedOpenNt ? 'formatting-only-opennt-on-conflict' : normalizedOpenNt45 ? 'formatting-only-opennt45-on-conflict' : upstreamOrigin === 'current-only' ? 'current-only-needs-provenance' : 'mirror-different-needs-divergence-review';
  const graphSelection = [...selections.entries()].filter(([, selected]) => selected.has(relativePath.toLowerCase())).map(([label]) => label).join('+') || 'not-directly-compiled';
  return { relative_path: relativePath, upstream_origin: upstreamOrigin, opennt_sha256: a?.sha256 ?? '', opennt45_sha256: b?.sha256 ?? '', current_sha256: c?.sha256 ?? '', current_identity: currentIdentity, graph_compile_selection: graphSelection, audit_disposition: disposition(relativePath, c, graphSelection) };
});
const counts = (field) => Object.entries(Object.groupBy(rows, (row) => row[field])).map(([classification, values]) => ({ classification, count: values.length })).sort((a, b) => a.classification.localeCompare(b.classification));
const summary = { rows: rows.length, upstream_origin: counts('upstream_origin'), current_identity: counts('current_identity'), graph_compile_selection: counts('graph_compile_selection'), audit_disposition: counts('audit_disposition'), limitation: 'Identity and full Ninja-graph compile-edge inventory only. Graph selection does not prove an object reaches a product target; disposition labels do not prove archive extraction/final linkage or runtime reachability.' };
await mkdir(output, { recursive: false });
const columns = Object.keys(rows[0]);
await writeFile(path.join(output, 'mvdm-union.csv'), `${columns.join(',')}\n${rows.map((row) => columns.map((column) => csv(row[column])).join(',')).join('\n')}\n`);
await writeFile(path.join(output, 'summary.json'), `${JSON.stringify(summary, null, 2)}\n`);
process.stdout.write(`${JSON.stringify(summary)}\n`);
