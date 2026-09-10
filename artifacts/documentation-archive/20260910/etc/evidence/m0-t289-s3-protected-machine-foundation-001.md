# M0 T289 S3 protected-machine foundation evidence

## Scope

S3 adds only selector-blind `adapter-bochs` mechanics.  It does not compile,
route or decode any `53:xx` DPMI BOP, and it introduces neither a new mapping
manager nor a host pointer carrier.

## Admitted contract

- `machine_facade_protected_frame` is a fixed-width copy of the current
  non-paged protected CPU state.  A commit first compares the complete current
  copy with the caller's expected copy, then changes only GPRs, EIP and native
  supported FLAGS bits.  CR0, execution mode and all segment selectors are
  rejected before the first mutation.
- `machine_facade_copy_protected_segment` exposes a copied inspection record
  for one active segment slot.  It does not add LDT/IDT mutation or host-LDT
  publication.
- `machine_facade_protected_span_transfer` delegates to the existing checked
  protected segment-span operation.  The operation retains A20, access and
  ordinary-RAM checks; it never returns a guest or host pointer.
- The commit is permitted only after `adapter-bochs` has returned from the
  native CPU loop.  It is therefore not an asynchronous CPU or scheduler API.

## Formal proof

The dedicated `New-T289S3DpmiFoundationNinja.ps1` graph compiles the full
admitted Bochs core/overlay and `adapter-bochs` with
`RUNTIME_ENABLE_MACHINE_UD_BRIDGE=1`.  Its manifest rejects `src.old`, every
DPMI provider source and prebuilt Bochs archives.

Both executions passed:

```text
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T289S3DpmiFoundationNinja.ps1 -Architecture x64 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T289/S3/x64
build/M0-T289/S3/x64/bin/t289-s3-protected-machine-fixture.exe  -> 0

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T289S3DpmiFoundationNinja.ps1 -Architecture x86 -RepositoryRoot O:\repos.hobby\ntvdm64
ninja -C build/M0-T289/S3/x86
build/M0-T289/S3/x86/bin/t289-s3-protected-machine-fixture.exe  -> 0
```

The fixture executes a tiny real-to-protected transition and stops on an
opaque #UD callback.  It proves: copied protected-frame admission; active CS
descriptor inspection; checked code-segment span read; valid atomic EAX/CF
commit; rejection without mutation for a segment-selector change; and stale
expected-frame rejection.

## Transfers

No S3 result proves a DPMI service, descriptor allocation, LDT/IDT mutation,
protected-mode transition provider, interrupt/fault hook, DOS translation or
raw `Sim32GetVDMPointer` replacement.  Those remain in the S4--S6 source-owner
packets defined by the T289 plan.
