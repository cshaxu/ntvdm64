import fs from 'node:fs';
import path from 'node:path';

function fail(message) {
  throw new Error(message);
}

function readLines(file) {
  return fs.readFileSync(file, 'utf8').replace(/\r\n/g, '\n').split('\n');
}

function tsv(value) {
  return String(value ?? '').replace(/[\t\r\n]/g, ' ');
}

function canonicalType(type) {
  const aliases = new Map([
    ['VOID', 'void'], ['void', 'void'],
    ['half_word', 'u8'], ['IU8', 'u8'],
    ['word', 'u16'], ['IU16', 'u16'],
    ['double_word', 'u32'], ['IU32', 'u32'],
    ['INT', 's32'], ['IBOOL', 's32'], ['ISM32', 's32'],
    ['IUH', 'u32']
  ]);
  return aliases.get(type) ?? type;
}

function canonicalParameters(parameters) {
  if (!parameters.trim()) return '';
  const tokens = parameters.split(',').map((value) => value.trim());
  const types = [];
  for (let index = 0; index < tokens.length; index += 2) {
    types.push(canonicalType(tokens[index]));
  }
  return types.join(',');
}

/* The source carries several historical CPU-engine selections with identical
 * external symbol spellings.  The formal T310 graph selects CCPU plus the
 * CPU-30 profile; a bare name scan must not let an inactive A2/A3 declaration
 * overwrite the active CCPU carrier. */
function selectedCcpuLines(lines) {
  const defined = new Set([
    'CCPU', 'CPU_30_STYLE', 'CPU_40_STYLE', 'NEW_CPU', 'C_VID', 'SPC386',
    'SIM32', 'ANSI', 'PROD', 'WIN32', 'WINNT', 'NTVDM'
  ]);
  const stack = [];
  let active = true;
  const result = [];
  for (const line of lines) {
    const directive = line.match(/^\s*#\s*(ifdef|ifndef|if|elif|else|endif)\b\s*(.*)$/);
    if (directive) {
      const [, kind, argument] = directive;
      if (kind === 'ifdef' || kind === 'ifndef') {
        const symbol = argument.trim().split(/\s+/)[0];
        const condition = kind === 'ifdef' ? defined.has(symbol) : !defined.has(symbol);
        stack.push({ parent: active, taken: condition });
        active = active && condition;
      } else if (kind === 'if') {
        const condition = argument.trim() === '0' ? false : argument.trim() === '1';
        stack.push({ parent: active, taken: condition });
        active = active && condition;
      } else if (kind === 'elif') {
        const state = stack.at(-1);
        if (!state) fail(`Unmatched #elif: ${line}`);
        const condition = argument.trim() === '1';
        active = state.parent && !state.taken && condition;
        state.taken ||= condition;
      } else if (kind === 'else') {
        const state = stack.at(-1);
        if (!state) fail(`Unmatched #else: ${line}`);
        active = state.parent && !state.taken;
        state.taken = true;
      } else {
        const state = stack.pop();
        if (!state) fail(`Unmatched #endif: ${line}`);
        active = state.parent;
      }
      result.push(false);
    } else {
      result.push(active);
    }
  }
  if (stack.length) fail('Unclosed preprocessor conditional in selected CCPU carrier.');
  return result;
}

function findFunctionBody(lines, needle) {
  const start = lines.findIndex((line) => line.includes(needle));
  if (start < 0) fail(`Cannot find ${needle}`);
  let depth = 0;
  let opened = false;
  for (let index = start; index < lines.length; index += 1) {
    for (const character of lines[index]) {
      if (character === '{') {
        depth += 1;
        opened = true;
      } else if (character === '}') {
        depth -= 1;
        if (opened && depth === 0) return lines.slice(start, index + 1);
      }
    }
  }
  fail(`Cannot close ${needle}`);
}

function declarations(lines, selected) {
  const table = new Map();
  for (let index = 0; index < lines.length; index += 1) {
    if (!selected[index]) continue;
    const match = lines[index].match(/^\s*GLOBAL\s+([A-Za-z_][A-Za-z0-9_]*)\s+\(\*([A-Za-z_][A-Za-z0-9_]*_func)\)\s*\(([^)]*)\);/);
    if (match) table.set(match[2], {
      returnType: match[1],
      parameters: match[3].trim(),
      line: index + 1
    });
  }
  return table;
}

function generatedDeclarations(lines, selected) {
  const table = new Map();
  for (let index = 0; index < lines.length; index += 1) {
    if (!selected[index]) continue;
    const match = lines[index].match(/^\s*IMPORT\s+([A-Za-z_][A-Za-z0-9_]*)\s+(c_[A-Za-z0-9_]+)\s+IPT(\d+)\((.*)\);/);
    if (match) table.set(match[2], { returnType: match[1], arity: match[3], parameters: match[4], line: index + 1 });
  }
  return table;
}

