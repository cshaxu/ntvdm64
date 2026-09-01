/*
 * Generates the add-on Ninja graph for T335/S3's source-shaped WOW
 * CallBack16 -> original CCPU40 bounded-return proof.
 *
 * The base CCPU lifecycle graph is deliberately generated first by
 * New-T313CcpuLifecycleNinja.ps1.  This graph consumes only that graph's
 * selected original libraries/objects and adds the selected original WOW
 * translation unit plus test-only unreachable-export seams.
 */
import { mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { resolve, join } from 'node:path';

const [rootArg, baseArg, outArg] = process.argv.slice(2);
if (!rootArg || !baseArg || !outArg) {
  throw new Error('usage: node Generate-T335S3WowCallbackCcpuNinja.mjs <repo-root> <base-build> <output-build>');
}
const root = resolve(rootArg);
const base = resolve(baseArg);
const out = resolve(outArg);
mkdirSync(out, { recursive: true });

const ninjaPath = value => value.replaceAll('\\', '/').replace(/^([A-Za-z]):/, '$1$:');
const baseNinja = readFileSync(join(base, 'build.ninja'), 'utf8');
const baseFlags = baseNinja.match(/^cflags = (.+)$/m)?.[1];
if (!baseFlags) throw new Error(`missing cflags in ${join(base, 'build.ninja')}`);
const cflags = `${baseFlags} /I "${ninjaPath(join(root, 'src/mvdm-host/inc'))}" /I "${ninjaPath(join(root, 'src/mvdm-host/wow32'))}" /I "${ninjaPath(join(root, 'src/adapter-mvdm-host-out/wow/include'))}"`;
const environment = ninjaPath(join(base, 'msvc-x86.cmd'));
const source = path => ninjaPath(join(root, path));
const local = path => ninjaPath(join(out, path));
const baseFile = path => ninjaPath(join(base, path));

const lines = [
  'ninja_required_version = 1.10',
  `cflags = ${cflags}`,
  '',
  'rule cc',
  `  command = cmd.exe /d /s /c call ${environment} cl.exe $cflags /Fo$out $in`,
  '  deps = msvc',
  '  msvc_deps_prefix = Note: including file:',
  'rule lib',
  `  command = cmd.exe /d /s /c call ${environment} lib.exe /nologo /out:$out $in`,
  'rule link',
  `  command = cmd.exe /d /s /c call ${environment} link.exe /nologo /out:$out $in kernel32.lib user32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib`,
  '',
  `build ${local('fixture.obj')}: cc ${source('tests/mvdm-host/t335_s3_original_callback_ccpu_fixture.c')}`,
  `build ${local('seams.obj')}: cc ${source('tests/mvdm-host/t335_s3_wcall16_unreachable_seams.c')}`,
  `build ${local('lease.obj')}: cc ${source('src/adapter-mvdm-host-out/wow/wow_callback_frame_lease.c')}`,
  `build ${local('nt-aorc.obj')}: cc ${source('src/mvdm-host/softpc.new/host/src/nt_aorc.c')}`,
  `build ${local('opennt-support-rtl.obj')}: cc ${source('src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c')}`,
  `build ${local('wcall16.obj')}: cc ${source('src/mvdm-host/wow32/wcall16.c')}`,
  `build ${local('original-wcall16.lib')}: lib ${local('wcall16.obj')}`,
  `build ${local('original-callback-ccpu-x86.exe')}: link ${local('fixture.obj')} ${local('seams.obj')} ${local('lease.obj')} ${local('nt-aorc.obj')} ${local('opennt-support-rtl.obj')} ${local('original-wcall16.lib')} ${baseFile('obj/test/ccpu_bounded_execution_fixture_seams.obj')} ${baseFile('obj/overlay/mvdm_gdp_state.obj')} ${baseFile('obj/controller/at_dma.obj')} ${baseFile('obj/controller/ica.obj')} ${baseFile('obj/support/ios.obj')} ${baseFile('obj/patch/fmstubs_edl_fast_bop.obj')} ${baseFile('original-ccpu40.lib')} ${baseFile('original-host-lifecycle.lib')} ${baseFile('lifecycle-adapter.lib')}`,
  `build original-callback-ccpu: phony ${local('original-callback-ccpu-x86.exe')}`,
  'default original-callback-ccpu',
  ''
];
writeFileSync(join(out, 'build.ninja'), lines.join('\n'));
console.log(`Generated T335/S3 CallBack16/CCPU40 add-on graph: ${out}`);
