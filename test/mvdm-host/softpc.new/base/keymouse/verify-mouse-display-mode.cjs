/* Exercise the actual INT33 entry against BIOS and hardware display states. */
const fs = require('node:fs');
const path = require('node:path');
const cp = require('node:child_process');
const root = path.resolve(__dirname, '../../../../..');
const out = path.resolve(root, process.argv[2]);
const relative = path.relative(path.join(root, 'build'), out);
if (!relative || relative.startsWith('..') || path.isAbsolute(relative)) throw Error('Expected build output');
const source = fs.readFileSync(process.argv[3] || path.join(root, 'src/mvdm-host/softpc.new/base/keymouse/mouse_io.c'), 'utf8');
const bodies = [...source.matchAll(/^void mouse_io_interrupt\(\)\s*\{[\s\S]*?^\}/gm)];
if (bodies.length !== 1) throw Error('Ambiguous INT33 source boundary');
fs.mkdirSync(out, {recursive: true});
const mode = source.match(/^#define jap_mouse .+$/m);
fs.writeFileSync(path.join(out, 'mouse-mode.h'), mode
  ? '#define MOUSE_DERIVED_MODE 1\n' + mode[0] + '\n'
  : 'static int jap_mouse;\n');
fs.writeFileSync(path.join(out, 'mouse-entry.h'), bodies[0][0]);
const exe = path.join(out, 'mouse-display.exe');
const build = cp.spawnSync('cl.exe', ['/nologo', '/MT', '/std:c11', '/W3', '/I'+out,
  '/Fo:'+path.join(out, 'mouse-display.obj'), '/Fe:'+exe,
  path.join(__dirname, 'original_mouse_display_mode_test.c')], {cwd:out, encoding:'utf8'});
fs.writeFileSync(path.join(out, 'build.log'), (build.stdout || '') + (build.stderr || ''));
if (build.error || build.status) throw Error(build.error || build.stdout || build.stderr);
const run = cp.spawnSync(exe, [], {cwd:out, encoding:'utf8'});
fs.writeFileSync(path.join(out, 'test.log'), (run.stdout || '') + (run.stderr || ''));
process.stdout.write(run.stdout || '');
if (run.error || run.status) process.exit(1);
