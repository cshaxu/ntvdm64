/* Run from an x86 MSVC development environment. Generated source/objects and
 * results are confined to the repository build root. No production rewrite. */
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const cp = require('node:child_process');
const root = path.resolve(__dirname, '../..');
const textCursor = process.argv[3] === 'text-cursor';
if (process.argv[3] && !textCursor) throw new Error('Expected optional text-cursor mode');
const output = path.resolve(root, process.argv[2] || 'build/M0-T423/S7/original-coordinates');
const relative = path.relative(path.join(root, 'build'), output);
if (!relative || relative.startsWith('..') || path.isAbsolute(relative))
    throw new Error('Expected a test-specific directory below repository build');
const sourcePath = path.join(root, textCursor
    ? 'src/mvdm/softpc.new/base/keymouse/mouse_io.c'
    : 'src/mvdm/softpc.new/host/src/nt_mouse.c');
const bytes = fs.readFileSync(sourcePath);
const source = bytes.toString('utf8');
const names = textCursor
    ? ['software_text_cursor_display', 'software_text_cursor_undisplay']
    : ['TextScale', 'LimitCoordinates', 'EmulateCoordinates'];
const bodies = names.map(name => {
    const signature = textCursor ? '^GLOBAL void ' + name + ' IFN0\\(\\)'
        : '^void ' + name + '\\([^;{]*\\)';
    const matches = [...source.matchAll(new RegExp(signature + '\\s*\\{[\\s\\S]*?^\\}', 'gm'))];
    if (matches.length !== 1) throw new Error('Ambiguous source boundary: ' + name);
    return matches[0][0];
});
fs.mkdirSync(output, { recursive: true });
fs.writeFileSync(path.join(output, textCursor ? 'original_mouse_text_cursor.h'
    : 'original_mouse_algorithms.h'), bodies.join('\r\n\r\n'));
const args = ['/nologo','/MT','/std:c11','/W4','/WX','/DCPU_40_STYLE', '/I' + output,
    '/Fe:' + path.join(output, 'coordinates.exe'),
    '/Fo:' + path.join(output, 'coordinates.obj'),
    path.join(__dirname, textCursor ? 'original_mouse_text_cursor_test.c'
        : 'original_mouse_coordinates_test.c')];
const build = cp.spawnSync('cl.exe', args, { cwd: output, encoding: 'utf8' });
fs.writeFileSync(path.join(output, 'build.log'), (build.stdout || '') + (build.stderr || ''));
if (build.error || build.status !== 0) throw new Error(build.error || build.stdout || build.stderr);
const run = cp.spawnSync(path.join(output, 'coordinates.exe'), [], { cwd: output, encoding: 'utf8' });
fs.writeFileSync(path.join(output, 'test.log'), (run.stdout || '') + (run.stderr || ''));
const sha = value => crypto.createHash('sha256').update(value).digest('hex');
fs.writeFileSync(path.join(output, 'result.json'), JSON.stringify({
    source: path.relative(root, sourcePath), sourceSha256: sha(bytes),
    functions: names.map((name, i) => ({ name, sha256: sha(bodies[i]) })),
    scope: 'Isolated original ' + (textCursor ? 'text cursor bodies (mock memory/geometry)' : 'coordinate algorithms') +
        '; no guest, IRQ, Window or production acceptance',
    architecture: 'x86 /MT; CCPU40 profile; no CPU execution', exitCode: run.status,
    stdout: run.stdout, stderr: run.stderr
}, null, 2));
process.stdout.write(run.stdout || '');
if (run.error || run.status !== 0) throw new Error(run.error || run.stderr || 'Test failed');
