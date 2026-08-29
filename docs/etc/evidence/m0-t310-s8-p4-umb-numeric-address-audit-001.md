# M0 T310 S8 P4 — UMB Numeric-Address ABI Audit

## Scope

This evidence covers only the reached non-`MONITOR` path of the original
`mvdm-host/softpc.new/host/src/nt_umb.c` package.  It is one package-level
x86/x64 ABI repair within the S8 whole-tree (`mvdm-host` + `mvdm-support`)
audit; it does not enable the historical kernel monitor product shell.

## Finding

`ReserveUMB`, `ReleaseUMB`, `VDDCommitUMB`, and `VDDDeCommitUMB` retain the
historical `PVOID` spelling for a UMB address.  In the selected SoftPC path,
the value is compared with `UMB->Base`, passed to `sas_connect_memory`, and
returned to XMS: it is a 32-bit guest physical address, not a host pointer.
The original casts between `PVOID` and `DWORD` therefore rely on the NT4
32-bit process-width coincidence and are unsafe on x64.

## Recovery disposition

`MVDM-HOST-DIV-049` preserves the original public function names, parameters,
control flow, error direction, UMB ownership transitions, and SAS ownership.
The private `mvdm_umb_address` overlay only validates that an encoded value is
representable as `DWORD`, then performs a numeric encode/decode.  It does not
allocate memory, map an identity, expose a host address, or use the session
mapping manager: the values are arithmetic guest physical addresses rather
than external/host object identities.

The `MONITOR` branches remain original, compiled out in the selected build,
and are recorded as the historical kernel-monitor boundary rather than being
misrepresented as a modern user-mode implementation.

## Required verification

Regenerate and execute the formal `original-softpc-candidate` Ninja target on
both x86 and x64.  Acceptance requires the selected `nt_umb.c` package and
the overlay codec to compile and link without a new width-conversion warning.
This is compile/ABI closure only; it is not a claim of VDD runtime closure.
