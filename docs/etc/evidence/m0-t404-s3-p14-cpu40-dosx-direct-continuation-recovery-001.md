# M0 T404 S3 P14 — CPU40 DOSX direct-continuation recovery

## Question and source basis

P13 established that the two `reset.c` CMOS-shutdown inputs are original DOSX
continuations, not BOP addresses.  This delivery makes CPU40 enter those
addresses without the historical two-byte adjustment while retaining the
unchanged historical expression for every non-CPU30/non-CPU40 build.

Original `dpmi/dxutil.asm` pushes `DXCODE:enrm50` for shutdown code 9 and
writes `DXCODE:enrm45` for shutdown code 0Ah.  Its comments specify an IRET
to the first and a far jump to the second; each label begins ordinary `FCLI`
guest code.  CPU40 fetches at live EIP.  The direct source form is therefore
the smallest source-first recovery, with no adapter, provider, guest rewrite,
CCPU change, or BOP-stub change.

## Change

`softpc.new/base/bios/reset.c` now excludes `CPU_40_STYLE` together with the
already direct `CPU_30_STYLE` path at exactly two sites: the shutdown-code-9
saved IRET IP and the shutdown-code-0Ah BIOS jump IP.  `setIP(temp_word)` and
all surrounding register/stack handling remain original.  The historical
`temp_word + HOST_BOP_IP_FUDGE` remains compiled for other profiles.

`MVDM-HOST-DIV-265` records the provenance and the profile boundary in
`src/mvdm-host/README.md`.

## Build evidence

The fresh selected CPU40/Win32-x86 graph was generated in
`build/M0-T404/S3/r010-p14-dosx-direct-entry/` using the required Node 22
runtime and built through the project `VsDevCmd` x86 launcher.  Its formal
`original-softpc-process.exe` linked successfully; SHA-256:

```text
4eb546624b56d0bfe42507653bd098e450205db8b911b3438ba3c724028928be
```

The generated graph declares `CPU_40_STYLE`; its produced
`obj/bios/reset.obj` is therefore the selected direct-entry condition, not an
unselected source-only edit.

## Runtime result and publication limitation

The existing package was exercised with its existing default-off DEM-open
observer, using the historical direct declared target and the explicit WOW
bootstrap form:

```text
ntvdm32.exe -f -o --command system32\WRITE.EXE --without-diagnostics
ntvdm32.exe -f -o -w -a system32\KRNL386.EXE --command system32\WRITE.EXE --without-diagnostics
```

The first route opened `WRITE.EXE`, but recorded no `DOSX.EXE` or
`KRNL386.EXE` opening.  The explicit WOW route emitted no DEM-open record.
Neither result reaches either `enrm` continuation.  The final bounded launch
reported `EXITCODE=-1`; the four test workers created by these bounded runs
were verified as `O:\ntvdm64\ntvdm32.exe` instances and stopped.  No worker
remained afterward.  Raw records remain under `O:\ntvdm64\logs\`:

- `t404-p14-dem-open.txt`
- `t404-p14-wow-dem-open.txt` (absent/empty because the route did not reach
  the observer)
- `t404-p14-write-exit.txt`

This is a runtime-coverage limitation, not evidence that the source repair
regresses CPU40.  It also means the P14 executable is **not published** as a
new owner-test candidate.  After the temporary staging check, the formal and
runtime package were restored to the accepted P12 SHA-256:

```text
3dc9d7f0e8a7836653dca50cf5d32ddacc316e85e4af14b9546a94051498b3aa
```

## Disposition

The narrowly proven source repair, x86 build, divergence registration and
publication rollback are delivered.  A future packet may investigate why the
current standalone WOW launch exits before the DOSX entry contract and leaves
a worker; it must not treat P14 as runtime proof of that separate boundary or
expand this repair into global BOP/CCPU work.
