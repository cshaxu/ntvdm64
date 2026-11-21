/* Run from an x86 MSVC development environment. Generated source/objects and
 * results are confined to the repository build root. No production rewrite. */
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const cp = require('node:child_process');
const root = path.resolve(__dirname, '../../../../..');
const textCursor = process.argv[3] === 'text-cursor';
const guest = process.argv[3] === 'guest';
if (process.argv[3] && !textCursor && !guest) throw new Error('Expected optional text-cursor or guest mode');
const output = path.resolve(root, process.argv[2] || 'build/M0-T423/S7/original-coordinates');
const relative = path.relative(path.join(root, 'build'), output);
if (!relative || relative.startsWith('..') || path.isAbsolute(relative))
    throw new Error('Expected a test-specific directory below repository build');
const sourcePath = path.join(root, textCursor
    ? 'src/mvdm-host/softpc.new/base/keymouse/mouse_io.c'
    : 'src/mvdm-host/softpc.new/host/src/nt_mouse.c');
const bytes = fs.readFileSync(sourcePath);
const source = bytes.toString('utf8');
const names = textCursor
    ? ['software_text_cursor_display', 'software_text_cursor_undisplay']
    : ['TextScale', 'LimitCoordinates', 'EmulateCoordinates', ...(guest ? ['mvdm_softpc_mouse_apply'] : [])];
const bodies = names.map(name => {
    const signature = textCursor ? '^GLOBAL void ' + name + ' IFN0\\(\\)'
        : (name === 'mvdm_softpc_mouse_apply' ? '^BOOL ' : '^void ') + name + '\\([^;{]*\\)';
    const matches = [...source.matchAll(new RegExp(signature + '\\s*\\{[\\s\\S]*?^\\}', 'gm'))];
    if (matches.length !== 1) throw new Error('Ambiguous source boundary: ' + name);
    return matches[0][0];
});
fs.mkdirSync(output, { recursive: true });
fs.writeFileSync(path.join(output, textCursor ? 'original_mouse_text_cursor.h'
    : 'original_mouse_algorithms.h'), bodies.join('\r\n\r\n'));
if (guest) {
    // Diagnostic replacement of nt_mouse.obj: intercept calls, preserve the body.
    fs.writeFileSync(path.join(output, 'mouse-coordinate-owner.c'),
        source.replace(bodies[2], bodies[2].replace('void EmulateCoordinates', 'void original_coordinates')));
}
const args = ['/nologo','/MT','/std:c11','/W4','/WX','/DCPU_40_STYLE', '/I' + output,
    '/Fe:' + path.join(output, 'coordinates.exe'),
    '/Fo:' + path.join(output, 'coordinates.obj'),
    path.join(__dirname, textCursor ? 'original_mouse_text_cursor_test.c'
        : 'original_mouse_coordinates_test.c')];
if (guest) {
    args.pop();
    for (const define of ['WIN32', 'WINNT', 'CCPU', 'PROD', 'ANSI', 'V7VGA']) args.push('/D' + define);
    for (const dir of ['src', 'src/compat/include', 'src/compat/include/generated/x86/prod',
        'src/mvdm-host/softpc.new/base/inc', 'src/mvdm-host/softpc.new/host/inc',
        'src/mvdm-host/softpc.new/base/ccpu386', 'src/mvdm-host/inc']) args.push('/I' + path.join(root, dir));
    args.splice(args.findIndex(arg => arg.startsWith('/Fo:')), 1, '/Fo:' + output + path.sep);
    args.push(path.join(root, 'test/ntvdm-exe/softpc/softpc_relative_mouse_guest_test.c'));
    for (const name of ['mvdm_softpc_mouse_guest', 'mvdm_softpc_mouse_bridge', 'mvdm_softpc_mouse_input'])
        args.push(path.join(root, 'src/compat', name + '.c'));
}
const build = cp.spawnSync('cl.exe', args, { cwd: output, encoding: 'utf8' });
fs.writeFileSync(path.join(output, 'build.log'), (build.stdout || '') + (build.stderr || ''));
if (build.error || build.status !== 0) throw new Error(build.error || build.stdout || build.stderr);
const run = cp.spawnSync(path.join(output, 'coordinates.exe'), [], { cwd: output, encoding: 'utf8' });
fs.writeFileSync(path.join(output, 'test.log'), (run.stdout || '') + (run.stderr || ''));
const sha = value => crypto.createHash('sha256').update(value).digest('hex');
fs.writeFileSync(path.join(output, 'result.json'), JSON.stringify({
    source: path.relative(root, sourcePath), sourceSha256: sha(bytes),
    functions: names.map((name, i) => ({ name, sha256: sha(bodies[i]) })),
    scope: (guest ? 'Actual C2 mouse application and B ingress/bridge, mocked memory/IRQ; ' : '') + 'Isolated original ' + (textCursor ? 'text cursor bodies (mock memory/geometry)' : 'coordinate algorithms') +
        '; no guest, IRQ, Window or production acceptance',
    architecture: 'x86 /MT; CCPU40 profile; no CPU execution', exitCode: run.status,
    stdout: run.stdout, stderr: run.stderr
}, null, 2));
process.stdout.write(run.stdout || '');
if (run.error || run.status !== 0) throw new Error(run.error || run.stderr || 'Test failed');
