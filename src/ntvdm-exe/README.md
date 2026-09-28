# NTVDM worker owner

This component owns worker-local bootstrap, session, guest-memory and thread
bindings around the original MVDM execution core. It does not own frontend
Console/Window presentation or broker record policy.

`package_layout.c` and its header configure worker-owned media roots and validate
the original COMMAND configuration limits. They were moved from product-package
without changing path or failure behavior; they are not shared product ABI.

The win32 directory keeps private declarations beside their implementation:
environment projection, thread alert, WOW hard-error presentation and the scoped
original-host CRT redirect. Their historical divergence records remain indexed
in opennt-abi/host-compat/README.md. Shared original declarations and genuinely
multi-owner bindings remain separate; this relocation does not introduce a
generic compatibility library or modify original guest media.
