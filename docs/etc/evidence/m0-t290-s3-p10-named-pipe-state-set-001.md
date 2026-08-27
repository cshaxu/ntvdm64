# M0 T290 S3 P10 — Original named-pipe state-set composition

The formal x86/x64 `vrnmpipe.c` fixture now executes original
`VrSetNamedPipeHandleState` with a byte-mode local public named pipe.  The
same session-owned opaque host-resource identity enters in `BP:BX`; the
original source converts DOS mode `CX=0` to Win32 wait/byte-read mode,
calls `SetNamedPipeHandleState`, and clears CF.  No native handle or pointer
crosses guest state, and no selector route is enabled.  Both fixture
executables exit `0`.
