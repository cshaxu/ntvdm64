import fs from 'node:fs';
import path from 'node:path';

const repository = process.argv[2] || process.cwd();
const operations = path.join(repository, 'docs', 'etc', 'operations');

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
function sourceFile(root, relative) { return path.join(root, ...relative.split('/')); }
function sourcePackage(root, relative) { const family = relative.split('/')[0]; return root.includes('OpenNT-4.5') ? `nt/private/mvdm/${family}` : `base/mvdm/${family}`; }
function parenClose(text, open) { let depth = 0; for (let index = open; index < Math.min(text.length, open + 4096); index += 1) { if (text[index] === '(') depth += 1; if (text[index] === ')' && --depth === 0) return index; } return -1; }
function argumentCount(text, open, close) { const value = text.slice(open + 1, close).trim(); if (!value || value.toLowerCase() === 'void') return 0; let nesting = 0; let count = 1; for (const character of value) { if ('([{'.includes(character)) nesting += 1; else if (')]}'.includes(character)) nesting -= 1; else if (character === ',' && nesting === 0) count += 1; } return count; }
function invocation(file, line, symbol) { const source = fs.readFileSync(file, 'utf8'); const lines = source.split(/\r?\n/); const start = lines.slice(0, line - 1).reduce((total, value) => total + value.length + 1, 0); const offset = source.indexOf(symbol, start); if (offset < start || offset > start + 4096) return null; const open = source.indexOf('(', offset + symbol.length); const close = parenClose(source, open); return close < 0 ? null : { source, count: argumentCount(source, open, close) }; }
const headerCache = new Map();
function headers(root) {
  if (headerCache.has(root)) return headerCache.get(root);
  const byBase = new Map();
  const visit = (directory) => { for (const entry of fs.readdirSync(directory, { withFileTypes: true })) { const item = path.join(directory, entry.name); if (entry.isDirectory()) visit(item); else if (/\.(h|inc)$/i.test(entry.name)) byBase.set(entry.name.toLowerCase(), [...(byBase.get(entry.name.toLowerCase()) || []), item]); } };
  visit(root); headerCache.set(root, byBase); return byBase;
}
function includePackages(root, source) {
  const pending = [source]; const seen = new Set(); const packages = new Set(); const headersSeen = [];
  while (pending.length) {
    const current = pending.pop(); if (seen.has(current) || !fs.existsSync(current)) continue; seen.add(current);
    for (const match of fs.readFileSync(current, 'utf8').matchAll(/^\s*#\s*include\s*["<]([^">]+)[">]/gm)) {
      const name = match[1]; const direct = [path.resolve(path.dirname(current), name), path.resolve(root, name)].find(fs.existsSync); const matches = headers(root).get(path.basename(name).toLowerCase()) || []; const selected = direct || (matches.length === 1 ? matches[0] : null);
      if (selected) { headersSeen.push(selected); packages.add(sourcePackage(root, path.relative(root, selected).replaceAll('\\', '/'))); pending.push(selected); }
    }
  }
  return { packages, headers: headersSeen };
}
function headerCounts(headers, symbol) {
  const counts = [];
  for (const header of headers) {
    const source = fs.readFileSync(header, 'utf8');
    for (const match of source.matchAll(new RegExp(`\\b${symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*\\(`, 'g'))) {
      const open = source.indexOf('(', match.index + symbol.length); const close = parenClose(source, open); if (close >= 0) counts.push(argumentCount(source, open, close));
    }
  }
  return [...new Set(counts)];
}

const edges = readTsv('mvdm-zero-degree-rebase-expansion-edge-ledger.tsv').filter((edge) => edge.internal_candidate_identity);
const rows = edges.map((edge) => {
  const [definition_root, definition_path, definition_sha256, definition_line] = edge.internal_candidate_identity.split('|');
  const callerFile = sourceFile(edge.caller_source_root, edge.caller_source_path); const definitionFile = sourceFile(definition_root, definition_path);
  const call = invocation(callerFile, Number(edge.call_source_line), edge.callee_spelling); const definition = invocation(definitionFile, Number(definition_line), edge.callee_spelling);
  const sameFile = edge.caller_source_root === definition_root && edge.caller_source_path === definition_path && edge.caller_source_sha256 === definition_sha256;
  const includes = includePackages(edge.caller_source_root, callerFile); const targetPackage = sourcePackage(definition_root, definition_path); const counts = sameFile ? [] : headerCounts(includes.headers, edge.callee_spelling);
  const sameTuMatch = sameFile && call && definition && call.count === definition.count;
  const headerMatch = !sameFile && includes.packages.has(targetPackage) && call && definition && counts.includes(call.count) && counts.includes(definition.count);
  return {
    caller_identity: edge.caller_identity,
    caller_source_path: edge.caller_source_path,
    call_source_line: edge.call_source_line,
    callee_spelling: edge.callee_spelling,
    selected_definition_identity: edge.internal_candidate_identity,
    call_argument_count: call ? String(call.count) : 'unresolved',
    definition_argument_count: definition ? String(definition.count) : 'unresolved',
    header_argument_counts: counts.join(';') || 'none',
    target_package_in_caller_frontier: String(includes.packages.has(targetPackage)),
    shape_basis: sameTuMatch ? 'same-translation-unit original definition' : headerMatch ? 'reachable original declaration and definition' : 'unresolved',
    next_disposition: (sameTuMatch || headerMatch) ? 'confirmed original-MVDM internal edge; admit zero-degree rebase' : 'unconfirmed internal candidate; retain for source-shape/frontier review',
  };
});
const columns = Object.keys(rows[0]);
fs.writeFileSync(path.join(operations, 'mvdm-zero-degree-rebase-expansion-edge-gate-ledger.tsv'), `${columns.join('\t')}\n${rows.map((row) => columns.map((column) => quote(row[column])).join('\t')).join('\n')}\n`);
const confirmed = rows.filter((row) => row.next_disposition.startsWith('confirmed')).length;
console.log(`internal expansion edges=${rows.length}; confirmed=${confirmed}; unresolved=${rows.length - confirmed}`);
