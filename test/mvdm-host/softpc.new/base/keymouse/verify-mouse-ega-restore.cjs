/* Extract the production EGA cursor body and test its geometry with bounded stubs. */
const fs = require('node:fs');
const path = require('node:path');
const cp = require('node:child_process');
const root = path.resolve(__dirname, '../../../../..');
const output = path.resolve(root, process.argv[2]);
const relative = path.relative(path.join(root, 'build'), output);
if (!relative || relative.startsWith('..') || path.isAbsolute(relative)) throw new Error('Expected a directory below build');
const source = fs.readFileSync(process.argv[3] || path.join(root, 'src/mvdm-host/softpc.new/base/keymouse/mouse_io.c'), 'utf8');
const bodies = [...source.matchAll(/^LOCAL EGA_graphics_cursor_display IFN0\(\)\s*\{[\s\S]*?^\}/gm)];
if (bodies.length !== 1) throw new Error('EGA cursor source boundary is ambiguous');
fs.mkdirSync(output, {recursive:true});
fs.writeFileSync(path.join(output, 'original_mouse_ega_restore.h'), bodies[0][0].replace('LOCAL EGA_graphics_cursor_display', 'LOCAL void EGA_graphics_cursor_display'));
const exe = path.join(output, 'mouse-ega.exe');
const args = ['/nologo','/MT','/std:c11','/W3','/I'+output,'/Fe:'+exe,'/Fo:'+path.join(output,'mouse-ega.obj'),path.join(__dirname,'original_mouse_ega_restore_test.c')];
const build = cp.spawnSync('cl.exe', args, {cwd:output, encoding:'utf8'});
fs.writeFileSync(path.join(output,'build.log'),(build.stdout||'')+(build.stderr||''));
if (build.error || build.status !== 0) throw new Error(build.error || build.stdout || build.stderr);
const run = cp.spawnSync(exe,[],{cwd:output,encoding:'utf8'});
fs.writeFileSync(path.join(output,'test.log'),(run.stdout||'')+(run.stderr||''));
process.stdout.write(run.stdout||'');
if (run.error || run.status !== 0) { process.stderr.write(run.stderr||''); process.exit(1); }
