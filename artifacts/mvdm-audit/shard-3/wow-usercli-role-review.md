# Shard 3: WOW USER client thunk profile

Propose **partial** for `wow32/wusercli.c` and `wow32/wusercli.h`. The
[observer](wow-usercli-image-observer.py) and
[observation](wow-usercli-image-observations.json) verify both original source
editions, their DOS/WOW inventory IDs, selected WOW32 objects/dependencies,
30 machine-code bodies and 32 initial USER dispatch-table entries in the
released image. Two entries share existing functions. Machine-code comparison
masks COFF relocation words; the table observations resolve each target.

The [preprocess runner](wow-usercli-preprocess.ps1) repeats the actual
selected WOW32 table command without altering product output. Retained
[header and table excerpts](wow-usercli-preprocessed-excerpts.json) show
the selected `WU32ClientToScreen` and `WU32GetKeyState` branches. The
[durable verifier](wow-usercli-durable-check.py) rechecks source, image,
objects, slots and excerpt hashes after temporary preprocessing files vanish.

The original `wow32.h` defines `PMODE32` under `i386`; original
`wusercli.h` would make many USER slots `LOCALAPI` in that profile.
The current actual host compilation selects the other branch and links
host `WU32` bodies. The immutable x86 guest may instead take its original
`wow16/user/usercli.asm` optimized local paths. These are two distinct source
profiles: a host table pointer does not establish that a given guest request
ever traverses it or that optimized guest state is correct. The separately
registered PMODE32 shared-object and clock boundaries remain in scope.

The original thunks convert USER arguments and results; Windows and the
project USER adapter own real host object identities and lifecycle. Retained
prior guest/user tests do not establish all 30 functions, direct/local route
selection, native handle destruction/reuse, keyboard state, error values,
reentry or concurrent guest task behavior. Both files retain 100% original
bytes, while a complete behavior-restoration percentage remains unmeasurable.
No guest or product image changed.
