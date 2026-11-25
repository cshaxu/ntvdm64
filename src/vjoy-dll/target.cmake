set(vjoy_source "${NTVDM_REPO}/src/mvdm-host/vdd/vjoy")
set(vjoy_sdk "${NTVDM_REPO}/src/non-mvdm/public/sdk/inc")
worker_unit("obj/vjoy/vjoy.obj" "${vjoy_source}/vjoy.c"
  "/I \"${vjoy_source}\" ${profile_18} /DUNICODE")
worker_resource("obj/vjoy/vjoy.res" "${vjoy_source}/vjoy.rc"
  "/nologo /I \"${vjoy_sdk}\"" "${vjoy_sdk}/ntverp.h" "${vjoy_sdk}/common.ver")
worker_link("VJOY.DLL" "/nologo /dll /machine:x86 /entry:DllEntryPoint@12 /base:0xfe80000 /map:$out.map /def:${vjoy_source}/vjoy.def /out:$out"
  "kernel32.lib" INPUTS "obj/vjoy/vjoy.obj" "obj/vjoy/vjoy.res" "ntvdm.lib"
  DEPENDS "${vjoy_source}/vjoy.def")
