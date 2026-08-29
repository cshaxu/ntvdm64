# M0 T310 S8 P4 — SoftPC error-dialog native callback closure

## Scope

This record closes the selected user-mode x86/x64 ABI defects in the original
`softpc.new/host/src/nt_error.c` path only. It does not claim runtime closure
for WOW hard errors, nor does it introduce a substitute kernel VDM or CSRSS.

## Original contracts retained

- `host_direct_access_error` still uses its original error-category bits,
  duplicate-prompt test and dialog sequence.
- `GetThreadTopLevelWindow`, `ErrorDialogBoxThread` and `ErrorDialogEvents`
  still execute their original enumeration, worker, dialog and reply ordering.

## Native-width repair

- The private TLS category mask is now `ULONG_PTR`; it is not a guest address
  or a host-object identity and therefore does not use the mapping manager.
- `EnumWindows` transports a real `HWND` through `LPARAM`. The callback is
  explicitly `int __stdcall`: the imported SoftPC headers redefine `BOOL` as
  `UINT`, whereas the public `WNDENUMPROC` return is signed Win32 `BOOL`.
- Dialog data remains a public `LPARAM`; dialog result and message result use
  `INT_PTR` and `LRESULT` until the original reply boundary.

## Formal evidence

The selected original object built with the formal Ninja graph on both host
targets:

```text
build/M0-T310/S8/p1-machine-source/x64: ninja obj/host/nt_error.obj
build/M0-T310/S8/p1-machine-source/x86: ninja obj/host/nt_error.obj
```

Neither build reports the selected TLS, HWND, `EnumWindows`, dialog-data or
dialog-result width/callback diagnostics.

## Explicit remaining boundary

`WOWpSysErrorBox` invokes `NtRaiseHardError` using the original 32-bit
kernel/CSRSS parameter-array ABI. That path remains a physical
`kernel-csrss-hard-boundary-not-host-runtime` disposition in the S8 worklist:
it is neither cast away nor given a fictitious user-mode implementation.
