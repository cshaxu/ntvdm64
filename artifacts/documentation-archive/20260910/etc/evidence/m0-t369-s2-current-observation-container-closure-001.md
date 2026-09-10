# M0 T369 S2 — Current observation-container closure

## Recovered cohort

The existing observer is not a guest service and has no production ABI.  It
is rebuilt solely so its binary identity matches the current source-shaped
package layout before T369's one admitted observation.

`tests/observation/console_startup_observer.c` now takes `argv[2]` as both
the child working directory and the reported fixed SystemRoot.  The app's
`app_package_layout_set_process_media_roots` derives the same root from the
product EXE directory; `Stage-OriginalSoftpcRuntime.mjs` places immutable
media directly under that directory with `system32` and `softpc` children.
The observer does not write guest memory, set a guest input, install a
debugger, or modify the product command line beyond the declared command.

## Reproducible build

The tracked Node generator creates an x86 Ninja graph and a local MSVC
environment wrapper:

```text
node tools/build/Generate-ObservationNinja.mjs <repository-root> build/M0-T369/S2/observer
cmd.exe /d /s /c "call build\\M0-T369\\S2\\observer\\msvc-x86.cmd ninja -C build/M0-T369/S2/observer -j 4"
```

The out-of-sandbox execution completed both compile and link actions:

```text
[1/2] cl.exe ... tests/observation/console_startup_observer.c
[2/2] link.exe ... console-startup-observer.obj kernel32.lib dbghelp.lib
```

The rebuilt observer is
`build/M0-T369/S2/observer/console-startup-observer.exe`:

- source SHA-256: `01031b006e0377b054aba6b2a4fa965e4ec5b4d4372b8fdcb7f9c21811e3c629`;
- observer SHA-256: `dabaf70d12a17d64ac319211a40d3cdb482d2c236aed32eb50a3ae3aad20826d`.

## Result

S2 closes the stale-observer discrepancy without modifying the DOS EXEC path.
The next S may formally rebuild/query the selected product and establish the
current product/stage identity.  Only then may S4 make one new fixed
observation; its result may prove `$Exec`, an earlier source terminal, or a
later owner, but cannot be pre-claimed here.
