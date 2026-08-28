import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const fullBoundary = process.argv.includes('--full');
const operations = path.join(repository, 'docs', 'etc', 'operations');
const roots = {
  'O:\\repos.external\\OpenNT\\base\\mvdm': 'O:\\repos.external\\OpenNT\\base\\mvdm',
  'O:\\repos.external\\OpenNT-4.5\\nt\\private\\mvdm': 'O:\\repos.external\\OpenNT-4.5\\nt\\private\\mvdm',
};

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
function sourceFile(root, relative) { return path.join(roots[root], ...relative.split('/')); }
function headerFile(identity) {
  const [editionPath] = identity.split('@');
  if (editionPath.startsWith('OpenNT:')) return path.join('O:\\repos.external\\OpenNT', ...editionPath.slice('OpenNT:'.length).split('/'));
  if (editionPath.startsWith('OpenNT-4.5:')) return path.join('O:\\repos.external\\OpenNT-4.5', ...editionPath.slice('OpenNT-4.5:'.length).split('/'));
  return '';
}
function parenClose(text, open) {
  let depth = 0;
  for (let index = open; index < Math.min(text.length, open + 4096); index += 1) {
    if (text[index] === '(') depth += 1;
    if (text[index] === ')' && --depth === 0) return index;
  }
  return -1;
}
function argumentCount(text, open, close) {
  const value = text.slice(open + 1, close).trim();
  if (!value || value.toLowerCase() === 'void') return 0;
  let nesting = 0; let count = 1;
  for (const character of value) {
    if ('([{'.includes(character)) nesting += 1;
    else if (')]}'.includes(character)) nesting -= 1;
    else if (character === ',' && nesting === 0) count += 1;
  }
  return count;
}
function invocationAt(file, line, symbol) {
  const source = fs.readFileSync(file, 'utf8');
  const lines = source.split(/\r?\n/);
  const newlineWidth = source.includes('\r\n') ? 2 : 1;
  const start = lines.slice(0, line - 1).reduce((total, value) => total + value.length + newlineWidth, 0);
  const marked = source.indexOf(symbol, start);
  const prior = source.lastIndexOf(symbol, start);
  const offset = marked >= start && marked < start + 8192 ? marked : prior;
  if (offset < Math.max(0, start - 2048) || offset > start + 8192) return null;
  const open = source.indexOf('(', offset + symbol.length);
  const close = parenClose(source, open);
  return close < 0 ? null : { count: argumentCount(source, open, close), source };
}
function definitionInvocationAt(file, line, symbol) {
  const source = fs.readFileSync(file, 'utf8'); const lines = source.split(/\r?\n/);
  const newlineWidth = source.includes('\r\n') ? 2 : 1;
  const start = lines.slice(0, line - 1).reduce((total, value) => total + value.length + newlineWidth, 0);
  const candidates = [];
  let offset = source.lastIndexOf(symbol, start + 8192);
  while (offset >= Math.max(0, start - 2048)) {
    const open = source.indexOf('(', offset + symbol.length); const close = parenClose(source, open); const body = source.indexOf('{', close);
    if (!(close < 0 || body < 0 || body > close + 1024 || body < start - 2048 || body > start + 8192)) {
      candidates.push({ source, count: argumentCount(source, open, close), distance: Math.abs(body - start) });
    }
    offset = source.lastIndexOf(symbol, offset - 1);
  }
  if (!candidates.length) return null;
  candidates.sort((left, right) => left.distance - right.distance); return candidates[0];
}
function conditionContext(source, line) {
  const stack = [];
  for (const value of source.split(/\r?\n/).slice(0, line)) {
    const directive = value.match(/^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)$/);
    if (!directive) continue;
    const [, kind, tail] = directive;
    if (kind === 'if' || kind === 'ifdef' || kind === 'ifndef') stack.push(`${kind}:${tail.trim()}`);
    else if (kind === 'elif' || kind === 'else') { if (stack.length) stack[stack.length - 1] = `${kind}:${tail.trim()}`; }
    else if (kind === 'endif') stack.pop();
  }
  return stack.join(' > ') || 'unconditional';
}

const bindingInput = fullBoundary
  ? 'mvdm-first-degree-rebaselined-full-mvdm-boundary-gate-ledger.tsv'
  : 'mvdm-first-degree-rebaselined-mvdm-binding-gate-ledger.tsv';
