# adapter-opennt-host

This component is the package-private historical host-interface adapter for
`opennt-host`. It exists because the imported original BaseSrv/client VDM
package calls NT4 CSR capture, dispatch, process and server-thread facilities
that are neither public modern Win32 APIs nor MVDM-facing interfaces.

It may preserve only the exact calling shapes reached by `opennt-host`:

- client CSR capture, message dispatch and wait/retry transport;
- server CSR request/reply, process/thread lookup and bounded registration;
- event-pair, duplicated-handle and cleanup operations required by the
  original BaseSrv record lifecycle; and
- the original WOWEXEC registration notification shape when its WOW owner is
  explicitly admitted.

It must not receive BOP, DOS, WOW guest-frame, SoftPC/Bochs or generic MVDM
semantics. `mvdm-host` does not include or call this component directly.
Every exported binding must retain an OpenNT source spelling/ABI/failure row
in the shared host tracker and identify its `opennt-host` caller.

No implementation body is admitted yet. The P4 interface audit determines the
smallest source-shaped surface before a single function is written.
