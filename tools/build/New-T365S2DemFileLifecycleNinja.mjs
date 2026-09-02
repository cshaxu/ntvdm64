import fs from 'node:fs';
import path from 'node:path';

const root = path.resolve(process.argv[2] || '.');
const build = path.resolve(process.argv[3] || path.join(root, 'build',
  'M0-T365', 'S2', 'dem-file-lifecycle-x86'));
const slash = value => value.replaceAll('\\', '/');
const ninjaRoot = slash(root).replace(/^([A-Za-z]):/, '$1$:' );
const cflags = [
  '/nologo', '/std:c11', '/MT', '/W4', '/showIncludes',
  `/I ${ninjaRoot}/src`,
  `/I ${ninjaRoot}/src/session`,
  `/I ${ninjaRoot}/src/adapter-mvdm-host-out/softpc/include`
].join(' ');

fs.mkdirSync(path.join(build, 'obj'), { recursive: true });
fs.writeFileSync(path.join(build, 'build.ninja'), [
  'ninja_required_version = 1.10',
  `root = ${ninjaRoot}`,
  `cflags = ${cflags}`,
  '',
  'rule cc',
  '  command = cl $cflags /c /Fo$out $in',
  '  deps = msvc',
  '  description = CC $in',
  'rule link',
  '  command = link /nologo /out:$out $in kernel32.lib',
  '  description = LINK $out',
  '',
  'build obj/mapping_manager.obj: cc $root/src/session/mapping_manager.c',
  'build obj/guest_memory_lease.obj: cc $root/src/session/guest_memory_lease.c',
  'build obj/session.obj: cc $root/src/session/session.c',
  'build obj/location.obj: cc $root/src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c',
  'build obj/identity.obj: cc $root/src/adapter-mvdm-host-out/softpc/mvdm_host_identity.c',
  'build obj/fixture.obj: cc $root/tests/adapter-mvdm-host-out/softpc/t365_s2_dem_file_lifecycle_lease_fixture.c',
  'build dem_file_lifecycle_lease_fixture.exe: link obj/mapping_manager.obj obj/guest_memory_lease.obj obj/session.obj obj/location.obj obj/identity.obj obj/fixture.obj',
  'default dem_file_lifecycle_lease_fixture.exe',
  ''
].join('\n'), { encoding: 'utf8' });
console.log(`Wrote T365 S2 DEM file lifecycle Ninja graph: ${path.join(build, 'build.ninja')}`);
