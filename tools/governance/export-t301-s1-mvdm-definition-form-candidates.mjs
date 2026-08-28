import childProcess from 'node:child_process';
import crypto from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const roots = [
  'O:\\repos.external\\OpenNT\\base\\mvdm',
  'O:\\repos.external\\OpenNT-4.5\\nt\\private\\mvdm',
];

function readTsv(name) {
  const split = (line) => {
    const fields = []; let field = ''; let quoted = false;
    for (let index = 0; index < line.length; index += 1) {
      const character = line[index];
      if (character === '"') {
        if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted;
      } else if (character === '\t' && !quoted) { fields.push(field); field = ''; } else field += character;
    }
    fields.push(field); return fields;
  };
  const [header, ...body] = fs.readFileSync(path.join(operations, name), 'utf8').split(/\r?\n/).filter(Boolean).map(split);
  return body.map((fields) => Object.fromEntries(header.map((key, index) => [key, fields[index] || ''])));
}

function quote(value) { return `"${String(value ?? '').replaceAll('"', '""')}"`; }
function normal(value) { return path.resolve(value).replaceAll('\\', '/').toLowerCase(); }
function owningRoot(sourcePath) { return roots.find((root) => normal(sourcePath).startsWith(`${normal(root)}/`)); }

function indexOriginalDefinitions() {
  const indexPath = path.join(os.tmpdir(), 'ntvdm64-t301-mvdm-definitions.jsonl');
  fs.rmSync(indexPath, { force: true });
  const result = childProcess.spawnSync('ctags', [
    '-R', '--languages=C,C++', '--kinds-C=f', '--kinds-C++=f', '--fields=+nKSt',
    '--output-format=json', '-f', indexPath, ...roots,
  ], { encoding: 'utf8' });
  if (result.status !== 0) throw new Error(result.stderr || 'ctags original-definition index failed');
  const records = fs.readFileSync(indexPath, 'utf8').split(/\r?\n/).filter(Boolean).map((line) => JSON.parse(line));
  fs.rmSync(indexPath, { force: true });
  return records.filter((record) => record._type === 'tag' && record.kind === 'function');
}

function definitionForm(sourceFile, line, symbol) {
  const source = fs.readFileSync(sourceFile, 'utf8');
  const lines = source.split(/\r?\n/);
  const offset = lines.slice(0, line - 1).reduce((total, value) => total + value.length + 1, 0)
    + lines[line - 1].indexOf(symbol);
  const open = source.indexOf('(', offset + symbol.length);
  let depth = 0; let close = -1;
  for (let index = open; index < Math.min(source.length, open + 4096); index += 1) {
    if (source[index] === '(') depth += 1;
    if (source[index] === ')' && --depth === 0) { close = index; break; }
  }
  const tail = source.slice(close + 1, close + 513);
  const first = tail.search(/\S/);
  if (first >= 0 && tail[first] === '{') return 'ansi-body';
  const brace = tail.indexOf('{');
  return brace >= 0 && /^\s*(?:(?:[A-Za-z_]\w*|\*|\s)+\s+[A-Za-z_]\w*\s*;\s*)+\{/.test(tail.slice(0, brace + 1))
    ? 'k-and-r-body' : 'ctags-function-body';
}

const names = new Set(readTsv('mvdm-first-degree-rebaselined-declaration-shape-ledger.tsv')
  .filter((row) => row.next_disposition.startsWith('function-declaration-candidate'))
  .map((row) => row.callee_spelling));
const cache = new Map();
const rows = indexOriginalDefinitions()
  .filter((record) => names.has(record.name) && owningRoot(record.path))
  .map((record) => {
    const root = owningRoot(record.path);
    if (!cache.has(record.path)) cache.set(record.path, crypto.createHash('sha256').update(fs.readFileSync(record.path)).digest('hex'));
    return {
      symbol: record.name,
      source_root: root,
      source_path: path.relative(root, record.path).replaceAll('\\', '/'),
      source_sha256: cache.get(record.path),
      source_line: String(record.line),
      definition_form: definitionForm(record.path, record.line, record.name),
    };
  })
  .sort((left, right) => left.symbol.localeCompare(right.symbol)
    || left.source_path.localeCompare(right.source_path)
    || Number(left.source_line) - Number(right.source_line));

const columns = ['symbol', 'source_root', 'source_path', 'source_sha256', 'source_line', 'definition_form'];
const output = path.join(operations, 'mvdm-first-degree-rebaselined-mvdm-definition-form-candidate-ledger.tsv');
fs.writeFileSync(output, `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
console.log(`requested spellings=${names.size}; physical original MVDM definition candidates=${rows.length}; resolved spellings=${new Set(rows.map((row) => row.symbol)).size}`);
