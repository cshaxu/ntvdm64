// Keeps the S23 text-Console boundary explicit until the separately admitted
// kvm-window presentation package owns real graphics display.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';

const root = path.resolve(process.argv[2] ?? '.');
const read = relative => fs.readFileSync(path.join(root, relative), 'utf8')
    .replaceAll('\r\n', '\n');
const mustContain = (text, needle, owner) =>
    assert.ok(text.includes(needle), `${owner}: missing ${needle}`);

function collect(relative) {
    const directory = path.join(root, relative);
    return fs.readdirSync(directory, { withFileTypes: true }).flatMap(entry => {
        const child = path.join(relative, entry.name);
        return entry.isDirectory() ? collect(child) : [child];
    });
}

const graph = read('build/M0-T420/S23/formal-x86-cvid-r7/build.ninja');
const mode = read('src/mvdm/softpc.new/base/video/vga_mode.c');
const compat = read('src/ntvdm-exe/win32/console_compat.c');
const proposal = read('docs/proposals/proposal-kvm-window-graphics-presentation-001.md');

// The selected original C-VID/video compilation actually enables the graphics
// provider family; it is not a profile-null claim.
mustContain(graph, '/DMONITOR /DX86GFX /DMVDM_STANDALONE_SAS_VIDEO',
    'selected C-VID/video graph');
mustContain(mode, 'set_gfx_update_routines( ega_graph_update, EGA_GRAPHICS_MARKING',
    'original VGA graphics mode selection');
mustContain(mode, 'set_gfx_update_routines( vga_graph_update, EGA_GRAPHICS_MARKING',
    'original VGA 256-colour mode selection');

// Public Console owns only the text-cell copy.  Graphics invalidations retain
// their original event shape and cannot turn into fabricated Console success.
mustContain(compat, 'if (output != GetStdHandle(STD_OUTPUT_HANDLE))',
    'text-only Console presentation gate');
mustContain(compat, 'return console_video_event(SESSION_VIDEO_EVENT_INVALIDATE, output, NULL,',
    'non-text presentation handoff');
mustContain(compat, 'SetLastError(ERROR_CALL_NOT_IMPLEMENTED);',
    'unpresented graphics palette boundary');

// A graphics event sink has one definition/declaration pair only; no product
// caller installs one today.  That is a bounded absence, not an implicit GUI.
const productionFiles = collect('src').filter(file => /\.(?:c|h)$/.test(file));
const sinkSites = productionFiles.filter(file =>
    read(file).includes('session_set_video_event_sink('))
    .map(file => file.replaceAll('\\', '/'));
assert.deepEqual(sinkSites.sort(), [
    'src/ntvdm-exe/session/session.c',
    'src/ntvdm-exe/session/session.h'
]);
mustContain(proposal, 'worker-local', 'queued graphics presentation owner');
mustContain(proposal, 'presentation adapter', 'queued graphics presentation owner');
mustContain(proposal, '文本模式继续走当前 native Console route',
    'queued graphics presentation boundary');

console.log('PASS: original graphics providers selected; public Console is text-only; no graphics sink is installed; kvm-window owns future presentation.');
