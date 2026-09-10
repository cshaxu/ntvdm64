# M0 T372 S5 P2 — RTL list-contract closure

`vrnetb.c` uses the original RTL `InsertTailList` and `RemoveHeadList` macros
for its NetBIOS post-work queue.  They must remain header expansion, not become
two adapter runtime functions.

The selected OpenNT `ntrtl.h` contains those original macro forms.  Modern
`winternl.h` can set `_NTRTL_` first, however, so the selected original carrier
is correctly found but its guarded list declarations are skipped.  The C
compiler then emits false external imports.

`adapter-mvdm-host-out/win32/include/nt.h` now supplies only those two original
macro forms under `#ifndef` guards.  The binding is registered as
`ADAPTER-WIN32-044`; it operates solely on caller-owned host-local
`LIST_ENTRY` records and adds no queue owner, guest identity, provider policy,
or new runtime implementation.

External validation used the formal CPU40/x86 graph:

```text
ninja -C build/M0-T371/S2/formal-x86 -j 8 original-mvdm-redir.lib
```

All sixteen original `vdmredir` translation units, including `vrnetb.c`,
compiled and the archive was rebuilt.  The preprocessor expansion contains no
remaining `InsertTailList` or `RemoveHeadList` call spelling.  Pre-existing
original-source warnings remain visible and were not suppressed.

This closes the list-primitives member of the S5 import audit.  It does not
compose `xactsrv`, `rxapi` or any live network provider.
