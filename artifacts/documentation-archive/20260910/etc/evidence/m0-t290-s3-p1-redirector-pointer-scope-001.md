# M0 T290 S3 P1 — Redirector synchronous guest-pointer scope

## Question

Can an exact original Redirector provider retain the historical
`GetVDMAddr`/`Sim32GetVDMPointer` call shape without treating Bochs guest RAM
as a native x86 or x64 process address?

## Inputs and procedure

- The original declaration carrier is
  `mvdm-support/inc/softpc.h`; Redirector provider bodies include that name.
- `adapter-mvdm-host-out/softpc/include/softpc.h` is a same-named include
  binding, selected only by the focused Redirector build's include order.
- The binding redirects only pointer-producing macros to a thread-bound
  session scope.  The scope leases copied real-mode guest spans and releases
  every lease before the provider call returns.
- `tools/build/New-T290S3RedirectorPointerScopeNinja.ps1` generated formal
  MSVC `/MT` Ninja graphs and fixtures were executed on both host targets.

## Observation

- x64 fixture `build/M0-T290/S3/redirector-pointer-scope-x64/bin/t290-s3-redirector-pointer-scope-fixture.exe` exited `0`.
- x86 fixture `build/M0-T290/S3/redirector-pointer-scope-x86/bin/t290-s3-redirector-pointer-scope-fixture.exe` exited `0`.
- Both prove: no pointer before a scope; exactly one active scope; two
  real-mode source aliases; commit back to checked guest memory; protected
  source-address rejection; and no pointer after scope teardown.

## Interpretation and boundary

This establishes only the mechanical, same-shaped synchronous pointer binding.
It neither compiles an original `vdmredir` body nor enables a `57:xx` selector.
Protected-mode aliases, asynchronous retention, host-handle conversion,
VDD-hook success and IRQ delivery remain excluded by the active S3 packet.
Those exclusions preserve the source-defined failure direction rather than
inventing a Redirector provider.

## Follow-up

The next S3 part must compile the selected original lifecycle/named-pipe body
with this binding and prove its original VDD/IRQ unavailable branch before any
successful Redirector lifecycle claim.
