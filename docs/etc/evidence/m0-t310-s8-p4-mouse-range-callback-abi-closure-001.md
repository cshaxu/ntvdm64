# M0 T310 S8 P4 — Mouse range callback ABI closure

## Source finding

`mouse_io.c` calls `host_x_range` and `host_y_range` with the address of
`MOUSE_SCALAR` cursor-window endpoints.  The paired declarations and
definitions in `mouse_io.c` and `nt_mouse.c` instead named those final two
parameters `word *`.  The values were the same width in the original NT4
build, but they are distinct signed scalar types and modern MSVC correctly
reported C4057 at both calls.

## Recovery

The existing two original host functions remain in place with their original
four-pointer ordering and bodies.  Only their final two parameter types, and
the matching imports, now use the actual `MOUSE_SCALAR *` endpoint form.
`MVDM-HOST-DIV-058` records this minimal declaration reconciliation.  There is
no mapping-manager use: each pointer is a synchronous private SoftPC host
address, never a guest-visible identity or durable guest alias.

## Verification

The selected original SoftPC candidate completed on both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/mouse-range-x64-build.log`
- `build/M0-T310/S8/p1-machine-source/x86/mouse-range-x86-build.log`

Neither log contains the former `mouse_io.c:3194`/`:3243` range-callback ABI
diagnostic.  Remaining mouse warnings are preserved for their separately
classified source contracts.
