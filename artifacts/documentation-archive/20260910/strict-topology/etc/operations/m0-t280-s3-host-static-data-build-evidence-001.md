# M0 T280 S3 — original host static-data build evidence

## Formal runs — 2026-08-26

Both generated graphs completed with MSVC `/MT /W4`:

```text
VsDevCmd -arch=x64 -host_arch=x64
ninja -C build/M0-T280/x64
[1/3] CC obj/nt_ertbl.obj
[2/3] CC obj/nt_mess.obj
[3/3] LIB host-static-data.lib

VsDevCmd -arch=x86 -host_arch=x64
ninja -C build/M0-T280/x86
[1/3] CC obj/nt_mess.obj
[2/3] CC obj/nt_ertbl.obj
[3/3] LIB host-static-data.lib
```

The selected original sources remain byte-exact inputs:

- `nt_ertbl.c` — `3A41BBCE47258534E4375615A960048A97143A2353AA5946DE13C8F870DC572F`;
- `nt_mess.c` — `00855DDB83164E8E20D8EDC482F48FF403C42D2808D4D951081FACB335550A83`.

The expected historical warning is the original duplicate `STRINGIFY` macro
definition between `insignia.h` and `base_def.h`. It remains visible under
`/W4`; it is neither suppressed nor converted into a mirror edit.

The library is compile-closure evidence only. It does not enable a BOP,
provider, guest memory operation, machine action, or application link.
