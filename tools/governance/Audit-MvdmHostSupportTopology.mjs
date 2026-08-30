import { createHash } from 'node:crypto';
import { existsSync, readdirSync, readFileSync, statSync, writeFileSync } from 'node:fs';
import { join, relative, sep } from 'node:path';

const repositoryRoot = process.cwd();
const sourceRootRelative = process.argv[2] ?? 'src/mvdm-host';
const supportRoot = join(repositoryRoot, sourceRootRelative);
const origins = [
  ['opennt', 'O:/repos.external/opennt/base/mvdm'],
  ['opennt-src-2', 'O:/repos.external/opennt-src-2/nt/private/mvdm'],
];
const output = process.argv[3];
const selectedRoots = ['dirs', 'makefil0', 'inc', 'oemuni', 'suballoc'];

function walk(directory) {
  const result = [];
  for (const entry of readdirSync(directory, { withFileTypes: true })) {
    const path = join(directory, entry.name);
    if (entry.isDirectory()) {
      result.push(...walk(path));
    } else {
      result.push(path);
    }
  }
  return result;
}

function sha256(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

function fileKind(path) {
  const extension = path.slice(path.lastIndexOf('.') + 1).toLowerCase();
  if (['c', 'cc', 'cpp'].includes(extension)) return 'source';
  if (['h', 'inc'].includes(extension)) return 'declaration';
  if (['obj', 'lib', 'exe', 'dll'].includes(extension)) return 'historical-build-product';
  if (['makefile', 'sources', 'dirs', 'makefil0'].some((name) => path.endsWith(name))) return 'build-control';
  return 'other';
}

if (!existsSync(supportRoot)) {
  throw new Error(`Missing support root: ${supportRoot}`);
}

const rows = [
  ['relative_path', 'file_kind', 'current_sha256', ...origins.map(([name]) => `${name}_identity`)],
];

const selectedFiles = selectedRoots.flatMap((selectedRoot) => {
  const selectedPath = join(supportRoot, selectedRoot);
  if (!existsSync(selectedPath)) {
    throw new Error(`Missing selected original MVDM path: ${selectedPath}`);
  }
  return statSync(selectedPath).isDirectory() ? walk(selectedPath) : [selectedPath];
});

for (const path of selectedFiles.sort()) {
  const relativePath = relative(supportRoot, path).split(sep).join('/');
  const currentHash = sha256(path);
  const identity = origins.map(([name, root]) => {
    const originPath = join(root, relativePath);
    if (!existsSync(originPath)) return `${name}:missing`;
    return sha256(originPath) === currentHash ? `${name}:same` : `${name}:different`;
  });
  rows.push([relativePath, fileKind(relativePath), currentHash, ...identity]);
}

const text = `${rows.map((row) => row.join('\t')).join('\n')}\n`;
if (output) {
  writeFileSync(output, text, 'utf8');
} else {
  process.stdout.write(text);
}
