import { mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { createHash } from "node:crypto";
import { resolve } from "node:path";

const [rootArgument, buildArgument, formalArgument] = process.argv.slice(2);
if (!rootArgument || !buildArgument || !formalArgument) {
  throw new Error("usage: node Generate-T420S17OemUniNinja.mjs <root> <build> <formal-build>");
}

const root = resolve(rootArgument).replaceAll("\\", "/");
const build = resolve(buildArgument).replaceAll("\\", "/");
const formal = resolve(formalArgument).replaceAll("\\", "/");
mkdirSync(`${build}/obj`, { recursive: true });
const demSource = readFileSync(`${root}/src/mvdm/dos/dem/demfile.c`, "utf8");
const demStart = demSource.indexOf("VOID demCreateCommon (flCreateType)");
const demEnd = demSource.indexOf("BOOL IsCdRomFile (PSTR pszPath)", demStart);
if (demStart < 0 || demEnd <= demStart) throw new Error("DEM source boundaries changed");
writeFileSync(`${build}/dem-create-body.inc`, demSource.slice(demStart, demEnd));
writeFileSync(`${build}/dem-create-source.sha256`, createHash("sha256").update(demSource).digest("hex"));

const cflags = [
  "/nologo", "/TC", "/c", "/MT", "/W4", "/showIncludes", "/DWIN32", "/DWINNT",
  "/FI", `\"${root}/src/opennt-abi/host-compat/include/nt.h\"`,
  "/I", `\"${root}/src/mvdm/inc\"`,
  "/I", `\"${root}/src/mvdm/oemuni\"`,
  "/I", `\"${root}/src/opennt-abi/host-compat/include\"`,
  "/I", `\"${root}/src/opennt-host/public/sdk/inc\"`
].join(" ");

const lines = [
  "ninja_required_version = 1.10",
  `root = ${root}`,
  `formal = ${formal}`,
  `cflags = ${cflags}`,
  "rule regenerate",
  `  command = \"${process.execPath.replaceAll("\\", "/")}\" \"$root/tools/build/Generate-T420S17OemUniNinja.mjs\" \"$root\" \"${build}\" \"$formal\"`,
  "  generator = 1",
  "build build.ninja: regenerate $root/tools/build/Generate-T420S17OemUniNinja.mjs $root/src/mvdm/dos/dem/demfile.c",
  "rule cc",
  "  command = cl.exe $cflags /Fo$out $in",
  "  deps = msvc",
  "  msvc_deps_prefix = Note: including file: ",
  "rule link",
  "  command = link.exe /nologo /out:$out $in $formal/obj/opennt-abi-host-compat/opennt_support_rtl.obj $formal/original-opennt-rtl-x86.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib",
  "build obj/file.obj: cc $root/src/mvdm/oemuni/file.c",
  "build obj/process.obj: cc $root/src/mvdm/oemuni/process.c",
  "build obj/toemuni.obj: cc $root/src/mvdm/oemuni/toemuni.c",
  "  cflags = $cflags /Dgetch=oemuni_no_pause",
  "build obj/pause.obj: cc $root/tests/mvdm/oemuni/oemuni_pause_stub.c",
  "build obj/failure.obj: cc $root/tests/mvdm/oemuni/oemuni_failure_fixture.c",
  "build obj/expand-failure.obj: cc $root/tests/mvdm/oemuni/oemuni_expand_failure_fixture.c",
  "build obj/family.obj: cc $root/tests/mvdm/oemuni/oemuni_family_fixture.c",
  "build obj/dbcs.obj: cc $root/tests/mvdm/oemuni/oemuni_dbcs_fixture.c",
  "build obj/font.obj: cc $root/tests/mvdm/oemuni/oemuni_font_fixture.c",
  "build oemuni-font-fixture.exe: link obj/font.obj",
  "build obj/config-path.obj: cc $root/src/ntvdm-exe/softpc/mvdm_softpc_firmware.c",
  '  cflags = $cflags /I "$root/src" /I "$root/src/ntvdm-exe/softpc/include"',
  "build obj/config-path-test.obj: cc $root/tests/mvdm/oemuni/oemuni_config_path_fixture.c",
  '  cflags = $cflags /I "$root/src" /I "$root/src/ntvdm-exe/softpc/include"',
  "build oemuni-config-path-fixture.exe: link obj/config-path.obj obj/config-path-test.obj",
  "build obj/font-retry.obj: cc $root/tests/mvdm/oemuni/oemuni_font_retry_fixture.c",
  `  cflags = $cflags /I \"${build}\"`,
  "build oemuni-font-retry-fixture.exe: link obj/font-retry.obj",
  "build oemuni-dbcs-fixture.exe: link obj/dbcs.obj",
  "build oemuni-family-fixture.exe: link obj/file.obj obj/process.obj obj/family.obj",
  "build oemuni-expand-failure-fixture.exe: link obj/expand-failure.obj",
  "build oemuni-original-test.exe: link obj/file.obj obj/process.obj obj/toemuni.obj obj/pause.obj",
  "build oemuni-failure-fixture.exe: link obj/file.obj obj/process.obj obj/failure.obj",
  "default oemuni-original-test.exe",
  ""
];

writeFileSync(`${build}/build.ninja`, lines.join("\n"), "utf8");
