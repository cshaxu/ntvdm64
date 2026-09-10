# M0 T256 S3 — Non-Paged Protected Range Action Closure

## Result

The admitted `BX-MANTLE-095` action is source-built in the formal graph.
It is a project-owned `bx-mantle` fixed-width copied range record; it does
not modify adopted Bochs or imported OpenNT code.

The implementation accepts only an active minimal-machine lifecycle, current
protected mode with `CR0.PG=0`, a valid current segment, and a bounded
ordinary-RAM span.  It calls the existing native segment-check helpers,
obtains the native linear address, rejects disabled-A20 alias spans, then uses
the existing whole-span ordinary-RAM copy helper.  It returns copied bytes or
a typed status only.  No address, pointer, descriptor, mapping object, BOP
term, or provider object crosses the surface.

## Formal evidence

Freshly rebuilt formal target:

```text
build/M0-T256-S3/formal-r1/bin/t255-s2-generic-context-resume-fixture.exe
```

The fixture passed after recompiling the new mantle source and its focused
test.  It proves:

- inactive lifecycle, real mode, protected paging-enabled mode, and malformed
  segment records decline without guest fault delivery;
- a readable protected data segment copies bytes from ordinary RAM, and a
  writable segment copies bytes back;
- write through the protected code segment is rejected by native access
  checks;
- disabled A20 rejects the `0x100000` domain rather than wrapping it;
- a VGA/handler-backed `0xA0000` span is rejected;
- a four-byte write beginning at `0xFFFFE` is rejected as a full span, and
  the preceding two ordinary-RAM bytes remain unchanged; and
- the prior generic real↔protected context-resume cases remain present in the
  same formal fixture and pass.

## Boundary conclusion

This closes only fault-contained protected copied access for the no-paging
ordinary-RAM profile.  It does not establish an OpenNT guest-linear mapping
identity, a DPMI allocator/LDT policy, or any `53:xx` provider.  Those remain
the next S4 source/ABI mapping question inside T256.
