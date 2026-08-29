# M0 T310 S8 P4 Console VDM registration call-contract closure

## Question

Can selected original SoftPC console initialization and fullscreen-resume
callers retain the exact `RegisterConsoleVDM` pointer-bearing ABI on x86 and
x64 without claiming that modern Windows still provides NT4's Console Server
VDM facility?

## Inputs

- `src/mvdm-host/softpc.new/host/src/nt_det.c`
- `src/mvdm-host/softpc.new/host/src/nt_fulsc.c`
- `O:/repos.external/OpenNT/public/internal/windows/inc/conapi.h`
- `O:/repos.external/OpenNT/windows/core/ntcon/client/private.c`
- `O:/repos.external/OpenNT/windows/core/ntcon/server/private.c`

## Procedure and observations

OpenNT declares `RegisterConsoleVDM` with eleven parameters: registration
flags; two hardware-event handles; state and VDM-buffer section names and
lengths; output state length; two output `PVOID *` mappings; and a `COORD`
buffer size. Its client captures the two names and sends the request to
Console Server. Its server owns VDM registration, event-handle duplication and
the state/text-section mappings used by fullscreen switching.

The selected original callers had no modern declaration and therefore were
compiled as implicit `int` calls. The adapter's declaration bridge now retains
the exact original Boolean return and all pointer-bearing parameters. Focused
formal Ninja builds of `nt_det.obj` and `nt_fulsc.obj` on x86 and x64 emit no
`RegisterConsoleVDM` C4013 diagnostic.

## Interpretation

This is a compile/call-contract closure only. The NT4 Console Server plus
Win32k fullscreen VDM provider is not a public modern Win32 facility and is
not recreated by this declaration. In particular, the adapter does not return
fabricated state or text-buffer mappings, consume the supplied event handles,
or report a false registration success. Any future runtime provider must
preserve the source-shaped registration/unregistration failure behavior and
clearly identify its session-owned replacement for the unavailable private
product shell.

No guest pointer, mapping-manager identity, controller implementation, warning
suppression or mirror-body modification was added.

## Follow-up

Keep the Console VDM registration capability in the SoftPC display/fullscreen
boundary ledger. The remaining focused warnings are original non-width source
records and are intentionally still compiler-visible.
