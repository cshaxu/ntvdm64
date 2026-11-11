/* Test actual production save/restore bodies with bounded host boundaries. */
const fs = require('node:fs');
const path = require('node:path');
const cp = require('node:child_process');
const root = path.resolve(__dirname, '../../../../..');
const output = path.resolve(root, process.argv[2]);
const relative = path.relative(path.join(root, 'build'), output);
if (!relative || relative.startsWith('..') || path.isAbsolute(relative)) throw new Error('Expected build subdirectory');
const source = fs.readFileSync(process.argv[3] || path.join(root, 'src/mvdm-host/softpc.new/base/keymouse/mouse_io.c'), 'utf8');
function body(name) {
    const matches = [...source.matchAll(new RegExp('^(?:GLOBAL|LOCAL) void '+name+' IFN0\\(\\)\\s*\\{[\\s\\S]*?^\\}', 'gm'))];
    if (matches.length !== 1) throw new Error('Ambiguous body: '+name);
    return matches[0][0];
}
fs.mkdirSync(output, {recursive:true});
fs.writeFileSync(path.join(output, 'original_mouse_text_cursor.h'), body('software_text_cursor_display')+'\n'+body('software_text_cursor_undisplay'));
const predicate = source.match(/^#define jap_mouse .*$/m);
if (!predicate) throw new Error('Expected current derived painter predicate');
fs.writeFileSync(path.join(output, 'original_mouse_restore_dispatch.h'), '#undef jap_mouse\n'+predicate[0]+'\n'+body('cursor_undisplay'));
const exe=path.join(output,'restore.exe');
const build=cp.spawnSync('cl.exe',['/nologo','/MT','/std:c11','/W3','/I'+output,'/Fe:'+exe,'/Fo:'+path.join(output,'restore.obj'),path.join(__dirname,'original_mouse_restore_dispatch_test.c')],{encoding:'utf8'});
fs.writeFileSync(path.join(output,'build.log'),(build.stdout||'')+(build.stderr||''));
if(build.error || build.status!==0) throw new Error(build.error || build.stdout || build.stderr);
const run=cp.spawnSync(exe,[],{encoding:'utf8'});
fs.writeFileSync(path.join(output,'result.json'),JSON.stringify({scope:'production cursor dispatch; VGA/memory/graphics mocked, no integrated guest assertion',exitCode:run.status,stdout:run.stdout,stderr:run.stderr},null,2));
process.stdout.write(run.stdout||''); process.stderr.write(run.stderr||'');
if(run.error || run.status!==0) process.exit(1);
