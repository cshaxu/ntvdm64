import fs from "node:fs";
import path from "node:path";

const root = path.resolve(process.argv[2] ?? process.cwd());
const architecture = process.argv[3] ?? "x86";
const buildRoot = path.resolve(process.argv[4] ?? path.join(root, "build", "M0-T325", "S2", architecture));
const vsDevCmd = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat";

if (architecture !== "x86" && architecture !== "x64") throw new Error("architecture must be x86 or x64");
if (!fs.existsSync(vsDevCmd)) throw new Error("Visual Studio Build Tools are required");
fs.mkdirSync(path.join(buildRoot, "obj"), { recursive: true });

const toNinja = value => value.replaceAll("\\", "/").replace(":", "$:");
const includeRoots = [
  "src",
  "src/adapter-mvdm-host-out/redir/include",
  "src/adapter-mvdm-host-out/softpc/include",
  "src/adapter-mvdm-host-out/win32/include",
  "src/mvdm-host/inc",
  "src/mvdm-host/vdmredir",
  "src/mvdm-host-overlay/vdmredir",
  "src/mvdm-host/dos/command",
  "src/mvdm-host/softpc.new/base/inc",
  "src/mvdm-host/softpc.new/host/inc",
  "src/opennt-host/netapi/netlib",
  "src/opennt-abi/source/public/internal/base/inc",
  "src/opennt-abi/source/public/internal/ds/inc",
  "src/opennt-abi/source/public/internal/net/inc",
  "src/opennt-abi/source/private/ds/netapi/rpcxlate",
  "src/opennt-host/public/sdk/inc",
  "src/opennt-abi/source/public/sdk/inc",
  "src/opennt-abi/source/public/ddk/inc",
  "src/opennt-abi/source/private/inc"
].map(value => `/I \"${path.join(root, value).replaceAll("\\", "/")}\"`).join(" ");
const environment = path.join(buildRoot, `msvc-${architecture}.cmd`);
fs.writeFileSync(environment, [
  "@echo off",
  "set \"MVDM_T325_CALLER_CWD=%CD%\"",
  "if defined VSCMD_VER goto ready",
  `call \"${vsDevCmd}\" -arch=${architecture} -host_arch=x64 >nul`,
  "if errorlevel 1 exit /b %errorlevel%",
  ":ready",
  "cd /d \"%MVDM_T325_CALLER_CWD%\"",
  "%*"
].join("\r\n") + "\r\n", "ascii");

const cflags = `/nologo /TC /c /std:c11 /MT /W4 /Gy /showIncludes /DWIN_32 /DVDMREDIR_DLL /DCPU_40_STYLE ${includeRoots}`;
const ninja = [
  "ninja_required_version = 1.10",
  `root = ${toNinja(root)}`,
  `environment = ${toNinja(environment)}`,
  `cflags = ${cflags}`,
  "",
  "rule cc",
  "  command = cmd /c \"$environment cl $cflags /Fo$out $in\"",
  "  deps = msvc",
  "  description = CC $in",
  "rule link",
  "  command = cmd /c \"$environment link /nologo /out:$out $in\"",
  "  description = LINK $out",
  "",
  "build obj/vrnetapi.obj: cc $root/src/mvdm-host/vdmredir/vrnetapi.c",
  "build obj/vrremote.obj: cc $root/src/mvdm-host/vdmredir/vrremote.c",
  "build obj/guest-copy.obj: cc $root/src/adapter-mvdm-host-out/redir/mvdm_redirector_guest_copy.c",
  "build obj/location.obj: cc $root/src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c",
  "build obj/session.obj: cc $root/src/session/session.c",
  "build obj/mapping.obj: cc $root/src/session/mapping_manager.c",
  "build obj/lease.obj: cc $root/src/session/guest_memory_lease.c",
  "build obj/fixture.obj: cc $root/tests/mvdm-host/vdmredir/redirector_guest_copy_fixture.c",
  "build bin/redirector-guest-copy-fixture.exe: link obj/guest-copy.obj obj/location.obj obj/session.obj obj/mapping.obj obj/lease.obj obj/fixture.obj",
  "default obj/vrnetapi.obj obj/vrremote.obj bin/redirector-guest-copy-fixture.exe",
  ""
].join("\n");
fs.writeFileSync(path.join(buildRoot, "build.ninja"), ninja, "utf8");
console.log(`Generated T325 S2 workstation-query graph: ${buildRoot}`);
