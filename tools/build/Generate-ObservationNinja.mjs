import { mkdirSync, writeFileSync } from 'node:fs';
import { resolve } from 'node:path';

if (process.argv.length !== 4) {
  throw new Error('usage: node tools/build/Generate-ObservationNinja.mjs <repository-root> <build-root>');
}

const root = resolve(process.argv[2]).replaceAll('\\', '/').replace(':', '$:');
const build = resolve(process.argv[3]);
const source = (path) => `${root}/${path}`;

mkdirSync(build, { recursive: true });
writeFileSync(resolve(build, 'build.ninja'), [
  'ninja_required_version = 1.10',
  'cflags = /nologo /TC /c /MT /W4 /WX /showIncludes',
  'rule cc',
  '  command = cl.exe $cflags /Fo$out $in',
  '  deps = msvc',
  '  msvc_deps_prefix = Note: including file:',
  'rule link',
  '  command = link.exe /nologo /OUT:$out $in kernel32.lib dbghelp.lib',
  'build console-startup-observer.obj: cc ' +
    source('tests/observation/console_startup_observer.c'),
  'build console-startup-observer.exe: link console-startup-observer.obj',
  'default console-startup-observer.exe',
  ''
].join('\n'));
writeFileSync(resolve(build, 'msvc-x86.cmd'), [
  '@echo off',
  'set "OBSERVATION_CALLER_CWD=%CD%"',
  'if defined VSCMD_VER goto ready',
  'call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
  'if errorlevel 1 exit /b %errorlevel%',
  ':ready',
  'cd /d "%OBSERVATION_CALLER_CWD%"',
  '%*',
  ''
].join('\r\n'));
console.log(build);
