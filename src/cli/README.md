# CLI component

`src/cli` is the user-facing, non-invasive `ntdos64-run` component.  It is a
separate fifth component beside the OpenNT guest payload, host capabilities,
minimal Bochs machine, and `bx-vdm` composition boundary.

It owns only:

- command-line parsing and image classification;
- fixed YAML configuration and component-identity admission;
- child-engine launch environment, cancellation, exit status, and diagnostics.

It must not include Bochs headers, manipulate guest memory or CPU state, or
implement DOS, DEM, BOP, filesystem, or WOW semantics.  The CLI passes a
validated profile/root selection to the engine; the adapter independently
validates any guest-memory transaction it performs.

`src/runner` remains only as the historical location of the associated test
fixtures.  New CLI production code belongs here.
