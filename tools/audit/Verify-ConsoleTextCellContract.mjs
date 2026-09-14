// Source/profile check complements the linked adapter fixture and real EDIT.
import fs from 'node:fs';
import assert from 'node:assert/strict';
const read = name => fs.readFileSync(name, 'utf8').replaceAll('\r\n', '\n');
const painter = read('src/mvdm-host/softpc.new/host/src/nt_cga.c');
const adapter = read('src/adapter-mvdm-host-out/win32/source/console_compat.c');
const fixture = read('tests/adapter-mvdm-host-out/win32/console_input_contract_fixture.c');
const graph = read(process.argv[2]);
const flags = graph.match(/^host_cflags = (.*)$/m)?.[1];
assert(flags?.includes('/DCPU_40_STYLE') && flags.includes('/DC_VID'));
assert(!/\/D(?:MONITOR|CPU_30_STYLE)\b/.test(flags));
assert.match(painter, /#ifdef MONITOR\s+#define TEXT_INCVAL 2\s+#else\s+#define TEXT_INCVAL 4\s+#endif/);
assert.match(painter, /to \+= 80 \* TEXT_INCVAL/);
assert.match(painter, /clen \* TEXT_INCVAL/);
const registration = adapter.slice(adapter.indexOf('BOOL WINAPI RegisterConsoleVDM('),
    adapter.indexOf('HMENU WINAPI ConsoleMenuControl('));
assert.match(registration.replace(/\/\*[\s\S]*?\*\//g, ''),
    /session_presentation_text_acquire_writable\(owner,\s*\(uint32_t\)buffer_size.X,\s*\(uint32_t\)buffer_size.Y,\s*4u,/);
assert.match(fixture, /SOFTPC_TEXT_CELL_BYTES = 4/);
assert.match(graph, /build obj\/host\/nt_cga.obj: cc_host .*\/src\/mvdm-host\/softpc.new\/host\/src\/nt_cga.c/);
console.log('PASS: selected CCPU40 painter stride=4, adapter allocation=4, fixture stride=4; no MONITOR');
