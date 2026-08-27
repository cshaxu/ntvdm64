import crypto from 'node:crypto';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';

const repositoryRoot = process.argv[2] || process.cwd();
const guestRoot = path.join(repositoryRoot, 'src', 'mvdm-guest');
const manifestPath = path.join(repositoryRoot, 'docs', 'etc', 'operations', 'm0-t297-s1-guest-root-move-manifest.tsv');
const duplicateManifestPath = path.join(repositoryRoot, 'docs', 'etc', 'operations', 'm0-t297-s1-guest-root-duplicate-elision-manifest.tsv');
const temporaryDosRoot = path.join(guestRoot, '.dos-v86-staging');

const moves = [
  ['win16/bin86', 'bin86'],
  ['win16/wow16', 'wow16'],
  ['win16/drivers', 'wow16/drivers'],
  ['win16/inc', 'wow16/inc'],
  ['win16/system', 'wow16/system'],
  ['win16/timer', 'wow16/timer'],
  ['win16/wfwnet', 'wow16/wfwnet'],
  ['win16/fonts', 'font16'],
];

// These pre-T297 paths duplicated the canonical WOW16 carry.  They are not
// copied twice into the final mirror: every entry must remain byte-identical
// to the named canonical target, and the separate ledger preserves its
// original path identity for the complete pre-move provenance record.
const duplicateElisions = [
  ['win16/drivers/comm/comm.drv', 'wow16/drivers/comm/comm.drv'],
  ['win16/drivers/display/vga.drv', 'wow16/drivers/display/vga.drv'],
  ['win16/drivers/keyboard/keyboard.drv', 'wow16/drivers/keyboard/keyboard.drv'],
  ['win16/drivers/mouse/mouse.drv', 'wow16/drivers/mouse/mouse.drv'],
  ['win16/drivers/sound/sound.drv', 'wow16/drivers/sound/sound.drv'],
  ['win16/inc/dosx.inc', 'wow16/inc/dosx.inc'],
  ['win16/inc/int31.inc', 'wow16/inc/int31.inc'],
  ['win16/system/system.drv', 'wow16/system/system.drv'],
  ['win16/timer/TIMER.DRV', 'wow16/timer/TIMER.DRV'],
  ['win16/wfwnet/WFWNET.DRV', 'wow16/wfwnet/WFWNET.DRV'],
];

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
function quote(value) { return `"${String(value).replaceAll('"', '""')}"`; }
function writeTsv(fileName, rows, columns) {
  const content = [columns.join('\t'), ...rows.map((row) => columns.map((column) => quote(row[column] || '')).join('\t'))].join('\r\n') + '\r\n';
  fs.writeFileSync(fileName, content, 'utf8');
}
function trackedBelow(relativePath) {
  return execFileSync('git', ['ls-files', '--', `src/mvdm-guest/${relativePath}`], { cwd: repositoryRoot, encoding: 'utf8' }).trim().length > 0;
}
function moveTree(sourceRelativePath, targetRelativePath) {
  const source = path.join(guestRoot, sourceRelativePath);
  const target = path.join(guestRoot, targetRelativePath);
  if (!fs.existsSync(source)) {
    if (fs.existsSync(target)) return;
    throw new Error(`Missing source tree: ${sourceRelativePath}`);
  }
  if (fs.existsSync(target)) {
    // The supplemental Win16 roots overlap the complete WOW16 source carry.
    // They can disappear only when every file has an equal canonical target;
    // a differing or unique byte refuses the merge rather than being replaced.
    for (const fileName of filesBelow(source)) {
      const relativePath = path.relative(source, fileName);
      const counterpart = path.join(target, relativePath);
      if (!fs.existsSync(counterpart) || sha256(fileName) !== sha256(counterpart)) {
        throw new Error(`Refusing non-identical duplicate merge: ${sourceRelativePath}/${relativePath}`);
      }
    }
    if (trackedBelow(sourceRelativePath)) {
      execFileSync('git', ['rm', '-r', '-f', '--', `src/mvdm-guest/${sourceRelativePath}`], { cwd: repositoryRoot, stdio: 'inherit' });
    }
    if (fs.existsSync(source)) fs.rmSync(source, { recursive: true, force: false });
    return;
  }
  fs.mkdirSync(path.dirname(target), { recursive: true });
  if (trackedBelow(sourceRelativePath)) {
    execFileSync('git', ['mv', `src/mvdm-guest/${sourceRelativePath}`, `src/mvdm-guest/${targetRelativePath}`], { cwd: repositoryRoot, stdio: 'inherit' });
  } else {
    fs.renameSync(source, target);
  }
}

