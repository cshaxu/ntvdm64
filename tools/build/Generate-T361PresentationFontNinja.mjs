import { mkdirSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';

if (process.argv.length !== 4) {
  throw new Error('usage: node tools/build/Generate-T361PresentationFontNinja.mjs <repository-root> <build-root>');
}

const root = resolve(process.argv[2]).replaceAll('\\', '/').replace(':', '$:');
const build = resolve(process.argv[3]);
mkdirSync(build, { recursive: true });
const source = (path) => `${root}/${path}`;
const lines = [
  'ninja_required_version = 1.10',
  'cflags = /nologo /TC /c /MT /W4 /WX /showIncludes /I ' + source('src') +
    ' /I ' + source('src/session') +
    ' /I ' + source('src/adapter-mvdm-host-out/softpc/include') +
    ' /I ' + source('src/adapter-mvdm-host-out/win32/include'),
  'rule cc',
  '  command = cl.exe $cflags /Fo$out $in',
  '  deps = msvc',
  'rule link',
  '  command = link.exe /nologo /OUT:$out $in $libs kernel32.lib',
  'rule run',
  '  command = $in',
  'build obj/mapping_manager.obj: cc ' + source('src/session/mapping_manager.c'),
  'build obj/guest_memory_lease.obj: cc ' + source('src/session/guest_memory_lease.c'),
  'build obj/session.obj: cc ' + source('src/session/session.c'),
  'build obj/presentation_font.obj: cc ' + source('src/adapter-mvdm-host-out/softpc/mvdm_softpc_presentation_font.c'),
  'build obj/fixture.obj: cc ' + source('tests/adapter-mvdm-host-out/softpc/presentation_font_snapshot_fixture.c'),
  'build presentation-font-snapshot-fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/presentation_font.obj obj/fixture.obj',
  'build obj/window.obj: cc ' + source('src/app/presentation_window.c'),
  'build obj/window_fixture.obj: cc ' + source('tests/app/presentation_window_contract_fixture.c'),
  'build presentation-window-contract-fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/window.obj obj/window_fixture.obj',
  '  libs = kernel32.lib user32.lib gdi32.lib',
  'build verify-font: run presentation-font-snapshot-fixture.exe',
  'build verify-window: run presentation-window-contract-fixture.exe',
  'build verify: phony verify-font verify-window',
  'default verify',
  ''
];
writeFileSync(resolve(build, 'build.ninja'), lines.join('\n'));
console.log(build);
