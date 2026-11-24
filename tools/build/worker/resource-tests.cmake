# Test-owned RPC stubs; product providers come from the formal worker archives.
set(resource_rpc "${CMAKE_BINARY_DIR}/resource-tests")
file(MAKE_DIRECTORY "${resource_rpc}")
add_custom_command(OUTPUT "${resource_rpc}/resource_attachment.h"
  "${resource_rpc}/resource_attachment_c.c" "${resource_rpc}/resource_attachment_s.c"
  COMMAND "${NTVDM_MIDL}" /nologo /env win32 /target NT100
    /prefix client Client_ /prefix server Server_ /out "${resource_rpc}"
    "${NTVDM_REPO}/test/broker/resource_attachment.idl"
  DEPENDS "${NTVDM_REPO}/test/broker/resource_attachment.idl" VERBATIM)
set(resource_flags "/nologo /c /MT /W4 /I \"${resource_rpc}\" /I \"${NTVDM_REPO}/src\" /I \"${NTVDM_REPO}/src/ntsrv-exe/transport\"")
worker_unit("resource-tests/server.obj" "${NTVDM_REPO}/test/broker/resource_attachment.c"
  "${resource_flags} /DRESOURCE_SERVER" "${resource_rpc}/resource_attachment.h")
worker_unit("resource-tests/client.obj" "${NTVDM_REPO}/test/broker/resource_attachment.c"
  "${resource_flags}" "${resource_rpc}/resource_attachment.h")
worker_unit("resource-tests/server-stub.obj" "${resource_rpc}/resource_attachment_s.c" "${resource_flags}")
worker_unit("resource-tests/client-stub.obj" "${resource_rpc}/resource_attachment_c.c" "${resource_flags}")
worker_unit("resource-tests/registration.obj" "${NTVDM_REPO}/test/broker/resource_registration.c" "${profile_44}")
set(resource_libraries "broker-transport.lib" "lib-rpc.lib" "lib-process-launch.lib"
  "lib-process-observation.lib" "lib-handle.lib" "common-base-srv-transport.lib"
  "common-root.lib" "common-rpc.lib" "lib-pipe.lib" "lib-file.lib" "common-codec.lib" "common-console.lib")
worker_link("resource-tests/resource-server.exe" "/nologo /opt:ref /out:$out /map:resource-tests/resource-server.map"
  "rpcrt4.lib advapi32.lib kernel32.lib ntdll.lib user32.lib legacy_stdio_definitions.lib"
  INPUTS "resource-tests/server.obj" "resource-tests/registration.obj" "obj/run16/support.obj"
    ${resource_libraries} "opennt-base-server.lib" "opennt-base-bindings.lib"
    "common-base-srv-bindings.lib" "original-opennt-rtl-x86.lib"
    "resource-tests/server-stub.obj" "resource-tests/client-stub.obj")
worker_link("resource-tests/resource-client.exe" "/nologo /out:$out"
  "rpcrt4.lib advapi32.lib kernel32.lib ntdll.lib user32.lib legacy_stdio_definitions.lib"
  INPUTS "resource-tests/client.obj" "resource-tests/client-stub.obj" ${resource_libraries})
worker_group("broker-resource-fixtures" "resource-tests/resource-server.exe" "resource-tests/resource-client.exe")
