import { mkdirSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";

const [rootArgument, buildArgument] = process.argv.slice(2);
if (!rootArgument || !buildArgument) {
  throw new Error("usage: node Generate-T420S16SuballocNinja.mjs <root> <build>");
}

const root = resolve(rootArgument).replaceAll("\\", "/");
const build = resolve(buildArgument).replaceAll("\\", "/");
mkdirSync(`${build}/obj`, { recursive: true });

const cflags = [
  "/nologo", "/TC", "/c", "/MT", "/W4", "/DWIN32", "/DWINNT",
  "/FI", `\"${root}/src/opennt-abi/host-compat/include/nt.h\"`,
  "/I", `\"${root}/src/mvdm/inc\"`,
  "/I", `\"${root}/src/opennt-abi/host-compat/include\"`,
  "/I", `\"${root}/src/opennt-host/public/sdk/inc\"`
].join(" ");

const lines = [
  "ninja_required_version = 1.10",
  `root = ${root}`,
  `cflags = ${cflags}`,
  "rule cc",
  "  command = cl.exe $cflags /Fo$out $in",
  "  deps = msvc",
  "  msvc_deps_prefix = Note: including file: ",
  "rule link",
  "  command = link.exe /nologo /out:$out $in kernel32.lib libcmt.lib libvcruntime.lib libucrt.lib",
  "build obj/suballoc.obj: cc $root/src/mvdm/suballoc/suballoc.c",
  "build obj/fixture.obj: cc $root/tests/mvdm/suballoc/suballoc_lifecycle_fixture.c",
  "build suballoc-lifecycle-fixture.exe: link obj/suballoc.obj obj/fixture.obj",
  "default suballoc-lifecycle-fixture.exe",
  ""
];

writeFileSync(`${build}/build.ninja`, lines.join("\n"), "utf8");