const bindings = readTsv(bindingInput)
  .filter((row) => (fullBoundary ? row.selectable_original_mvdm_definition_count : row.selectable_definition_count) === '1');
const declarations = new Map();
for (const row of readTsv('mvdm-first-degree-rebaselined-declaration-frontier-ledger.tsv')) {
  declarations.set(row.candidate_id, [...(declarations.get(row.candidate_id) || []), row]);
}
const identities = new Map(readTsv('mvdm-first-degree-rebaselined-mvdm-definition-form-candidate-ledger.tsv')
  .map((row) => [`${row.source_root}|${row.source_path}|${row.source_sha256}|${row.source_line}`, row]));

const rows = bindings.map((binding) => {
  const definitionIdentity = fullBoundary
    ? binding.selectable_original_mvdm_definition_identities : binding.selectable_definition_identities;
  const definition = fullBoundary
    ? (() => {
      const [source_root, source_path, source_sha256, source_line] = definitionIdentity.split('|');
      return { source_root, source_path, source_sha256, source_line, symbol: binding.callee_spelling };
    })()
    : identities.get(definitionIdentity);
  if (!definition) throw new Error(`missing selected definition ${definitionIdentity}`);
  const caller = invocationAt(sourceFile(binding.caller_source_root, binding.caller_source_path), Number(binding.caller_source_line), binding.callee_spelling);
  const body = definitionInvocationAt(sourceFile(definition.source_root, definition.source_path), Number(definition.source_line), definition.symbol);
  const headerRows = (declarations.get(binding.candidate_id) || []).filter((row) => headerFile(row.declaration_header_identity));
  const headerCounts = headerRows.map((row) => {
    const item = invocationAt(headerFile(row.declaration_header_identity), Number(row.declaration_line), binding.callee_spelling);
    return item ? item.count : null;
  }).filter((value) => value !== null);
  const sameTranslationUnit = binding.caller_source_path === definition.source_path
    && binding.caller_source_sha256 === definition.source_sha256;
  const headerMatch = caller && body && headerCounts.length
    && caller.count === body.count && headerCounts.includes(body.count);
  const localBodyMatch = fullBoundary && caller && body && sameTranslationUnit && caller.count === body.count;
  const arity = headerMatch || localBodyMatch;
  const callerContext = caller ? conditionContext(caller.source, Number(binding.caller_source_line)) : 'unresolved-call-form';
  const definitionSource = fs.readFileSync(sourceFile(definition.source_root, definition.source_path), 'utf8');
  const definitionContext = conditionContext(definitionSource, Number(definition.source_line));
  return {
    candidate_id: binding.candidate_id,
    caller_symbol: binding.caller_symbol,
    caller_source_path: binding.caller_source_path,
    caller_source_sha256: binding.caller_source_sha256,
    caller_source_line: binding.caller_source_line,
    callee_spelling: binding.callee_spelling,
    selected_definition_identity: definitionIdentity,
    call_argument_count: caller ? String(caller.count) : 'unresolved',
    declaration_argument_counts: headerCounts.join(';') || 'unresolved',
    definition_argument_count: body ? String(body.count) : 'unresolved',
    shape_basis: headerMatch ? 'reachable original declaration and definition' : localBodyMatch ? 'same-translation-unit original definition' : 'unresolved',
    caller_condition_context: callerContext,
    definition_condition_context: definitionContext,
    next_disposition: arity
      ? 'source call/declaration/definition arity confirmed; retain conditional contexts for profile binding and admit zero-degree closure rebase'
      : 'call/declaration/definition shape not fully confirmed; preserve first-degree record',
  };
});
const columns = Object.keys(rows[0]);
const output = fullBoundary
  ? 'mvdm-first-degree-rebaselined-full-mvdm-signature-gate-ledger.tsv'
  : 'mvdm-first-degree-rebaselined-mvdm-signature-gate-ledger.tsv';
fs.writeFileSync(path.join(operations, output), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
const passed = rows.filter((row) => row.next_disposition.startsWith('source call')).length;
console.log(`eligible bindings=${rows.length}; source shape confirmed=${passed}; shape unresolved=${rows.length - passed}`);
