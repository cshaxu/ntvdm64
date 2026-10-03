// Physical separation gate: bodies must match the accepted S7 provider.
import fs from 'node:fs';
import {execFileSync} from 'node:child_process';
const prefix = 'src/ntsrv-exe/opennt/';
const baselineRevision = 'd83d2b212';
const modules = ['base_service', 'service_core', 'worker_registry',
  'frontend_registry', 'native_commands', 'lifecycle', 'management'];
const read = path => fs.readFileSync(path, 'utf8').replaceAll('\r\n', '\n');
const original = path => execFileSync('git', ['show', `${baselineRevision}:${path}`],
  {encoding: 'utf8', maxBuffer: 4 * 1024 * 1024}).replaceAll('\r\n', '\n');
function definitions(source) {
  const blank = s => s.replace(/[^\n]/g, ' ');
  const clean = source.replace(/\/\*[\s\S]*?\*\/|\/\/[^\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'/g, blank);
  const result = new Map();
  const pattern = /^(?:static )?[A-Za-z_][\w *]+\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{/gm;
  for (const match of clean.matchAll(pattern)) {
    const open = match.index + match[0].lastIndexOf('{');
    let end = open + 1, depth = 1;
    while (depth && end < clean.length) {
      if (clean[end] === '{') ++depth;
      if (clean[end] === '}') --depth;
      ++end;
    }
    if (depth || result.has(match[1])) throw Error('Duplicate/incomplete definition: ' + match[1]);
    result.set(match[1], {body: source.slice(open, end),
      signature: source.slice(match.index, open).trim().replace(/^static /, '')});
  }
  return result;
}
const accepted = definitions(original(prefix + 'source/base_service.c'));
if (accepted.size !== 137) throw Error('Accepted inventory must contain 137 definitions');
function verify(sources) {
  const actual = new Map();
  for (const [owner, source] of sources) {
    if (/^\s*#include\s*[<"][^>"\n]+\.c[>"]/m.test(source)) throw Error('Embedded implementation: ' + owner);
    const functions = definitions(source);
    if (!functions.size) throw Error('Empty private module: ' + owner);
    for (const [name, definition] of functions) {
      if (actual.has(name)) throw Error('Duplicate provider: ' + name);
      const prior = accepted.get(name);
      if (!prior || prior.body !== definition.body || prior.signature !== definition.signature)
        throw Error('Semantic/signature drift: ' + name);
      actual.set(name, owner);
    }
  }
  if (actual.size !== accepted.size) throw Error('Missing service definition');
}
const sources = new Map(modules.map(owner => [owner, read(prefix + 'source/' + owner + '.c')]));
verify(sources);
const controls = [
  source => source.replace('InitializeConditionVariable(', 'ChangedConditionVariable('),
  source => source + '\n' + sources.get('management'),
  source => source.replace(/OpenNtBaseServiceStart\(void\)/, 'OpenNtBaseServiceStart(int changed)'),
];
for (const mutation of controls) {
  const bad = new Map(sources);
  bad.set('service_core', mutation(bad.get('service_core')));
  let rejected = false;
  try { verify(bad); } catch { rejected = true; }
  if (!rejected) throw Error('Accepted semantic/duplicate/signature negative control');
}
const missing = new Map(sources);
missing.delete('management');
let rejected = false;
try { verify(missing); } catch { rejected = true; }
if (!rejected) throw Error('Accepted missing-module negative control');
const header = read(prefix + 'include/service_internal.h');
const old = original(prefix + 'source/base_service.c');
for (const type of ['OPENNT_BASE_SERVICE', 'OPENNT_BASE_CONNECTION', 'OPENNT_BASE_WIN32RECORD',
  'OPENNT_FRONTEND_ROUTE', 'OPENNT_BASE_WORKER_WATCH', 'OPENNT_BASE_MANAGEMENT_LABEL',
  'OPENNT_BASE_CONSOLE_CONTEXT', 'OPENNT_BASE_SERVICE_RESOURCES']) {
  const suffix = type === 'OPENNT_BASE_SERVICE' || type === 'OPENNT_BASE_CONNECTION' ? ';' : ' ' + type + ';';
  const expression = new RegExp('^(?:typedef )?struct ' + type + ' \\{[\\s\\S]*?^}' + suffix, 'm');
  const prior = old.match(expression)?.[0], current = header.match(expression)?.[0];
  if (!prior || prior !== current) throw Error('Private state layout drift: ' + type);
}
for (const path of ['src/opennt-host/base/win32/server/srvvdm.c',
  'src/opennt-host/base/win32/server/srvinit.c', 'src/opennt-host/base/win32/client/vdm.c',
  prefix + 'include/base_service.h', 'src/common/protocol/service.idl', 'src/common/protocol/version.h']) {
  if (read(path) !== original(path)) throw Error('Original/public-contract drift: ' + path);
}
if (/include[^\n]+base_service\.c/.test(read('tests/adapter-basesrv/base_service_fixture.c')))
  throw Error('Fixture embeds a second service provider');
const consumers = execFileSync('rg', ['-l', '#include [<"]service_internal.h[>"]', 'src', 'tests'],
  {encoding: 'utf8'}).trim().split(/\r?\n/).map(path => path.replaceAll('\\', '/')).sort();
const expected = modules.map(owner => prefix + 'source/' + owner + '.c')
  .concat('tests/adapter-basesrv/base_service_fixture.c').sort();
if (JSON.stringify(consumers) !== JSON.stringify(expected)) throw Error('Private state consumer leakage');
if (process.argv[2]) {
  const graph = read(process.argv[2] + '/build.ninja');
  const archive = graph.split('\n').find(line => line.startsWith('build opennt-base-bindings.lib:'));
  if (!archive) throw Error('Missing actual production archive');
  for (const owner of modules) {
    const object = owner === 'base_service' ? 'service' :
      ({service_core:'service-core',worker_registry:'worker-registry',frontend_registry:'frontend-registry',
        native_commands:'native-commands',lifecycle:'service-lifecycle',management:'service-management'})[owner];
    if (!archive.includes('obj/opennt-base-bindings/' + object + '.obj'))
      throw Error('Private module absent from production provider: ' + owner);
  }
}
console.log('PASS service separation: 137 unchanged bodies/signatures, eight unchanged private types, pinned original/public contracts, four rejected mutations; fixture uses production provider.');
