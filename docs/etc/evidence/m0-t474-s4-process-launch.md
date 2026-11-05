# M0 T474 S4 — shared process-launch mechanisms

The owner approved one common Win32 process-launch component. This project now
owns `src/lib-win32/process-launch/{attributes,process,command_line}.{c,h}`;
the neighboring Win16-style project may independently import that component.
The command-line source and header were copied unchanged from Win16-style
commit `cc8a6730918686e4a4586a36e7e231a7bb5c617d` and carry the existing MIT
notice at `src/lib-win32/process-hook/LICENSE.win16style`. Their behavior is
covered by command-line tests. Neighboring sources were not modified.

The existing attribute state and cleanup moved into the component; attribute 0
with a null value and zero size leaves an initialized list empty. NTVWM uses
that shared implementation instead of independently allocating the list. The
generic creation operation passes through the caller's flags and Win32 error;
NTSRV worker and frontend and NTVWM target paths keep their existing decisions,
including suspended launch, handle selection, admission, resume and rollback.
The component never owns NTSRV policy, calls user callbacks, terminates a child,
or waits on behalf of the application. Raw command-line traversal is supplied
for later sharing but does not replace the existing Run16 parser.

The x86/x64 cached products built. Link-ownership verification, 39 attribute
failure/cleanup checks and nine raw command-line cases including `cmd /c ver`,
`>`, `>>`, and `|` passed. A real suspended CMD launch and exit-code test also passed. The Full selected
product gates passed, covering nested DOS/native execution, redirected streams,
GUI startup, RPC/worker failures, Console return and DOS4GW launch. WOW windows
were observed, not gameplay-validated. The tested ten images and hook license
were published and SHA-256 verified at both `assets/release` and
`O:/winnt/system32`. No guest source or image was changed.

The attribute helper intentionally provides an empty or single-attribute list;
it does not claim general multi-attribute list support. The shared command-line
helper is not yet substituted for Run16's path-specific parsing. The neighboring
project has not yet imported or tested this new combined component.
