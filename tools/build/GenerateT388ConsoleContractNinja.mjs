import fs from "node:fs";
import path from "node:path";

const root = process.cwd().replaceAll("\\", "/");
const build = path.resolve("build/M0-T388/S7/console-contract-x86");
const objectDir = path.join(build, "obj");
const environment = path.join(build, "msvc-mt.cmd");

fs.mkdirSync(objectDir, { recursive: true });
fs.writeFileSync(environment, [
  "@echo off",
  "set \"MVDM_T388_CALLER_CWD=%CD%\"",
  "if defined VSCMD_VER goto ready",
  "call \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat\" -arch=x86 -host_arch=x64 >nul",
  "if errorlevel 1 exit /b %errorlevel%",
  ":ready",
  "cd /d \"%MVDM_T388_CALLER_CWD%\"",
  "%*",
].join("\r\n"));

const normalizedEnvironment = environment.replaceAll("\\", "/");
const ninja = [
  "ninja_required_version = 1.10",
  `root = ${root}`,
  `cflags = /nologo /TC /MT /W4 /showIncludes /I ${root}/src /I ${root}/src/adapter-mvdm-host-out/win32/include`,
  `environment = ${normalizedEnvironment}`,
  "",
  "rule cc",
  "  command = cmd /d /s /c call $environment cl $cflags /Fo$out /c $in",
  "  deps = msvc",
  "rule link",
  "  command = cmd /d /s /c call $environment link /nologo /out:$out $in kernel32.lib user32.lib gdi32.lib",
  "rule run",
  "  command = $in",
  "",
  "build obj/fixture.obj: cc $root/tests/adapter-mvdm-host-out/win32/console_input_contract_fixture.c",
  "build obj/console_compat.obj: cc $root/src/adapter-mvdm-host-out/win32/source/console_compat.c",
  "build obj/session.obj: cc $root/src/session/session.c",
  "build obj/mapping_manager.obj: cc $root/src/session/mapping_manager.c",
  "build obj/guest_memory_lease.obj: cc $root/src/session/guest_memory_lease.c",
  "build console_input_contract_fixture.exe: link obj/fixture.obj obj/console_compat.obj obj/session.obj obj/mapping_manager.obj obj/guest_memory_lease.obj",
  "build test: run console_input_contract_fixture.exe",
  "default console_input_contract_fixture.exe",
  "",
].join("\n");

fs.writeFileSync(path.join(build, "build.ninja"), ninja);
console.log(`wrote ${path.join(build, "build.ninja")}`);
