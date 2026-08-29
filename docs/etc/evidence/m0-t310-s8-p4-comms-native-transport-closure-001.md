# M0 T310 S8 P4 — SoftPC serial/parallel native transport closure

## Scope

Selected original SoftPC serial/parallel host code only:
`base/comms/com.c`, `base/inc/host_com.h`, `host/src/nt_com.c` and
`host/src/nt_lpt.c`.

## Recovery

- `host_com_ioctl` still uses the original request codes, scalar device values
  and output-pointer roles. Its private transport is now `intptr_t`, instead
  of the NT4-only `long` that could not hold a host pointer on x64.
- The original `config_inquire` return convention remains unchanged. Its
  bounded scalar values are first interpreted as `ULONG_PTR`; string returns
  remain pointers.
- The COM receive-worker adapter index crosses `CreateThread` through
  `ULONG_PTR`, then returns to its original `DWORD` range.
- No guest address, session token or host object is exposed; the mapping
  manager is correctly not involved.

## Verification

Formal MSVC/Ninja objects were forcibly rebuilt on both host targets:

```text
x64: ninja obj/host/nt_com.obj obj/host/nt_lpt.obj
x86: ninja obj/host/nt_com.obj obj/host/nt_lpt.obj
```

The selected C4311, C4312 and C4305 diagnostics are absent. Remaining
diagnostics are visible non-width warnings (`C4100`, `C4101`, `C4189`,
`C4996`) and are intentionally outside this ABI closure.

The one original controller-side caller outside the current Ninja source list
was also compiled directly with the same formal x64 and x86 MSVC environments:

```text
x64/x86: softpc.new/base/comms/com.c
```

Its `host_com_ioctl` calls now match the typed `intptr_t` transport. Its
remaining diagnostics concern fixed-width UART register fields, which are a
separate device-value audit rather than host pointer-width transport.
