# M0 T271 S3 P16 — COMMAND composition-body overlay isolation

`opennt_command_composition.c` was a 732-line project-authored product-shell
bridge inside the `opennt-bop` mirror root.  It binds a bounded active call,
models source-specific guest pointer materialization/writeback and preserves
the original terminal non-return convention; OpenNT supplies those facilities
only through its larger CCPU/SAS/CSR product composition, not through a
separable translation unit.

P16 applies the mirror-component rule without changing linkage:

- `git mv` moved the complete body to
  `opennt-bop-overlay/command/opennt_command_composition.c`.
- The original mirror path now contains only a registered `BOP-DIV-103`
  include boundary, so it remains the sole compilation/linkage owner.
- The overlay includes the mirror's existing OpenNT-facing declaration header
  but has no standalone source list, public ABI, or caller outside its
  matching mirror translation unit.
- Generic CCPU portions already extracted in P12/P13/P15 remain in
  `adapter-softpc`; the overlay retains only the irreducibly source-specific
  active-call/pointer/writeback/terminal composition pending its individually
  named COMMAND, Redirector and WOW owner work.

Verification was performed outside the sandbox after a formal Ninja relink
in `build/M0-T271-S3/r003`:

- `t231-s8-command-lifecycle-direct-fixture.exe` exited zero.
- `t236-s1-command-local-child-fixture.exe` exited zero.
- `t231-s10-command-native-session-fixture.exe` exited zero and retained the
  native original COMMAND entry without a v1 fallback.
