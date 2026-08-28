# adapter-opennt-host

This component is the package-private historical host-interface adapter for
`opennt-host`. It has one named subfamily for each accepted non-MVDM OpenNT
owner package that needs a private historical binding. BaseSrv/client VDM is
the first such package, not the component's permanent scope.

The current BaseSrv/client subfamily may preserve only the exact calling shapes
reached by its owning `opennt-host` package:

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

No implementation body is admitted yet. Each accepted package's interface
audit determines its smallest source-shaped subfamily before a single function
is written.