if (!fs.existsSync(path.join(guestRoot, 'dos'))) throw new Error('Missing DOS guest root');
if (fs.existsSync(temporaryDosRoot)) throw new Error('Unexpected existing DOS staging root');

const before = [];
for (const fileName of filesBelow(guestRoot)) {
  const relativePath = path.relative(guestRoot, fileName).replaceAll('\\', '/');
  if (relativePath === 'README.md') continue;
  let sourceRelativePath = relativePath;
  if (relativePath.startsWith('dos/v86/')) sourceRelativePath = `dos/${relativePath.slice(8)}`;
  else if (relativePath.startsWith('bin86/')) sourceRelativePath = `win16/bin86/${relativePath.slice(6)}`;
  else if (relativePath.startsWith('wow16/')) sourceRelativePath = `win16/wow16/${relativePath.slice(6)}`;
  else if (relativePath.startsWith('font16/')) sourceRelativePath = `win16/fonts/${relativePath.slice(7)}`;
  before.push({ source_relative_path: sourceRelativePath, sha256: sha256(fileName) });
}

// `dos` becomes the parent of `dos/v86`, so use a temporary sibling to keep
// this path-only operation explicit and let Git preserve tracked-file history.
if (!fs.existsSync(path.join(guestRoot, 'dos', 'v86'))) {
  moveTree('dos', '.dos-v86-staging');
  fs.mkdirSync(path.join(guestRoot, 'dos'));
  moveTree('.dos-v86-staging', 'dos/v86');
}
for (const [source, target] of moves) moveTree(source, target);
if (fs.existsSync(path.join(guestRoot, 'win16')) && fs.readdirSync(path.join(guestRoot, 'win16')).length === 0) fs.rmdirSync(path.join(guestRoot, 'win16'));

const rows = [];
for (const item of before) {
  let targetRelativePath;
  if (item.source_relative_path.startsWith('dos/')) targetRelativePath = `dos/v86/${item.source_relative_path.slice(4)}`;
  else if (item.source_relative_path.startsWith('win16/bin86/')) targetRelativePath = `bin86/${item.source_relative_path.slice(12)}`;
  else if (item.source_relative_path.startsWith('win16/wow16/')) targetRelativePath = `wow16/${item.source_relative_path.slice(12)}`;
  else if (/^win16\/(drivers|inc|system|timer|wfwnet)\//.test(item.source_relative_path)) targetRelativePath = `wow16/${item.source_relative_path.slice(6)}`;
  else if (item.source_relative_path.startsWith('win16/fonts/')) targetRelativePath = `font16/${item.source_relative_path.slice(12)}`;
  else throw new Error(`Unexpected pre-move guest file: ${item.source_relative_path}`);
  const target = path.join(guestRoot, targetRelativePath);
  if (!fs.existsSync(target)) throw new Error(`Move target missing: ${targetRelativePath}`);
  const targetHash = sha256(target);
  if (targetHash !== item.sha256) throw new Error(`Hash changed during move: ${item.source_relative_path}`);
  rows.push({ source_relative_path: item.source_relative_path, target_relative_path: targetRelativePath, sha256: item.sha256, move_kind: 'path-only-byte-identical-guest-mirror' });
}
writeTsv(manifestPath, rows.sort((left, right) => left.source_relative_path.localeCompare(right.source_relative_path)), ['source_relative_path', 'target_relative_path', 'sha256', 'move_kind']);
const duplicateRows = duplicateElisions.map(([sourceRelativePath, targetRelativePath]) => {
  const target = path.join(guestRoot, targetRelativePath);
  if (!fs.existsSync(target)) throw new Error(`Duplicate-elision target missing: ${targetRelativePath}`);
  return {
    source_relative_path: sourceRelativePath,
    canonical_target_relative_path: targetRelativePath,
    sha256: sha256(target),
    disposition: 'deduplicated-only-after-byte-identical-verification',
  };
});
writeTsv(duplicateManifestPath, duplicateRows, ['source_relative_path', 'canonical_target_relative_path', 'sha256', 'disposition']);
console.log(`canonical guest files moved: ${rows.length}; verified duplicate elisions: ${duplicateRows.length}`);
