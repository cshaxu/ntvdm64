# T198 S5 P1 Generic #UD Bridge Record 001

P1 establishes the mantle-owned fixed record before changing CPU exception
control flow.  `bx_ntvdm_generic_ud_event_v1` contains only copied scalar CPU
facts and a bounded 15-byte instruction observation; its outcome contains only
pass-through, resume or controlled-stop plus typed register/CF deltas.  The
default bridge is synchronous and declines.

The record has no `bx-vdm` include, service identity, guest-memory request,
environment selection, product SIM/GUI dependency or callback registration.
`tests/bx-mantle-generic-ud-bridge-boundary.ps1` verifies that negative
boundary.  MSVC x86 compiled the standalone mantle object at
`artifacts/build/t198-s5-generic-ud-bridge-syntax-r1/bridge.obj`.

This is not yet an active CPU seam: P2 must replace the old core callback path
and use this record to invoke the fixed bridge.
