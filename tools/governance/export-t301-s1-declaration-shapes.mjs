import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');
const roots = {
  OpenNT: 'O:\\repos.external\\OpenNT',
  'OpenNT-4.5': 'O:\\repos.external\\OpenNT-4.5',
};
function parseTsv(fileName) {
  const lines = fs.readFileSync(path.join(operations, fileName), 'utf8').replace(/^\uFEFF/, '').split(/\r?\n/).filter(Boolean);
  const split = (line) => { const row = []; let field = ''; let quoted = false; for (let index = 0; index < line.length; index += 1) { const ch = line[index]; if (ch === '"') { if (quoted && line[index + 1] === '"') { field += '"'; index += 1; } else quoted = !quoted; } else if (ch === '\t' && !quoted) { row.push(field); field = ''; } else field += ch; } row.push(field); return row; };
  const [header, ...body] = lines.map(split); return body.map((row) => Object.fromEntries(header.map((key, index) => [key, row[index] || ''])));
}
function writeTsv(fileName, rows, columns) { const quote = (value) => `"${String(value ?? '').replaceAll('"', '""')}"`; fs.writeFileSync(path.join(operations, fileName), [columns.join('\t'), ...rows.map((row) => columns.map((column) => quote(row[column])).join('\t'))].join('\n') + '\n'); }
function mask(text) { return text.replace(/\/\*[\s\S]*?\*\//g, (m) => m.replace(/[^\r\n]/g, ' ')).replace(/\/\/[^\r\n]*/g, (m) => m.replace(/[^\r\n]/g, ' ')); }
// P15/P16 establish the rebased frontier as the only live first-degree input.
const boundaries = parseTsv('mvdm-first-degree-rebased-boundary-ledger.tsv');
const declarations = parseTsv('mvdm-first-degree-rebaselined-declaration-frontier-ledger.tsv');
const byCandidate = new Map();
for (const row of declarations) byCandidate.set(row.candidate_id, [...(byCandidate.get(row.candidate_id) || []), row]);
const cache = new Map();
function shape(row, symbol) {
  const [edition, relative] = row.declaration_header_identity.split(':');
  const fileName = path.join(roots[edition], ...relative.split('@')[0].split('/'));
  const cacheKey = `${fileName}\u0000${row.declaration_line}`;
  if (cache.has(cacheKey)) return cache.get(cacheKey);
  if (!fs.existsSync(fileName)) return 'source-header-unavailable';
  const lines = mask(fs.readFileSync(fileName, 'utf8')).split(/\r?\n/); const index = Number(row.declaration_line) - 1;
  const window = lines.slice(Math.max(0, index - 1), Math.min(lines.length, index + 8)).join(' ');
  const result = /#\s*define\b/.test(window) ? 'macro-token' : new RegExp(`\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*\\(`).test(window) && /;|\{/.test(window) ? 'function-form-token' : 'unclassified-header-token';
  cache.set(cacheKey, result); return result;
}
const rows = boundaries.map((boundary) => {
  const forms = [...new Set((byCandidate.get(boundary.candidate_id) || []).map((row) => shape(row, boundary.callee_spelling)))];
  const disposition = forms.length === 1 && forms[0] === 'function-form-token' ? 'function-declaration-candidate; resolve compatible original body next' : forms.length === 1 && forms[0] === 'macro-token' ? 'macro-only-at-known-declaration-sites; retain as conditional-form evidence' : forms.length ? 'conditional-or-unclassified-declaration-form; preserve variants before body selection' : 'no-original-declaration-token; classify public leaf, hard boundary, finite shallow owner, or source unavailable';
  return { candidate_id: boundary.candidate_id, caller_source_path: boundary.caller_source_path, caller_source_line: boundary.caller_source_line, callee_spelling: boundary.callee_spelling, declaration_shapes: forms.join(';') || 'none', declaration_count: String((byCandidate.get(boundary.candidate_id) || []).length), next_disposition: disposition };
});
writeTsv('mvdm-first-degree-rebaselined-declaration-shape-ledger.tsv', rows, ['candidate_id', 'caller_source_path', 'caller_source_line', 'callee_spelling', 'declaration_shapes', 'declaration_count', 'next_disposition']);
const counts = new Map(); for (const row of rows) counts.set(row.next_disposition, (counts.get(row.next_disposition) || 0) + 1);
console.log([...counts].map(([name, count]) => `${name}=${count}`).join('; '));
