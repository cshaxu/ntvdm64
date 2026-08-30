# M0 T310 S33 P1 — support-controller callbacks closure

## Original support controller

`mvdm-host/softpc.new/base/support/ios.c` is the selected original SoftPC I/O
routing controller.  It owns, without a replacement:

- the input/output port-to-adapter tables;
- typed byte, word, dword and string function-vector tables;
- default empty-port behavior and generic width expansion;
- adapter registration, disconnection and port lookup; and
- `io_init` initialization of the complete port namespace.

The selected CCPU40 profile retains `SPC386`, so the original dword/string
forms are compiled with the same original `ios.c` body.  Function pointers are
native private controller state; they do not cross guest or component ABIs and
therefore do not require mapping-manager identities.  No adapter, wrapper,
overlay or warning suppression was introduced.

## Termination source boundary

The same original support archive contains `terminat.c`.  Its cleanup ordering
is retained as source evidence, but its final `host_terminate → ExitProcess`
tail is a historical whole-`ntvdm.exe` product exit—not a controller callback.
The existing source-shaped `ExitVDM` Base-client boundary already completes the
current session rather than a host process.  A runnable bounded session must
therefore prove no return resumes guest execution after this tail before it can
replace the process exit.  That is the mandatory S49 lifecycle/teardown proof;
S33 neither changes the original cleanup algorithm nor accepts `ExitProcess`
as an app-runtime behavior.

## Formal evidence and transfer

The formal 381-edge Win32/x86 CCPU40 graph compiles `ios.c` and `terminat.c`
in `original-softpc-support.lib`.  The static callback inventory's nine S33
records reduce to the eight `ios.c` function-vector forms and the one
`terminat.c` lifecycle form.  `ios.c` has no external semantic adapter edge;
the latter has the explicit S49 lifecycle transfer above.  End-to-end port
delivery and controlled-stop behavior remain S49 integrated machine evidence,
not substitute fixtures in this source packet.
