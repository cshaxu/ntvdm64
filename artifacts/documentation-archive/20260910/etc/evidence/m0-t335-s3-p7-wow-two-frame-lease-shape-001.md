# M0 T335 S3 P7 — WOW two-frame lease shape

## Original shape recovered

`wow32/wcall16.c:CallBack16` first reads the caller `VDMFRAME`, then prepares
a separate writable `CBVDMFRAME`. Both aliases are live until the callback
frame is flushed and the original source releases them immediately before
`host_simulate`.

The existing session guest-memory lease context has eight scoped slots. The
focused WOW frame fixture now proves this precise two-slot shape through the
already-existing `adapter-mvdm-host-out/softpc` boundary:

1. acquire a packed-VP caller frame for read;
2. acquire a distinct linear callback frame for write;
3. commit/release the callback frame;
4. discard/release the caller frame;
5. reject a second release and discard a later uncommitted write.

No second mapping manager, alias registry, raw `FlatAddress` pointer, or
asynchronous lifetime is introduced. The aliases are bounded and cannot cross
the later recursive `host_simulate` call.

## Verification

Fresh focused Ninja graphs compiled and ran on both selected profiles:

- x86: callback-frame fixture exit code 0;
- x64: callback-frame fixture exit code 0.

## Limit

This closes the mechanical precondition for the fixed `CallBack16` frame
cohort only. It does not bind the general `GETVDMPTR` macro family, strings,
variable-length aliases, `wucomm` long-lived aliases, fast WOW, or a generic
Win16 API.