function publicDeclarations(lines, selected) {
  const table = new Map();
  for (let index = 0; index < lines.length; index += 1) {
    if (!selected[index]) continue;
    const match = lines[index].match(/^\s*IMPORT\s+([A-Za-z_][A-Za-z0-9_]*)\s+(c_[A-Za-z0-9_]+)\s+IPT(\d+)\((.*)\);/);
    if (match) table.set(match[2], { returnType: match[1], arity: match[3], parameters: match[4], line: index + 1 });
  }
  return table;
}

const [rootArgument, outputArgument] = process.argv.slice(2);
if (!rootArgument || !outputArgument) {
  fail('Usage: node Export-T310CcpuAccessTableLedger.mjs <repository-root> <output-tsv>');
}
const root = path.resolve(rootArgument);
const sourcePath = path.join(root, 'src/mvdm-host/softpc.new/host/src/nt_cprgs.c');
const generatedPath = path.join(root, 'src/mvdm-host/softpc.new/base/ccpu386/cpu4gen.h');
const publicPath = path.join(root, 'src/mvdm-host/softpc.new/base/inc/cpu.h');
const sourceLines = readLines(sourcePath);
const slots = declarations(sourceLines, selectedCcpuLines(sourceLines));
const selectedBody = findFunctionBody(sourceLines, 'load_sw_cpu_access_functions');
const generatedLines = readLines(generatedPath);
const publicLines = readLines(publicPath);
const generated = generatedDeclarations(generatedLines, selectedCcpuLines(generatedLines));
const publicContract = publicDeclarations(publicLines, selectedCcpuLines(publicLines));
const assignments = [];
for (let index = 0; index < selectedBody.length; index += 1) {
  const match = selectedBody[index].match(/^\s*([A-Za-z_][A-Za-z0-9_]*_func)\s*=\s*(c_[A-Za-z0-9_]+)\s*;/);
  if (match) assignments.push({ slot: match[1], implementation: match[2], sourceLine: sourceLines.indexOf(selectedBody[index]) + 1 });
}
const rows = [
  ['slot', 'slot_declaration_line', 'slot_return_type', 'slot_canonical_return_type', 'selected_implementation', 'selected_assignment_line', 'generated_declaration_line', 'generated_return_type', 'generated_canonical_return_type', 'generated_arity', 'generated_parameters', 'generated_canonical_parameters', 'public_declaration_line', 'public_return_type', 'public_canonical_return_type', 'public_arity', 'public_parameters', 'public_canonical_parameters', 'review_focus'].join('\t')
];
for (const assignment of assignments) {
  const slot = slots.get(assignment.slot);
  const generatedContract = generated.get(assignment.implementation);
  const publicEntry = publicContract.get(assignment.implementation);
  const slotReturn = canonicalType(slot?.returnType ?? '');
  const generatedReturn = canonicalType(generatedContract?.returnType ?? '');
  const generatedParameters = canonicalParameters(generatedContract?.parameters ?? '');
  const slotParameters = slot?.parameters ?? '';
  const slotIsUnprototyped = slot && slotParameters === '';
  const slotParametersCanonical = canonicalParameters(slotParameters === 'void' ? '' : slotParameters);
  const publicReturn = canonicalType(publicEntry?.returnType ?? '');
  const publicParameters = canonicalParameters(publicEntry?.parameters ?? '');
  const returnMismatch = slot && generatedContract && slotReturn !== generatedReturn;
  const parameterMismatch = slot && generatedContract && !slotIsUnprototyped &&
    slotParametersCanonical !== generatedParameters;
  const publicMismatch = publicEntry && generatedContract && (publicReturn !== generatedReturn || publicEntry.arity !== generatedContract.arity || publicParameters !== generatedParameters);
  const focus = !slot || !generatedContract
    ? 'missing-selected-contract-carrier'
    : returnMismatch
      ? 'selected-return-contract-mismatch'
      : publicMismatch
        ? 'selected-public-generated-contract-mismatch'
        : parameterMismatch
          ? 'selected-parameter-contract-mismatch'
          : slotIsUnprototyped && generatedContract.arity !== '0'
          ? 'selected-unprototyped-slot-requires-typed-contract'
          : 'selected-zero-argument-contract-observed';
  rows.push([
    assignment.slot,
    slot?.line ?? '',
    slot?.returnType ?? '',
    slotReturn,
    assignment.implementation,
    assignment.sourceLine,
    generatedContract?.line ?? '',
    generatedContract?.returnType ?? '',
    generatedReturn,
    generatedContract?.arity ?? '',
    generatedContract?.parameters ?? '',
    generatedParameters,
    publicEntry?.line ?? '',
    publicEntry?.returnType ?? '',
    publicReturn,
    publicEntry?.arity ?? '',
    publicEntry?.parameters ?? '',
    publicParameters,
    focus
  ].map(tsv).join('\t'));
}
fs.mkdirSync(path.dirname(path.resolve(outputArgument)), { recursive: true });
fs.writeFileSync(path.resolve(outputArgument), `${rows.join('\n')}\n`);
console.log(JSON.stringify({ selected_assignments: assignments.length, review_rows: rows.length - 1 }));
