# M0 T303 S2 typed-resume contract closure

## Scope

This closure recovers one selector-blind mechanical contract only.  It keeps
the reached historical `host_simulate(void)` spelling while replacing its
untyped direct CPU-loop call with a copied finite-resume request and copied
mechanical outcome.  No BOP selector, OpenNT provider, guest-service policy,
or pointer conversion enters `adapter-bochs`.

## Contract

- `adapter_bochs_machine_lifecycle_resume` accepts a versioned, fixed-width
  finite tick budget and returns only input, inactive, machine-failure,
  unexpected-return, or budget-exhaustion status.
- The pre-existing `run_budget` form is a compatibility wrapper over that
  contract.
- A thread-bound `session` carries only the opaque budget and copied status.
- `host_simulate(void)` retains its historical no-argument form and delegates
  the mechanical request through `adapter-bochs`; it does not invoke a raw CPU
  loop itself.

## Formal build and execution

Both formal Ninja graphs are generated from source manifests which reject
`src.old`, any SoftPC/CCPU executor, BOP ingress, and prebuilt Bochs archives.
Each graph compiles the live Bochs core/overlay, adapter-bochs, the selected
same-shaped SoftPC forms and the dedicated fixture, then links and executes
the fixture.

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T303TypedResumeNinja.ps1 -Architecture x86
ninja -C build/M0-T303/S2/typed-resume-x86 test -j8

powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T303TypedResumeNinja.ps1 -Architecture x64
ninja -C build/M0-T303/S2/typed-resume-x64 test -j8
```

The fixture verifies rejected invalid input before machine creation; a live
machine and real-mode `jmp $` input; a thread-bound session guest-memory lease;
the source-shaped scalar `setAX/getAX` facade; a numeric real-mode location
copy/read lease; and `host_simulate()` returning the mechanical budget result.

## Limits retained

This is not a BOP dispatcher, a service continuation, an XMS/DPMI protected
mechanism, or a WOW callback closure.  Those source-site contracts remain in
their respective owner packages.
