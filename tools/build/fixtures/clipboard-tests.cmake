set(source "${NTVDM_REPO}/src/mvdm-host/wow32/wuclip.c")
file(READ "${source}" text)
string(REPLACE "\r\n" "\n" text "${text}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source}")
set(selected)
foreach(name IN ITEMS WU32OpenClipboard WU32CloseClipboard)
  string(REGEX MATCH "ULONG FASTCALL ${name}\\(PVDMFRAME pFrame\\)\n\\{[^}]*\n\\}" bodies "${text}")
  string(FIND "${text}" "ULONG FASTCALL ${name}(" first)
  string(FIND "${text}" "ULONG FASTCALL ${name}(" last REVERSE)
  if(first LESS 0 OR NOT first EQUAL last OR NOT bodies)
    message(FATAL_ERROR "Clipboard projection changed: ${name}")
  endif()
  string(APPEND selected "${bodies}\n")
endforeach()
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/selected-clipboard-open-close.inc" CONTENT "${selected}" @ONLY)
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/clipboard-api-binding.h" CONTENT [=[#include <windows.h>
DWORD WINAPI epoch_sequence(void);
HWND WINAPI epoch_owner(void);
DWORD WINAPI epoch_owner_pid(HWND, LPDWORD);
DWORD WINAPI epoch_current_pid(void);
#define GetClipboardSequenceNumber epoch_sequence
#define GetClipboardOwner epoch_owner
#define GetWindowThreadProcessId epoch_owner_pid
#define GetCurrentProcessId epoch_current_pid
]=] @ONLY)
fixture(clipboard-epoch CL test/adapter-mvdm-host-out/wow/wow_clipboard_epoch_fixture.c
  src/compat/wow_clipboard_epoch.c)
set_property(TARGET clipboard-epoch_objects PROPERTY COMPILE_OPTIONS /nologo /W4 /WX
  "/FI${CMAKE_BINARY_DIR}/clipboard-api-binding.h")
target_include_directories(clipboard-epoch_objects PRIVATE "${CMAKE_BINARY_DIR}")
