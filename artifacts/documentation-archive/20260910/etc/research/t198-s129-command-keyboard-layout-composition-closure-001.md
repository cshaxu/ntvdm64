# M0 T198 S129: COMMAND Keyboard-Layout Composition Closure

S129 moves the already retained finite `54:0E` no-install provider into the
new T198 composition.  The composition first performs common BOP ingress,
provider selection and COMMAND-plane classification; only a deferred COMMAND
`CONSOLE` record for service 14 may invoke the provider.  The detached legacy
runtime gate is not used.

The manifest and the direct MSVC/x64 composition closure both name the
provider.  The focused r2 result at
`artifacts/build/t198-s129-keyboard-layout-composition-test-r2` compiles,
links and runs successfully.  Its test establishes:

- exact real-mode `C4 C4 54 0E` resumes at `+4` and writes low `DX=0`;
- the COMMAND launch transaction remains one-slot and declines its second
  `54:01` as before; and
- the same `54:0E` form in protected mode declines.

This is a CLI capability substitution for the original no-install branch.
It deliberately does not supply the original branch's `cmdInitConsole`
side-effect, KB16 file/options writes, keyboard device, registry, VDD or
Win32 console behavior.  No native guest fixture was run in S129.
