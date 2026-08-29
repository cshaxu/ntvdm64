# M0 T310 S8 P4 — SoftPC video word-fill contract closure

`base/video/ega_vide.c` calls the original host `memset4` provider for private
EGA plane storage. The base header stated a `ULONG *` contract, while the
selected original `host/src/copy_fnc.c` provider is `unsigned int *`; their
32-bit widths masked a distinct-pointer ABI mismatch.

The mirror now publishes the provider's exact `unsigned int` data, pointer and
count contract and uses that exact private-plane pointer form. No guest address
or mapping-manager identity is involved.

Formal Ninja rebuilt `original-softpc-video.lib` for both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/p4-video-word-abi.log`
- `build/M0-T310/S8/p1-machine-source/x86/p4-video-word-abi.log`

Both logs contain zero reviewed ABI diagnostics for `ega_vide.c` and `host.h`.
