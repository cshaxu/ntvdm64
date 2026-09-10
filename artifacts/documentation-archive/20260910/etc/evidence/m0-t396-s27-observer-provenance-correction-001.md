# M0 T396 S27 — WOW32 observer provenance correction 001

## Scope and fixed inputs

S27 recompiled the current checked-in non-debug Console-owning observer from
`tests/observation/console_startup_observer.c` rather than reusing a prior
binary.  The source SHA-256 was
`6102ef72bdd4744f22f6552d1593af98fedc282787349384108306a6dc9462d5`; the
build-local x86 observer SHA-256 was
`55444cd9b8052129ccaf93affaa696952244c421696b6364fbd5d1daad3254b0`.

The fixed `O:\\t396` test stage contained the S20 parent product
`ntvdm32.exe` (`ef60d6a993041df34635c7c867e53d560f624190de2136cc857aa94399decf44`)
and S25 provider `WOW32.DLL`
(`cec8951b5ce20c64cf53393c22587e26acc7441306c65e34fc6b6e0fd5bd9c3a`).
The observer owned a normal Console, passed exactly
`-f -o --command system32\\WOWEXEC.EXE`, supplied no product diagnostic
environment variables, and neither attached a debugger nor wrote child or
guest memory.

## Result

The 120,000-ms run timed out at 120,360 ms.  Its report SHA-256 is
`8925215281745afda5ba141ac071cc3034f0125888dad4a35f92e347aac8759ca` and
records:

```text
result=timeout
exit=0x53504354
loaded-module-count=57
loaded-wow32=no
stop-eip=0x778d9bcc
stop-fault=CS:05d8 IP:20d6 OP:63 69 65 73 5c
```

The sampled primary-thread return chain maps in the S20 parent map to original
CCPU, keyboard/PPI, VGA and host-event/COM owners, including
`SINIT013`, `SEVID020`, `ppi_outb`, `nt_vga_med_graph_huge`,
`ReturnUnusedKeyEvents`, and `host_com_ioctl`.  It is a live CPU-machine
frontier, not a WOW32 loader, initializer, BOP 51, session-terminal, or CRT
abort result.

## Correction

The earlier S24--S26 ordinary-run claims used an older observer executable.
That executable produced a short `loaded-wow32=yes` / `0xc0000409` result
which the current source-derived observer cannot reproduce under the same
parent and provider identities.  Therefore the following claims are
superseded and must not be used as product evidence:

- an ordinary post-load `RtlAllocateHeap` access violation;
- an ordinary post-initializer CRT `_abort` / fast-fail terminal;
- the absence of BOP 51 or session conversion as a property of that claimed
  terminal.

This correction does not invalidate S23's static import-alias audit, S25's
map-confirmed original `W32DllInitialize` bridge, or its focused direct
`LoadLibrary` proof.  It invalidates only the old observer's asserted
end-to-end WOWEXEC runtime progression.

## Disposition

The debug-only S27 contrast remains non-acceptance diagnostic material.  The
real current frontier is before `WOW32.DLL` loading.  Its next owner must use
the current-source observer and existing default-off source observations to
attribute the original CPU-machine path before proposing any recovery.
