import assert from 'node:assert/strict';
import fs from 'node:fs';

const read = file => fs.readFileSync(file, 'utf8').replace(/\r\n/g, '\n');
const cmdmisc = read('src/mvdm/dos/command/cmdmisc.c');
const firmware = read('src/adapter-mvdm-host-out/softpc/mvdm_softpc_firmware.c');
const firmwareHeader = read('src/adapter-mvdm-host-out/softpc/include/mvdm_softpc_firmware.h');
const sessionHeader = read('src/session/session.h');
const session = read('src/session/session.c');
const launcher = read('src/app/run16_entry.c');
const client = read('src/opennt-host/base/win32/client/vdm.c');

for (const source of [cmdmisc, firmware, firmwareHeader, sessionHeader, session]) {
  assert(!source.includes('mvdm_wow_bootstrap_kernel'),
    'retired session WOW fallback remains');
}
assert(!cmdmisc.includes('mvdm_softpc_firmware.h'),
  'COMMAND must not depend on the retired fallback adapter');
assert.match(cmdmisc, /pch = strstr\(pszCmdLine, " -a "\);/,
  'original worker -a parser is absent');
assert.match(cmdmisc, /else \{\n\s*pszCmdLine = NULL;\n\s*\}/,
  'missing -a must keep original worker failure path');
assert.match(launcher, /BaseGetVdmConfigInfo\(worker_path,[\s\S]*?binary, &worker_command, &vdm_size\)/,
  'run16 no longer obtains the original configured worker command');
assert.match(launcher, /CreateProcessW\(worker_path, worker_command\.Buffer/,
  'run16 does not pass the original constructed command to its worker');
assert.match(client, /\*pDst\+\+ = 'w';[\s\S]*?pWowKernel \+= 4;[\s\S]*?strcpy\(pDst, "\.exe"\);/,
  'original WOW -w/-a construction changed');
console.log('PASS: T416 S2 removes dead WOW fallback and retains the original run16 -> -w/-a -> worker carrier');
