import { mkdirSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';

const [repositoryRoot, buildRoot] = process.argv.slice(2);
if (!repositoryRoot || !buildRoot) {
  throw new Error('usage: node New-T366DeclaredDosRecordNinja.mjs <repository-root> <build-root>');
}

const root = resolve(repositoryRoot).replaceAll('\\', '/');
const build = resolve(buildRoot).replaceAll('\\', '/');
const environment = `${build}/msvc-x86.cmd`;
const vs = 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat';
const cflags = [
  '/nologo', '/std:c11', '/MT', '/W4', '/showIncludes',
  `/I ${root}/src`,
  `/I ${root}/src/opennt-abi/source/public/internal/base/inc`,
  `/I ${root}/src/opennt-host/public/sdk/inc`,
  `/I ${root}/src/adapter-mvdm-host-out/basesrv/include`,
  `/I ${root}/src/adapter-mvdm-host-out/win32/include`,
  `/I ${root}/src/session`
].join(' ');

mkdirSync(build, { recursive: true });
writeFileSync(environment, [
  '@echo off',
  'set "MVDM_T366_CALLER_CWD=%CD%"',
  'if defined VSCMD_VER goto ready',
  `call "${vs}" -arch=x86 -host_arch=x64 >nul`,
  'if errorlevel 1 exit /b %errorlevel%',
  ':ready',
  'cd /d "%MVDM_T366_CALLER_CWD%"',
  '%*',
  ''
].join('\r\n'), 'ascii');

const objects = [
  ['fixture', 'tests/adapter-mvdm-host-out/basesrv/base_vdm_local_fixture.c'],
  ['base_vdm_client', 'src/adapter-mvdm-host-out/basesrv/source/base_vdm_client.c'],
  ['base_vdm_local', 'src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c'],
  ['base_vdm_broker', 'src/adapter-mvdm-host-out/basesrv/source/base_vdm_broker.c'],
  ['base_vdm_record', 'src/broker/base_vdm_record.c'],
  ['launch_declaration', 'src/app/launch_declaration.c'],
  ['thread_start_compat', 'src/adapter-mvdm-host-out/win32/source/thread_start_compat.c'],
  ['session', 'src/session/session.c'],
  ['mapping_manager', 'src/session/mapping_manager.c'],
  ['guest_memory_lease', 'src/session/guest_memory_lease.c']
];
const rules = [
  'ninja_required_version = 1.10',
  `root = ${root}`,
  `cflags = ${cflags}`,
  `environment = ${environment}`,
  '',
  'rule cc',
  '  command = cmd /d /c "$environment cl $cflags /Fo$out /c $in"',
  '  deps = msvc',
  '  description = CC $out',
  'rule link',
  '  command = cmd /d /c "$environment link /nologo /out:$out $in kernel32.lib"',
  '  description = LINK $out',
  'rule run',
  '  command = $in',
  '  description = RUN $in',
  ''
];
for (const [name, input] of objects) rules.push(`build obj/${name}.obj: cc $root/${input}`);
rules.push(`build declared_dos_record_fixture.exe: link ${objects.map(([name]) => `obj/${name}.obj`).join(' ')}`);
rules.push('build test: run declared_dos_record_fixture.exe');
rules.push('default declared_dos_record_fixture.exe', '');
writeFileSync(`${build}/build.ninja`, rules.join('\n'), 'utf8');
console.log(`Wrote T366 x86 declared-DOS-record Ninja graph: ${build}/build.ninja`);
