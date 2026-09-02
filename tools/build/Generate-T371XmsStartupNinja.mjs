import { mkdirSync, writeFileSync } from "node:fs";
import { resolve, join, basename } from "node:path";

const root = resolve(process.argv[2] ?? process.cwd());
const build = resolve(process.argv[3] ?? join(root, "build", "M0-T371", "S2", "xms-startup"));
const source = (path) => join(root, path).replaceAll("\\", "/");
const ninja = (path) => path.replace(/^([A-Za-z]):/, "$1$:").replaceAll("\\", "/");

mkdirSync(join(build, "obj"), { recursive: true });
const msvc = join(build, "msvc-mt.cmd");
writeFileSync(msvc, [
  "@echo off",
  'call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
  "if errorlevel 1 exit /b %errorlevel%",
  "%*"
].join("\r\n") + "\r\n", "ascii");

const sources = [
  "tests/mvdm-host/xms/xms_init_fixture.c",
  "src/mvdm-host/xms.486/xms.c",
  "src/mvdm-host/suballoc/suballoc.c",
  "src/adapter-mvdm-host-out/softpc/mvdm_xms_memory.c",
  "src/session/mapping_manager.c",
  "src/session/guest_memory_lease.c",
  "src/session/session.c"
];
const includes = [
  "src",
  "src/adapter-mvdm-host-out/win32/include",
  "src/adapter-mvdm-host-out/softpc/include",
  "src/opennt-host/public/sdk/inc",
  "src/mvdm-host/inc",
  "src/mvdm-host/xms.486",
  "src/mvdm-host/softpc.new/base/inc",
  "src/opennt-abi/source/public/sdk/inc",
  "src/opennt-abi/source/public/internal/base/inc",
  "src/opennt-abi/source/public/ddk/inc"
].map((path) => `/I "${source(path)}"`).join(" ");
const flags = [
  "/nologo", "/std:c11", "/MT", "/W4", "/showIncludes",
  "/DWIN32", "/DWINNT", "/DWIN_32", "/DCPU_40_STYLE", "/DDEVL",
  `/FI "${source("src/adapter-mvdm-host-out/win32/include/nt.h")}"`, includes
].join(" ");
const objects = sources.map((path, index) => `obj/${String(index).padStart(2, "0")}-${basename(path, ".c")}.obj`);
const lines = [
  "ninja_required_version = 1.10",
  `root = ${ninja(root)}`,
  `flags = ${flags}`,
  "rule cc",
  `  command = cmd.exe /d /s /c call ${ninja(msvc)} cl.exe $flags /Fo$out /c $in`,
  "  deps = msvc",
  "  description = CC $out",
  "rule link",
  `  command = cmd.exe /d /s /c call ${ninja(msvc)} link.exe /nologo /out:$out $in`,
  "  description = LINK $out",
  ...sources.map((path, index) => `build ${objects[index]}: cc ${ninja(source(path))}`),
  `build xms-init-fixture.exe: link ${objects.join(" ")}`,
  "default xms-init-fixture.exe",
  ""
];
writeFileSync(join(build, "build.ninja"), lines.join("\n"));
console.log(`Wrote T371 XMS startup graph: ${join(build, "build.ninja")}`);
