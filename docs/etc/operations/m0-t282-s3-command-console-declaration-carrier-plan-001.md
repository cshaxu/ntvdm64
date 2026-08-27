# M0 T282 S3 — COMMAND console declaration carrier

The original `cmdkeyb.c` is the complete keyboard-layout source unit within
the COMMAND package. Its next static composition boundary is the exact
historical `public/internal/windows/inc/winconp.h`, which declares
`GetConsoleKeyboardLayoutName` and related console layouts.

Import that file byte-for-byte into `opennt-platform-abi`, select it only
through an adapter-win32 declaration forwarder, and compile original
`cmdkeyb.c` on x86 and x64. This S neither implements the function nor enables
the keyboard service: the original registry, system-directory, guest-memory
and console-policy branches remain later named session/adapter boundaries.
