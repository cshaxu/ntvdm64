# M0 T310 S8 P4 — CCPU generated video-rule storage ABI closure

## Question

Can the selected original CCPU `vglob.c` video-rule accessors preserve their
OpenNT interface on both x86 and x64 without truncating private code addresses?

## Source evidence

The unchanged OpenNT `ccpu386/vglob.c` and `base/inc/egacpu.h` declare the
six rule accessors as `IUH *`: forward/backward string-read, byte/word/string
mark, and byte-read.  The selected generated `ccpu386/evid_c.h` stored those
same private code addresses as `IU32 *`, which was representation-compatible
with pointers only in the historical 32-bit host.

## Resolution

`vglob.c` and `egacpu.h` are restored to their original interface.  A single
registered `MVDM-HOST-DIV-051` changes only the six generated storage fields
in `ccpu386/evid_c.h` to `IUH *`.  These values are native private SoftPC
code addresses, not guest addresses or exported host identities, so the
session mapping manager is deliberately not involved.

## Verification

Forced formal Ninja rebuilds of `obj/ccpu/vglob.obj` completed on x64 and x86
after the change.  Neither architecture emitted its twelve prior `C4057`
storage/return mismatch diagnostics.  The remaining `C4005` command-line
macro redefinition warnings are unrelated and remain visible.

The twelve corresponding whole-tree worklist entries are closed as
`closed-native-video-rule-storage`.  This establishes compile-time ABI
correctness only; it does not claim runtime video-controller validation.
