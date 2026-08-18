# T228 S1 P4 — Terminal CS-transition provenance result

## Result

The existing explicit-provenance derivative was rebuilt after adding only a post-watchdog copied wrapper around registered `BX-MANTLE-075`.  The unchanged hash-locked Direct and Readonly input pair again reached the same budget terminal and produced byte-identical stdout and stderr.

The retained latest CS transition is identical in both profiles:

```
previous=1797:3AB8
current =5A5F:5EF9
predecessor=FF9E4A971793109717000000000000
stack      =BC3A9717D20897176E00
```

The terminal later remains in `5A5F` through `9F51`, where P3 proved its instruction window is zero-filled RAM.  The copied record itself does not decode instructions; external analysis of its first bytes is consistent with a 16-bit indirect far-call form, and the copied stack retains return `1797:3ABC`.

## Source comparison

The exact `FF 9E` prefix and complete four-byte `FF 9E 4A 97` sequence occur in none of the declared source-built NTDOS, NTIO, or COMMAND images.  Therefore neither the predecessor call site nor its target may be attributed merely by fixed static file offset to those inputs.  This disproves the provisional assumption that `1797:3AB8` was a simple unmodified NTDOS-image mapping.

## Disposition

The failure remains outside BOP/provider and host-mutation ownership.  It is now a complete **runtime guest-memory producer / control-transfer state** question spanning original Guest DOS initialization and bx-core/bx-mantle execution mechanics.  The next source map must identify the runtime producer of the predecessor code or recover its required register/pointer state by a separately bounded, selector-blind diagnostic.  No leaf BOP, image patch, address rule, device enablement or CPU semantic repair is selected.
