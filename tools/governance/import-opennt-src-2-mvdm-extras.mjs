/*
 * T301 source-root supplement importer.
 *
 * This is deliberately a path-and-hash copier, not a source selector: it
 * never overwrites a target and it keeps every source product under its
 * source-shaped component root.  The emitted manifest is the review record.
 */
import { createHash } from 'node:crypto';
import { execFile } from 'node:child_process';
import { cp, mkdir, readFile, readdir, stat, writeFile } from 'node:fs/promises';
import path from 'node:path';

const repo = path.resolve(import.meta.dirname, '..', '..');
const sourceRoot = 'O:\\repos.external\\opennt-src-2\\nt\\private\\mvdm';
const manifestPath = path.join(repo, 'docs', 'etc', 'operations',
  'm0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv');
const apply = process.argv.includes('--apply');
const gitTracked = new Set((await new Promise((resolve, reject) => {
  execFile('git', ['ls-files', '-z'], { cwd: repo }, (error, stdout) =>
    error ? reject(error) : resolve(stdout));
})).toString().split('\0').filter(Boolean).map(name => name.replaceAll('\\', '/')));

const sha256 = async file => createHash('sha256').update(await readFile(file)).digest('hex');
const walk = async root => {
  const result = [];
  const visit = async dir => {
    for (const entry of await readdir(dir, { withFileTypes: true })) {
      const full = path.join(dir, entry.name);
      if (entry.isDirectory()) await visit(full);
      else if (entry.isFile()) result.push(full);
    }
  };
  await visit(root);
  return result;
};

function targetFor(relative) {
  const normalized = relative.replaceAll('\\', '/');
  if (normalized.startsWith('bin86/')) return `src/mvdm-guest/${normalized}`;
  if (normalized.startsWith('dos/v86/')) return `src/mvdm-guest/${normalized}`;
  if (normalized.startsWith('wow16/')) return `src/mvdm-guest/${normalized}`;
  if (normalized.startsWith('tools16/')) return `tools/opennt/${normalized}`;
  if (normalized.startsWith('vdmutils/')) return `src/mvdm-tools/${normalized}`;
  if (/^(dirs|makefil0|inc\/|oemuni\/|suballoc\/)/.test(normalized)) {
    return `src/mvdm-support/${normalized}`;
  }
  if (/^softpc\.new\/(base\/(bios|keymouse)\/|bios\/|roms\/|data\/)/.test(normalized)) {
    return `src/mvdm-softpc-firmware/${normalized}`;
  }
  return `src/mvdm-host/${normalized}`;
}

const files = await walk(sourceRoot);
const rows = [];
for (const source of files.sort()) {
  const relative = path.relative(sourceRoot, source).replaceAll('\\', '/');
  const targetRelative = targetFor(relative);
  const target = path.join(repo, targetRelative);
  let disposition = 'missing-target';
  let targetHash = '';
  try {
    targetHash = await sha256(target);
    disposition = targetHash === await sha256(source)
      ? (gitTracked.has(targetRelative.replaceAll('\\', '/'))
        ? 'already-tracked-identical'
        : 'supplement-imported-verified')
      : 'target-different-no-overwrite';
  } catch {}
  const info = await stat(source);
  rows.push({ relative, targetRelative: targetRelative.replaceAll('\\', '/'), disposition,
    size: info.size, sourceHash: await sha256(source), targetHash });
}

if (apply) {
  for (const row of rows.filter(row => row.disposition === 'missing-target')) {
    const source = path.join(sourceRoot, ...row.relative.split('/'));
    const target = path.join(repo, ...row.targetRelative.split('/'));
    await mkdir(path.dirname(target), { recursive: true });
    await cp(source, target, { force: false, errorOnExist: true });
  }
}

const header = [
  'source_root', 'source_relative_path', 'target_path', 'disposition',
  'size_bytes', 'source_sha256', 'target_sha256', 'copy_applied'
].join('\t');
const data = rows.map(row => [sourceRoot, row.relative, row.targetRelative, row.disposition,
  row.size, row.sourceHash, row.targetHash,
  row.disposition === 'supplement-imported-verified' ? 'yes' : 'no'
].join('\t'));
await writeFile(manifestPath, `${header}\n${data.join('\n')}\n`);
const summary = Object.groupBy(rows, row => row.disposition);
for (const [name, group] of Object.entries(summary)) console.log(`${name}\t${group.length}`);
console.log(`manifest\t${path.relative(repo, manifestPath)}`);
